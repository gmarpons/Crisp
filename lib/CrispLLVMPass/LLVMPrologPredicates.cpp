// LLVMPrologPredicates.cpp ------------------------------------------*- C++ -*-

// Copyright (C) 2011, 2012 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>
//
// This file is part of Crisp.
//
// Crisp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Crisp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Crisp.  If not, see <http://www.gnu.org/licenses/>.

#include <string>

#include "llvm/Argument.h"
#include "llvm/Function.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Use.h"

#include "LLVMPrologPredicates.h"

using namespace llvm;

namespace crisp {

  namespace prolog {

    const char* getSortName(unsigned OpCode) {
      switch (OpCode) {
      // Terminators
      case Instruction::Ret:    return "RetInst";
      case Instruction::Br:     return "BranchInst";
      case Instruction::Switch: return "SwitchInst";
      case Instruction::IndirectBr: return "IndirectBrInst";
      case Instruction::Invoke: return "InvokeInst";
      case Instruction::Resume: return "ResumeInst";
      case Instruction::Unreachable: return "UnreachableInst";

      // Standard binary operators...
      // BinaryOperator is a subclass of Instruction, AddOperator is a
      // subclass of Operator, etc.
      case Instruction::Add: return "BinaryOperator-AddOperator";
      // No specific Operator subclass for 'fadd'
      case Instruction::FAdd: return "BinaryOperator-fadd";
      case Instruction::Sub: return "BinaryOperator-SubOperator";
      case Instruction::FSub: return "BinaryOperator-fsub";
      case Instruction::Mul: return "BinaryOperator-MulOperator";
      case Instruction::FMul: return "BinaryOperator-fmul";
      case Instruction::UDiv: return "BinaryOperator-UDivOperator";
      case Instruction::SDiv: return "BinaryOperator-SDivOperator";
      case Instruction::FDiv: return "BinaryOperator-fdiv";
      case Instruction::URem: return "urem";
      case Instruction::SRem: return "srem";
      case Instruction::FRem: return "frem";

      // Logical operators...
      case Instruction::And: return "and";
      case Instruction::Or : return "or";
      case Instruction::Xor: return "xor";

      // Memory instructions...
      case Instruction::Alloca:        return "AllocaInst";
      case Instruction::Load:          return "LoadInst";
      case Instruction::Store:         return "StoreInst";
      case Instruction::AtomicCmpXchg: return "CmpXchgInst";
      case Instruction::AtomicRMW:     return "AtomicRMWInst";
      case Instruction::Fence:         return "FenceInst";
      case Instruction::GetElementPtr: return "GetElementPtrInst";

      // Convert instructions...
      case Instruction::Trunc:     return "TruncInst";
      case Instruction::ZExt:      return "ZExtInst";
      case Instruction::SExt:      return "SExtInst";
      case Instruction::FPTrunc:   return "FPTruncInst";
      case Instruction::FPExt:     return "FPExtInst";
      case Instruction::FPToUI:    return "FPToUIInst";
      case Instruction::FPToSI:    return "FPToSIInst";
      case Instruction::UIToFP:    return "UIToFPInst";
      case Instruction::SIToFP:    return "SIToFPInst";
      case Instruction::IntToPtr:  return "IntToPtrInst";
      case Instruction::PtrToInt:  return "PtrToIntInst";
      case Instruction::BitCast:   return "BitCastInst";

      // Other instructions...
      case Instruction::ICmp:           return "ICmpInst";
      case Instruction::FCmp:           return "FCmpInst";
      case Instruction::PHI:            return "PHINode";
      case Instruction::Select:         return "SelectInst";
      case Instruction::Call:           return "CallInst";
      case Instruction::Shl:            return "ShlInst";
      case Instruction::LShr:           return "BinaryOperator-LShrOperator";
      case Instruction::AShr:           return "BinaryOperator-AShrOperator";
      case Instruction::VAArg:          return "VAArgInst";
      case Instruction::ExtractElement: return "ExtractElementInst";
      case Instruction::InsertElement:  return "InsertElementInst";
      case Instruction::ShuffleVector:  return "ShuffleVectorInst";
      case Instruction::ExtractValue:   return "ExtractValueInst";
      case Instruction::InsertValue:    return "InsertValueInst";
      case Instruction::LandingPad:     return "LandingPadInst";

      default: return "<Invalid operator>";
      }
    }

    // Version of the template \c contains where begin and end are
    // ordinary functions.
    template< typename ContainerType,
              typename ElemType,
              typename IteratorType,
              IteratorType (* BeginFunc) (ContainerType *),
              IteratorType (* EndFunc) (ContainerType *)
              >
    foreign_t contains(const char* PredicateName,
                       term_t ContainerT, term_t ElemT, control_t Handle) {
      ContainerType *C;         // SWI-Prolog API doesn't support constness
      IteratorType *Context;
      IteratorType It;

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get first contained element
        It = BeginFunc(C);

        // Return next contained element (when exists)
        if (It == EndFunc(C)) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) &(*It));
          Context = new IteratorType(It);
          PL_retry_address((void *) Context);
        }
      case PL_REDO:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get next contained element
        Context = (IteratorType *) PL_foreign_context_address(Handle);
        It = *Context;
        ++It;

        // Return next contained element (when exists)
        if (It == EndFunc(C)) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) &(*It));
          *Context = It;
          PL_retry_address((void *) Context);
        }
      case PL_PRUNED:
        Context = (IteratorType *) PL_foreign_context_address(Handle);
        if (Context) delete Context;
      default:
        return TRUE;
      }
    }

    // Version of the template \c contains where begin and end are
    // member functions.
    template< typename ContainerType,
              typename ElemType,
              typename IteratorType,
              IteratorType (ContainerType::* BeginFunc) (),
              IteratorType (ContainerType::* EndFunc) ()
              >
    foreign_t contains(const char* PredicateName,
                       term_t ContainerT, term_t ElemT, control_t Handle) {
      ContainerType *C;         // SWI-Prolog API doesn't support constness
      IteratorType *Context;
      IteratorType It;

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get first contained element
        It = (C ->* BeginFunc)();

        // Return next contained element (when exists)
        if (It == (C ->* EndFunc)()) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) *It);
          Context = new IteratorType(It);
          PL_retry_address((void *) Context);
        }
      case PL_REDO:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get next contained element
        Context = (IteratorType *) PL_foreign_context_address(Handle);
        It = *Context;
        ++It;

        // Return next contained element (when exists)
        if (It == (C ->* EndFunc)()) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) *It);
          *Context = It;
          PL_retry_address((void *) Context);
        }
      case PL_PRUNED:
        Context = (IteratorType *) PL_foreign_context_address(Handle);
        if (Context) delete Context;
      default:
        return TRUE;
      }
    }

    // FIXME: Merge all contains* templates and use
    // overloading/specialization to get the needed diversity of
    // implementations under one single name.

    /// \param PredicateName <doc>
    /// \param ContainerT <doc>
    /// \param ElemT <doc>
    /// \param Handle <doc>
    /// \return <doc>
    // Template specialization for the case that \c IteratorType is
    // implemented as a plain pointer, thus dynamic memory allocation
    // is not needed.
    template< typename ContainerType,
              typename ElemType,
              typename IteratorType,
              ilist_iterator<ElemType> (ContainerType::* BeginFunc) (),
              ilist_iterator<ElemType> (ContainerType::* EndFunc) ()
              >
    foreign_t contains2(const char* PredicateName, term_t ContainerT,
                       term_t ElemT, control_t Handle) {
      ContainerType *C;         // SWI-Prolog API doesn't support constness
      IteratorType It;

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get first contained element
        It = (C ->* BeginFunc)();

        // Return next contained element (when exists)
        if (It == (C ->* EndFunc)()) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) It);
          PL_retry_address((void *) It);
        }
      case PL_REDO:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get next contained element
        It = (ElemType *) PL_foreign_context_address(Handle);
        ++It;

        // Return next contained element (when exists)
        if (It == (C ->* EndFunc)()) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) It);
          PL_retry_address((void *) It);
        }
      case PL_PRUNED:           // Nothing needs to be cleaned up
      default:
        return TRUE;
      }
    }

    foreign_t pl_getName(term_t ValueT, term_t NameT) {
      Value *V;
      if ( !PL_get_pointer(ValueT, (void **) &V))
        return PL_warning("getName/2: instantiation fault on first arg");
      return PL_unify_atom_chars(NameT, V->getName().str().c_str());
    }

    foreign_t pl_containsUse(term_t ValueT, term_t UserT, control_t Handle) {
      return contains< Value, User, Value::use_iterator,
                       &Value::use_begin, &Value::use_end >
        ("containsUse", ValueT, UserT, Handle);
    }

    foreign_t pl_isA_computed(term_t InstT, term_t SortT) {
      Instruction *I;
      if ( !PL_get_pointer(InstT, (void **) &I))
        return PL_warning("isA/2: instantiation fault on first arg");
      return PL_unify_atom_chars(SortT, getSortName(I->getOpcode()));
    }

    // This method exists for a number of classes not related by
    // inheritance
    foreign_t pl_getPointerOperand(term_t InstT, term_t OpT) {
      const Instruction *I;
      if ( !PL_get_pointer(InstT, (void **) &I))
        return PL_warning("getPointerOperand/2: "
                          "instantiation fault on first arg");
      if (const StoreInst *SI = dyn_cast<StoreInst>(I)) {
        return PL_unify_pointer(OpT, (void *) SI->getPointerOperand());
      } else if (const LoadInst *LI = dyn_cast<LoadInst>(I)) {
        return PL_unify_pointer(OpT, (void *) LI->getPointerOperand());
      }
      // else: llvm::AtomicCmpXchgInst, llvm::AtomicRMWInst,
      // llvm::GetElementPtrInst, llvm::VAArgInst, llvm::GEPOperator,
      // llvm::VAArgInst
      return FALSE;
    }

    foreign_t pl_getValueOperand(term_t StoreT, term_t OpT) {
      const StoreInst *I;
      if ( !PL_get_pointer(StoreT, (void **) &I))
        return PL_warning("getValueOperand/2: "
                          "instantiation fault on first arg");
      return PL_unify_pointer(OpT, (void *) I->getValueOperand());
    }

    foreign_t pl_containsArgument(term_t FuncT, term_t ArgT, control_t Handle) {
      return contains2< Function, Argument, Function::arg_iterator,
                       &Function::arg_begin, &Function::arg_end >
        ("containsArgument", FuncT, ArgT, Handle);
    }

    foreign_t pl_containsInstruction(term_t FuncT, term_t InstT,
                                     control_t Handle) {
      return contains< Function, Instruction, inst_iterator,
                       &inst_begin, &inst_end >
        ("containsInstruction", FuncT, InstT, Handle);
    }

    foreign_t pl_containsOp(term_t UserT, term_t UseT, control_t Handle) {
      return contains< User, Use, User::op_iterator,
                       &User::op_begin, &User::op_end >
        ("containsOp", UserT, UseT, Handle);
    }

    foreign_t pl_get_(term_t UseT, term_t ValueT) {
      const Use *U;
      if ( !PL_get_pointer(UseT, (void **) &U))
        return PL_warning("get_/2: instantiation fault on first arg");
      return PL_unify_pointer(ValueT, U->get());
    }

    foreign_t pl_getFunction(term_t ModuleT, term_t NameT,
                             term_t FunctionT) {
      const Module *M;
      if ( !PL_get_pointer(ModuleT, (void **) &M))
        return PL_warning("getFunction/3: instantiation fault on first arg");
      char *N;
      if ( !PL_get_atom_chars(NameT, &N))
        return PL_warning("getFunction/3: instantiation fault on second arg");
      return PL_unify_pointer(FunctionT, M->getFunction(StringRef(N)));
    }

  } // End namespace crisp::prolog

} // End namespace crisp
