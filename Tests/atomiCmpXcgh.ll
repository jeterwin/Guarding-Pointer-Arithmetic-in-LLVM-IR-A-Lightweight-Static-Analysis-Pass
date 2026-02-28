@glob = global i16 0, align 2

define i16 @test_atomic_cmpxchg_trunc() {
entry:
  %p8 = ptrtoint ptr @glob to i8
  %p16 = zext i8 %p8 to i16
  %q = inttoptr i16 %p16 to ptr
  %expected = alloca i16, align 2
  store i16 0, ptr %expected
  %res = cmpxchg ptr %q, i16 0, i16 1 seq_cst seq_cst
  %old = extractvalue { i16, i1 } %res, 0
  ret i16 %old
}
