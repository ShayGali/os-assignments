#include <stdio.h>

int main() {
    long long x= 0x7165498511230;
    while(x) putchar(32+((0xC894A7B75116601 >>((x >>= 4)&15)*7)&0x7F));

    putchar(10);
}