#include <emscripten.h>
#include <emscripten/wasm_worker.h>
#include <emscripten/threading.h>
#include <stdlib.h>
#include <assert.h>

// Tests emscripten_lock_init(), emscripten_lock_waitinf_acquire() and emscripten_lock_release()

emscripten_lock_t lock = (emscripten_lock_t)12345315; // initialize with garbage

// Two shared variables, always a delta distance of one from each other.
volatile int sharedState0 = 0;
volatile int sharedState1 = 1;

volatile int numWorkersAlive = 0;

void test_ended()
{
  assert(sharedState0 == sharedState1 + 1 || sharedState1 == sharedState0 + 1);
#ifdef REPORT_RESULT
  REPORT_RESULT(sharedState0 > sharedState1 ? sharedState0 : sharedState1);
#endif
}

void worker_main()
{
  // Create contention on the lock from each thread, and stress the shared state
  // in a racy way that would show a breakage if the lock is not watertight.
  for(int i = 0; i < 1000; ++i)
  {
    emscripten_lock_waitinf_acquire(&lock);
    volatile int x = sharedState0;
    volatile int y = sharedState1;
    assert(x == y+1 || y == x+1);
    if (x < y)
    {
      x = y + 1;
      emscripten_wasm_worker_sleep(/*nsecs=*/(rand()%100000));
      sharedState0 = x;
    }
    else
    {
      y = x + 1;
      emscripten_wasm_worker_sleep(/*nsecs=*/(rand()%100000));
      sharedState1 = y;
    }
    emscripten_lock_release(&lock);
  }

  // Are we the last thread to finish? If so, test has ended.
  uint32_t v = emscripten_atomic_sub_u32((void*)&numWorkersAlive, 1);
  if (v == 1)
  {
    test_ended();
  }
}

int main()
{
  emscripten_lock_init(&lock);

#define NUM_THREADS 10
  numWorkersAlive = NUM_THREADS;
  for(int i = 0; i < NUM_THREADS; ++i)
  {
    void *stack = malloc(1024);
    emscripten_wasm_worker_t worker = emscripten_create_wasm_worker(stack, 1024);
    emscripten_wasm_worker_post_function_v(worker, worker_main);
  }
}
