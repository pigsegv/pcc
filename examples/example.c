#include <stdio.h>

#define LOL(x) int lol_##x;

int test(int a, ...);

int main(void) {
  char *a = "\\hi";
  int b = 10;
  b = ~b;

  return 0;
}
