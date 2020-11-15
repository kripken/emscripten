onmessage = function(d) {
	// The first message sent to the Worker is always the bootstrap message.
	// Drop this message listener, it served its purpose of bootstrapping
	// the Wasm Module load, and is no longer needed. Let user code register
	// any desired message handlers from now on.
	onmessage = null;
	d = d.data;
#if !MODULARIZE
	self.{{{ EXPORT_NAME }}} = d;
#endif
	importScripts(d.js);
#if MODULARIZE
	{{{ EXPORT_NAME }}}(d);
#endif
	// Drop now unneeded references to from the Module object in this Worker,
	// these are not needed anymore.
	d.wasm = d.mem = d.js = 0;
}
