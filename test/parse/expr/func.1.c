double f(double x) {
  double t = x - 1;
  return t * t;
}

int g(int x) {
  return f(2 * x);
}

int h(int a, int b) {
  return f(g(a)) + g(f(b));
}
