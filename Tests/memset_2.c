#include <stdint.h>
#include <string.h>

extern uint8_t src_buf[4];
extern uint8_t dst_buf[4];

// Requires -O1 at compilation

__attribute__((noinline))
void test_memcpy_trunc_roundtrip(void) {
    uint8_t x = (uint8_t)(uintptr_t)src_buf;
    uint16_t y = (uint16_t)x;
    uint8_t *dst = (uint8_t *)(uintptr_t)y;

    memset(dst, 0, sizeof(src_buf));
}
