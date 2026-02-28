; ModuleID = '.\struct_oob.c'
source_filename = ".\\struct_oob.c"
target datalayout = "e-m:o-p0:16:16:16-p1:16:16:16-p2:32:16:16:16-p3:32:16:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:8-n16:16-S16"
target triple = "rl78-unknown-unknown-elf"

%struct.Data = type { i16, [4 x i8] }

; Function Attrs: noinline nounwind optnone
define dso_local void @struct_oob() #0 {
  %1 = alloca [2 x %struct.Data], align 2
  %2 = getelementptr inbounds [2 x %struct.Data], ptr %1, i16 0, i16 0
  %3 = getelementptr inbounds %struct.Data, ptr %2, i32 0, i32 1
  %4 = getelementptr inbounds [4 x i8], ptr %3, i16 0, i16 6
  store i8 97, ptr %4, align 2
  ret void
}
