#include <stdio.h>

int foo(int x) {
    return x;
}

int main(int argc, char ** argv) {
  if (argc < 0) {
    return foo(argc);
  }
  return 0;
}
