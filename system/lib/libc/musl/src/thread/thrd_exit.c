#include <threads.h>
#include <pthread.h>
#include <stdint.h>

// XXX Emscripten implements pthread_exit directly rather than __pthread_exit
#ifdef __EMSCRIPTEN__
#define __pthread_exit pthread_exit
#endif

_Noreturn void thrd_exit(int result)
{
	__pthread_exit((void*)(intptr_t)result);
}
