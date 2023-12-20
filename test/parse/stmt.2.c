int f() {
  {}

  42;

  { int a; }

  {
    int a;
    { int b; }
  }

  {
    {} {} {}
  }

  {
    int answer = 6 * 7;
  }
}

int ret1() {
  return;
}

int ret2() {
  return 1;
}
