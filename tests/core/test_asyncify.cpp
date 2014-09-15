#include <stdio.h>
#include <emscripten.h>
#include <exception>
#include <alloca.h>

int get_stacktop() { return (int)alloca(10); }
/*
Running :
-----
-----
Running :
-----
-----
Running :
-----
-----
Running :
-----
-----
Running :
-----
-----
 */

void f(void *p) {
  *(int*)p = 99;
  printf("!");
}
void test_basic() {
  int i = 0;
  printf("Hello");
  emscripten_async_call(f, &i, 1);
  printf("World");
  emscripten_sleep(100);
  printf("%d\n", i);
}

volatile int tmp = 17;
int return_value(int i) {
  int j = tmp;
  emscripten_sleep(1);
  // test use of stashed function argument and local variable
  return i + j;
}
void test_return_value() {
  // test use of async return value
  int rv = return_value(10);
  printf("Got async return %d\n", rv);
}

void throw_after_sleep() {
  emscripten_sleep(1);
  throw std::exception();
}
void call_throw_after_sleep() { throw_after_sleep(); }
void test_exceptions1() {
  int stack_initial = get_stacktop();
  try {
    throw_after_sleep();
  } catch (std::exception&) {
    printf("caught exception 1, stack leaked = %d\n",
           get_stacktop() == stack_initial);
  }
  try {
    call_throw_after_sleep();
  } catch (std::exception&) {
    printf("caught exception 2, stack leaked = %d\n",
           get_stacktop() == stack_initial);
  }
}

volatile int tmp2 = 1;
void async_fn_which_throws() {
  if (tmp2) throw std::exception();
  else emscripten_sleep(1);
}
void call_async_fn_which_throws() { async_fn_which_throws(); }
void throw_after_sleep_inside_async_call() {
  emscripten_sleep(1);
  call_async_fn_which_throws();
}
void call_throw_after_sleep_inside_async_call()
{ throw_after_sleep_inside_async_call(); }
void test_exceptions2() {
  int stack_initial = get_stacktop();
  try {
    throw_after_sleep_inside_async_call();
  } catch (std::exception&) {
    printf("caught exception 1, stack leaked = %d\n",
           get_stacktop() == stack_initial);
  }
  try {
    call_throw_after_sleep_inside_async_call();
  } catch (std::exception&) {
    printf("caught exception 2, stack leaked = %d\n",
           get_stacktop() == stack_initial);
  }
}

void test_async_free_after_invoke() {
  try {
    call_async_fn_which_throws();
  } catch (std::exception&) {
    printf("free'd ctx after sync call and caught exception\n");
  }
}

#define CALL(x) printf("Running " #x ":\n-----\n"); x(); printf("-----\n\n");
int main() {
CALL(test_basic)
CALL(test_return_value)
CALL(test_exceptions1)
CALL(test_exceptions2)
CALL(test_async_free_after_invoke)
}
