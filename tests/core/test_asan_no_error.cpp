#include <stdlib.h>

int f(int *x) {
  int z = *x;
  delete [] x;
  return z;
}

int main() {
  int *x = new int[10];
  return f(x);
}
