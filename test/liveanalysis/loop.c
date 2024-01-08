int f(int n) {
  int result = 0;
  int i = 1;
loop_begin:
  result = result + i;
  if (n - i)
    ;
  else
    return result;
  i = i + 1;
  goto loop_begin;
}
