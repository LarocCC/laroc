void f() {
  goto label_1;
  int x = 1;
label_1:;
  int y = 2;
  goto end;
  int z = 3;
end:
  return;
}
