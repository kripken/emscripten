/**
 * @license
 * Copyright 2010 The Emscripten Authors
 * SPDX-License-Identifier: MIT
 */

var LibraryExceptions = {
  __exception_last: '0',
  __exception_caught: ' []',

  __ExceptionInfoAttrs: {
    TYPE_OFFSET: 0,
    DESTRUCTOR_OFFSET: Runtime.POINTER_SIZE,
    REFCOUNT_OFFSET: Runtime.POINTER_SIZE * 2,
    CAUGHT_OFFSET: Runtime.POINTER_SIZE * 2 + 4,
    RETHROWN_OFFSET: Runtime.POINTER_SIZE * 2 + 5,

    // Should be multiple of allocation alignment.
    SIZE: alignMemory(Runtime.POINTER_SIZE * 2 + 6, 16)
  },

  ExceptionInfo__deps: ['__ExceptionInfoAttrs'],
  ExceptionInfo: function(excPtr) {
    this.excPtr = excPtr;
    this.ptr = excPtr - ___ExceptionInfoAttrs.SIZE;

    this.set_type = function(type) {
      {{{ makeSetValue('this.ptr', '___ExceptionInfoAttrs.TYPE_OFFSET', 'type', '*') }}};
    }

    this.get_type = function() {
      return {{{ makeGetValue('this.ptr', '___ExceptionInfoAttrs.TYPE_OFFSET', '*') }}};
    }

    this.set_destructor = function(destructor) {
      {{{ makeSetValue('this.ptr', '___ExceptionInfoAttrs.DESTRUCTOR_OFFSET', 'destructor', '*') }}};
    }

    this.get_destructor = function() {
      return {{{ makeGetValue('this.ptr', '___ExceptionInfoAttrs.DESTRUCTOR_OFFSET', '*') }}};
    }

    this.set_refcount = function(refcount) {
      {{{ makeSetValue('this.ptr', '___ExceptionInfoAttrs.REFCOUNT_OFFSET', 'refcount', '*') }}};
    }

    this.set_caught = function (caught) {
      caught = caught ? 1 : 0;
      {{{ makeSetValue('this.ptr', '___ExceptionInfoAttrs.CAUGHT_OFFSET', 'caught', 'i8') }}};
    }

    this.get_caught = function () {
      return {{{ makeGetValue('this.ptr', '___ExceptionInfoAttrs.CAUGHT_OFFSET', 'i8') }}} != 0;
    }

    this.set_rethrown = function (rethrown) {
      rethrown = rethrown ? 1 : 0;
      {{{ makeSetValue('this.ptr', '___ExceptionInfoAttrs.RETHROWN_OFFSET', 'rethrown', 'i8') }}};
    }

    this.get_rethrown = function () {
      return {{{ makeGetValue('this.ptr', '___ExceptionInfoAttrs.RETHROWN_OFFSET', 'i8') }}} != 0;
    }

    this.init = function(type, destructor) {
      this.set_type(type);
      this.set_destructor(destructor);
      this.set_refcount(0);
      this.set_caught(false);
      this.set_rethrown(false);
    }

    this.add_ref = function() {
      Atomics.add(HEAP32, (this.ptr + ___ExceptionInfoAttrs.REFCOUNT_OFFSET) >> 2, 1);
    }

    // Returns true if last reference released.
    this.release_ref = function() {
      var prev = Atomics.sub(HEAP32, (this.ptr + ___ExceptionInfoAttrs.REFCOUNT_OFFSET) >> 2, 1);
      if (prev === 0) {
        err('Exception reference counter underflow');
      }
      return prev === 1;
    }
  },

    __exception_addRef: function(info) {
#if EXCEPTION_DEBUG
      err('addref ' + info.excPtr);
#endif
      info.add_ref();
    },

  __exception_decRef__deps: ['__cxa_free_exception'
#if EXCEPTION_DEBUG
    , '__exception_last', '__exception_caught'
#endif
  ],
  __exception_decRef: function(info) {
#if EXCEPTION_DEBUG
    err('decref ' + info.excPtr);
#endif
    // A rethrown exception can reach refcount 0; it must not be discarded
    // Its next handler will clear the rethrown flag and addRef it, prior to
    // final decRef and destruction here
    if (info.release_ref() && !info.rethrown) {
      var destructor = info.get_destructor();
      if (destructor) {
#if WASM_BACKEND == 0
        Module['dynCall_vi'](destructor, info.excPtr);
#else
        // In Wasm, destructors return 'this' as in ARM
        Module['dynCall_ii'](destructor, info.excPtr);
#endif
      }
      ___cxa_free_exception(info.ptr);
#if EXCEPTION_DEBUG
      err('decref freeing exception ' + [info.excPtr, ___exception_last, 'stack', ___exception_caught]);
#endif
    }
  },

  // Exceptions
  __cxa_allocate_exception__deps: ['__ExceptionInfoAttrs'],
  __cxa_allocate_exception: function(size) {
    // Exception object is prepended by exception info block
    return _malloc(size + ___ExceptionInfoAttrs.SIZE) + ___ExceptionInfoAttrs.SIZE;
  },

  __cxa_allocate_exception__deps: ['ExceptionInfo'],
  __cxa_free_exception: function(ptr) {
#if ABORTING_MALLOC || ASSERTIONS
    try {
#endif
      return _free(new _ExceptionInfo(ptr).ptr);
#if ABORTING_MALLOC || ASSERTIONS
    } catch(e) {
#if ASSERTIONS
      err('exception during cxa_free_exception: ' + e);
#endif
    }
#endif
  },

  __cxa_increment_exception_refcount__deps: ['__exception_addRef', 'ExceptionInfo'],
  __cxa_increment_exception_refcount: function(ptr) {
    if (!ptr) return;
    ___exception_addRef(new _ExceptionInfo(ptr));
  },

  __cxa_decrement_exception_refcount__deps: ['__exception_decRef', 'ExceptionInfo'],
  __cxa_decrement_exception_refcount: function(ptr) {
    if (!ptr) return;
    ___exception_decRef(new _ExceptionInfo(ptr));
  },

  // Here, we throw an exception after recording a couple of values that we need to remember
  // We also remember that it was the last exception thrown as we need to know that later.
  __cxa_throw__sig: 'viii',
  __cxa_throw__deps: ['__exception_infos', '__exception_last', '_ZSt18uncaught_exceptionv', 'ExceptionInfo'],
  __cxa_throw: function(ptr, type, destructor) {
#if EXCEPTION_DEBUG
    err('Compiled code throwing an exception, ' + [ptr,type,destructor]);
#endif
    var info = new _ExceptionInfo(ptr);
    info.init(type, destructor);
    ___exception_last = ptr;
    if (!("uncaught_exception" in __ZSt18uncaught_exceptionv)) {
      __ZSt18uncaught_exceptionv.uncaught_exceptions = 1;
    } else {
      __ZSt18uncaught_exceptionv.uncaught_exceptions++;
    }
    {{{ makeThrow('ptr') }}}
  },

  // This exception will be caught twice, but while begin_catch runs twice,
  // we early-exit from end_catch when the exception has been rethrown, so
  // pop that here from the caught exceptions.
  __cxa_rethrow__deps: ['__exception_caught', 'ExceptionInfo', '__exception_last'],
  __cxa_rethrow: function() {
    var ptr = ___exception_caught.pop();
    var info = new _ExceptionInfo(ptr);
    if (!info.get_rethrown()) {
      // Only pop if the corresponding push was through rethrow_primary_exception
      ___exception_caught.push(ptr);
      info.set_rethrown(true);
    }
#if EXCEPTION_DEBUG
    err('Compiled code RE-throwing an exception, popped ' + [ptr, ___exception_last, 'stack', ___exception_caught]);
#endif
    ___exception_last = ptr;
    {{{ makeThrow('ptr') }}}
  },

  llvm_eh_exception__deps: ['__exception_last'],
  llvm_eh_exception: function() {
    return ___exception_last;
  },

  llvm_eh_selector__jsargs: true,
  llvm_eh_selector__deps: ['__exception_last'],
  llvm_eh_selector: function(unused_exception_value, personality/*, varargs*/) {
    var type = ___exception_last;
    for (var i = 2; i < arguments.length; i++) {
      if (arguments[i] ==  type) return type;
    }
    return 0;
  },

  llvm_eh_typeid_for: function(type) {
    return type;
  },

  __cxa_begin_catch__deps: ['ExceptionInfo', '__exception_caught', '__exception_addRef', '_ZSt18uncaught_exceptionv'],
  __cxa_begin_catch: function(ptr) {
    var info = new _ExceptionInfo(ptr);
    if (!info.get_caught()) {
      info.set_caught(true);
      __ZSt18uncaught_exceptionv.uncaught_exceptions--;
    }
    info.set_rethrown(false);
    ___exception_caught.push(ptr);
#if EXCEPTION_DEBUG
    err('cxa_begin_catch ' + [ptr, 'stack', ___exception_caught]);
#endif
    ___exception_addRef(info);
    return ptr;
  },

  // We're done with a catch. Now, we can run the destructor if there is one
  // and free the exception. Note that if the dynCall on the destructor fails
  // due to calling apply on undefined, that means that the destructor is
  // an invalid index into the FUNCTION_TABLE, so something has gone wrong.
  __cxa_end_catch__deps: ['__exception_caught', '__exception_last', '__exception_decRef', 'ExceptionInfo'
#if WASM_BACKEND == 0
  , 'setThrew'
#endif
  ],
  __cxa_end_catch: function() {
    // Clear state flag.
    _setThrew(0);
    // Call destructor if one is registered then clear it.
    var ptr = ___exception_caught.pop();
#if EXCEPTION_DEBUG
    err('cxa_end_catch popped ' + [ptr, ___exception_last, 'stack', ___exception_caught]);
#endif
    if (ptr) {
      ___exception_decRef(new _ExceptionInfo(ptr));
      ___exception_last = 0; // XXX in decRef?
    }
  },
  __cxa_get_exception_ptr: function(ptr) {
#if EXCEPTION_DEBUG
    err('cxa_get_exception_ptr ' + ptr);
#endif
    return ptr;
  },

  _ZSt18uncaught_exceptionv: function() { // std::uncaught_exception()
    return __ZSt18uncaught_exceptionv.uncaught_exceptions > 0;
  },

  __cxa_uncaught_exceptions__deps: ['_ZSt18uncaught_exceptionv'],
  __cxa_uncaught_exceptions: function() {
    return __ZSt18uncaught_exceptionv.uncaught_exceptions;
  },

  __cxa_call_unexpected: function(exception) {
    err('Unexpected exception thrown, this is not properly supported - aborting');
#if !MINIMAL_RUNTIME
    ABORT = true;
#endif
    throw exception;
  },

  __cxa_current_primary_exception__deps: ['__exception_caught', '__exception_addRef', 'ExceptionInfo'],
  __cxa_current_primary_exception: function() {
    var ret = ___exception_caught[___exception_caught.length-1] || 0;
    if (ret) ___exception_addRef(new _ExceptionInfo(ret));
    return ret;
  },

  __cxa_rethrow_primary_exception__deps: ['ExceptionInfo', '__exception_caught', '__cxa_rethrow'],
  __cxa_rethrow_primary_exception: function(ptr) {
    if (!ptr) return;
    var info = new _ExceptionInfo(ptr);
    ___exception_caught.push(ptr);
    info.set_rethrown(true);
    ___cxa_rethrow();
  },

  // Finds a suitable catch clause for when an exception is thrown.
  // In normal compilers, this functionality is handled by the C++
  // 'personality' routine. This is passed a fairly complex structure
  // relating to the context of the exception and makes judgements
  // about how to handle it. Some of it is about matching a suitable
  // catch clause, and some of it is about unwinding. We already handle
  // unwinding using 'if' blocks around each function, so the remaining
  // functionality boils down to picking a suitable 'catch' block.
  // We'll do that here, instead, to keep things simpler.

  __cxa_find_matching_catch__deps: ['__exception_last', 'ExceptionInfo', '__resumeException'],
  __cxa_find_matching_catch: function() {
    var thrown = ___exception_last;
    if (!thrown) {
      // just pass through the null ptr
      {{{ makeStructuralReturn([0, 0]) }}};
    }
    var info = new _ExceptionInfo(thrown);
    var thrownType = info.get_type();
    if (!thrownType) {
      // just pass through the thrown ptr
      {{{ makeStructuralReturn(['thrown', 0]) }}};
    }
    var typeArray = Array.prototype.slice.call(arguments);

    // can_catch receives a **, add indirection
#if EXCEPTION_DEBUG
    out("can_catch on " + [thrown]);
#endif
#if DISABLE_EXCEPTION_CATCHING == 1
    var thrownBuf = 0;
#else
    var thrownBuf = {{{ makeStaticAlloc(4) }}};
#endif
    {{{ makeSetValue('thrownBuf', '0', 'thrown', '*') }}};
    // The different catch blocks are denoted by different types.
    // Due to inheritance, those types may not precisely match the
    // type of the thrown object. Find one which matches, and
    // return the type of the catch block which should be called.
    for (var i = 0; i < typeArray.length; i++) {
      var caughtType = typeArray[i];
      if (caughtType === 0 || caughtType === thrownType) {
        // Catch all clause matched or exactly the same type is caught
        break;
      }
      if ({{{ exportedAsmFunc('___cxa_can_catch') }}}(caughtType, thrownType, thrownBuf)) {
        var adjusted = {{{ makeGetValue('thrownBuf', '0', '*') }}};
        if (thrown !== adjusted) {
          err('Exception pointer adjusted')
        }
#if EXCEPTION_DEBUG
        out("  can_catch found " + [thrown, caughtType]);
#endif
        {{{ makeStructuralReturn(['thrown', 'caughtType']) }}};
      }
    }
    {{{ makeStructuralReturn(['thrown', 'thrownType']) }}};
  },

  __resumeException__deps: [function() { '__exception_last', Functions.libraryFunctions['___resumeException'] = 1 }], // will be called directly from compiled code
  __resumeException: function(ptr) {
#if EXCEPTION_DEBUG
    out("Resuming exception " + [ptr, ___exception_last]);
#endif
    if (!___exception_last) { ___exception_last = ptr; }
    {{{ makeThrow('ptr') }}}
  },
};

// In LLVM, exceptions generate a set of functions of form __cxa_find_matching_catch_1(), __cxa_find_matching_catch_2(), etc.
// where the number specifies the number of arguments. In Emscripten, route all these to a single function '__cxa_find_matching_catch'
// that variadically processes all of these functions using JS 'arguments' object.
addCxaCatch = function(n) {
  LibraryManager.library['__cxa_find_matching_catch_' + n] = LibraryExceptions['__cxa_find_matching_catch'];
  LibraryManager.library['__cxa_find_matching_catch_' + n + '__sig'] = new Array(n + 2).join('i');
};

mergeInto(LibraryManager.library, LibraryExceptions);
