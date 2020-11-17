#pragma once

#ifdef __EMSCRIPTEN_PTHREADS__
#error Emscripten Wasm Workers API is not available when building with -pthread (-s USE_PTHREADS=1). Compile instead with -s USE_WASM_WORKERS=1!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define emscripten_wasm_worker_t int
#define EMSCRIPTEN_WASM_WORKER_ID_PARENT 0

emscripten_wasm_worker_t _emscripten_create_wasm_worker(void *stackLowestAddress, uint32_t stackSize);

// If not building with Wasm workers enabled (-s USE_WASM_WORKERS=0), returns 0.
emscripten_wasm_worker_t emscripten_create_wasm_worker(void *stackLowestAddress, uint32_t stackSize)
{
	uintptr_t stackBase = ((uintptr_t)stackLowestAddress + 15) & -16;
	stackSize = ((uintptr_t)stackLowestAddress + stackSize - stackBase) & -16;
	return _emscripten_create_wasm_worker((void*)stackBase, stackSize);
}

// Exists, but is a no-op if not building with Wasm Workers enabled (-s USE_WASM_WORKERS=0)
void emscripten_terminate_wasm_worker(emscripten_wasm_worker_t id);

// Exists, but is a no-op if not building with Wasm Workers enabled (-s USE_WASM_WORKERS=0)
void emscripten_terminate_all_wasm_workers(void);

// postMessage()s a function call over to the given Wasm Worker.
// Note that if the Wasm Worker runs in an infinite loop, it will not process the postMessage
// queue to dispatch the function call, until the infinite loop is broken and execution is returned
// back to the Worker event loop.
// Exists, but is a no-op if not building with Wasm Workers enabled (-s USE_WASM_WORKERS=0)
void emscripten_wasm_worker_post_function_v(emscripten_wasm_worker_t id, void (*funcPtr)(void));
void emscripten_wasm_worker_post_function_vi(emscripten_wasm_worker_t id, void (*funcPtr)(int), int arg0);
void emscripten_wasm_worker_post_function_vii(emscripten_wasm_worker_t id, void (*funcPtr)(int, int), int arg0, int arg1);
void emscripten_wasm_worker_post_function_viii(emscripten_wasm_worker_t id, void (*funcPtr)(int, int, int), int arg0, int arg1, int arg2);
void emscripten_wasm_worker_post_function_sig(emscripten_wasm_worker_t id, void *funcPtr, const char *sig, ...);

#define ATOMICS_WAIT_RESULT_T int

// Numbering dictated by https://github.com/WebAssembly/threads/blob/master/proposals/threads/Overview.md#wait
#define ATOMICS_WAIT_OK 0
#define ATOMICS_WAIT_NOT_EQUAL 1
#define ATOMICS_WAIT_TIMED_OUT 2

// If the given memory address contains value 'expectedValue', puts the calling
// thread to sleep to wait for that address to be notified.
// Returns one of the ATOMICS_WAIT_* return codes.
ATOMICS_WAIT_RESULT_T emscripten_atomic_wait_i32(int32_t *addr, int expectedValue, int64_t maxWaitMilliseconds)
{
	return __builtin_wasm_atomic_wait_i32(addr, expectedValue, maxWaitMilliseconds);
}

ATOMICS_WAIT_RESULT_T emscripten_atomic_wait_i64(int64_t *addr, int64_t expectedValue, int64_t maxWaitMilliseconds)
{
	return __builtin_wasm_atomic_wait_i64(addr, expectedValue, maxWaitMilliseconds);
}

#define EMSCRIPTEN_NOTIFY_ALL_WAITERS (-1LL)

int64_t emscripten_atomic_notify(int32_t *addr, int64_t count)
{
	return __builtin_wasm_atomic_notify(addr, count);
}

ATOMICS_WAIT_RESULT_T emscripten_atomic_async_wait(volatile void *addr,
                                                    uint32_t val,
                                                    void (*asyncWaitFinished)(volatile void *addr, uint32_t val, ATOMICS_WAIT_RESULT_T waitResult, void *userData),
                                                    void *userData,
                                                    double maxWaitMilliseconds);
/*
// Notifies the given number of threads waiting on a location.
// Pass count == INT_MAX to notify all waiters on the given location.
// Returns the number of threads that were woken up.
int emscripten_atomics_notify(volatile void *addr, int count);
*/
void emscripten_wasm_worker_sleep(double msecs)
{
	uint32_t addr = 0;
	emscripten_atomic_wait_i32((int32_t*)&addr, 0, msecs);
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

// Returns true if the lock was successfully acquired. False on timeout.
EM_BOOL emscripten_lock_wait_acquire(emscripten_lock_t *lock, double maxWaitMilliseconds) // only in worker
{
	emscripten_lock_t val;
	double waitEnd = 0;
	do
	{
		val = emscripten_atomic_cas_u32((void*)lock, 0, 1);
		if (val)
		{
			double t = emscripten_performance_now();
			if (waitEnd)
			{
				if (t > waitEnd) return EM_FALSE;
				maxWaitMilliseconds = waitEnd - t;
			}
			else
			{
				waitEnd = emscripten_performance_now() + maxWaitMilliseconds;
			}
			emscripten_atomic_wait_i32((int32_t*)lock, val, maxWaitMilliseconds);
		}
	} while(val);
}

void emscripten_lock_waitinf_acquire(emscripten_lock_t *lock) // only in worker
{
	emscripten_lock_t val;
	do
	{
		val = emscripten_atomic_cas_u32((void*)lock, 0, 1);
		if (val)
			emscripten_atomic_wait_i32((int32_t*)lock, val, EMSCRIPTEN_WAIT_INFINITY);
	} while(val);
}

// main thread + worker, raise an event when the lock is acquired. If you use this API in Worker, you cannot run an infinite loop.
ATOMICS_WAIT_RESULT_T emscripten_lock_async_acquire(emscripten_lock_t *lock,
                                                    void (*asyncWaitFinished)(volatile void *addr, uint32_t val, ATOMICS_WAIT_RESULT_T waitResult, void *userData),
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
	emscripten_atomic_notify((int32_t*)lock, 1);
}

#define emscripten_semaphore_t volatile uint32_t

// Use with syntax emscripten_semaphore_t s = EMSCRIPTEN_SEMAPHORE_T_STATIC_INITIALIZER(num);
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
			ATOMICS_WAIT_RESULT_T waitResult = emscripten_atomic_wait_i32((int32_t*)sem, val, maxWaitMilliseconds);
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
			emscripten_atomic_wait_i32((int32_t*)sem, val, EMSCRIPTEN_WAIT_INFINITY);
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
	uint32_t ret = emscripten_atomic_add_u32((void*)sem, num);
	emscripten_atomic_notify((int*)sem, num);
	return ret;
}

#define emscripten_condvar_t volatile uint32_t

// Use with syntax emscripten_condvar_t cv = EMSCRIPTEN_CONDVAR_T_STATIC_INITIALIZER();
#define EMSCRIPTEN_CONDVAR_T_STATIC_INITIALIZER() ((int)(0))

void emscripten_condvar_init(emscripten_condvar_t *condvar)
{
	*condvar = EMSCRIPTEN_CONDVAR_T_STATIC_INITIALIZER();
}

void emscripten_condvar_waitinf(emscripten_condvar_t *condvar, emscripten_lock_t *lock)
{
	int val = emscripten_atomic_load_u32((void*)condvar);
	emscripten_lock_release(lock);
	emscripten_atomic_wait_i32((int32_t*)condvar, val, EMSCRIPTEN_WAIT_INFINITY);
	emscripten_lock_waitinf_acquire(lock);
}

// TODO:
//void emscripten_condvar_wait(emscripten_condvar_t *condvar, emscripten_lock_t *lock, double maxWaitMilliseconds);
//void emscripten_condvar_wait_async(emscripten_condvar_t *condvar, emscripten_lock_t *lock, double maxWaitMilliseconds);

// Pass numWaitersToSignal == EMSCRIPTEN_NOTIFY_ALL_WAITERS to wake all waiters ("broadcast").
void emscripten_condvar_signal(emscripten_condvar_t *condvar, int64_t numWaitersToSignal)
{
	emscripten_atomic_add_u32((void*)condvar, 1);
	emscripten_atomic_notify(condvar, numWaitersToSignal);
}

#ifdef __cplusplus
}
#endif
