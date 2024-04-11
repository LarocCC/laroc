struct mystruct {
  int a, b;
} myvalue1, myvalue2;

struct outer {
  int a, b;
  struct inner {
    int c;
  } y;
  int d;
} x;
