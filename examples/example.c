
int foo(static int a, int b);

int main() {

  /* long int long unsigned volatile a; */
  static long double volatile b;
  int foo;

  printf("%Lf\n", b);
  return 0;
}
