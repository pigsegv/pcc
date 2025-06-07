
float foo(int a());

// int *foo(int *(int a));

int main(void) {

  /* long int long unsigned volatile a; */
  static long double volatile b;
  int foo;

  printf("%Lf\n", b);
  return 0;
}
