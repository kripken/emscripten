#include <emscripten.h>
#include <emscripten/wasm_worker.h>
#include <assert.h>

// Test emscripten_wasm_worker_post_function_*() API

EM_JS(void, console_log, (char* str), {
  console.log(UTF8ToString(str));
});

volatile int success = 0;

void v()
{
  console_log("v");
  ++success;
}

void vi(int i)
{
  console_log("vi");
  assert(i == 1);
  ++success;
}

void vii(int i, int j)
{
  console_log("vii");
  assert(i == 2);
  assert(j == 3);
  ++success;
}

void viii(int i, int j, int k)
{
  console_log("viii");
  assert(i == 4);
  assert(j == 5);
  assert(k == 6);
  ++success;
}

void vd(double i)
{
  console_log("vd");
  assert(i == 1.5);
  ++success;
}

void vdd(double i, double j)
{
  console_log("vdd");
  assert(i == 2.5);
  assert(j == 3.5);
  ++success;
}

void vddd(double i, double j, double k)
{
  console_log("vddd");
  assert(i == 4.5);
  console_log("viiiiiidddddd");
  assert(j == 5.5);
  console_log("viiiiiidddddd");
  assert(k == 6.5);
  console_log("viiiiiidddddd");
  ++success;
}

void viiiiiidddddd(int a, int b, int c, int d, int e, int f, double g, double h, double i, double j, double k, double l)
{
  console_log("viiiiiidddddd");
  assert(a == 10);
  console_log("viiiiiidddddd");
  assert(b == 11);
  console_log("viiiiiidddddd");
  assert(c == 12);
  console_log("viiiiiidddddd");
  assert(d == 13);
  console_log("viiiiiidddddd");
  assert(e == 14);
  console_log("viiiiiidddddd");
  assert(f == 15);
  console_log("viiiiiidddddd");
  assert(g == 16.5);
  console_log("viiiiiidddddd");
  assert(h == 17.5);
  console_log("viiiiiidddddd");
  assert(i == 18.5);
  console_log("viiiiiidddddd");
  assert(j == 19.5);
  console_log("viiiiiidddddd");
  assert(k == 20.5);
  console_log("viiiiiidddddd");
  assert(l == 21.5);
  console_log("viiiiiidddddd");
  ++success;
}

void test_finished()
{
#ifdef REPORT_RESULT
  REPORT_RESULT(success);
#endif
}

char stack[1024];

int main()
{
  assert(!emscripten_current_thread_is_wasm_worker());
  emscripten_wasm_worker_t worker = emscripten_create_wasm_worker(stack, sizeof(stack));
  emscripten_wasm_worker_post_function_v(worker, v);
  emscripten_wasm_worker_post_function_vi(worker, vi, 1);
  emscripten_wasm_worker_post_function_vii(worker, vii, 2, 3);
  emscripten_wasm_worker_post_function_viii(worker, viii, 4, 5, 6);
  emscripten_wasm_worker_post_function_vd(worker, vd, 1.5);
  emscripten_wasm_worker_post_function_vdd(worker, vdd, 2.5, 3.5);
  emscripten_wasm_worker_post_function_vddd(worker, vddd, 4.5, 5.5, 6.5);
  emscripten_wasm_worker_post_function_sig(worker, viiiiiidddddd, "iiiiiidddddd", 10, 11, 12, 13, 14, 15, 16.5, 17.5, 18.5, 19.5, 20.5, 21.5);
  emscripten_wasm_worker_post_function_v(worker, test_finished);
}
