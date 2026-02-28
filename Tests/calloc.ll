; ModuleID = '.\calloc.c'
source_filename = ".\\calloc.c"
target datalayout = "e-m:o-p0:16:16:16-p1:16:16:16-p2:32:16:16:16-p3:32:16:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:8-n16:16-S16"
target triple = "rl78-unknown-unknown-elf"

; Function Attrs: noinline nounwind optnone
define dso_local void @test_calloc_oob() #0 {
  %1 = alloca ptr, align 2
  %2 = call noalias ptr @calloc(i16 noundef 4, i16 noundef 2) #3
  store ptr %2, ptr %1, align 2
  %3 = load ptr, ptr %1, align 2
  %4 = getelementptr inbounds i16, ptr %3, i16 5
  store i16 123, ptr %4, align 2
  %5 = load ptr, ptr %1, align 2
  call void @free(ptr noundef %5)
  ret void
}

; Function Attrs: allocsize(0,1)
declare dso_local noalias ptr @calloc(i16 noundef, i16 noundef) #1

declare dso_local void @free(ptr noundef) #2
