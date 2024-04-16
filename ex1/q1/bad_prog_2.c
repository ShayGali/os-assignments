/*
This program will cause a zero division error and crash the program.
*/
#include <stdio.h>

int main(void) {
  int x = 1;
  int y = 0;
  int z = x / y;
  printf("%d\n", z);
  return 0;
}