int printf(const char *, ...);

int main(void) {

  /* long int long unsigned volatile a; */
  static long double volatile b;

  printf("%Lf\n", b);
  return 0;
}
