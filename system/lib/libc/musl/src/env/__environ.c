#include "libc.h"

#ifdef __EMSCRIPTEN__
#include <stdlib.h>
#include <wasi/wasi.h>
#endif

char **__environ = 0;
weak_alias(__environ, ___environ);
weak_alias(__environ, _environ);
weak_alias(__environ, environ);

#ifdef __EMSCRIPTEN__
__attribute__((constructor))
void __emscripten_environ_constructor(void) {
    size_t environ_count;
    size_t environ_buf_size;
    __wasi_errno_t err = __wasi_environ_sizes_get(&environ_count,
                                                  &environ_buf_size);
    if (err != __WASI_ESUCCESS) {
        return err;
    }

    __environ = malloc(sizeof(char *) * (environ_count + 1));
    if (__environ == 0) {
        return;
    }
    char *environ_buf = malloc(sizeof(char) * environ_buf_size);
    if (__environ == 0 || environ_buf == 0) {
        __environ = 0;
        return __WASI_ENOMEM;
    }

    // Ensure null termination.
    __environ[environ_count] = 0;

    err = __wasi_environ_get(__environ, environ_buf);
    if (err != __WASI_ESUCCESS) {
        __environ = 0;
    }
}
#endif
