#include <stdlib.h>

#include "include/kv_basic.h"

int main(int argc, char *argv[]) {
    unsigned seed = 1;
    if (argc>=2) {
        seed = strtoul(argv[1], NULL, 10);
    }
    printf("seed = %u", seed );
    srand(seed);
}