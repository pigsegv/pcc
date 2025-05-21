#include <stdio.h>

struct a {
  struct b {
    int a;
  };
  
  struct b lol;
};

int main(void) {

  struct a foo;
  foo.lol.a = 10;

  return 0;
}
