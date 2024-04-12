struct mystruct {
  int a, b;
  struct inner {
    int c, d;
  } container;
} myvalue;

int f() {
  myvalue.a;
  return myvalue.container.c;
}
