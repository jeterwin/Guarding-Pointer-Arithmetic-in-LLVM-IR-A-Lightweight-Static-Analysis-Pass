; ModuleID = '.\memcpy.c'
source_filename = ".\\memcpy.c"
target datalayout = "e-m:o-p0:16:16:16-p1:16:16:16-p2:32:16:16:16-p3:32:16:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:8-n16:16-S16"
target triple = "rl78-unknown-unknown-elf"

@dst = dso_local global [4 x i8] zeroinitializer, align 1
@src = dso_local global [8 x i8] zeroinitializer, align 1

; Function Attrs: noinline nounwind optnone
define dso_local void @test_memcpy_oob_dst() #0 {
  call void @llvm.memcpy.p0.p0.i16(ptr align 1 getelementptr inbounds (i8, ptr @dst, i16 2), ptr align 1 @src, i16 4, i1 false)
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i16(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i16, i1 immarg) #1

