#include <emscripten.h>
#include <emscripten/wasm_worker.h>

EM_JS(void, console_log, (char* str), {
  console.log(UTF8ToString(str));
});

void worker_main()
{
  console_log("Hello from wasm worker!");
}

char stack[1024];

int main()
{
	emscripten_wasm_worker_t worker = emscripten_create_wasm_worker(stack, sizeof(stack));
	emscripten_wasm_worker_post_function_v(worker, worker_main);
}
