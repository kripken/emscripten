mergeInto(LibraryManager.library, {
	wasm_workers: {},
	wasm_workers_id: 1,

	_wasm_worker_delayedMessageQueue: [],

	_wasm_worker_appendToQueue: function(e) {
		__wasm_worker_delayedMessageQueue.push(e);
	},

	_wasm_worker_runPostMessage: function(e) {
		var data = e.data, wasmCall = data['_wsc'];
		if (wasmCall) {
			var func = wasmTable.get(wasmCall);
			if (data['a'] === 0) func();
			else if (data['a'] === 1) func(data['x']);
			else if (data['a'] === 2) func(data['x'], data['y']);
			else if (data['a'] === 3) func(data['x'], data['y'], data['z']);
			else func.apply(null, data['x']);
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
		var stackTop = Module["sb"] + Module["sz"];
#if ASSERTIONS
		assert(stackTop % 16 == 0);
		assert(Module["sb"] % 16 == 0);
#endif
		// TODO: Fuse these to the same function "emscripten_establish_stack".
		_emscripten_stack_set_limits(stackTop, Module["sb"]);
		stackRestore(stackTop);
#if STACK_OVERFLOW_CHECK >= 2
	    ___set_stack_limits(_emscripten_stack_get_base(), _emscripten_stack_get_end());
#endif

	    removeEventListener('message', __wasm_worker_appendToQueue);
	    __wasm_worker_flushDelayedMessages();
	    addEventListener('message', __wasm_worker_runPostMessage);
	},

#if MODULARIZE
	_wasmWorkerBlobUrl: "URL.createObjectURL(new Blob(['onmessage=function(d){onmessage=null;d=d.data;importScripts(d.js);{{{ EXPORT_NAME }}}(d);d.wasm=d.mem=d.js=0;}'], {type: 'application/javascript'}))",
#else
	_wasmWorkerBlobUrl: "URL.createObjectURL(new Blob(['onmessage=function(d){onmessage=null;d=d.data;self.Module=d;importScripts(d.js);d.wasm=d.mem=d.js=0;}'], {type: 'application/javascript'}))",
#endif

	_emscripten_create_wasm_worker__deps: ['wasm_workers', 'wasm_workers_id', '_wasm_worker_appendToQueue', '_wasm_worker_runPostMessage', '_wasm_worker_flushDelayedMessages'
#if USE_WASM_WORKERS == 2
		, '_wasmWorkerBlobUrl'
#endif
	],
	_emscripten_create_wasm_worker__postset: 'if (ENVIRONMENT_IS_WASM_WORKER) {\n'
		+ '_wasm_workers[0] = this;\n'
		+ 'addEventListener("message", __wasm_worker_appendToQueue);\n'
		+ '}\n',
	_emscripten_create_wasm_worker: function(stackLowestAddress, stackSize) {
#if ASSERTIONS
		assert(stackLowestAddress % 16 == 0);
		assert(stackSize % 16 == 0);
#endif
#if USE_WASM_WORKERS == 2
		var worker = new Worker(__wasmWorkerBlobUrl);
#else
		var worker = new Worker(Module['wasmWorker']);
#endif
		// Craft the Module object for the Wasm Worker scope:
		worker.postMessage({
			'$ww': 1, // Signal that this Worker will be in a Wasm Worker scope, and not the main browser thread scope.
			'wasm': Module['wasm'],
			'js': Module['js'],
			'mem': wasmMemory,
			'sb': stackLowestAddress,
			'sz': stackSize,
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
	emscripten_terminate_all_wasm_workers: function() {
		_wasm_workers.forEach((worker) => {
			worker.terminate();
		});
		_wasm_workers = {};
	},
	emscripten_current_thread_is_wasm_worker: function() {
#if USE_WASM_WORKERS
		return ENVIRONMENT_IS_WASM_WORKER;
#else
		// implicit return 0;
#endif
	},
	emscripten_wasm_worker_post_function_v__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_v__sig: 'vii',
	emscripten_wasm_worker_post_function_v: function(id, funcPtr) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': 0,
		});
	},
	emscripten_wasm_worker_post_function_1__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_1__sig: 'viid',
	emscripten_wasm_worker_post_function_1: function(id, funcPtr, arg0) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': 1,
			'x': arg0
		});
	},
	emscripten_wasm_worker_post_function_vi: 'emscripten_wasm_worker_post_function_1',
	emscripten_wasm_worker_post_function_vd: 'emscripten_wasm_worker_post_function_1',

	emscripten_wasm_worker_post_function_2__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_2__sig: 'viidd',
	emscripten_wasm_worker_post_function_2: function(id, funcPtr, arg0, arg1) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': 2,
			'x': arg0,
			'y': arg1
		});
	},
	emscripten_wasm_worker_post_function_vii: 'emscripten_wasm_worker_post_function_2',
	emscripten_wasm_worker_post_function_vdi: 'emscripten_wasm_worker_post_function_2',

	emscripten_wasm_worker_post_function_3__deps: ['$dynCall'],
	emscripten_wasm_worker_post_function_3__sig: 'viiddd',
	emscripten_wasm_worker_post_function_3: function(id, funcPtr, arg0, arg1, arg2) {
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': 3,
			'x': arg0,
			'y': arg1,
			'z': arg2
		});
	},
	emscripten_wasm_worker_post_function_viii: 'emscripten_wasm_worker_post_function_3',
	emscripten_wasm_worker_post_function_vddd: 'emscripten_wasm_worker_post_function_3',

	emscripten_wasm_worker_post_function_sig__deps: ['$readAsmConstArgs'],
	emscripten_wasm_worker_post_function_sig: function(id, funcPtr, sigPtr, varargs) {
#if ASSERTIONS
		assert(id >= 0);
		assert(funcPtr);
		assert(sigPtr);
		assert(UTF8ToString(sigPtr)[0] == 'v'); // User must remember to specify the return value as void.
		assert(varargs);
#endif
		var worker = _wasm_workers[id];
		worker.postMessage({
			'_wsc': funcPtr, // "WaSm Call"
			'a': -1,
			'x': readAsmConstArgs(sigPtr, varargs)
		});
	},

	_emscripten_atomics_wait_states: "['ok', 'not-equal', 'timed-out']",

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
		function dispatch(val, ret) {
			setTimeout(() => {
				{{{ makeDynCall('viiii', 'asyncWaitFinished') }}}(lock, val, /*waitResult=*/ret, userData);
			}, 0);
		}
		function tryAcquireLock() {
			do {
				var val = Atomics.compareExchange(HEAPU32, lock >> 2, 0/*zero represents lock being free*/, 1/*one represents lock being acquired*/);
				if (!val) return dispatch(0, 0/*'ok'*/);
				var wait = Atomics['waitAsync'](HEAPU32, lock >> 2, val, maxWaitMilliseconds);
			} while(wait.value === 'not-equal');
#if ASSERTIONS
			assert(wait.value === 'timed-out');
#endif
			if (wait.value) dispatch(val, 2/*'timed-out'*/);
			else wait.then(tryAcquireLock);
		}
		tryAcquireLock();
	},

	// The dependency emscripten_semaphore_async_acquire -> emscripten_atomics_async_wait is artificial
	// so that we get the waitAsync polyfill emitted if code calls emscripten_semaphore_async_acquire() but
	// not emscripten_atomics_async_wait().
	emscripten_semaphore_async_acquire__deps: ['emscripten_atomics_async_wait'],
	emscripten_semaphore_async_acquire: function(sem, num, asyncWaitFinished, userData, maxWaitMilliseconds) {
		function dispatch(idx, ret) {
			setTimeout(() => {
				{{{ makeDynCall('viiii', 'asyncWaitFinished') }}}(sem, /*val=*/idx, /*waitResult=*/ret, userData);
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

			if (wait.value) dispatch(-1/*idx*/, 2/*'timed-out'*/);
			else wait.then(tryAcquireSemaphore);
		}
		tryAcquireSemaphore();
	}
});
