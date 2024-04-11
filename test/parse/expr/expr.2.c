int f(int a, int b, int c) {
  int v1 = a;
  int v2 = 123;
  int v3 = a + b * c;
  int v4 = a * b + c;
  int v5 = a, v6 = b;
  int v7 = (((a)));
  int v8 = (a + b) * c;
  int v9 = a * (b + c);

  v1 = a;
  v2 = a + 1;
  v3 = 1 + a;

  int v10 = + +-+-v1;
  int v11 = +v1 - v2 + v3;

  v4 += v1 + 2;
  v5 -= 1 + v2;
  v8 += (v7 -= v6);

  int v12 = v1++;
  int v13 = v1--;
  int v14 = ++v2;
  int v15 = --v2;

  return 0;
}
