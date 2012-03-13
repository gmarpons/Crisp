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

// #include <boost/lambda/bind.hpp>
// #include <boost/lambda/construct.hpp>
// #include <boost/lambda/control_structures.hpp>
// #include <boost/lambda/lambda.hpp>
#include <string>

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Argument.h"
#include "llvm/Function.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Use.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Type.h"

#include "crisp/PrologPredBaseTemplates.h"
#include "LLVMCompilationInfo.h"
#include "LLVMPrologPredicates.h"

using namespace llvm;
// namespace b = boost;
// namespace l = boost::lambda;

namespace crisp {

  namespace prolog {

    // Automatically generated function definitions.

#include "crisp/PrologPredDefinitionMacros.h"
#include "LLVMFunctions.inc"

    // Manual function definition.
    const char* getSortName(unsigned OpCode) {
      switch (OpCode) {
      // Terminators
      case Instruction::Ret:    return "ReturnInst";
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

    // foreign_t pl_containsUse(term_t ValueT, term_t UserT, control_t Handle) {
    //   return getMany<Value, Value::const_use_iterator,
    //                  &Value::use_begin, &Value::use_end>
    //     (ValueT, UserT, "use/2", Handle);
    // }

    // foreign_t pl_containsUse(term_t ValueT, term_t UserT, control_t Handle) {
    //   return contains< Value, Value::use_iterator >
    //     (ValueT, UserT, Handle, &Value::use_begin, &Value::use_end,
    //      l::ret<User *>(*l::_1));
    // }

    // foreign_t pl_containsArgument(term_t FuncT, term_t ArgT, control_t Handle) {
    //   return getMany<Function, Function::const_arg_iterator,
    //                  &Function::arg_begin, &Function::arg_end>
    //     (FuncT, ArgT, "argument/2", Handle);
    // }

    // foreign_t pl_containsArgument(term_t FuncT, term_t ArgT, control_t Handle) {
    //   return contains< Function, Argument >
    //     (FuncT, ArgT, Handle, &Function::arg_begin, &Function::arg_end);
    // }

    // foreign_t pl_containsInstruction(term_t FuncT, term_t InstT,
    //                                  control_t Handle) {
    //   return getMany<Function, const_inst_iterator, &inst_begin, &inst_end>
    //     (FuncT, InstT, "instruction/2", Handle);
    // }

    // foreign_t pl_containsInstruction(term_t FuncT, term_t InstT,
    //                                  control_t Handle) {
    //   return contains< Function, inst_iterator >
    //     (FuncT, InstT, Handle, &inst_begin, &inst_end, &(*l::_1));
    // }

    // foreign_t pl_containsOp(term_t UserT, term_t ValueT, control_t Handle) {
    //   return getMany<User, User::const_op_iterator,
    //                  &User::op_begin, &User::op_end>
    //     (UserT, ValueT, "operand/2", Handle);
    // }

    // foreign_t pl_containsOp(term_t UserT, term_t ValueT, control_t Handle) {
    //   return contains< User, User::op_iterator >
    //     (UserT, ValueT, Handle, &User::op_begin, &User::op_end,
    //      l::bind<Value *>(&Use::get, l::_1));
    // }

    foreign_t pl_getName(term_t ValueT, term_t NameT) {
      Value *V;
      if ( !PL_get_pointer(ValueT, (void **) &V))
        return PL_warning("getName/2: instantiation fault on first arg");
      return PL_unify_atom_chars(NameT, V->getName().str().c_str());
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
      } else if (const GetElementPtrInst *GI = dyn_cast<GetElementPtrInst>(I)) {
        return PL_unify_pointer(OpT, (void *) GI->getPointerOperand());
      }
      // TODO:
      // else: llvm::AtomicCmpXchgInst, llvm::AtomicRMWInst,
      // llvm::VAArgInst, llvm::GEPOperator, llvm::VAArgInst
      return FALSE;
    }

    foreign_t pl_getValueOperand(term_t StoreT, term_t OpT) {
      const StoreInst *I;
      if ( !PL_get_pointer(StoreT, (void **) &I))
        return PL_warning("getValueOperand/2: "
                          "instantiation fault on first arg");
      return PL_unify_pointer(OpT, (void *) I->getValueOperand());
    }

    // FIXME: comparison (unification) with existing locations makes
    // no sense
    foreign_t pl_getLocationFromStoreUser(term_t StoreT, term_t LocationT) {
      const StoreInst *I;
      if ( !PL_get_pointer(StoreT, (void **) &I))
        return PL_warning("getLocationFromStoreUser/2: "
                          "instantiation fault on first arg");
      const Pass &P = getLLVMCompilationInfo()->getPass();
      Location L = P.getAnalysis<AliasAnalysis>().getLocation(I);
      std::list<Location> &LS = getLLVMCompilationInfo()->getLocations();
      LS.push_back(L);
      return PL_unify_pointer(LocationT, &LS.back());
    }

    // FIXME: comparison (unification) with existing locations makes
    // no sense
    foreign_t pl_getLocationFromLoadUser(term_t LoadT, term_t LocationT) {
      const LoadInst *I;
      if ( !PL_get_pointer(LoadT, (void **) &I))
        return PL_warning("getLocationFromLoadUser/2: "
                          "instantiation fault on first arg");
      const Pass &P = getLLVMCompilationInfo()->getPass();
      Location L = P.getAnalysis<AliasAnalysis>().getLocation(I);
      std::list<Location> &LS = getLLVMCompilationInfo()->getLocations();
      LS.push_back(L);
      return PL_unify_pointer(LocationT, &LS.back());
    }

    // FIXME: comparison (unification) with existing locations makes
    // no sense
    foreign_t pl_createLocation(term_t ValueT, term_t LocationT) {
      const Value* V;
      if ( !PL_get_pointer(ValueT, (void **) &V))
        return PL_warning("createLocation/2: "
                          "instantiation fault on first arg");
      const Type* TypeOfV = V->getType();
      assert(TypeOfV && "Value 'V'has no type!");
      assert(TypeOfV->isPointerTy() && "Type of 'V' is not a pointer type!");
      Type* ElementTypeOfV = (cast<PointerType>(TypeOfV))->getElementType();

      const Pass &P = getLLVMCompilationInfo()->getPass();
      Location L;
      if (ElementTypeOfV->isSized()) {
        TargetData& TD = P.getAnalysis<TargetData>();
        uint64_t Size = TD.getTypeAllocSize(ElementTypeOfV);
        L = Location(V, Size);
      } else {
        L = Location(V);
      }
      std::list<Location> &LS = getLLVMCompilationInfo()->getLocations();
      LS.push_back(L);
      return PL_unify_pointer(LocationT, &LS.back());
    }

    foreign_t pl_aliasLessThanNoAlias(term_t LocationT1, term_t LocationT2) {
      Location *L1, *L2;
      if ( !PL_get_pointer(LocationT1, (void **) &L1))
        return PL_warning("aliasLessThanNoAlias/2: "
                          "instantiation fault on first arg");
      if ( !PL_get_pointer(LocationT2, (void **) &L2))
        return PL_warning("aliasLessThanNoAlias/2: "
                          "instantiation fault on second arg");
      const Pass &P = getLLVMCompilationInfo()->getPass();
      AliasAnalysis::AliasResult Res
        = P.getAnalysis<AliasAnalysis>().alias(*L1, *L2);
      return (Res == AliasAnalysis::NoAlias ? FALSE : TRUE);
    }

    foreign_t pl_alias(term_t LocationT1, term_t LocationT2, term_t AliasT) {
      Location *L1, *L2;
      if ( !PL_get_pointer(LocationT1, (void **) &L1))
        return PL_warning("alias/2: "
                          "instantiation fault on first arg");
      if ( !PL_get_pointer(LocationT2, (void **) &L2))
        return PL_warning("alias/2: "
                          "instantiation fault on second arg");
      const Pass &P = getLLVMCompilationInfo()->getPass();
      AliasAnalysis::AliasResult Res
        = P.getAnalysis<AliasAnalysis>().alias(*L1, *L2);
      return PL_unify_int64(AliasT, Res);
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
