; ModuleID = '.\memcpy_2.c'
source_filename = ".\\memcpy_2.c"
target datalayout = "e-m:o-p0:16:16:16-p1:16:16:16-p2:32:16:16:16-p3:32:16:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:8-n16:16-S16"
target triple = "rl78-unknown-unknown-elf"

@src_buf = external dso_local global [4 x i8], align 1

; Function Attrs: mustprogress nofree noinline nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define dso_local void @test_memcpy_trunc_roundtrip() local_unnamed_addr #0 {
  %1 = zext i8 ptrtoint (ptr @src_buf to i8) to i16
  %2 = inttoptr i16 %1 to ptr
  tail call void @llvm.memcpy.p0.p0.i16(ptr noundef nonnull align 1 dereferenceable(4) %2, ptr noundef nonnull align 1 dereferenceable(4) @src_buf, i16 4, i1 false)
  ret void
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i16(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i16, i1 immarg) #1
