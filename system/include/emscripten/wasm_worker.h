#pragma once

#ifdef __EMSCRIPTEN_PTHREADS__
#error Emscripten Wasm Workers API is not available when building with -pthread (-s USE_PTHREADS=1). Compile instead with -s WASM_WORKERS=1!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define emscripten_wasm_worker_t int
#define EMSCRIPTEN_WASM_WORKER_ID_PARENT 0

// If not building with Wasm workers enabled (-s USE_WASM_WORKERS=0), returns 0.
emscripten_wasm_worker_t emscripten_create_wasm_worker(void *stackLowestAddress, uint32_t stackSize);

// Exists, but is a no-op if not building with Wasm Workers enabled (-s USE_WASM_WORKERS=0)
void emscripten_terminate_wasm_worker(emscripten_wasm_worker_t id);

// postMessage()s a function call over to the given Wasm Worker.
// Note that if the Wasm Worker runs in an infinite loop, it will not process the postMessage
// queue to dispatch the function call, until the infinite loop is broken and execution is returned
// back to the Worker event loop.
// Exists, but is a no-op if not building with Wasm Workers enabled (-s USE_WASM_WORKERS=0)
void emscripten_wasm_worker_post_function_v(emscripten_wasm_worker_t id, void (*funcPtr)(void));
void emscripten_wasm_worker_post_function_vi(emscripten_wasm_worker_t id, void (*funcPtr)(int), int arg0);
void emscripten_wasm_worker_post_function_vii(emscripten_wasm_worker_t id, void (*funcPtr)(int, int), int arg0, int arg1);
void emscripten_wasm_worker_post_function_viii(emscripten_wasm_worker_t id, void (*funcPtr)(int, int, int), int arg0, int arg1, int arg2);
void emscripten_wasm_worker_post_function_sig(emscripten_wasm_worker_t id, void *funcPtr, EM_FUNC_SIGNATURE sig, ...);

#define ATOMICS_WAIT_RESULT_T int
#define ATOMICS_WAIT_OK 0
#define ATOMICS_WAIT_TIMED_OUT 1
#define ATOMICS_WAIT_NOT_EQUAL 2

// If the given memory address contains value val, puts the calling
// thread to sleep to wait for that address to be notified.
// Returns one of the ATOMICS_WAIT_* return codes.
ATOMICS_WAIT_RESULT_T emscripten_atomics_wait(volatile void *addr, uint32_t val, double maxWaitMilliseconds);

ATOMICS_WAIT_RESULT_T emscripten_atomics_async_wait(volatile void *addr,
                                                    uint32_t val,
                                                    void (*asyncWaitFinished)(volatile void *addr, uint32_t val, ATOMICS_WAIT_RESULT_T result, void *userData),
                                                    void *userData,
                                                    double maxWaitMilliseconds);

// Notifies the given number of threads waiting on a location.
// Pass count == INT_MAX to notify all waiters on the given location.
// Returns the number of threads that were woken up.
int emscripten_atomics_notify(volatile void *addr, int count);

void emscripten_wasm_worker_sleep(double msecs)
{
	uint32_t addr = 0;
	emscripten_atomics_wait((void*)&addr, 0, msecs);
}

int emscripten_navigator_hardware_concurrency(void);

// Returns true if the given memory access width can be accessed atomically. Generally will return true
// on 1, 2, 4 and 8 byte accesses.
int emscripten_atomics_is_lock_free(int byteWidth);

#define emscripten_lock_t volatile uint32_t

// Use with syntax "emscripten_lock_t l = EMSCRIPTEN_LOCK_T_STATIC_INITIALIZER;"
#define EMSCRIPTEN_LOCK_T_STATIC_INITIALIZER 0

#define EMSCRIPTEN_WAIT_INFINITY __builtin_inf()

void emscripten_lock_init(emscripten_lock_t *lock)
{
	emscripten_atomic_store_u32((void*)lock, EMSCRIPTEN_LOCK_T_STATIC_INITIALIZER);
}

void emscripten_lock_wait_acquire(emscripten_lock_t *lock, double maxWaitMilliseconds) // only in worker
{
	emscripten_lock_t val;
	do
	{
		val = emscripten_atomic_cas_u32((void*)lock, 0, 1);
		if (val) // TODO: shave off maxWaitMilliseconds
			emscripten_atomics_wait((void*)lock, val, maxWaitMilliseconds);
	} while(val);
}

void emscripten_lock_waitinf_acquire(emscripten_lock_t *lock) // only in worker
{
	emscripten_lock_t val;
	do
	{
		val = emscripten_atomic_cas_u32((void*)lock, 0, 1);
		if (val)
			emscripten_atomics_wait((void*)lock, val, __builtin_inf());
	} while(val);
}

// main thread + worker, raise an event when the lock is acquired. If you use this API in Worker, you cannot run an infinite loop.
ATOMICS_WAIT_RESULT_T emscripten_lock_async_acquire(emscripten_lock_t *lock,
                                                    void (*asyncWaitFinished)(volatile void *addr, uint32_t val, ATOMICS_WAIT_RESULT_T result, void *userData),
                                                    void *userData,
                                                    double maxWaitMilliseconds);

EM_BOOL emscripten_lock_try_acquire(emscripten_lock_t *lock) // main thread + worker
{
	uint32_t val = emscripten_atomic_cas_u32((void*)lock, 0, 1);
	return !val;
}

void emscripten_lock_release(emscripten_lock_t *lock)
{
	emscripten_atomic_store_u32((void*)lock, 0);
	emscripten_atomics_notify((void*)lock, 1);
}

#define emscripten_semaphore_t volatile uint32_t

// Use with syntax emscripten_semaphore_t s = EMSCRIPTEN_SEMAPHORE_T_STATIC_INITIALIZER(num); where
// num is a compile time constant.
/*
#define EMSCRIPTEN_SEMAPHORE_T_STATIC_INITIALIZER(num) __extension__ ({       \
        _Static_assert(__builtin_constant_p((num)), "Expected constant in static initialization of emscripten_semaphore_t"); \
        ((int)(num)); \
    })
*/
#define EMSCRIPTEN_SEMAPHORE_T_STATIC_INITIALIZER(num) ((int)(num))

void emscripten_semaphore_init(emscripten_semaphore_t *sem, int num)
{
	emscripten_atomic_store_u32((void*)sem, num);
}

// main thread, try acquire num instances, but do not sleep to wait if not available.
// Returns idx that was acquired or -1 if acquire failed.
int emscripten_semaphore_try_acquire(emscripten_semaphore_t *sem, int num)
{
	uint32_t val = num;
	for(;;)
	{
		uint32_t ret = emscripten_atomic_cas_u32((void*)sem, val, val - num);
		if (ret == val) return val - num;
		if (ret < num) return -1;
		val = ret;
	}
}

// main thread, poll to try acquire num instances. Returns idx that was acquired. If you use this API in Worker, you cannot run an infinite loop.
void emscripten_semaphore_async_acquire(emscripten_semaphore_t *sem,
                                        int num,
                                        void (*asyncWaitFinished)(volatile void *addr, uint32_t idx, ATOMICS_WAIT_RESULT_T result, void *userData),
                                        void *userData,
                                        double maxWaitMilliseconds);

// worker, sleep to acquire num instances. Returns idx that was acquired, or -1 if timed out unable to acquire.
int emscripten_semaphore_wait_acquire(emscripten_semaphore_t *sem, int num, double maxWaitMilliseconds)
{
	int val = emscripten_atomic_load_u32((void*)sem);
	for(;;)
	{
		while(val < num)
		{
			// TODO: Shave off maxWaitMilliseconds
			ATOMICS_WAIT_RESULT_T waitResult = emscripten_atomics_wait(sem, val, maxWaitMilliseconds);
			if (waitResult == ATOMICS_WAIT_TIMED_OUT) return -1;
			val = emscripten_atomic_load_u32((void*)sem);
		}
		int ret = (int)emscripten_atomic_cas_u32((void*)sem, val, val - num);
		if (ret == val) return val - num;
		val = ret;
	}
}

// worker, sleep infinitely long to acquire num instances. Returns idx that was acquired, or -1 if timed out unable to acquire.
int emscripten_semaphore_waitinf_acquire(emscripten_semaphore_t *sem, int num)
{
	int val = emscripten_atomic_load_u32((void*)sem);
	for(;;)
	{
		while(val < num)
		{
			emscripten_atomics_wait(sem, val, __builtin_inf());
			val = emscripten_atomic_load_u32((void*)sem);
		}
		int ret = (int)emscripten_atomic_cas_u32((void*)sem, val, val - num);
		if (ret == val) return val - num;
		val = ret;
	}
}

// Releases the given number of resources back to the semaphore. Note
// that the ownership of resources is completely conceptual - there is
// no actual checking that the calling thread had previously acquired
// that many resouces, so programs need to keep check of their
// semaphore usage consistency themselves.
// Returns how many resources were available in the semaphore before the
// new resources were released back to the semaphore. (i.e. the index where
// the resource was put back to)
// [main thread or worker]
uint32_t emscripten_semaphore_release(emscripten_semaphore_t *sem, int num)
{
//	uint32_t was = emscripten_atomic_load_u32((void*)sem);

	uint32_t ret = emscripten_atomic_add_u32((void*)sem, num);
	/*
	for(;;)
	{
		uint32_t ret = emscripten_atomic_cas_u32((void*)sem, was, was + num);
		if (ret == was) { was = ret; break; }
		was = ret;
	}
*/
	emscripten_atomics_notify((void*)sem, num);
	return ret;
	/*
	uint32_t is = emscripten_atomic_load_u32((void*)sem);
	EM_ASM(console.log('Released ' + $0 + 'semaphores. Was ' + $1 + ', is ' + $2), num, was, is);*/
}

#ifdef __cplusplus
}
#endif
