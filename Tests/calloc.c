#include <stdlib.h>
#include <stdint.h>

__attribute__((noinline))
void test_calloc_oob(void) {
    uint16_t *p = (uint16_t *)calloc(4, sizeof(uint16_t));
    p[5] = 123;
    free(p);
}
