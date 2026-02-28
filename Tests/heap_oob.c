#include <stdlib.h>

void heap_oob() {
    char *ptr = (char *)malloc(10);
    ptr[12] = 'x';
}
