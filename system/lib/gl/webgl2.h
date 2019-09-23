#pragma once

#include <GLES3/gl3.h>

#if USE_WEBGL2
#if GL_MAX_FEATURE_LEVEL != 20
#error "cannot resolve conflicting USE_WEBGL* settings"
#endif
#endif

GL_APICALL void GL_APIENTRY emscripten_glReadBuffer (GLenum src);
GL_APICALL void GL_APIENTRY emscripten_glDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
GL_APICALL void GL_APIENTRY emscripten_glTexImage3D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
GL_APICALL void GL_APIENTRY emscripten_glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
GL_APICALL void GL_APIENTRY emscripten_glCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL void GL_APIENTRY emscripten_glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
GL_APICALL void GL_APIENTRY emscripten_glCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
GL_APICALL void GL_APIENTRY emscripten_glGenQueries (GLsizei n, GLuint *ids);
GL_APICALL void GL_APIENTRY emscripten_glDeleteQueries (GLsizei n, const GLuint *ids);
GL_APICALL GLboolean GL_APIENTRY emscripten_glIsQuery (GLuint id);
GL_APICALL void GL_APIENTRY emscripten_glBeginQuery (GLenum target, GLuint id);
GL_APICALL void GL_APIENTRY emscripten_glEndQuery (GLenum target);
GL_APICALL void GL_APIENTRY emscripten_glGetQueryiv (GLenum target, GLenum pname, GLint *params);
GL_APICALL void GL_APIENTRY emscripten_glGetQueryObjectuiv (GLuint id, GLenum pname, GLuint *params);
GL_APICALL GLboolean GL_APIENTRY emscripten_glUnmapBuffer (GLenum target);
GL_APICALL void GL_APIENTRY emscripten_glGetBufferPointerv (GLenum target, GLenum pname, void **params);
GL_APICALL void GL_APIENTRY emscripten_glDrawBuffers (GLsizei n, const GLenum *bufs);
GL_APICALL void GL_APIENTRY emscripten_glUniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void GL_APIENTRY emscripten_glUniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void GL_APIENTRY emscripten_glUniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void GL_APIENTRY emscripten_glUniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void GL_APIENTRY emscripten_glUniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void GL_APIENTRY emscripten_glUniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void GL_APIENTRY emscripten_glBlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
GL_APICALL void GL_APIENTRY emscripten_glRenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
GL_APICALL void GL_APIENTRY emscripten_glFramebufferTextureLayer (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
GL_APICALL void *GL_APIENTRY emscripten_glMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
GL_APICALL void GL_APIENTRY emscripten_glFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length);
GL_APICALL void GL_APIENTRY emscripten_glBindVertexArray (GLuint array);
GL_APICALL void GL_APIENTRY emscripten_glDeleteVertexArrays (GLsizei n, const GLuint *arrays);
GL_APICALL void GL_APIENTRY emscripten_glGenVertexArrays (GLsizei n, GLuint *arrays);
GL_APICALL GLboolean GL_APIENTRY emscripten_glIsVertexArray (GLuint array);
GL_APICALL void GL_APIENTRY emscripten_glGetIntegeri_v (GLenum target, GLuint index, GLint *data);
GL_APICALL void GL_APIENTRY emscripten_glBeginTransformFeedback (GLenum primitiveMode);
GL_APICALL void GL_APIENTRY emscripten_glEndTransformFeedback (void);
GL_APICALL void GL_APIENTRY emscripten_glBindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
GL_APICALL void GL_APIENTRY emscripten_glBindBufferBase (GLenum target, GLuint index, GLuint buffer);
GL_APICALL void GL_APIENTRY emscripten_glTransformFeedbackVaryings (GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
GL_APICALL void GL_APIENTRY emscripten_glGetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
GL_APICALL void GL_APIENTRY emscripten_glGetVertexAttribIiv (GLuint index, GLenum pname, GLint *params);
GL_APICALL void GL_APIENTRY emscripten_glGetVertexAttribIuiv (GLuint index, GLenum pname, GLuint *params);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribI4iv (GLuint index, const GLint *v);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribI4uiv (GLuint index, const GLuint *v);
GL_APICALL void GL_APIENTRY emscripten_glGetUniformuiv (GLuint program, GLint location, GLuint *params);
GL_APICALL GLint GL_APIENTRY emscripten_glGetFragDataLocation (GLuint program, const GLchar *name);
GL_APICALL void GL_APIENTRY emscripten_glUniform1ui (GLint location, GLuint v0);
GL_APICALL void GL_APIENTRY emscripten_glUniform2ui (GLint location, GLuint v0, GLuint v1);
GL_APICALL void GL_APIENTRY emscripten_glUniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2);
GL_APICALL void GL_APIENTRY emscripten_glUniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
GL_APICALL void GL_APIENTRY emscripten_glUniform1uiv (GLint location, GLsizei count, const GLuint *value);
GL_APICALL void GL_APIENTRY emscripten_glUniform2uiv (GLint location, GLsizei count, const GLuint *value);
GL_APICALL void GL_APIENTRY emscripten_glUniform3uiv (GLint location, GLsizei count, const GLuint *value);
GL_APICALL void GL_APIENTRY emscripten_glUniform4uiv (GLint location, GLsizei count, const GLuint *value);
GL_APICALL void GL_APIENTRY emscripten_glClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint *value);
GL_APICALL void GL_APIENTRY emscripten_glClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint *value);
GL_APICALL void GL_APIENTRY emscripten_glClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat *value);
GL_APICALL void GL_APIENTRY emscripten_glClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
GL_APICALL const GLubyte *GL_APIENTRY emscripten_glGetStringi (GLenum name, GLuint index);
GL_APICALL void GL_APIENTRY emscripten_glCopyBufferSubData (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
GL_APICALL void GL_APIENTRY emscripten_glGetUniformIndices (GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
GL_APICALL void GL_APIENTRY emscripten_glGetActiveUniformsiv (GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
GL_APICALL GLuint GL_APIENTRY emscripten_glGetUniformBlockIndex (GLuint program, const GLchar *uniformBlockName);
GL_APICALL void GL_APIENTRY emscripten_glGetActiveUniformBlockiv (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
GL_APICALL void GL_APIENTRY emscripten_glGetActiveUniformBlockName (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
GL_APICALL void GL_APIENTRY emscripten_glUniformBlockBinding (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
GL_APICALL void GL_APIENTRY emscripten_glDrawArraysInstanced (GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
GL_APICALL GLsync GL_APIENTRY emscripten_glFenceSync (GLenum condition, GLbitfield flags);
GL_APICALL GLboolean GL_APIENTRY emscripten_glIsSync (GLsync sync);
GL_APICALL void GL_APIENTRY emscripten_glDeleteSync (GLsync sync);
GL_APICALL GLenum GL_APIENTRY emscripten_glClientWaitSync (GLsync sync, GLbitfield flags, GLuint timeoutLo, GLuint timeoutHi);
GL_APICALL void GL_APIENTRY emscripten_glWaitSync (GLsync sync, GLbitfield flags, GLuint timeoutLo, GLuint timeoutHi);
GL_APICALL void GL_APIENTRY emscripten_glGetInteger64v (GLenum pname, GLint64 *data);
GL_APICALL void GL_APIENTRY emscripten_glGetSynciv (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
GL_APICALL void GL_APIENTRY emscripten_glGetInteger64i_v (GLenum target, GLuint index, GLint64 *data);
GL_APICALL void GL_APIENTRY emscripten_glGetBufferParameteri64v (GLenum target, GLenum pname, GLint64 *params);
GL_APICALL void GL_APIENTRY emscripten_glGenSamplers (GLsizei count, GLuint *samplers);
GL_APICALL void GL_APIENTRY emscripten_glDeleteSamplers (GLsizei count, const GLuint *samplers);
GL_APICALL GLboolean GL_APIENTRY emscripten_glIsSampler (GLuint sampler);
GL_APICALL void GL_APIENTRY emscripten_glBindSampler (GLuint unit, GLuint sampler);
GL_APICALL void GL_APIENTRY emscripten_glSamplerParameteri (GLuint sampler, GLenum pname, GLint param);
GL_APICALL void GL_APIENTRY emscripten_glSamplerParameteriv (GLuint sampler, GLenum pname, const GLint *param);
GL_APICALL void GL_APIENTRY emscripten_glSamplerParameterf (GLuint sampler, GLenum pname, GLfloat param);
GL_APICALL void GL_APIENTRY emscripten_glSamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat *param);
GL_APICALL void GL_APIENTRY emscripten_glGetSamplerParameteriv (GLuint sampler, GLenum pname, GLint *params);
GL_APICALL void GL_APIENTRY emscripten_glGetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat *params);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribDivisor (GLuint index, GLuint divisor);
GL_APICALL void GL_APIENTRY emscripten_glBindTransformFeedback (GLenum target, GLuint id);
GL_APICALL void GL_APIENTRY emscripten_glDeleteTransformFeedbacks (GLsizei n, const GLuint *ids);
GL_APICALL void GL_APIENTRY emscripten_glGenTransformFeedbacks (GLsizei n, GLuint *ids);
GL_APICALL GLboolean GL_APIENTRY emscripten_glIsTransformFeedback (GLuint id);
GL_APICALL void GL_APIENTRY emscripten_glPauseTransformFeedback (void);
GL_APICALL void GL_APIENTRY emscripten_glResumeTransformFeedback (void);
GL_APICALL void GL_APIENTRY emscripten_glGetProgramBinary (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
GL_APICALL void GL_APIENTRY emscripten_glProgramBinary (GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
GL_APICALL void GL_APIENTRY emscripten_glProgramParameteri (GLuint program, GLenum pname, GLint value);
GL_APICALL void GL_APIENTRY emscripten_glInvalidateFramebuffer (GLenum target, GLsizei numAttachments, const GLenum *attachments);
GL_APICALL void GL_APIENTRY emscripten_glInvalidateSubFramebuffer (GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL void GL_APIENTRY emscripten_glTexStorage2D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
GL_APICALL void GL_APIENTRY emscripten_glTexStorage3D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
GL_APICALL void GL_APIENTRY emscripten_glGetInternalformativ (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params);

// Extensions:
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribDivisorNV(GLuint index, GLuint divisor);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribDivisorEXT(GLuint index, GLuint divisor);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribDivisorARB(GLuint index, GLuint divisor);
GL_APICALL void GL_APIENTRY emscripten_glVertexAttribDivisorANGLE(GLuint index, GLuint divisor);
GL_APICALL void GL_APIENTRY emscripten_glDrawArraysInstancedNV(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glDrawArraysInstancedEXT(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glDrawArraysInstancedARB(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glDrawArraysInstancedANGLE(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glDrawElementsInstancedNV(GLenum mode, GLsizei count, GLenum type, GLintptr indices, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glDrawElementsInstancedEXT(GLenum mode, GLsizei count, GLenum type, GLintptr indices, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glDrawElementsInstancedARB(GLenum mode, GLsizei count, GLenum type, GLintptr indices, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glDrawElementsInstancedANGLE(GLenum mode, GLsizei count, GLenum type, GLintptr indices, GLsizei instancecount);
GL_APICALL void GL_APIENTRY emscripten_glBindVertexArrayOES(GLuint array);
GL_APICALL void GL_APIENTRY emscripten_glDeleteVertexArraysOES(GLsizei n, const GLuint *arrays);
GL_APICALL void GL_APIENTRY emscripten_glGenVertexArraysOES(GLsizei n, GLuint *arrays);
GL_APICALL GLboolean GL_APIENTRY emscripten_glIsVertexArrayOES(GLuint array);
GL_APICALL void GL_APIENTRY emscripten_glDrawBuffersEXT(GLsizei n, const GLenum *bufs);
GL_APICALL void GL_APIENTRY emscripten_glDrawBuffersWEBGL(GLsizei n, const GLenum *bufs);
