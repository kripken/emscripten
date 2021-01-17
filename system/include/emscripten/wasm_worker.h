#pragma once

#ifdef __EMSCRIPTEN_PTHREADS__
#error Emscripten Wasm Workers API is not available when building with -pthread (-s USE_PTHREADS=1). Compile instead with -s WASM_WORKERS=1!
#endif

#include <stdint.h>
#include <emscripten/html5.h>

#ifdef __cplusplus
extern "C" {
#endif

#define emscripten_wasm_worker_t int
#define EMSCRIPTEN_WASM_WORKER_ID_PARENT 0

// If not building with Wasm workers enabled (-s WASM_WORKERS=0), returns 0.
emscripten_wasm_worker_t emscripten_create_wasm_worker(void *stackLowestAddress, uint32_t stackSize);

// Exists, but is a no-op if not building with Wasm Workers enabled (-s WASM_WORKERS=0)
void emscripten_terminate_wasm_worker(emscripten_wasm_worker_t id);

// Exists, but is a no-op if not building with Wasm Workers enabled (-s WASM_WORKERS=0)
void emscripten_terminate_all_wasm_workers(void);

// Returns EM_TRUE if the current thread is executing a Wasm Worker, EM_FALSE otherwise.
// Note that calling this function can be relatively slow as it incurs a Wasm->JS transition,
// so avoid calling it in hot paths.
EM_BOOL emscripten_current_thread_is_wasm_worker(void);

// postMessage()s a function call over to the given Wasm Worker.
// Note that if the Wasm Worker runs in an infinite loop, it will not process the postMessage
// queue to dispatch the function call, until the infinite loop is broken and execution is returned
// back to the Worker event loop.
// Exists, but is a no-op if not building with Wasm Workers enabled (-s WASM_WORKERS=0)
void emscripten_wasm_worker_post_function_v(emscripten_wasm_worker_t id, void (*funcPtr)(void));
void emscripten_wasm_worker_post_function_vi(emscripten_wasm_worker_t id, void (*funcPtr)(int), int arg0);
void emscripten_wasm_worker_post_function_vii(emscripten_wasm_worker_t id, void (*funcPtr)(int, int), int arg0, int arg1);
void emscripten_wasm_worker_post_function_viii(emscripten_wasm_worker_t id, void (*funcPtr)(int, int, int), int arg0, int arg1, int arg2);
void emscripten_wasm_worker_post_function_vd(emscripten_wasm_worker_t id, void (*funcPtr)(double), double arg0);
void emscripten_wasm_worker_post_function_vdd(emscripten_wasm_worker_t id, void (*funcPtr)(double, double), double arg0, double arg1);
void emscripten_wasm_worker_post_function_vddd(emscripten_wasm_worker_t id, void (*funcPtr)(double, double, double), double arg0, double arg1, double arg2);
void emscripten_wasm_worker_post_function_sig(emscripten_wasm_worker_t id, void *funcPtr, const char *sig, ...);

#define ATOMICS_WAIT_RESULT_T int

// Numbering dictated by https://github.com/WebAssembly/threads/blob/master/proposals/threads/Overview.md#wait
#define ATOMICS_WAIT_OK 0
#define ATOMICS_WAIT_NOT_EQUAL 1
#define ATOMICS_WAIT_TIMED_OUT 2

// Issues the wasm 'memory.atomic.wait32' instruction:
// If the given memory address contains value 'expectedValue', puts the calling thread to sleep to wait for that address to be notified.
// Returns one of the ATOMICS_WAIT_* return codes.
// NOTE: This function takes in the wait value in int64_t nanosecond units. Pass in maxWaitNanoseconds = -1 to wait infinitely long.
static inline __attribute__((always_inline)) ATOMICS_WAIT_RESULT_T emscripten_wasm_wait_i32(int32_t *addr, int expectedValue, int64_t maxWaitNanoseconds)
{
	return __builtin_wasm_memory_atomic_wait32(addr, expectedValue, maxWaitNanoseconds);
}

// Issues the wasm 'memory.atomic.wait64' instruction:
// If the given memory address contains value 'expectedValue', puts the calling thread to sleep to wait for that address to be notified.
// Returns one of the ATOMICS_WAIT_* return codes.
// NOTE: This function takes in the wait value in int64_t nanosecond units. Pass in maxWaitNanoseconds = -1 to wait infinitely long.
static inline __attribute__((always_inline)) ATOMICS_WAIT_RESULT_T emscripten_wasm_wait_i64(int64_t *addr, int64_t expectedValue, int64_t maxWaitNanoseconds)
{
	return __builtin_wasm_memory_atomic_wait64(addr, expectedValue, maxWaitNanoseconds);
}

#define EMSCRIPTEN_NOTIFY_ALL_WAITERS (-1LL)

// Issues the wasm 'memory.atomic.notify' instruction:
// Notifies the given number of threads waiting on a location.
// Pass count == EMSCRIPTEN_NOTIFY_ALL_WAITERS to notify all waiters on the given location.
// Returns the number of threads that were woken up.
// Note: this function is used to notify both waiters waiting on an i32 and i64 addresses.
static inline __attribute__((always_inline)) int64_t emscripten_wasm_notify(int32_t *addr, int64_t count)
{
	return __builtin_wasm_memory_atomic_notify(addr, count);
}

#define EMSCRIPTEN_WAIT_ASYNC_INFINITY __builtin_inf()

// Issues the JavaScript 'Atomics.waitAsync' instruction:
// performs an asynchronous wait operation on the main thread. If the given address contains val, issues a
// deferred wait that will invoke the specified callback function 'asyncWaitFinished' once that
// address has been notified by another thread.
// NOTE: Unlike functions emscripten_wasm_wait_i32() and emscripten_wasm_wait_i64() which take in the
// wait timeout parameter as int64 nanosecond units, this function takes in the wait timeout parameter
// as double millisecond units. See https://github.com/WebAssembly/threads/issues/175 for more information.
// Pass in maxWaitMilliseconds == EMSCRIPTEN_WAIT_ASYNC_INFINITY (==__builtin_inf()) to wait infinitely long.
ATOMICS_WAIT_RESULT_T emscripten_atomic_wait_async(int32_t *addr,
                                                   uint32_t val,
                                                   void (*asyncWaitFinished)(int32_t *addr, uint32_t val, ATOMICS_WAIT_RESULT_T waitResult, void *userData),
                                                   void *userData,
                                                   double maxWaitMilliseconds);

// Sleeps the calling wasm worker for the given nanoseconds. Calling this function on the main thread
// either results in a TypeError exception (Firefox), or a silent return without waiting (Chrome),
// see https://github.com/WebAssembly/threads/issues/174
void emscripten_wasm_worker_sleep(int64_t nanoseconds);

// Returns the value of navigator.hardwareConcurrency, i.e. the number of logical threads available for
// the user agent. NOTE: If the execution environment does not support navigator.hardwareConcurrency,
// this function will return zero to signal no support. (If the value 1 is returned, then it means that
// navigator.hardwareConcurrency is supported, but there is only one logical thread of concurrency available)
int emscripten_navigator_hardware_concurrency(void);

// Returns the value of the expression "Atomics.isLockFree(byteWidth)": true if the given memory access
// width can be accessed atomically, and false otherwise. Generally will return true
// on 1, 2 and 4 byte accesses. On 8 byte accesses, behavior differs across browsers, see
//  - https://bugzilla.mozilla.org/show_bug.cgi?id=1246139
//  - https://bugs.chromium.org/p/chromium/issues/detail?id=1167449
int emscripten_atomics_is_lock_free(int byteWidth);

#define emscripten_lock_t volatile uint32_t

// Use with syntax "emscripten_lock_t l = EMSCRIPTEN_LOCK_T_STATIC_INITIALIZER;"
#define EMSCRIPTEN_LOCK_T_STATIC_INITIALIZER 0

void emscripten_lock_init(emscripten_lock_t *lock);

// Returns true if the lock was successfully acquired. False on timeout.
// Can be only called in a Worker, not on the main browser thread.
EM_BOOL emscripten_lock_wait_acquire(emscripten_lock_t *lock, int64_t maxWaitNanoseconds);
void emscripten_lock_waitinf_acquire(emscripten_lock_t *lock);

// main thread + worker, raise an event when the lock is acquired. If you use this API in Worker, you cannot run an infinite loop.
void emscripten_lock_async_acquire(emscripten_lock_t *lock,
                                   void (*asyncWaitFinished)(volatile void *addr, uint32_t val, ATOMICS_WAIT_RESULT_T waitResult, void *userData),
                                   void *userData,
                                   double maxWaitMilliseconds);

// Can be called on both main thread and in Workers.
EM_BOOL emscripten_lock_try_acquire(emscripten_lock_t *lock);

void emscripten_lock_release(emscripten_lock_t *lock);

#define emscripten_semaphore_t volatile uint32_t

// Use with syntax emscripten_semaphore_t s = EMSCRIPTEN_SEMAPHORE_T_STATIC_INITIALIZER(num);
#define EMSCRIPTEN_SEMAPHORE_T_STATIC_INITIALIZER(num) ((int)(num))

void emscripten_semaphore_init(emscripten_semaphore_t *sem, int num);

// main thread, try acquire num instances, but do not sleep to wait if not available.
// Returns idx that was acquired or -1 if acquire failed.
int emscripten_semaphore_try_acquire(emscripten_semaphore_t *sem, int num);

// main thread, poll to try acquire num instances. Returns idx that was acquired. If you use this API in Worker, you cannot run an infinite loop.
void emscripten_semaphore_async_acquire(emscripten_semaphore_t *sem,
                                        int num,
                                        void (*asyncWaitFinished)(volatile void *addr, uint32_t idx, ATOMICS_WAIT_RESULT_T result, void *userData),
                                        void *userData,
                                        double maxWaitMilliseconds);

// worker, sleep to acquire num instances. Returns idx that was acquired, or -1 if timed out unable to acquire.
int emscripten_semaphore_wait_acquire(emscripten_semaphore_t *sem, int num, int64_t maxWaitNanoseconds);

// worker, sleep infinitely long to acquire num instances. Returns idx that was acquired, or -1 if timed out unable to acquire.
int emscripten_semaphore_waitinf_acquire(emscripten_semaphore_t *sem, int num);

// Releases the given number of resources back to the semaphore. Note
// that the ownership of resources is completely conceptual - there is
// no actual checking that the calling thread had previously acquired
// that many resouces, so programs need to keep check of their
// semaphore usage consistency themselves.
// Returns how many resources were available in the semaphore before the
// new resources were released back to the semaphore. (i.e. the index where
// the resource was put back to)
// [main thread or worker]
uint32_t emscripten_semaphore_release(emscripten_semaphore_t *sem, int num);

#define emscripten_condvar_t volatile uint32_t

// Use with syntax emscripten_condvar_t cv = EMSCRIPTEN_CONDVAR_T_STATIC_INITIALIZER();
#define EMSCRIPTEN_CONDVAR_T_STATIC_INITIALIZER() ((int)(0))

void emscripten_condvar_init(emscripten_condvar_t *condvar);

void emscripten_condvar_waitinf(emscripten_condvar_t *condvar, emscripten_lock_t *lock);

// TODO:
//void emscripten_condvar_wait(emscripten_condvar_t *condvar, emscripten_lock_t *lock, int64_t maxWaitNanoseconds);
//void emscripten_condvar_wait_async(emscripten_condvar_t *condvar, emscripten_lock_t *lock, int64_t maxWaitNanoseconds);

// Pass numWaitersToSignal == EMSCRIPTEN_NOTIFY_ALL_WAITERS to wake all waiters ("broadcast").
void emscripten_condvar_signal(emscripten_condvar_t *condvar, int64_t numWaitersToSignal);

#ifdef __cplusplus
}
#endif
