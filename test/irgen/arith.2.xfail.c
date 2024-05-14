int f(int x, int y, int z) {
  int r;

  r = x * y;
  r = x / y;
  r = x % y;

  r = x + y;
  r = x - y;

  r = x << y;
  r = x >> y;

  r = x < y;
  r = x > y;
  r = x <= y;
  r = x >= y;

  r = x == y;
  r = x != y;

  r = x & y;
  r = x | y;
  r = x ^ y;
  r = x && y;
  r = x || y;

  r = x ? y : z;

  return r;
}
