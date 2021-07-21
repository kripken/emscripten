/*
 * Copyright 2021 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

#define _GNU_SOURCE
#include "pthread_impl.h"
#include <pthread.h>

int pthread_setcancelstate(int new, int* old) {
  if (new > 1U)
    return EINVAL;
  struct pthread* self = pthread_self();
  if (old)
    *old = self->canceldisable;
  self->canceldisable = new;
  return 0;
}
