#include <stdio.h>
#include <stdlib.h>

#include <emscripten.h>

void during_exit() {
  // An attempt to sleep during exit, which is not allowed (the exit process is
  // done in synchronous JS).
  EM_ASM({ out("during_exit 1") });
  emscripten_sleep(100);
  EM_ASM({ out("during_exit 2") });
}

int main() {
  atexit(&during_exit);
}
