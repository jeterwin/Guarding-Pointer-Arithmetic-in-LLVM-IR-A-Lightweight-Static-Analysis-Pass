; ModuleID = '.\heap_oob.c'
source_filename = ".\\heap_oob.c"
target datalayout = "e-m:o-p0:16:16:16-p1:16:16:16-p2:32:16:16:16-p3:32:16:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:8-n16:16-S16"
target triple = "rl78-unknown-unknown-elf"

; Function Attrs: noinline nounwind optnone
define dso_local void @heap_oob() #0 {
  %1 = alloca ptr, align 2
  %2 = call noalias ptr @malloc(i16 noundef 10) #2
  store ptr %2, ptr %1, align 2
  %3 = load ptr, ptr %1, align 2
  %4 = getelementptr inbounds i8, ptr %3, i16 12
  store i8 120, ptr %4, align 1
  ret void
}

; Function Attrs: allocsize(0)
declare dso_local noalias ptr @malloc(i16 noundef) #1