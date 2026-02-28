#include <string.h>
#include <stdint.h>

uint8_t src[8];
uint8_t dest[4];

__attribute__((noinline))
void test_memmove(void) {
    size_t n = sizeof(dest) + 1;
    memmove(dest, src, n);
}

int main() {
return 0;
}
