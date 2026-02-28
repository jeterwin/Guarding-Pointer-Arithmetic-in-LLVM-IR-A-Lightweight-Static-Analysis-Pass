; ModuleID = '.\neg_array.c'
source_filename = ".\\neg_array.c"
target datalayout = "e-m:o-p0:16:16:16-p1:16:16:16-p2:32:16:16:16-p3:32:16:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:8-n16:16-S16"
target triple = "rl78-unknown-unknown-elf"

; Function Attrs: noinline nounwind optnone
define dso_local i16 @main() #0 {
  %1 = alloca i16, align 2
  %2 = alloca [5 x i16], align 2
  store i16 0, ptr %1, align 2
  %3 = getelementptr inbounds [5 x i16], ptr %2, i16 0, i16 -1
  store i16 2, ptr %3, align 2
  ret i16 0
}

