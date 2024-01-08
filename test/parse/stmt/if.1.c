int main(int argc) {
  if (argc)
    return 0;
  else
    return 1;
}

int g(int x, int y) {
  if (x)
    return 1;
  else if (y)
    return 2;
  else
    return 3;
}

int h(int x) {
  if (x)
    return 1;
  return 0;
}
