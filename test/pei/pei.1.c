int f(int a) {
  int c = a + 1;
  return a - 1;
}

void g() {
  return;
}

int main(int argc) {
  return argc - 1;
}

int p(int x, int y) {
  if (x)
    return 1;
  else if (y)
    return 2;
  else
    return 3;
}
