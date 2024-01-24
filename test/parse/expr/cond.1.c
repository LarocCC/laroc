int f(int x) {
  return x ? 1 : 0;
}

int g(int x, int y, int z) {
  int a, b, c, d;
  a = x ? y : z;
  b = x + y ? y : z;
  c = x + y ? y + z : z + x;
  d = a, b ? 1 : x, y;
  d = a ? b ? c : d : x;
  d = a ? b : c ? d : x;
  return d;
}

int h(int x, int y, short z) {
  return x ? y : z;
}
