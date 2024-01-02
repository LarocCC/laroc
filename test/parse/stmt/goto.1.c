int f() {
  int a;
  goto label_1;
  a = 0;
label_1:
  a = 1;
  goto end;
  a = 2;
end:
  return a;
}
