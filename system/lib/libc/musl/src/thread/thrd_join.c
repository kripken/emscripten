#include <stdint.h>
#include <threads.h>
#include <pthread.h>

// XXX Emscripten implements implements pthread_join directly rather than __pthread_join
#ifdef __EMSCRIPTEN__
#define __pthread_join pthread_join
#endif

int thrd_join(thrd_t t, int *res)
{
        void *pthread_res;
        int rtn = __pthread_join(t, &pthread_res);
        // XXX Emscripten added handling of error case
        if (rtn) {
          return thrd_error;
        }
        if (res) *res = (int)(intptr_t)pthread_res;
        return thrd_success;
}
