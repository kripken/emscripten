#!/usr/bin/env python2

'''
emar - ar helper script
=======================

This script acts as a frontend replacement for ar. See emcc.
'''

from __future__ import print_function
from tools.toolchain_profiler import ToolchainProfiler
if __name__ == '__main__':
  ToolchainProfiler.record_process_start()

import os, subprocess, sys
from tools import shared
from tools.append_hashes import append_hash_to_file_list

#
# Main run() function
#
def run():
  DEBUG = os.environ.get('EMCC_DEBUG')
  if DEBUG == "0":
    DEBUG = None

  newargs = [shared.LLVM_AR] + sys.argv[1:]

  if DEBUG:
    print('emar:', sys.argv, '  ==>  ', newargs, file=sys.stderr)

  to_delete = []
  if len(newargs) > 2:
    if 'r' in newargs[1]:
      # we are adding files to the archive.
      # find the .a; everything after it is an input file.
      # we add a hash to each input, to make them unique as
      # possible, as llvm-ar cannot extract duplicate names
      # (and only the basename is used!)
      i = 1
      while i < len(newargs):
        if newargs[i].endswith('.a'):
          (hashed_names, to_delete) = append_hash_to_file_list(newargs[(i + 1):])
          newargs[(i + 1):] = hashed_names
          break
        i += 1

  if DEBUG:
    print('Invoking ' + str(newargs))
  subprocess.call(newargs, stdin=sys.stdin)
  for d in to_delete:
    shared.try_delete(d)

if __name__ == '__main__':
  run()
  sys.exit(0)
