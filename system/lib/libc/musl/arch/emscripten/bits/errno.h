#include <wasi/wasi.h>

#define EPERM              __WASI_EPERM
#define ENOENT             __WASI_ENOENT
#define ESRCH              __WASI_ESRCH
#define EINTR              __WASI_EINTR
#define EIO                __WASI_EIO
#define ENXIO              __WASI_ENXIO
#define E2BIG              __WASI_E2BIG
#define ENOEXEC            __WASI_ENOEXEC
#define EBADF              __WASI_EBADF
#define ECHILD             __WASI_ECHILD
#define EAGAIN             __WASI_EAGAIN
#define ENOMEM             __WASI_ENOMEM
#define EACCES             __WASI_EACCES
#define EFAULT             __WASI_EFAULT
#define EBUSY              __WASI_EBUSY
#define EEXIST             __WASI_EEXIST
#define EXDEV              __WASI_EXDEV
#define ENODEV             __WASI_ENODEV
#define ENOTDIR            __WASI_ENOTDIR
#define EISDIR             __WASI_EISDIR
#define EINVAL             __WASI_EINVAL
#define ENFILE             __WASI_ENFILE
#define EMFILE             __WASI_EMFILE
#define ENOTTY             __WASI_ENOTTY
#define ETXTBSY            __WASI_ETXTBSY
#define EFBIG              __WASI_EFBIG
#define ENOSPC             __WASI_ENOSPC
#define ESPIPE             __WASI_ESPIPE
#define EROFS              __WASI_EROFS
#define EMLINK             __WASI_EMLINK
#define EPIPE              __WASI_EPIPE
#define EDOM               __WASI_EDOM
#define ERANGE             __WASI_ERANGE
#define EDEADLK            __WASI_EDEADLK
#define ENAMETOOLONG       __WASI_ENAMETOOLONG
#define ENOLCK             __WASI_ENOLCK
#define ENOSYS             __WASI_ENOSYS
#define ENOTEMPTY          __WASI_ENOTEMPTY
#define ELOOP              __WASI_ELOOP
#define ENOMSG             __WASI_ENOMSG
#define EIDRM              __WASI_EIDRM
#define EL2NSYNC           __WASI_EL2NSYNC
#define ENOLINK            __WASI_ENOLINK
#define EPROTO             __WASI_EPROTO
#define EMULTIHOP          __WASI_EMULTIHOP
#define EBADMSG            __WASI_EBADMSG
#define EOVERFLOW          __WASI_EOVERFLOW
#define EILSEQ             __WASI_EILSEQ
#define ENOTSOCK           __WASI_ENOTSOCK
#define EDESTADDRREQ       __WASI_EDESTADDRREQ
#define EMSGSIZE           __WASI_EMSGSIZE
#define EPROTOTYPE         __WASI_EPROTOTYPE
#define ENOPROTOOPT        __WASI_ENOPROTOOPT
#define EPROTONOSUPPORT    __WASI_EPROTONOSUPPORT
#define EAFNOSUPPORT       __WASI_EAFNOSUPPORT
#define EADDRINUSE         __WASI_EADDRINUSE
#define EADDRNOTAVAIL      __WASI_EADDRNOTAVAIL
#define ENETDOWN           __WASI_ENETDOWN
#define ENETUNREACH        __WASI_ENETUNREACH
#define ENETRESET          __WASI_ENETRESET
#define ECONNABORTED       __WASI_ECONNABORTED
#define ECONNRESET         __WASI_ECONNRESET
#define ENOBUFS            __WASI_ENOBUFS
#define EISCONN            __WASI_EISCONN
#define ENOTCONN           __WASI_ENOTCONN
#define ETIMEDOUT          __WASI_ETIMEDOUT
#define ECONNREFUSED       __WASI_ECONNREFUSED
#define EHOSTUNREACH       __WASI_EHOSTUNREACH
#define EALREADY           __WASI_EALREADY
#define EINPROGRESS        __WASI_EINPROGRESS
#define ESTALE             __WASI_ESTALE
#define EDQUOT             __WASI_EDQUOT
#define ECANCELED          __WASI_ECANCELED
#define EOWNERDEAD         __WASI_EOWNERDEAD
#define ENOTRECOVERABLE    __WASI_ENOTRECOVERABLE

// Codes without a wasi equivalent, make sure they start
// above the wasi ones, which are dense [1,76].
// Also try to fit the codes in a single byte signed wasm SLEB.

#define ENOSTR          100
#define EBFONT          101
#define EBADSLT         102
#define EBADRQC         103
#define ENOANO          104
#define ENOTBLK         105
#define ECHRNG          106
#define EL3HLT          107
#define EL3RST          108
#define ELNRNG          109
#define EUNATCH         110
#define ENOCSI          111
#define EL2HLT          112
#define EBADE           113
#define EBADR           114
#define EXFULL          115
#define ENODATA         116
#define ETIME           117
#define ENOSR           118
#define ENONET          119
#define ENOPKG          120
#define EREMOTE         121
#define EADV            122
#define ESRMNT          123
#define ECOMM           124
#define EDOTDOT         125
#define ENOTUNIQ        126
#define EBADFD          127
#define EREMCHG         128
#define ELIBACC         129
#define ELIBBAD         130
#define ELIBSCN         131
#define ELIBMAX         132
#define ELIBEXEC        133
#define ERESTART        134
#define ESTRPIPE        135
#define EUSERS          136
#define ESOCKTNOSUPPORT 137
#define EOPNOTSUPP      138
#define EPFNOSUPPORT    139
#define ESHUTDOWN       140
#define ETOOMANYREFS    141
#define EHOSTDOWN       142
#define EUCLEAN         143
#define ENOTNAM         144
#define ENAVAIL         145
#define EISNAM          146
#define EREMOTEIO       147
#define ENOMEDIUM       148
#define EMEDIUMTYPE     149
#define ENOKEY          150
#define EKEYEXPIRED     151
#define EKEYREVOKED     152
#define EKEYREJECTED    153
#define ERFKILL         154
#define EHWPOISON       155

// codes which musl defines as aliases

#define EWOULDBLOCK     EAGAIN
#define EDEADLOCK       EDEADLK
#define ENOTSUP         EOPNOTSUPP
