#include <unistd.h>
#include <errno.h>
#include "syscall.h"
#include "libc.h"

static int dummy(int fd)
{
	return fd;
}

weak_alias(dummy, __aio_close);

int close(int fd)
{
	fd = __aio_close(fd);
#ifdef __EMSCRIPTEN__
	int r = __wasi_fd_close(fd);
	if (r == __WASI_EINTR) r = __WASI_ESUCCESS;
	return __wasi_syscall_ret(r);
#else
	int r = __syscall_cp(SYS_close, fd);
	if (r == -EINTR) r = 0;
	return __syscall_ret(r);
#endif
}
