@g = global i16 0, align 2

define void @test_trunc_roundtrip(ptr %base) {
  %p8 = ptrtoint ptr %base to i8
  %p16 = zext i8 %p8 to i16
  %q = inttoptr i16 %p16 to ptr
  load i8, ptr %q
  ret void
}
