#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    printf("Hello world, I am another program!\n");
    printf("I have %d arguments, and ", argc);
    printf(" the first one is %s\n", argv[0]);
    exit(55);
    return 0;
}