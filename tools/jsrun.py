from toolchain_profiler import ToolchainProfiler
import time, os, sys, logging, json
from subprocess import Popen, PIPE, STDOUT
import cache

TRACK_PROCESS_SPAWNS = True if (os.getenv('EM_BUILD_VERBOSE') and int(os.getenv('EM_BUILD_VERBOSE')) >= 3) else False
ENGINES_WORK = {}

def timeout_run(proc, timeout=None, note='unnamed process', full_output=False):
  start = time.time()
  if timeout is not None:
    while time.time() - start < timeout and proc.poll() is None:
      time.sleep(0.1)
    if proc.poll() is None:
      proc.kill() # XXX bug: killing emscripten.py does not kill it's child process!
      raise Exception("Timed out: " + note)
  out = proc.communicate()
  out = map(lambda o: '' if o is None else o, out)
  if TRACK_PROCESS_SPAWNS:
    logging.info('Process ' + str(proc.pid) + ' finished after ' + str(time.time() - start) + ' seconds. Exit code: ' + str(proc.returncode))
  return '\n'.join(out) if full_output else out[0]

def make_command(filename, engine=None, args=[]):
  if type(engine) is not list:
    engine = [engine]
  # Emscripten supports multiple javascript runtimes.  The default is nodejs but
  # it can also use d8 (the v8 engine shell) or jsc (JavaScript Core aka
  # Safari).  Both d8 and jsc require a '--' to delimit arguments to be passed
  # to the executed script from d8/jsc options.  Node does not require a
  # delimeter--arguments after the filename are passed to the script.
  #
  # Check only the last part of the engine path to ensure we don't accidentally
  # label a path to nodejs containing a 'd8' as spidermonkey instead.
  jsengine = os.path.split(engine[0])[-1]
  # Use "'d8' in" because the name can vary, e.g. d8_g, d8, etc.
  return engine + [filename] + (['--expose-wasm', '--'] if 'd8' in jsengine or 'jsc' in jsengine else []) + args


def check_engine(engine, rootpath):
  if type(engine) is list: # XXX wat? When is this not a list?
    engine_path = engine[0]
  global ENGINES_WORK
  if engine_path in ENGINES_WORK:
    return ENGINES_WORK[engine_path]
  try:
    temp_cache = cache.Cache(use_subdir=False)
    def get_engine_cache():
      saved_file = os.path.join(temp_cache.dirname, 'js_engine_check.txt')
      with open(saved_file, 'w') as f:
        print 'json!'
        print ENGINES_WORK
        json.dumps(ENGINES_WORK)
        json.dump(ENGINES_WORK, f)
      return saved_file
    engine_file = temp_cache.get('js_engine_check', get_engine_cache, '.txt')
    with open(engine_file) as f:
      engine_cache = json.load(f)
      print 'loadjson!'
      print engine_cache
      ENGINES_WORK = engine_cache
    if engine_path in ENGINES_WORK:
      return ENGINES_WORK[engine_path]
    logging.info('Checking JS engine %s' % engine)
    if 'hello, world!' in run_js(os.path.join(rootpath, 'src', 'hello_world.js'), engine, skip_check=True):
      ENGINES_WORK[engine_path] = True
    with open(engine_file, 'w') as f:
      json.dump(ENGINES_WORK, f)
  except Exception, e:
    logging.info('Checking JS engine %s failed. Check your config file. Details: %s' % (str(engine), str(e)))
    ENGINES_WORK[engine_path] = False
  return ENGINES_WORK[engine_path]


def require_engine(engine):
  engine_path = engine[0]
  assert engine_path in ENGINES_WORK, 'JS engine %s should have been checked at startup' % engine
  if not ENGINES_WORK[engine_path]:
    logging.critical('The JavaScript shell (%s) does not seem to work, check the paths in the config file' % engine)
    sys.exit(1)


def run_js(filename, engine=None, args=[], check_timeout=False, stdin=None, stdout=PIPE, stderr=None, cwd=None,
           full_output=False, assert_returncode=0, error_limit=-1, skip_check=False):
  #  # code to serialize out the test suite files
  #  # XXX make sure to disable memory init files, and clear out the base_dir. you may also need to manually grab e.g. paper.pdf.js from a run of test_poppler
  #  import shutil, json
  #  base_dir = '/tmp/emscripten_suite'
  #  if not os.path.exists(base_dir):
  #    os.makedirs(base_dir)
  #  commands_file = os.path.join(base_dir, 'commands.txt')
  #  commands = ''
  #  if os.path.exists(commands_file):
  #    commands = open(commands_file).read()
  #  i = 0
  #  while True:
  #    curr = os.path.join(base_dir, str(i) + '.js')
  #    if not os.path.exists(curr): break
  #    i += 1
  #  shutil.copyfile(filename, curr)
  #  commands += os.path.basename(curr) + ',' + json.dumps(args) + '\n'
  #  open(commands_file, 'w').write(commands)

  if not skip_check:
    require_engine(engine)
  command = make_command(filename, engine, args)
  try:
    if cwd is not None: os.environ['EMCC_BUILD_DIR'] = os.getcwd()
    proc = Popen(
        command,
        stdin=stdin,
        stdout=stdout,
        stderr=stderr,
        cwd=cwd)
  finally:
    if cwd is not None: del os.environ['EMCC_BUILD_DIR']
  timeout = 15*60 if check_timeout else None
  if TRACK_PROCESS_SPAWNS:
    logging.info('Blocking on process ' + str(proc.pid) + ': ' + str(command) + (' for ' + str(timeout) + ' seconds' if timeout else ' until it finishes.'))
  ret = timeout_run(
    proc,
    timeout,
    'Execution',
    full_output=full_output)
  if assert_returncode is not None and proc.returncode is not assert_returncode:
    raise Exception('Expected the command ' + str(command) + ' to finish with return code ' + str(assert_returncode) + ', but it returned with code ' + str(proc.returncode) + ' instead! Output: ' + str(ret)[:error_limit])
  return ret
