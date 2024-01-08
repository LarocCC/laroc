int main(int argc) {
  if (argc)
    return 0;
  else
    return 1;
}

int g(int x, int y) {
  int ret;
  if (x)
    ret = 1;
  else if (y)
    ret = 2;
  else
    ret = 3;
  return ret;
}

int h(int x) {
  if (x)
    return 1;
  return 0;
}
