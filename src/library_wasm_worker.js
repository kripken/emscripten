mergeInto(LibraryManager.library, {
	wasm_workers: {},
	wasm_workers__postset: 'if (ENVIRONMENT_IS_WASM_WORKER) _wasm_workers[0] = this;',
	wasm_workers_id: 1,

	_wasm_worker_delayedMessageQueue: [],

	_wasm_worker_appendToQueue: function(e) {
		__wasm_worker_delayedMessageQueue.push(e);
	},

	_wasm_worker_runPostMessage: function(e) {
		var d = e.data;
		if (d['_wsc'] /* "WaSm Call" */) {
			var f = wasmTable.get(d['_wsc']);
			if (d['a'] === 0) f();
			else if (d['a'] === 1) f(d['x']);
			else if (d['a'] === 2) f(d['x'], d['y']);
			else if (d['a'] === 3) f(d['x'], d['y'], d['z']);
			else dynCall(d['sig'], f, d['x']);
		}
	},

	_wasm_worker_flushDelayedMessages__deps: ['_wasm_worker_delayedMessageQueue', '_wasm_worker_runPostMessage'],
	_wasm_worker_flushDelayedMessages: function() {
		__wasm_worker_delayedMessageQueue.forEach(__wasm_worker_runPostMessage);
		__wasm_worker_delayedMessageQueue = null;
	},

	// src/postamble_minimal.js brings this in to the build, and calls this function.
	_wasm_worker_initializeRuntime__deps: ['_wasm_worker_flushDelayedMessages'],
	_wasm_worker_initializeRuntime: function() {
		var stackTop = Module["stackBase"] + Module["stackSize"];
#if ASSERTIONS
		assert(stackTop % 16 == 0);
		assert(Module["stackBase"] % 16 == 0);
#endif
		_emscripten_stack_set_limits(stackTop, Module["stackBase"]);
		stackRestore(stackTop);
#if STACK_OVERFLOW_CHECK >= 2
	    ___set_stack_limits(_emscripten_stack_get_base(), _emscripten_stack_get_end());
#endif

	    removeEventListener('message', __wasm_worker_appendToQueue);
	    __wasm_worker_flushDelayedMessages();
	    addEventListener('message', __wasm_worker_runPostMessage);
	},

	emscripten_create_wasm_worker__deps: ['wasm_workers', 'wasm_workers_id', '_wasm_worker_appendToQueue', '_wasm_worker_runPostMessage', '_wasm_worker_flushDelayedMessages'],
	emscripten_create_wasm_worker__postset: 'if (ENVIRONMENT_IS_WASM_WORKER) addEventListener("message", __wasm_worker_appendToQueue);',
	emscripten_create_wasm_worker: function(stackLowestAddress, stackSize) {
		var worker = new Worker(Module['wasmWorker']);
		// Craft the Module object for the Wasm Worker scope:
		var stackBase = (stackLowestAddress + 15) & -16;
		worker.postMessage({
			'$ww': 1, // Signal that this Worker will be in a Wasm Worker scope, and not the main browser thread scope.
			'wasm': Module['wasm'],
			'js': Module['js'],
			'memory': wasmMemory,
			'stackBase': stackBase,
			'stackSize': (stackLowestAddress + stackSize - stackBase) & -16,
		});
		worker.addEventListener('message', __wasm_worker_runPostMessage);
		_wasm_workers[_wasm_workers_id] = worker;
		return _wasm_workers_id++;
	},
	emscripten_terminate_wasm_worker: function(id) {
		if (_wasm_workers[_wasm_workers_id]) {
			_wasm_workers[_wasm_workers_id].terminate();
			delete _wasm_workers[_wasm_workers_id];
		}
	},
	emscripten_wasm_worker_post_function_v__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_v: function(id, funcPtr) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': 0,
		});
	},
	emscripten_wasm_worker_post_function_vi__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_vi: function(id, funcPtr, arg0) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': 1,
			'x': arg0
		});
	},
	emscripten_wasm_worker_post_function_vii__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_vii: function(id, funcPtr, arg0, arg1) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': 2,
			'x': arg0,
			'y': arg1
		});
	},
	emscripten_wasm_worker_post_function_viii__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_viii: function(id, funcPtr, arg0, arg1, arg2) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': 3,
			'x': arg0,
			'y': arg1,
			'z': arg2
		});
	},
	emscripten_wasm_worker_post_function_varargs__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_varargs: function(id, funcPtr, sig, varargs) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': -1,
			'sig': 'v', /*todo craft varargs string*/
			'x': [/*todo extract varargs*/]
		});
	},

	_emscripten_atomics_wait_states: "['ok', 'timed-out', 'not-equal']",

	emscripten_atomics_wait__deps: ['_emscripten_atomics_wait_states'],
	emscripten_atomics_wait: function(addr, val, maxWaitMilliseconds) {
		return __emscripten_atomics_wait_states.indexOf(Atomics.wait(HEAP32, addr >> 2, val, maxWaitMilliseconds));
	},

	// Partially polyfill Atomics.asyncWait() if not available in the browser.
	// https://github.com/tc39/proposal-atomics-wait-async/blob/master/PROPOSAL.md
	// This polyfill performs polling with setTimeout() to observe a change in the target memory location.
	emscripten_atomics_async_wait__postset: "if (!Atomics['waitAsync']) { \n"+
"var __Atomics_waitAsyncAddresses = [/*[i32a, index, value, maxWaitMilliseconds, promiseResolve]*/];\n"+
"function __Atomics_pollWaitAsyncAddresses() {\n"+
"	var now = performance.now();\n"+
"	var l = __Atomics_waitAsyncAddresses.length;\n"+
"	for(var i = 0; i < l; ++i) {\n"+
"		var a = __Atomics_waitAsyncAddresses[i];\n"+
"		var expired = (now > a[3]);\n"+
"		var awoken = (Atomics.load(a[0], a[1]) != a[2]);\n"+
"		if (expired || awoken) {\n"+
"			__Atomics_waitAsyncAddresses[i--] = __Atomics_waitAsyncAddresses[--l];\n"+
"			__Atomics_waitAsyncAddresses.length = l;\n"+
"			a[4](awoken ? 'ok': 'timed-out');\n"+
"		}\n"+
"	}\n"+
"	if (l) {\n"+
"		// If we still have addresses to wait, loop the timeout handler to continue polling.\n"+
"		setTimeout(__Atomics_pollWaitAsyncAddresses, 10);\n"+
"	}\n"+
"}\n"+
"Atomics['waitAsync'] = function(i32a, index, value, maxWaitMilliseconds) {\n"+
"	var val = Atomics.load(i32a, index);\n"+
"	if (val != value) return { async: false, value: 'not-equal' };\n"+
"	if (maxWaitMilliseconds <= 0) return { async: false, value: 'timed-out' };\n"+
"	var maxWaitMilliseconds = performance.now() + (maxWaitMilliseconds || Infinity);\n"+
"	var promiseResolve;\n"+
"	var promise = new Promise((resolve) => { promiseResolve = resolve; });\n"+
"	if (!__Atomics_waitAsyncAddresses.length) setTimeout(__Atomics_pollWaitAsyncAddresses, 10);\n"+
"	__Atomics_waitAsyncAddresses.push([i32a, index, value, maxWaitMilliseconds, promiseResolve]);\n"+
"	return promise;\n"+
"};\n"+
"}",
	emscripten_atomics_async_wait__deps: ['_emscripten_atomics_wait_states'],
	emscripten_atomics_async_wait: function(addr, val, asyncWaitFinished, userData, maxWaitMilliseconds) {
		var wait = Atomics['waitAsync'](HEAPU32, addr >> 2, val, maxWaitMilliseconds);
		if (wait.value) return __emscripten_atomics_wait_states.indexOf(wait.value);
		wait.then((value) => {
			{{{ makeDynCall('viiii', 'asyncWaitFinished') }}}(addr, val, __emscripten_atomics_wait_states.indexOf(value), userData);
		});
		// Implicit return 0 /*ATOMICS_WAIT_OK*/;
	},

	emscripten_atomics_notify: function(addr, count) {
		return Atomics.notify(HEAP32, addr >> 2, count);
	},

	emscripten_navigator_hardware_concurrency: function() {
#if ENVIRONMENT_MAY_BE_NODE
		if (ENVIRONMENT_IS_NODE) return require('os').cpus().length;
#endif
		return navigator['hardwareConcurrency'];
	},

	emscripten_atomics_is_lock_free: function(width) {
		return Atomics.isLockFree(width);
	},

	emscripten_lock_async_acquire: function(lock, asyncWaitFinished, userData, maxWaitMilliseconds) {
		function dispatch(ret) {
			setTimeout(() => {
				{{{ makeDynCall('viiii', 'asyncWaitFinished') }}}(lock, ret, ret, userData);
			}, 0);
		}
		function tryAcquireLock() {
			do {
				var val = Atomics.compareExchange(HEAPU32, lock >> 2, 0/*zero represents lock being free*/, 1/*one represents lock being acquired*/);
				if (!val) return dispatch(0/*'ok'*/);
				var wait = Atomics['waitAsync'](HEAPU32, lock >> 2, val, maxWaitMilliseconds);
			} while(wait.value === 'not-equal');

			if (wait.value) dispatch(1/*'timed-out'*/);
			else wait.then(tryAcquireLock);
		}
		tryAcquireLock();
	},

	emscripten_semaphore_async_acquire__deps: ['emscripten_atomics_async_wait'],
	emscripten_semaphore_async_acquire: function(sem, num, asyncWaitFinished, userData, maxWaitMilliseconds) {
		function dispatch(idx, ret) {
			setTimeout(() => {
				{{{ makeDynCall('viiii', 'asyncWaitFinished') }}}(sem, idx, ret, userData);
			}, 0);
		}
		function tryAcquireSemaphore() {
			var val = num;
			do {
				var ret = Atomics.compareExchange(HEAPU32, sem >> 2,
				                                  val, /* We expect this many semaphore resoures to be available*/
				                                  val - num /* Acquire 'num' of them */);
				if (ret == val) return dispatch(ret/*index of resource acquired*/, 0/*'ok'*/);
				val = ret;
				var wait = Atomics['waitAsync'](HEAPU32, sem >> 2, ret, maxWaitMilliseconds);
			} while(wait.value === 'not-equal');

			if (wait.value) dispatch(-1/*idx*/, 1/*'timed-out'*/);			
			else wait.then(tryAcquireSemaphore);
		}
		tryAcquireSemaphore();
	}
});
