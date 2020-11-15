/*
 * Copyright 2019 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

// This file contains support machinery to implement multithreading support for asm.js specifically.
// Split out to a separate file so that its contents can be easily left out when only doing Wasm
// multithreading.

#include <emscripten.h>
#include <emscripten/threading.h>
#include <pthread.h>

extern void __wasm_init_tls(void *memory);
void *emscripten_builtin_memalign(size_t align, size_t size);
void emscripten_builtin_free(void *memory);

// Note that ASan constructor priority is 50, and we must be higher.
__attribute__((constructor(49)))
void EMSCRIPTEN_KEEPALIVE emscripten_tls_init(void) {
  size_t tls_size = __builtin_wasm_tls_size();
  size_t tls_align = __builtin_wasm_tls_align();
  if (tls_size) {
    void *tls_block = emscripten_builtin_memalign(tls_align, tls_size);
    __wasm_init_tls(tls_block);
    pthread_cleanup_push(emscripten_builtin_free, tls_block);
  }
}
