/*
 * Copyright 2011 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
  int f, f2, f3;
  off_t offset;

  char *fnam = "/tmp/emscripten_test_file";
  remove(fnam);
  errno = 0;

  printf("DUP\n");
  f = open("/", O_RDONLY);
  f2 = open("/", O_RDONLY);
  f3 = dup(f);
  printf("errno: %d\n", errno);
  printf("f: %d\n", f != f2 && f != f3);
  printf("f2,f3: %d\n", f2 != f3);
  printf("close(f1): %d\n", close(f));
  printf("close(f2): %d\n", close(f2));
  printf("close(f3): %d\n", close(f3));
  printf("\n");
  errno = 0;

  printf("DUP2\n");
  f = open("/", O_RDONLY);
  f2 = open("/", O_RDONLY);
  f3 = dup2(f, f2);
  printf("errno: %d\n", errno);
  printf("f: %d\n", f != f2 && f != f3);
  printf("f2,f3: %d\n", f2 == f3);
  printf("close(f1): %d\n", close(f));
  printf("close(f2): %d\n", close(f2));
  printf("close(f3): %d\n", close(f3));
  printf("\n");
  errno = 0;

  f = open(fnam, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
  close(f);

  printf("DUP3\n");
  f = open(fnam, O_RDONLY);
  f2 = open(fnam, O_RDONLY);
  f3 = dup(f);
  printf("errno: %d\n", errno);
  printf("f: %d\n", f != f2 && f != f3);
  printf("f2,f3: %d\n", f2 != f3);

  // dup()ed file descriptors should share all flags and even seek position
  offset = lseek(f3, 0, SEEK_CUR);
  printf("1. f3 offset was %d.    Should be 0\n", (int)offset);
  offset = lseek(f, 1, SEEK_SET);
  printf("2. f  offset set to %d. Should be 1\n", (int)offset);
  offset = lseek(f2, 2, SEEK_SET);
  printf("3. f2 offset set to %d. Should be 2\n", (int)offset);
  offset = lseek(f, 0, SEEK_CUR);
  printf("4. f  offset now is %d. Should be 1\n", (int)offset);
  offset = lseek(f2, 0, SEEK_CUR);
  printf("5. f2 offset now is %d. Should be 2\n", (int)offset);
  offset = lseek(f3, 0, SEEK_CUR);
  printf("6. f3 offset now is %d. Should be 1 (and not 0)\n", (int)offset);
  offset = lseek(f3, 3, SEEK_SET);
  printf("7. f3 offset set to %d. Should be 3\n", (int)offset);
  offset = lseek(f, 0, SEEK_CUR);
  printf("8. f  offset now is %d. Should be 3 (and not 1)\n", (int)offset);

  printf("close(f1): %d\n", close(f));
  printf("close(f2): %d\n", close(f2));
  printf("close(f3): %d\n", close(f3));
  printf("\n");
  errno = 0;

  printf("DUP4\n");
  f = open("/", O_RDONLY);
  f2 = open("/", O_RDONLY);
  f3 = dup2(f, f2);
  printf("errno: %d\n", errno);
  printf("f: %d\n", f != f2 && f != f3);
  printf("f2,f3: %d\n", f2 == f3);
  printf("close(f1): %d\n", close(f));
  printf("close(f2): %d\n", close(f2));
  printf("close(f3): %d\n", close(f3));
  errno = 0;

  return 0;
}
