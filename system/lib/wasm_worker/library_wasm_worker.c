#include <emscripten/wasm_worker.h>
#include <emscripten/threading.h>

// Internal implementation function in JavaScript side that emscripten_create_wasm_worker() calls to
// to perform the wasm worker creation.
emscripten_wasm_worker_t _emscripten_create_wasm_worker(void *stackLowestAddress, uint32_t stackSize);

emscripten_wasm_worker_t emscripten_create_wasm_worker(void *stackLowestAddress, uint32_t stackSize)
{
	uintptr_t stackBase = ((uintptr_t)stackLowestAddress + 15) & -16;
	stackSize = ((uintptr_t)stackLowestAddress + stackSize - stackBase) & -16;
	return _emscripten_create_wasm_worker((void*)stackBase, stackSize);
}

void emscripten_wasm_worker_sleep(int64_t nsecs)
{
	int32_t addr = 0;
	emscripten_atomic_wait_i32(&addr, 0, nsecs);
}

void emscripten_lock_init(emscripten_lock_t *lock)
{
	emscripten_atomic_store_u32((void*)lock, EMSCRIPTEN_LOCK_T_STATIC_INITIALIZER);
}

EM_BOOL emscripten_lock_wait_acquire(emscripten_lock_t *lock, int64_t maxWaitNanoseconds)
{
	emscripten_lock_t val;
	int64_t waitEnd = 0;
	do
	{
		val = emscripten_atomic_cas_u32((void*)lock, 0, 1);
		if (val)
		{
			int64_t t = (int64_t)(emscripten_performance_now() * 1e6);
			if (waitEnd)
			{
				if (t > waitEnd) return EM_FALSE;
				maxWaitNanoseconds = waitEnd - t;
			}
			else
			{
				waitEnd = (int64_t)(emscripten_performance_now()*1e6) + maxWaitNanoseconds;
			}
			emscripten_atomic_wait_i32((int32_t*)lock, val, maxWaitNanoseconds);
		}
	} while(val);
	return EM_TRUE;
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

EM_BOOL emscripten_lock_try_acquire(emscripten_lock_t *lock)
{
	uint32_t val = emscripten_atomic_cas_u32((void*)lock, 0, 1);
	return !val;
}

void emscripten_lock_release(emscripten_lock_t *lock)
{
	emscripten_atomic_store_u32((void*)lock, 0);
	emscripten_atomic_notify((int32_t*)lock, 1);
}

void emscripten_semaphore_init(emscripten_semaphore_t *sem, int num)
{
	emscripten_atomic_store_u32((void*)sem, num);
}

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

int emscripten_semaphore_wait_acquire(emscripten_semaphore_t *sem, int num, int64_t maxWaitNanoseconds)
{
	int val = emscripten_atomic_load_u32((void*)sem);
	for(;;)
	{
		while(val < num)
		{
			// TODO: Shave off maxWaitNanoseconds
			ATOMICS_WAIT_RESULT_T waitResult = emscripten_atomic_wait_i32((int32_t*)sem, val, maxWaitNanoseconds);
			if (waitResult == ATOMICS_WAIT_TIMED_OUT) return -1;
			val = emscripten_atomic_load_u32((void*)sem);
		}
		int ret = (int)emscripten_atomic_cas_u32((void*)sem, val, val - num);
		if (ret == val) return val - num;
		val = ret;
	}
}

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

uint32_t emscripten_semaphore_release(emscripten_semaphore_t *sem, int num)
{
	uint32_t ret = emscripten_atomic_add_u32((void*)sem, num);
	emscripten_atomic_notify((int*)sem, num);
	return ret;
}

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

void emscripten_condvar_signal(emscripten_condvar_t *condvar, int64_t numWaitersToSignal)
{
	emscripten_atomic_add_u32((void*)condvar, 1);
	emscripten_atomic_notify((int*)condvar, numWaitersToSignal);
}
