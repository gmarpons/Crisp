// CrispFunctionPass.cpp ---------------------------------------------*- C++ -*-

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

/// \file
/// \brief Crisp LLVM Function pass plugin entry point.

#define DEBUG_TYPE "crisp"

#include <string>

#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetData.h"

#include "crisp/SWIPrologInterface.h"
#include "PrologAssertLLVMFacts.h"

using namespace llvm;
using namespace prolog;

STATISTIC(StNumFunctionsFun, "Number of functions analyzed by crisp-fun");

namespace {
  unsigned NumFunctionsFun = 0;
}

namespace {
  class CrispFunctionPass : public FunctionPass {
  public:
    static char ID;
    CrispFunctionPass() : FunctionPass(ID) {}
    virtual bool doInitialization(Module& M);
    virtual bool runOnFunction(Function& F);
    virtual void getAnalysisUsage(AnalysisUsage& AU) const;
    virtual void releaseMemory();
    virtual bool doFinalization(Module& M);
  private:
    int Success;                // Prolog Engine status
  };                            // end of struct CrispFunctionPass

  char CrispFunctionPass::ID = 0;

  static RegisterPass<CrispFunctionPass> 
  Fun("crisp-fun", "Data extraction function pass for CRISP",
      false,                    // If true, only looks at CFG
      true);                    // If true, analysis Pass
}                               // end of anonymous namespace

bool CrispFunctionPass::doInitialization(Module& M) {
  DEBUG(dbgs() << "Initializing Crisp Function Pass.\n");
  DEBUG(dbgs() << "Handling LLVM Module.\n");
  // TODO: plRegisterPredicates();
  Success = plRunEngine("PrologBootForCrispLLVMPass.sh");

  if (Success) {
    // Read clang Prolog facts about the module
    const std::string& ModuleId(M.getModuleIdentifier());
    DEBUG(dbgs() << "LLVM Module name: " << ModuleId << "\n");

    plReadModuleFacts(ModuleId.c_str());
  }

  DEBUG(if (Success) dbgs() << "Crisp Function Pass initialized.\n";
        else dbgs() << "Analysis aborted: Prolog engine failed.\n";);

  return false;                 // M is not modified
}

bool CrispFunctionPass::runOnFunction(Function& F) {
  if (Success) {
    ++NumFunctionsFun;
    DEBUG(dbgs() << "Processing function: " << F.getName() << "\n");
  }

  return false;                 // F is not modified
}

// Analysis pass (it does not modify the program), but has some
// prerequisites.
void CrispFunctionPass::getAnalysisUsage(AnalysisUsage& AU) const {
  AU.setPreservesAll();
  AU.addRequired<AliasAnalysis>();
  AU.addRequired<TargetData>(); // Necessary to get location sizes
}

void CrispFunctionPass::releaseMemory() {
}

bool CrispFunctionPass::doFinalization(Module& M) {
  StNumFunctionsFun = NumFunctionsFun;

  if (Success) {
    // When debugging, open a PROLOG interactive session
    DEBUG(Success = plInteractiveSession());
  }
  DEBUG(if (Success) dbgs() << "LLVM Module analyzed.\n";
        else dbgs() << "LLVM Module analysis aborted: "
                    << "Prolog engine failed.\n";);
  (void) plCleanUp(Success ? 0 : 1); // Return value ignored

  return false;                 // M is not modified
}
