; ModuleID = 'd:\Repos\llvm-rl78\tests\lit\benchmarks\rl78_ccrl_iar_gcc_llvm_benchmark\Inputs\single_source\20080222-1.c'
source_filename = "d:\\Repos\\llvm-rl78\\tests\\lit\\benchmarks\\rl78_ccrl_iar_gcc_llvm_benchmark\\Inputs\\single_source\\20080222-1.c"
target datalayout = "e-m:o-p0:16:16:16-p1:16:16:16-p2:32:16:16:16-p3:32:16:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:8-n16:16-S16"
target triple = "rl78-unknown-unknown-elf"

@space = dso_local local_unnamed_addr global [6 x i8] c"\01\02\03\04\05\06", align 1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define dso_local i16 @foo(ptr nocapture noundef readonly %0) local_unnamed_addr #0 {
  %2 = getelementptr inbounds [1 x i8], ptr %0, i16 0, i16 4
  %3 = load i8, ptr %2, align 1, !tbaa !5
  %4 = zext i8 %3 to i16
  ret i16 %4
}

; Function Attrs: nounwind
define dso_local i16 @main() local_unnamed_addr #1 {
  %1 = load i8, ptr getelementptr inbounds ([6 x i8], ptr @space, i16 0, i16 4), align 1, !tbaa !5
  %2 = icmp eq i8 %1, 5
  br i1 %2, label %4, label %3

3:                                                ; preds = %0
  tail call void @abort() #3
  unreachable

4:                                                ; preds = %0
  ret i16 0
}

; Function Attrs: noreturn
declare dso_local void @abort() local_unnamed_addr #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) "no-jump-tables"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="RL78_S3" "target-features"="+mirror-source-0,+relax" }
attributes #1 = { nounwind "no-jump-tables"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="RL78_S3" "target-features"="+mirror-source-0,+relax" }
attributes #2 = { noreturn "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="RL78_S3" "target-features"="+mirror-source-0,+relax" }
attributes #3 = { noreturn nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 1, !"target-abi", !"nc_nd_d32"}
!2 = !{i32 1, !"ThinLTO", i32 0}
!3 = !{i32 1, !"EnableSplitLTOUnit", i32 1}
!4 = !{!"LLVM for Renesas RL78 (internal) clang version 17.0.1 (https://gitlab.global.renesas.com/compiler_dev/llvm-project.git d0ec877869f5a648702ae8f60e04047ae70ca3c4)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"omnipotent char", !7, i64 0}
!7 = !{!"Simple C/C++ TBAA"}

^0 = module: (path: "", hash: (0, 0, 0, 0, 0))
^1 = gv: (name: "abort") ; guid = 3331046420433779035
^2 = gv: (name: "foo", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 4, funcFlags: (readNone: 0, readOnly: 1, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0)))) ; guid = 6699318081062747564
^3 = gv: (name: "main", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 6, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 0, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0), calls: ((callee: ^1)), refs: (^4)))) ; guid = 15822663052811949562
^4 = gv: (name: "space", summaries: (variable: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), varFlags: (readonly: 1, writeonly: 1, constant: 0)))) ; guid = 16438206528846308351
^5 = flags: 8
^6 = blockcount: 0
