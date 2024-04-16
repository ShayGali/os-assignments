/*
This program will cause a stack overflow and crash the program.
*/

void stack_overflow() { stack_overflow(); }

int main(void) {
  stack_overflow();
  return 0;
}