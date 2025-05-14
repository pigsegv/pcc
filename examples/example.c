#include <stdio.h>

#define LOL(x) int lol_##x;

int main(void) {
  char *a = "\\hi";
  int b = 10;
  b = ~b;

  return 0;
}
