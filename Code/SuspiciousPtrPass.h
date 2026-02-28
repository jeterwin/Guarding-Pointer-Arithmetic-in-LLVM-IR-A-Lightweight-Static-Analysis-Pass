//===-- SuspiciousPtrPass.h - Example Transformations ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_SUSPICIOUSPTRPASS_H
#define LLVM_TRANSFORMS_UTILS_SUSPICIOUSPTRPASS_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

class SuspiciousPtrPass : public PassInfoMixin<SuspiciousPtrPass> {
public:
  PreservedAnalyses run(Function &F, AnalysisManager<Function> &AM);


private:
  void handleGEP(GetElementPtrInst *GEP, Function &F);
  void handleIntToPtr(IntToPtrInst *ITP, Function &F);
  void handleIntToPtr(ConstantExpr *ITP, Function &F);
  void handlePtrToInt(PtrToIntInst *ITP, Function &F);
  void handlePtrToInt(ConstantExpr *ITP, Function &F);
  bool handleComplexIntToPtr(BinaryOperator *BO, IntToPtrInst *ITP,
                             Function &F);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_SUSPICIOUSPTRPASS_H
