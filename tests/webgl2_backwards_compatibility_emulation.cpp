// Copyright 2017 The Emscripten Authors.  All rights reserved.
// Emscripten is available under two separate licenses, the MIT license and the
// University of Illinois/NCSA Open Source License.  Both these licenses can be
// found in the LICENSE file.

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <emscripten.h>
#include <emscripten/html5.h>

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;

int main()
{
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;

  int result = 0;

  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(0, &attrs);
  assert(context);
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(context);
  assert(res == EMSCRIPTEN_RESULT_SUCCESS);
  assert(emscripten_webgl_get_current_context() == context);
  const char *gles20 = "OpenGL ES 2.0 ";
  const char *glsles100 = "OpenGL ES GLSL ES 1.00 ";
  const char *ver;
  ver = (const char *)glGetString(GL_VERSION);
  assert(strncmp(ver, gles20, strlen(gles20)) == 0);
  ver = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
  assert(strncmp(ver, glsles100, strlen(glsles100)) == 0);
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_HALF_FLOAT_OES, 0);
  assert(glGetError() == 0);
  emscripten_webgl_destroy_context(context);

#ifdef REPORT_RESULT
  REPORT_RESULT(result);
#endif
  return 0;
}
