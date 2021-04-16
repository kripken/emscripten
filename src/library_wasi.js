/**
 * @license
 * Copyright 2019 The Emscripten Authors
 * SPDX-License-Identifier: MIT
 */

var WasiLibrary = {
  proc_exit__deps: ['exit'],
  proc_exit__sig: 'vi',
  proc_exit: function(code) {
    _exit(code);
  },

  $getEnvStrings__deps: ['$ENV', '$getExecutableName'],
  $getEnvStrings: function() {
    if (!getEnvStrings.strings) {
      // Default values.
#if !DETERMINISTIC
      // Browser language detection #8751
      var lang = ((typeof navigator === 'object' && navigator.languages && navigator.languages[0]) || 'C').replace('-', '_') + '.UTF-8';
#else
      // Deterministic language detection, ignore the browser's language.
      var lang = 'C.UTF-8';
#endif
      var env = {
        'USER': 'web_user',
        'LOGNAME': 'web_user',
        'PATH': '/',
        'PWD': '/',
        'HOME': '/home/web_user',
        'LANG': lang,
        '_': getExecutableName()
      };
      // Apply the user-provided values, if any.
      for (var x in ENV) {
        env[x] = ENV[x];
      }
      var strings = [];
      for (var x in env) {
        strings.push(x + '=' + env[x]);
      }
      getEnvStrings.strings = strings;
    }
    return getEnvStrings.strings;
  },

  environ_sizes_get__deps: ['$getEnvStrings'],
  environ_sizes_get__sig: 'iii',
  environ_sizes_get: function(penviron_count, penviron_buf_size) {
    var strings = getEnvStrings();
    {{{ makeSetValue('penviron_count', 0, 'strings.length', 'i32') }}};
    var bufSize = 0;
    strings.forEach(function(string) {
      bufSize += string.length + 1;
    });
    {{{ makeSetValue('penviron_buf_size', 0, 'bufSize', 'i32') }}};
    return 0;
  },

  environ_get__deps: ['$getEnvStrings'
#if MINIMAL_RUNTIME
    , '$writeAsciiToMemory'
#endif
  ],
  environ_get__sig: 'iii',
  environ_get: function(__environ, environ_buf) {
    var bufSize = 0;
    getEnvStrings().forEach(function(string, i) {
      var ptr = environ_buf + bufSize;
      {{{ makeSetValue('__environ', 'i * 4', 'ptr', 'i32') }}};
      writeAsciiToMemory(string, ptr);
      bufSize += string.length + 1;
    });
    return 0;
  },

  // In normal (non-standalone) mode arguments are passed direclty
  // to main, and the `mainArgs` global does not exist.
#if STANDALONE_WASM
  args_sizes_get__sig: 'iii',
  args_sizes_get: function(pargc, pargv_buf_size) {
#if MAIN_READS_PARAMS
    {{{ makeSetValue('pargc', 0, 'mainArgs.length', 'i32') }}};
    var bufSize = 0;
    mainArgs.forEach(function(arg) {
      bufSize += arg.length + 1;
    });
    {{{ makeSetValue('pargv_buf_size', 0, 'bufSize', 'i32') }}};
#else
    {{{ makeSetValue('pargc', 0, '0', 'i32') }}};
#endif
    return 0;
  },

  args_get__sig: 'iii',
#if MINIMAL_RUNTIME && MAIN_READS_PARAMS
  args_get__deps: ['$writeAsciiToMemory'],
#endif
  args_get: function(argv, argv_buf) {
#if MAIN_READS_PARAMS
    var bufSize = 0;
    mainArgs.forEach(function(arg, i) {
      var ptr = argv_buf + bufSize;
      {{{ makeSetValue('argv', 'i * 4', 'ptr', 'i32') }}};
      writeAsciiToMemory(arg, ptr);
      bufSize += arg.length + 1;
    });
#endif
    return 0;
  },
#endif

  $checkWasiClock: function(clock_id) {
    return clock_id == {{{ cDefine('__WASI_CLOCKID_REALTIME') }}} ||
           clock_id == {{{ cDefine('__WASI_CLOCKID_MONOTONIC') }}} ||
           clock_id == {{{ cDefine('__WASI_CLOCKID_PROCESS_CPUTIME_ID') }}} ||
           clock_id == {{{ cDefine('__WASI_CLOCKID_THREAD_CPUTIME_ID') }}};
  },

  // TODO: the i64 in the API here must be legalized for this JS code to run,
  // but the wasm file can't be legalized in standalone mode, which is where
  // this is needed. To get this code to be usable as a JS shim we need to
  // either wait for BigInt support or to legalize on the client.
  clock_time_get__sig: 'iiiii',
  clock_time_get__deps: ['emscripten_get_now', 'emscripten_get_now_is_monotonic', '$checkWasiClock'],
  clock_time_get: function(clk_id, {{{ defineI64Param('precision') }}}, ptime) {
    {{{ receiveI64ParamAsI32s('precision') }}}
    if (!checkWasiClock(clk_id)) {
      return {{{ cDefine('EINVAL') }}};
    }
    var now;
    // all wasi clocks but realtime are monotonic
    if (clk_id === {{{ cDefine('__WASI_CLOCKID_REALTIME') }}}) {
      now = Date.now();
    } else if (_emscripten_get_now_is_monotonic) {
      now = _emscripten_get_now();
    } else {
      return {{{ cDefine('ENOSYS') }}};
    }
    // "now" is in ms, and wasi times are in ns.
    var nsec = Math.round(now * 1000 * 1000);
    {{{ makeSetValue('ptime', 0, 'nsec >>> 0', 'i32') }}};
    {{{ makeSetValue('ptime', 4, '(nsec / Math.pow(2, 32)) >>> 0', 'i32') }}};
    return 0;
  },

  clock_res_get__sig: 'iii',
  clock_res_get__deps: ['emscripten_get_now', 'emscripten_get_now_res', 'emscripten_get_now_is_monotonic', '$checkWasiClock'],
  clock_res_get: function(clk_id, pres) {
    if (!checkWasiClock(clk_id)) {
      return {{{ cDefine('EINVAL') }}};
    }
    var nsec;
    // all wasi clocks but realtime are monotonic
    if (clk_id === {{{ cDefine('CLOCK_REALTIME') }}}) {
      nsec = 1000 * 1000; // educated guess that it's milliseconds
    } else if (_emscripten_get_now_is_monotonic) {
      nsec = _emscripten_get_now_res();
    } else {
      return {{{ cDefine('ENOSYS') }}};
    }
    {{{ makeSetValue('pres', 0, 'nsec >>> 0', 'i32') }}};
    {{{ makeSetValue('pres', 4, '(nsec / Math.pow(2, 32)) >>> 0', 'i32') }}};
    return 0;
  },

#if SYSCALLS_REQUIRE_FILESYSTEM == 0 && (!MINIMAL_RUNTIME || EXIT_RUNTIME)
  $flush_NO_FILESYSTEM: function() {
    // flush anything remaining in the buffers during shutdown
    if (typeof _fflush !== 'undefined') _fflush(0);
    var buffers = SYSCALLS.buffers;
    if (buffers[1].length) SYSCALLS.printChar(1, {{{ charCode("\n") }}});
    if (buffers[2].length) SYSCALLS.printChar(2, {{{ charCode("\n") }}});
  },
  fd_write__deps: ['$flush_NO_FILESYSTEM'],
  fd_write__postset: function() {
    addAtExit('flush_NO_FILESYSTEM()');
  },
#endif
  fd_write__sig: 'iiiii',
  fd_write: function(fd, iov, iovcnt, pnum) {
#if SYSCALLS_REQUIRE_FILESYSTEM
    var stream = SYSCALLS.getStreamFromFD(fd);
    var num = SYSCALLS.doWritev(stream, iov, iovcnt);
#else
    // hack to support printf in SYSCALLS_REQUIRE_FILESYSTEM=0
    var num = 0;
    for (var i = 0; i < iovcnt; i++) {
      var ptr = {{{ makeGetValue('iov', 'i*8', 'i32') }}};
      var len = {{{ makeGetValue('iov', 'i*8 + 4', 'i32') }}};
      for (var j = 0; j < len; j++) {
        SYSCALLS.printChar(fd, HEAPU8[ptr+j]);
      }
      num += len;
    }
#endif // SYSCALLS_REQUIRE_FILESYSTEM
    {{{ makeSetValue('pnum', 0, 'num', 'i32') }}}
    return 0;
  },

  fd_pwrite: function(fd, iov, iovcnt, {{{ defineI64Param('offset') }}}, pnum) {
    {{{ receiveI64ParamAsI32s('offset') }}}
    var stream = SYSCALLS.getStreamFromFD(fd)
#if ASSERTIONS
    assert(!offset_high, 'offsets over 2^32 not yet supported');
#endif
    var num = SYSCALLS.doWritev(stream, iov, iovcnt, offset_low);
    {{{ makeSetValue('pnum', 0, 'num', 'i32') }}}
    return 0;
  },

  fd_close__sig: 'ii',
  fd_close: function(fd) {
#if SYSCALLS_REQUIRE_FILESYSTEM
    var stream = SYSCALLS.getStreamFromFD(fd);
    FS.close(stream);
#else
#if PROXY_POSIX_SOCKETS
    // close() is a tricky function because it can be used to close both regular file descriptors
    // and POSIX network socket handles, hence an implementation would need to track for each
    // file descriptor which kind of item it is. To simplify, when using PROXY_POSIX_SOCKETS
    // option, use shutdown() to close a socket, and this function should behave like a no-op.
    warnOnce('To close sockets with PROXY_POSIX_SOCKETS bridge, prefer to use the function shutdown() that is proxied, instead of close()')
#else
#if ASSERTIONS
    abort('it should not be possible to operate on streams when !SYSCALLS_REQUIRE_FILESYSTEM');
#endif
#endif
#endif
    return 0;
  },

  fd_read__sig: 'iiiii',
  fd_read: function(fd, iov, iovcnt, pnum) {
    var stream = SYSCALLS.getStreamFromFD(fd);
    var num = SYSCALLS.doReadv(stream, iov, iovcnt);
    {{{ makeSetValue('pnum', 0, 'num', 'i32') }}}
    return 0;
  },

  fd_pread__sig: 'iiiiii',
  fd_pread: function(fd, iov, iovcnt, {{{ defineI64Param('offset') }}}, pnum) {
    {{{ receiveI64ParamAsI32s('offset') }}}
#if ASSERTIONS
    assert(!offset_high, 'offsets over 2^32 not yet supported');
#endif
    var stream = SYSCALLS.getStreamFromFD(fd)
    var num = SYSCALLS.doReadv(stream, iov, iovcnt, offset_low);
    {{{ makeSetValue('pnum', 0, 'num', 'i32') }}}
    return 0;
  },

  fd_seek: function(fd, {{{ defineI64Param('offset') }}}, whence, newOffset) {
    {{{ receiveI64ParamAsI32s('offset') }}}
    var stream = SYSCALLS.getStreamFromFD(fd);
    var HIGH_OFFSET = 0x100000000; // 2^32
    // use an unsigned operator on low and shift high by 32-bits
    var offset = offset_high * HIGH_OFFSET + (offset_low >>> 0);

    var DOUBLE_LIMIT = 0x20000000000000; // 2^53
    // we also check for equality since DOUBLE_LIMIT + 1 == DOUBLE_LIMIT
    if (offset <= -DOUBLE_LIMIT || offset >= DOUBLE_LIMIT) {
      return -{{{ cDefine('EOVERFLOW') }}};
    }

    FS.llseek(stream, offset, whence);
    {{{ makeSetValue('newOffset', '0', 'stream.position', 'i64') }}};
    if (stream.getdents && offset === 0 && whence === {{{ cDefine('SEEK_SET') }}}) stream.getdents = null; // reset readdir state
    return 0;
  },

  $wasiRightsToMuslOFlags: function(rights) {
#if SYSCALL_DEBUG
    err('wasiRightsToMuslOFlags: ' + rights);
#endif
    if ((rights & {{{ cDefine('__WASI_RIGHTS_FD_READ') }}}) && (rights & {{{ cDefine('__WASI_RIGHTS_FD_WRITE') }}})) {
      return {{{ cDefine('O_RDWR') }}};
    }
    if (rights & {{{ cDefine('__WASI_RIGHTS_FD_READ') }}}) {
      return {{{ cDefine('O_RDONLY') }}};
    }
    if (rights & {{{ cDefine('__WASI_RIGHTS_FD_WRITE') }}}) {
      return {{{ cDefine('O_WRONLY') }}};
    }
    throw new FS.ErrnoError({{{ cDefine('EINVAL') }}});
  },

  $wasiOFlagsToMuslOFlags: function(oflags) {
    var musl_oflags = 0;
    if (oflags & {{{ cDefine('__WASI_OFLAGS_CREAT') }}}) {
      musl_oflags |= {{{ cDefine('O_CREAT') }}};
    }
    if (oflags & {{{ cDefine('__WASI_OFLAGS_TRUNC') }}}) {
      musl_oflags |= {{{ cDefine('O_TRUNC') }}};
    }
    if (oflags & {{{ cDefine('__WASI_OFLAGS_DIRECTORY') }}}) {
      musl_oflags |= {{{ cDefine('O_DIRECTORY') }}};
    }
    if (oflags & {{{ cDefine('__WASI_OFLAGS_EXCL') }}}) {
      musl_oflags |= {{{ cDefine('O_EXCL') }}};
    }
    return musl_oflags;
  },

#if PURE_WASI
  // preopen maps open file descriptors to pathname
  $preopens: "{3: '/'}",

  path_open__sig: 'iiiiiiiiii',
  path_open__deps: ['$wasiRightsToMuslOFlags', '$wasiOFlagsToMuslOFlags', '$preopens'],
  path_open: function(fd, dirflags, path, path_len, oflags,
                      fs_rights_base, fs_rights_inherting,
                      fdflags, opened_fd) {
    if (!(fd in preopens)) {
      return {{{ cDefine('EBADF') }}};
    }
    var pathname = UTF8ToString(path, path_len);
    var musl_oflags = wasiRightsToMuslOFlags(Number(fs_rights_base));
#if SYSCALL_DEBUG
    err("oflags1: 0x" + musl_oflags.toString(16));
#endif
    musl_oflags |= wasiOFlagsToMuslOFlags(Number(oflags));
#if SYSCALL_DEBUG
    err("oflags2: 0x" + musl_oflags.toString(16));
#endif
    var stream = FS.open(pathname, musl_oflags);
    {{{ makeSetValue('opened_fd', '0', 'stream.fd', 'i32') }}};
    return 0;
  },

  fd_prestat_dir_name__deps: ['$preopens'],
  fd_prestat_dir_name__sig: 'iiii',
  fd_prestat_dir_name__nothrow: true,
  fd_prestat_dir_name: function(fd, path, path_len) {
    if (!(fd in preopens)) {
      return {{{ cDefine('EBADF') }}};
    }
    var preopen = preopens[fd];
    stringToUTF8Array(preopens, HEAP8, path, path_len)
    return 0;
  },

  fd_prestat_get__deps: ['$preopens'],
  fd_prestat_get__sig: 'iii',
  fd_prestat_get__nothrow: true,
  fd_prestat_get: function(fd, stat_buf) {
    if (!(fd in preopens)) {
      return {{{ cDefine('EBADF') }}};
    }
    var preopen = preopens[fd];
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_prestat_t.pr_type, cDefine('__WASI_PREOPENTYPE_DIR'), 'i8') }}};
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_prestat_t.u + C_STRUCTS.__wasi_prestat_dir_t.pr_name_len, 'preopen.length', 'i64') }}};
    return 0;
  },

  fd_fdstat_set_flags__sig: 'iii',
  fd_fdstat_set_flags: function(fd, flags) {
    // TODO(sbc): implement
    var stream = SYSCALLS.getStreamFromFD(fd);
    return 0;
  },

  fd_filestat_get__sig: 'iii',
  fd_filestat_get: function(fd, stat_buf) {
    // TODO(sbc): implement
    var stream = SYSCALLS.getStreamFromFD(fd);
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_filestat_t.dev, '0', 'i64') }}};
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_filestat_t.ino, '0', 'i64') }}};
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_filestat_t.filetype, '0', 'i8') }}};
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_filestat_t.nlink, '0', 'i64') }}};
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_filestat_t.size, '0', 'i64') }}};
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_filestat_t.atim, '0', 'i64') }}};
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_filestat_t.mtim, '0', 'i64') }}};
    {{{ makeSetValue('stat_buf', C_STRUCTS.__wasi_filestat_t.ctim, '0', 'i64') }}};
    return 0;
  },
#endif

#if PURE_WASI
  fd_fdstat_get__deps: ['$preopens'],
#endif
  fd_fdstat_get__sig: 'iii',
  fd_fdstat_get: function(fd, pbuf) {
    var rights_base = 0;
    var rights_inheriting = 0;
#if PURE_WASI
    if (fd in preopens) {
      var type = {{{ cDefine('__WASI_FILETYPE_DIRECTORY') }}};
      rights_base =  {{{ cDefine('__WASI_RIGHTS_PATH_CREATE_FILE') |
                             cDefine('__WASI_RIGHTS_PATH_OPEN') }}};
      rights_inheriting =  {{{ cDefine('__WASI_RIGHTS_FD_READ') |
                                   cDefine('__WASI_RIGHTS_FD_WRITE') }}}
    } else
#endif
    {
#if SYSCALLS_REQUIRE_FILESYSTEM
      var stream = SYSCALLS.getStreamFromFD(fd);
      // All character devices are terminals (other things a Linux system would
      // assume is a character device, like the mouse, we have special APIs for).
      var type = stream.tty ? {{{ cDefine('__WASI_FILETYPE_CHARACTER_DEVICE') }}} :
                 FS.isDir(stream.mode) ? {{{ cDefine('__WASI_FILETYPE_DIRECTORY') }}} :
                 FS.isLink(stream.mode) ? {{{ cDefine('__WASI_FILETYPE_SYMBOLIC_LINK') }}} :
                 {{{ cDefine('__WASI_FILETYPE_REGULAR_FILE') }}};
#else
      // hack to support printf in SYSCALLS_REQUIRE_FILESYSTEM=0
      var type = fd == 1 || fd == 2 ? {{{ cDefine('__WASI_FILETYPE_CHARACTER_DEVICE') }}} : abort();
      if (fd == 0) {
        rights_base = {{{ cDefine('__WASI_RIGHTS_FD_READ') }}};
      } else if (fd == 1 || fd == 2) {
        rights_base = {{{ cDefine('__WASI_RIGHTS_FD_WRITE') }}};
      }
#endif
    }
    {{{ makeSetValue('pbuf', C_STRUCTS.__wasi_fdstat_t.fs_filetype, 'type', 'i8') }}};
    {{{ makeSetValue('pbuf', C_STRUCTS.__wasi_fdstat_t.fs_flags, 0, 'i16') }}};
    {{{ makeSetValue('pbuf', C_STRUCTS.__wasi_fdstat_t.fs_rights_base, 'rights_base', 'i64') }}};
    {{{ makeSetValue('pbuf', C_STRUCTS.__wasi_fdstat_t.fs_rights_inheriting, 'rights_inheriting', 'i64') }}};
    return 0;
  },

  fd_sync__sig: 'ii',
  fd_sync: function(fd) {
    var stream = SYSCALLS.getStreamFromFD(fd);
#if ASYNCIFY
    return Asyncify.handleSleep(function(wakeUp) {
      var mount = stream.node.mount;
      if (!mount.type.syncfs) {
        // We write directly to the file system, so there's nothing to do here.
        wakeUp(0);
        return;
      }
      mount.type.syncfs(mount, false, function(err) {
        if (err) {
          wakeUp(function() { return {{{ cDefine('EIO') }}} });
          return;
        }
        wakeUp(0);
      });
    });
#else
    if (stream.stream_ops && stream.stream_ops.fsync) {
      return -stream.stream_ops.fsync(stream);
    }
    return 0; // we can't do anything synchronously; the in-memory FS is already synced to
#endif // ASYNCIFY
  },
};

for (var x in WasiLibrary) {
  wrapSyscallFunction(x, WasiLibrary, true);
}

mergeInto(LibraryManager.library, WasiLibrary);
