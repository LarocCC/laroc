int f(int a, int b) {
  int c = a + b;
  return a - b;
}

void g() {
  return;
}

int main(int argc) {
  return argc - 1;
}

int h() {
  int a = 1;
label_1:;
  int b = 2;
label_2:
  return a;
}
