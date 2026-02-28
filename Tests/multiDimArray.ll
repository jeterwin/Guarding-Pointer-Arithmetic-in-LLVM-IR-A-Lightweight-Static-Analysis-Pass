; Function Attrs: noinline nounwind optnone
define dso_local i16 @main() #0 {
  %1 = alloca [5 x [5 x i16]], align 2
  %2 = getelementptr inbounds [5 x [5 x i16]], ptr %1, i16 0, i16 6
  %3 = getelementptr inbounds [5 x i16], ptr %2, i16 0, i16 2
  store i16 1, ptr %3, align 2
  ret i16 0
}

