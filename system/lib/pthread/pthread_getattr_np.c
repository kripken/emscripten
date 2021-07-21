/*
 * Copyright 2021 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

#define _GNU_SOURCE
#include "pthread_impl.h"
#include <pthread.h>

int pthread_getattr_np(pthread_t t, pthread_attr_t* a) {
  *a = (pthread_attr_t){0};
  a->_a_detach = !!t->detached;
  a->_a_stackaddr = (uintptr_t)t->stack;
  a->_a_stacksize = t->stack_size - DEFAULT_STACK_SIZE;
  return 0;
}

