/*
this program will access an invalid memory location and crash the program.
*/

#include <stdio.h>

int main(void) {
    int *p = NULL;  // p is a null pointer
    *p = 42;
    return 0;
}