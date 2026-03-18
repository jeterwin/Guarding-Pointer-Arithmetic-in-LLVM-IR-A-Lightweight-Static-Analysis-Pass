; ModuleID = 'd:\Repos\llvm-rl78\tests\lit\benchmarks\rl78_ccrl_iar_gcc_llvm_benchmark\Inputs\single_source\20120207-1.c'
source_filename = "d:\\Repos\\llvm-rl78\\tests\\lit\\benchmarks\\rl78_ccrl_iar_gcc_llvm_benchmark\\Inputs\\single_source\\20120207-1.c"
target datalayout = "e-m:o-p0:16:16:16-p1:16:16:16-p2:32:16:16:16-p3:32:16:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:8-n16:16-S16"
target triple = "rl78-unknown-unknown-elf"

@.str = private unnamed_addr constant [11 x i8] c"0123456789\00", align 1

; Function Attrs: mustprogress nofree noinline nosync nounwind willreturn memory(none)
define dso_local i8 @test(i16 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [16 x i8], align 1
  call void @llvm.lifetime.start.p0(i64 16, ptr nonnull %2) #5
  call void @llvm.memcpy.p0.p0.i16(ptr noundef nonnull align 1 dereferenceable(11) %2, ptr noundef nonnull align 1 dereferenceable(11) @.str, i16 11, i1 false)
  %3 = getelementptr inbounds i8, ptr %2, i16 %0
  %4 = getelementptr inbounds i8, ptr %3, i16 -1
  %5 = load i8, ptr %4, align 1, !tbaa !5
  call void @llvm.lifetime.end.p0(i64 16, ptr nonnull %2) #5
  ret i8 %5
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nounwind
define dso_local i16 @main() local_unnamed_addr #2 {
  %1 = tail call i8 @test(i16 noundef 2)
  %2 = icmp eq i8 %1, 49
  br i1 %2, label %4, label %3

3:                                                ; preds = %0
  tail call void @abort() #6
  unreachable

4:                                                ; preds = %0
  ret i16 0
}

; Function Attrs: noreturn
declare dso_local void @abort() local_unnamed_addr #3

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i16(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i16, i1 immarg) #4

attributes #0 = { mustprogress nofree nosync nounwind willreturn memory(none) "no-jump-tables"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="RL78_S3" "target-features"="+mirror-source-0,+relax" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { nounwind "no-jump-tables"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="RL78_S3" "target-features"="+mirror-source-0,+relax" }
attributes #3 = { noreturn "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="RL78_S3" "target-features"="+mirror-source-0,+relax" }
attributes #4 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { nounwind }
attributes #6 = { noreturn nounwind }

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
^1 = gv: (name: ".str", summaries: (variable: (module: ^0, flags: (linkage: private, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), varFlags: (readonly: 1, writeonly: 0, constant: 1)))) ; guid = 1482116161646612805
^2 = gv: (name: "abort") ; guid = 3331046420433779035
^3 = gv: (name: "test", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 8, funcFlags: (readNone: 1, readOnly: 0, noRecurse: 0, returnDoesNotAlias: 0, noInline: 1, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0), refs: (^1)))) ; guid = 8346051122425466633
^4 = gv: (name: "main", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 6, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 0, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0), calls: ((callee: ^3, relbf: 256), (callee: ^2))))) ; guid = 15822663052811949562
^5 = flags: 8
^6 = blockcount: 0
