# Copyright 2020 The Emscripten Authors.  All rights reserved.
# Emscripten is available under two separate licenses, the MIT license and the
# University of Illinois/NCSA Open Source License.  Both these licenses can be
# found in the LICENSE file.

import os
import re

from tools.shared import Settings, path_from_root, unsuffixed, NODE_JS, check_call, exit_with_error


def do_wasm2c(infile):
  assert Settings.STANDALONE_WASM
  WASM2C = NODE_JS + [path_from_root('node_modules', 'wasm2c', 'wasm2c.js')]
  WASM2C_DIR = path_from_root('node_modules', 'wasm2c')
  c_file = unsuffixed(infile) + '.wasm.c'
  h_file = unsuffixed(infile) + '.wasm.h'
  cmd = WASM2C + [infile, '-o', c_file]
  check_call(cmd)
  total = '''\
/*
* This file was generated by emcc+wasm2c. To compile it, use something like
*
*   $CC FILE.c -O2 -lm -DWASM_RT_MAX_CALL_STACK_DEPTH=8000
*/
'''
  SEP = '\n/* ==================================== */\n'

  def bundle_file(total, filename):
    with open(filename) as f:
      total += '// ' + filename + '\n' + f.read() + SEP
    return total

  # hermeticize the C file, by bundling in the wasm2c/ includes
  headers = [
    (WASM2C_DIR, 'wasm-rt.h'),
    (WASM2C_DIR, 'wasm-rt-impl.h'),
    ('', h_file)
  ]
  for header in headers:
    total = bundle_file(total, os.path.join(header[0], header[1]))
  # add the wasm2c output
  with open(c_file) as read_c:
    c = read_c.read()
  total += c + SEP
  # add the wasm2c runtime
  total = bundle_file(total, os.path.join(WASM2C_DIR, 'wasm-rt-impl.c'))
  # add the support code
  support_files = ['base']
  if Settings.AUTODEBUG:
    support_files.append('autodebug')
  if Settings.EXPECT_MAIN:
    # TODO: add an option for direct OS access. For now, do that when building
    #       an executable with main, as opposed to a library
    support_files.append('os')
    support_files.append('main')
  else:
    support_files.append('os_sandboxed')
    support_files.append('reactor')
    # for a reactor, also append wasmbox_* API definitions
    with open(h_file, 'a') as f:
      f.write('''
// wasmbox_* API
// TODO: optional prefixing
extern void wasmbox_init(void);
''')
  for support_file in support_files:
    total = bundle_file(total, path_from_root('tools', 'wasm2c', support_file + '.c'))
  # remove #includes of the headers we bundled
  for header in headers:
    total = total.replace('#include "%s"\n' % header[1], '/* include of %s */\n' % header[1])
  # generate the necessary invokes
  invokes = []
  for sig in re.findall(r"\/\* import\: 'env' 'invoke_(\w+)' \*\/", total):
    def s_to_c(s):
      if s == 'v':
        return 'void'
      elif s == 'i':
        return 'u32'
      elif s == 'j':
        return 'u64'
      elif s == 'f':
        return 'f32'
      elif s == 'd':
        return 'f64'
      else:
        exit_with_error('invalid sig element:' + str(s))

    def name(i):
      return 'a' + str(i)

    wabt_sig = sig[0] + 'i' + sig[1:]
    typed_args = ['u32 fptr'] + [s_to_c(sig[i]) + ' ' + name(i) for i in range(1, len(sig))]
    types = ['u32'] + [s_to_c(sig[i]) for i in range(1, len(sig))]
    args = ['fptr'] + [name(i) for i in range(1, len(sig))]
    invokes.append(
      '%s_INVOKE_IMPL(%sZ_envZ_invoke_%sZ_%s, (%s), (%s), (%s), Z_dynCall_%sZ_%s);' % (
        'VOID' if sig[0] == 'v' else 'RETURNING',
        (s_to_c(sig[0]) + ', ') if sig[0] != 'v' else '',
        sig,
        wabt_sig,
        ', '.join(typed_args),
        ', '.join(types),
        ', '.join(args),
        sig,
        wabt_sig
      ))
  total += '\n'.join(invokes)
  # write out the final file
  with open(c_file, 'w') as out:
    out.write(total)
