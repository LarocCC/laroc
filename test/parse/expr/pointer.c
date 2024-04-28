short *f(short *base, int m, int n) {
  short *mth = base + m;
  short *nth = base + n;
  int i = mth - nth;
  return base + i;
}
