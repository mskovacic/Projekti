#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int a = 1, i;
    unsigned char* c = (unsigned char*) malloc(4);
    memcpy(c, &a, 4);
    for (i=0; i< 4; i++)
        printf("c[%d] = %d\n", i, c[i]);

    return 0;
}
