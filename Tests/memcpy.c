#include <string.h>
#include <stdint.h>

uint8_t src[8];
uint8_t dst[4];

__attribute__((noinline))
void test_memcpy_oob_dst(void) {
    memcpy(dst + 2, src, 4);
}
