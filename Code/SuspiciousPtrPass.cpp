#include "llvm/Transforms/Utils/SuspiciousPtrPass.h"

#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdint>
#include <optional>

using namespace llvm;

namespace {

static cl::opt<bool> WarnRoundTrip(
    "suspiciousptr-warn-roundtrip", cl::init(false),
    cl::desc("Warn on ptrtoint/arithmetic/inttoptr round-trips when "
             "dereferenced (can be noisy in low-level code)."));

static cl::opt<bool> WarnComputedIntToPtr(
    "suspiciousptr-warn-computed-inttoptr", cl::init(false),
    cl::desc("Warn on dereferenced inttoptr from non-constant integer values "
             "(can be noisy in allocators / low-level runtimes)."));

static cl::opt<bool> WarnConstIntToPtrNonVolatile(
    "suspiciousptr-warn-const-inttoptr", cl::init(false),
    cl::desc(
        "Warn on dereferenced inttoptr from constant addresses when the access "
        "is non-volatile (default off to avoid MMIO false positives)."));

static void printLoc(raw_ostream &OS, const Instruction &I) {
  if (const DILocation *Loc = I.getDebugLoc()) {
    OS << Loc->getFilename() << ":" << Loc->getLine() << ":"
       << Loc->getColumn();
  } else {
    OS << "<no debugloc>";
  }
}

static std::optional<uint64_t> getFixedTypeStoreSize(Type *Ty,
                                                     const DataLayout &DL) {
  TypeSize TS = DL.getTypeStoreSize(Ty);
  if (TS.isScalable())
    return std::nullopt;
  return TS.getFixedValue();
}

static bool hasDefiniteSubobjectOOB(GetElementPtrInst *GEP,
                                    const DataLayout &DL, int64_t &BadOffset,
                                    uint64_t &SubObjSize) {
  Type *CurTy = GEP->getSourceElementType();
  int64_t Offset = 0;

  auto IdxIt = GEP->idx_begin();

  // Skip the first index (base pointer),
  if (IdxIt != GEP->idx_end())
    ++IdxIt;

  for (; IdxIt != GEP->idx_end(); ++IdxIt) {
    auto *CI = dyn_cast<ConstantInt>(IdxIt->get());
    if (!CI)
      return false;

    int64_t Idx = CI->getSExtValue();

    if (auto *ST = dyn_cast<StructType>(CurTy)) {
      // Idx has to be != 0 to not flag variable arrays.
      int64_t NumElems = (int64_t)ST->getNumElements();
      if (Idx < 0 || (Idx >= NumElems && Idx != 0 && NumElems > 0)) {
        BadOffset = Offset;
        return true;
      }

      const StructLayout *SL = DL.getStructLayout(ST);
      Offset += SL->getElementOffset(Idx);
      CurTy = ST->getElementType(Idx);

    } else if (auto *AT = dyn_cast<ArrayType>(CurTy)) {
      uint64_t NumElems = AT->getNumElements();
      uint64_t ElemSize = DL.getTypeAllocSize(AT->getElementType());

      if (Idx < 0 || ((uint64_t)Idx >= NumElems && Idx != 0 && NumElems > 0)) {
        BadOffset = Offset + Idx * ElemSize;
        SubObjSize = NumElems * ElemSize;
        return true;
      }

      Offset += Idx * ElemSize;
      CurTy = AT->getElementType();
    } else {
      return false;
    }
  }

  return false;
}

static std::optional<uint64_t> getKnownObjectSize(Value *Obj,
                                                  const DataLayout &DL) {
  if (!Obj)
    return std::nullopt;

  Obj = Obj->stripPointerCasts();

  if (auto *GV = dyn_cast<GlobalVariable>(Obj)) {
    TypeSize TS = DL.getTypeAllocSize(GV->getValueType());
    if (TS.isScalable())
      return std::nullopt;
    return TS.getFixedValue();
  }

  if (auto *AI = dyn_cast<AllocaInst>(Obj)) {
    auto SizeOpt = AI->getAllocationSize(DL);
    if (!SizeOpt)
      return std::nullopt;
    if (SizeOpt->isScalable())
      return std::nullopt;
    return SizeOpt->getFixedValue();
  }

  return std::nullopt;
}

static bool isVolatileAccess(const Instruction &I) {
  if (auto *LI = dyn_cast<LoadInst>(&I))
    return LI->isVolatile();
  if (auto *SI = dyn_cast<StoreInst>(&I))
    return SI->isVolatile();
  if (auto *RMW = dyn_cast<AtomicRMWInst>(&I))
    return RMW->isVolatile();
  if (auto *CX = dyn_cast<AtomicCmpXchgInst>(&I))
    return CX->isVolatile();
  return false;
}

static void reportOOB(const Function &F, const Instruction &AccessI,
                      Value *BaseObj, int64_t Offset, uint64_t ObjSize,
                      uint64_t AccessSize) {
  errs() << "[SuspiciousPtr][DEF-OOB] " << F.getName() << " ";
  printLoc(errs(), AccessI);
  errs() << ": constant out-of-bounds memory access (offset=" << Offset
         << ", access=" << AccessSize << "B, object=" << ObjSize << "B)";

  if (auto *GV = dyn_cast_or_null<GlobalVariable>(BaseObj)) {
    errs() << " base=@" << GV->getName();
  } else if (isa_and_nonnull<AllocaInst>(BaseObj)) {
    errs() << " base=%alloca";
  }

  errs() << "\n  IR: ";
  AccessI.print(errs());
  BaseObj->print(errs());
  errs() << "\n";
}

static void reportRoundTrip(const Function &F, const Instruction &AccessI,
                            const char *Kind, const Value &From) {
  errs() << "[SuspiciousPtr][" << Kind << "] " << F.getName() << " ";
  printLoc(errs(), AccessI);
  errs() << ": dereferenced integer-to-pointer materialization\n  IR: ";
  AccessI.print(errs());
  errs() << "\n  From: ";
  From.print(errs());
  errs() << "\n";
}

static const PtrToIntOperator *findPtrToIntInIntExpr(Value *V,
                                                     unsigned Depth = 0) {
  if (!V || Depth > 8)
    return nullptr;

  // Strip integer casts (zext/sext/trunc) to reach the underlying producer.
  if (auto *CI = dyn_cast<CastInst>(V)) {
    unsigned Op = CI->getOpcode();
    if (Op == Instruction::ZExt || Op == Instruction::SExt ||
        Op == Instruction::Trunc)
      return findPtrToIntInIntExpr(CI->getOperand(0), Depth + 1);
  } else if (auto *CE = dyn_cast<ConstantExpr>(V)) {
    if (CE->isCast()) {
      unsigned Op = CE->getOpcode();
      if (Op == Instruction::ZExt || Op == Instruction::SExt ||
          Op == Instruction::Trunc)
        return findPtrToIntInIntExpr(CE->getOperand(0), Depth + 1);
    }
  }

  if (auto *PTI = dyn_cast<PtrToIntOperator>(V))
    return PTI;

  if (auto *BO = dyn_cast<BinaryOperator>(V)) {
    unsigned Op = BO->getOpcode();
    if (Op == Instruction::Add || Op == Instruction::Sub) {
      // Only follow through if one side is a constant.
      if (isa<ConstantInt>(BO->getOperand(0)))
        return findPtrToIntInIntExpr(BO->getOperand(1), Depth + 1);
      if (isa<ConstantInt>(BO->getOperand(1)))
        return findPtrToIntInIntExpr(BO->getOperand(0), Depth + 1);
    }
  }

  return nullptr;
}

static bool pointerOperandIsIntToPtr(Value *PtrVal) {
  if (!PtrVal)
    return false;
  Value *Root = PtrVal->stripPointerCasts();
  if (isa<IntToPtrInst>(Root))
    return true;
  if (auto *CE = dyn_cast<ConstantExpr>(Root))
    return CE->getOpcode() == Instruction::IntToPtr;
  return false;
}

static void analyzeIntToPtrAtDeref(Function &F, Instruction &AccessI,
                                   Value *PtrVal, const DataLayout &DL) {
  if (!PtrVal)
    return;

  Value *Root = PtrVal->stripPointerCasts();
  bool Vol = isVolatileAccess(AccessI);

  Value *IntSrc = nullptr;
  if (auto *ITP = dyn_cast<IntToPtrInst>(Root)) {
    IntSrc = ITP->getOperand(0);
  } else if (auto *CE = dyn_cast<ConstantExpr>(Root)) {
    if (CE->getOpcode() == Instruction::IntToPtr)
      IntSrc = CE->getOperand(0);
  }
  if (!IntSrc)
    return;

  // Definite bug: truncating ptr->int (narrow) feeding int->ptr, then
  // dereferenced.
  if (const PtrToIntOperator *PTI = findPtrToIntInIntExpr(IntSrc)) {
    unsigned IntBits = PTI->getType()->getIntegerBitWidth();
    unsigned AS = PTI->getPointerOperandType()->getPointerAddressSpace();
    unsigned PtrBits = DL.getPointerSizeInBits(AS);

    if (IntBits < PtrBits) {
      reportRoundTrip(F, AccessI, "DEF-TRUNC-ROUNDTRIP", *Root);
      return;
    }

    if (WarnRoundTrip) {
      reportRoundTrip(F, AccessI, "ROUNDTRIP", *Root);
      return;
    }
  }

  // Optional: constant inttoptr used in a *non-volatile* deref.
  if (auto *CI = dyn_cast<ConstantInt>(IntSrc)) {
    (void)CI;
    if (!Vol && WarnConstIntToPtrNonVolatile) {
      reportRoundTrip(F, AccessI, "CONST-INTTOPTR", *Root);
    }
    return;
  }

  // Optional: computed inttoptr from non-constant integer.
  if (WarnComputedIntToPtr && !Vol) {
    reportRoundTrip(F, AccessI, "COMPUTED-INTTOPTR", *Root);
  }
}

static std::optional<uint64_t> getHeapAllocSize(Value *V,
                                                const DataLayout &DL) {
  // If this is a load from a stack slot, try to recover the stored value.
  if (auto *LI = dyn_cast<LoadInst>(V)) {
    Value *Ptr = LI->getPointerOperand();

    if (auto *AI = dyn_cast<AllocaInst>(Ptr)) {
      Value *Stored = nullptr;

      for (User *U : AI->users()) {
        if (auto *SI = dyn_cast<StoreInst>(U)) {
          if (SI->getPointerOperand() == AI) {
            if (Stored)
              return std::nullopt;
            Stored = SI->getValueOperand();
          }
        }
      }

      if (Stored)
        V = Stored;
    }
  }

  auto *CI = dyn_cast<CallInst>(V);
  if (!CI)
    return std::nullopt;

  Function *Callee = CI->getCalledFunction();
  if (!Callee)
    return std::nullopt;

  StringRef Name = Callee->getName();

  if (Name == "malloc") {
    if (auto *C = dyn_cast<ConstantInt>(CI->getArgOperand(0)))
      return C->getZExtValue();
    return std::nullopt;
  }

  if (Name == "calloc") {
    auto *N = dyn_cast<ConstantInt>(CI->getArgOperand(0));
    auto *S = dyn_cast<ConstantInt>(CI->getArgOperand(1));
    if (N && S)
      return N->getZExtValue() * S->getZExtValue();
  }

  return std::nullopt;
}

static std::optional<uint64_t> resolveConstantInt(Value *V) {
  // Case 1: It is already a ConstantInt.
  if (auto *CI = dyn_cast<ConstantInt>(V))
    return CI->getZExtValue();

  // Case 2: It is a LoadInst.
  if (auto *LI = dyn_cast<LoadInst>(V)) {
    Value *Ptr = LI->getPointerOperand();

    // Possible unoptimised code check.
    if (auto *AI = dyn_cast<AllocaInst>(Ptr)) {
      Value *Stored = nullptr;

      for (User *U : AI->users()) {
        if (auto *SI = dyn_cast<StoreInst>(U)) {
          if (SI->getPointerOperand() == AI) {
            if (Stored)
              return std::nullopt;

            Stored = SI->getValueOperand();
          }
        }
      }

      if (Stored)
        return resolveConstantInt(Stored);
    }
  }

  return std::nullopt;
}

static bool getPtrAndSize(Instruction &I, Value *&Ptr, uint64_t &Sz,
                          const DataLayout &DL) {
  Ptr = nullptr;
  Sz = 0;

  Type *ValTy = nullptr;

  if (auto *LI = dyn_cast<LoadInst>(&I)) {
    ValTy = LI->getType();
    Ptr = LI->getPointerOperand();
  } else if (auto *SI = dyn_cast<StoreInst>(&I)) {
    ValTy = SI->getValueOperand()->getType();
    Ptr = SI->getPointerOperand();
  } else if (auto *RMW = dyn_cast<AtomicRMWInst>(&I)) {
    ValTy = RMW->getValOperand()->getType();
    Ptr = RMW->getPointerOperand();
  } else if (auto *CX = dyn_cast<AtomicCmpXchgInst>(&I)) {
    ValTy = CX->getCompareOperand()->getType();
    Ptr = CX->getPointerOperand();
  } else {
    return false;
  }

  auto MaybeSize = getFixedTypeStoreSize(ValTy, DL);
  if (!MaybeSize)
    return false;

  Sz = *MaybeSize;
  return true;
}

static void analyzeDefiniteOOB(Function &F, Instruction &AccessI, Value *PtrVal,
                               uint64_t AccessSize, const DataLayout &DL) {
  if (!PtrVal)
    return;

  if (auto *GEP = dyn_cast<GetElementPtrInst>(PtrVal)) {
    int64_t BadOffset = 0;
    uint64_t SubSize = 0;

    if (hasDefiniteSubobjectOOB(GEP, DL, BadOffset, SubSize)) {
      reportOOB(F, AccessI, GEP, BadOffset, SubSize, AccessSize);
      return;
    }
  }

  int64_t Offset = 0;
  Value *Base = GetPointerBaseWithConstantOffset(PtrVal, Offset, DL,
                                                 /*AllowNonInBounds=*/true);
  if (!Base)
    return;

  Base = getUnderlyingObject(Base);
  if (!Base)
    return;

  uint64_t ObjSize;
  if (auto *AI = dyn_cast<AllocaInst>(Base)) {
    ObjSize = DL.getTypeAllocSize(AI->getAllocatedType());
  } else if (auto *GV = dyn_cast<GlobalVariable>(Base)) {
    ObjSize = DL.getTypeAllocSize(GV->getValueType());
  } else {
    if (auto HeapSize = getHeapAllocSize(Base, DL)) {
      ObjSize = *HeapSize;
    } else {
      auto ObjSizeOpt = getKnownObjectSize(Base, DL);
      if (!ObjSizeOpt)
        return;
      ObjSize = *ObjSizeOpt;
    }
  }

  // Might be a variable length array / struct, don't emit false positive.
  if (ObjSize == 0)
    return;

  // Negative offsets are definitely OOB relative to the base object.
  if (Offset < 0 || AccessSize > ObjSize) {
    reportOOB(F, AccessI, Base, Offset, ObjSize, AccessSize);
    return;
  }

  uint64_t UOff = static_cast<uint64_t>(Offset);
  // Avoid overflow: require UOff <= ObjSize and ObjSize - UOff >= AccessSize.
  if (UOff > ObjSize || AccessSize > (ObjSize - UOff)) {
    reportOOB(F, AccessI, Base, Offset, ObjSize, AccessSize);
    return;
  }
}

} // namespace

PreservedAnalyses SuspiciousPtrPass::run(Function &F,
                                         FunctionAnalysisManager &AM) {
  const DataLayout &DL = F.getParent()->getDataLayout();

  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {

      Value *Ptr;
      uint64_t Sz;

      if (getPtrAndSize(I, Ptr, Sz, DL)) {
        analyzeDefiniteOOB(F, I, Ptr, Sz, DL);
        if (pointerOperandIsIntToPtr(Ptr))
          analyzeIntToPtrAtDeref(F, I, Ptr, DL);
        continue;
      }

      if (auto *MI = dyn_cast<MemIntrinsic>(&I)) {
        auto LenCI = resolveConstantInt(MI->getLength());
        if (!LenCI)
          continue;

        uint64_t Len = *LenCI;

        if (auto *MS = dyn_cast<MemSetInst>(MI)) {
          Value *Dst = MS->getRawDest();
          analyzeDefiniteOOB(F, I, Dst, Len, DL);
          if (pointerOperandIsIntToPtr(Dst))
            analyzeIntToPtrAtDeref(F, I, Dst, DL);
          continue;
        }

        if (auto *MT = dyn_cast<MemTransferInst>(MI)) {
          Value *Dst = MT->getRawDest();
          Value *Src = MT->getRawSource();

          analyzeDefiniteOOB(F, I, Dst, Len, DL);
          analyzeDefiniteOOB(F, I, Src, Len, DL);

          if (pointerOperandIsIntToPtr(Dst))
            analyzeIntToPtrAtDeref(F, I, Dst, DL);
          if (pointerOperandIsIntToPtr(Src))
            analyzeIntToPtrAtDeref(F, I, Src, DL);
          continue;
        }
      }
    }
  }

  return PreservedAnalyses::all();
}
