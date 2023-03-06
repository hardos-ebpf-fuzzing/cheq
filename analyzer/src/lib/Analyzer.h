#ifndef ANALYZER_GLOBAL_H
#define ANALYZER_GLOBAL_H

#include <fstream>
#include <iostream>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Common.h"

//
// typedefs
//
typedef vector<pair<llvm::Module *, llvm::StringRef>> ModuleList;
// Mapping module to its file name.
typedef unordered_map<llvm::Module *, llvm::StringRef> ModuleNameMap;
// The set of all functions.
typedef llvm::SmallPtrSet<llvm::Function *, 8> FuncSet;
// Mapping from function name to function.
typedef unordered_map<string, llvm::Function *> NameFuncMap;
typedef llvm::SmallPtrSet<llvm::CallInst *, 8> CallInstSet;
typedef DenseMap<Function *, CallInstSet> CallerMap;
typedef DenseMap<CallInst *, FuncSet> CalleeMap;

struct GlobalContext {

  GlobalContext() {
    // Initialize statistucs.
    NumSecurityChecks = 0;
    NumCondStatements = 0;
  }

  unsigned NumSecurityChecks;
  unsigned NumCondStatements;

  // Map global function name to function.
  NameFuncMap GlobalFuncs;

  // Functions whose addresses are taken.
  FuncSet AddressTakenFuncs;

  // Map a callsite to all potential callee functions.
  CalleeMap Callees;

  // Map a function to all potential caller instructions.
  CallerMap Callers;

  // Indirect call instructions.
  std::vector<CallInst *> IndirectCallInsts;

  // Unified functions -- no redundant inline functions
  DenseMap<size_t, Function *> UnifiedFuncMap;
  set<Function *> UnifiedFuncSet;

  // Map function signature to functions
  DenseMap<size_t, FuncSet> sigFuncsMap;

  // Modules.
  ModuleList Modules;
  ModuleNameMap ModuleMaps;
  set<string> InvolvedModules;

  // SecurityChecksPass
  // Functions handling errors
  set<string> ErrorHandleFuncs;
  map<string, tuple<int8_t, int8_t, int8_t>> CopyFuncs;

  // Identified sanity checks
  DenseMap<Function *, set<SecurityCheck>> SecurityCheckSets;
  DenseMap<Function *, set<Value *>> CheckInstSets;
};

class IterativeModulePass {
protected:
  GlobalContext *Ctx;
  const char *ID;

public:
  IterativeModulePass(GlobalContext *Ctx_, const char *ID_)
      : Ctx(Ctx_), ID(ID_) {}

  // Run on each module before iterative pass.
  virtual bool doInitialization(llvm::Module *M) { return true; }

  // Run on each module after iterative pass.
  virtual bool doFinalization(llvm::Module *M) { return true; }

  // Iterative pass.
  virtual bool doModulePass(llvm::Module *M) { return false; }

  virtual void run(ModuleList &modules);
};

#endif
