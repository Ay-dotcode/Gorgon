/*
	Contains OpenGL types, defines, typedefs, function pointers etc.
*/
#pragma once

#include "OS.h"
#include <GL/gl.h>

#define GL_MAP_READ_BIT					0x0001
#define GL_MAP_WRITE_BIT				0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT		0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT	0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT		0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT		0x0020
#define GL_TEXTURE0						0x84C0
#define GL_TEXTURE1						0x84C1
#define GL_TEXTURE2						0x84C2
#define GL_TEXTURE3						0x84C3
#define GL_TEXTURE4						0x84C4
#define GL_TEXTURE5						0x84C5
#define GL_TEXTURE6						0x84C6
#define GL_TEXTURE7						0x84C7
#define GL_TEXTURE8						0x84C8
#define GL_TEXTURE9						0x84C9
#define GL_TEXTURE10					0x84CA
#define GL_TEXTURE11					0x84CB
#define GL_TEXTURE12					0x84CC
#define GL_TEXTURE13					0x84CD
#define GL_TEXTURE14					0x84CE
#define GL_TEXTURE15					0x84CF
#define GL_TEXTURE16					0x84D0
#define GL_TEXTURE17					0x84D1
#define GL_TEXTURE18					0x84D2
#define GL_TEXTURE19					0x84D3
#define GL_TEXTURE20					0x84D4
#define GL_TEXTURE21					0x84D5
#define GL_TEXTURE22					0x84D6
#define GL_TEXTURE23					0x84D7
#define GL_TEXTURE24					0x84D8
#define GL_TEXTURE25					0x84D9
#define GL_TEXTURE26					0x84DA
#define GL_TEXTURE27					0x84DB
#define GL_TEXTURE28					0x84DC
#define GL_TEXTURE29					0x84DD
#define GL_TEXTURE30					0x84DE
#define GL_TEXTURE31					0x84DF
#define GL_ARRAY_BUFFER					0x8892
#define GL_STREAM_DRAW					0x88E0
#define GL_STREAM_READ					0x88E1
#define GL_STREAM_COPY					0x88E2
#define GL_STATIC_DRAW					0x88E4
#define GL_STATIC_READ					0x88E5
#define GL_STATIC_COPY					0x88E6
#define GL_DYNAMIC_DRAW					0x88E8
#define GL_DYNAMIC_READ					0x88E9
#define GL_DYNAMIC_COPY					0x88EA
#define GL_UNIFORM_BUFFER				0x8A11
#define GL_FRAGMENT_SHADER				0x8B30
#define GL_VERTEX_SHADER                0x8B31
#define GL_COMPILE_STATUS               0x8B81
#define GL_LINK_STATUS                  0x8B82
#define GL_INFO_LOG_LENGTH              0x8B84
#define GL_GEOMETRY_SHADER              0x8DD9



typedef char							GLchar;
typedef ptrdiff_t						GLsizeiptr;
typedef ptrdiff_t						GLintptr;

#ifndef GL_FRAMEBUFFER
	const GLenum						GL_FRAMEBUFFER=0x8D40;
	const GLenum						GL_COLOR_ATTACHMENT0=0x8CE0;
	const GLenum						GL_GENERATE_MIPMAP=0x8191;
#endif

#ifdef WIN32
#  ifdef APIENTRY
#    undef APIENTRY
#  endif
#  ifdef APIENTRYP
#    undef APIENTRYP
#  endif
#  define APIENTRY	__stdcall
#  define APIENTRYP	__stdcall *
#endif

#ifndef LINUX
typedef GLint		(APIENTRYP PFNGLGETATTRIBLOCATIONPROC)			(GLuint program, const GLchar *name);
typedef void		(APIENTRYP PFNGLBINDATTRIBLOCATIONPROC)			(GLuint program, GLuint index, const GLchar * name); 
typedef void		(APIENTRYP PFNGLACTIVETEXTUREPROC)				(GLenum texture);
typedef void		(APIENTRYP PFNGLATTACHSHADERPROC)				(GLuint program, GLuint shader);
typedef void		(APIENTRYP PFNGLBINDBUFFERPROC)					(GLenum target, GLuint buffer);
typedef void		(APIENTRYP PFNGLBINDBUFFERBASEPROC)				(GLenum target, GLuint index, GLuint buffer);
typedef void		(APIENTRYP PFNGLBINDFRAMEBUFFERPROC)			(GLenum target, GLuint framebuffer);
typedef void		(APIENTRYP PFNGLBINDVERTEXARRAYPROC)			(GLuint array);
typedef void		(APIENTRYP PFNGLBUFFERDATAPROC)					(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void		(APIENTRYP PFNGLBUFFERSUBDATAPROC)				(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
typedef GLenum		(APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)		(GLenum);
typedef void		(APIENTRYP PFNGLCOMPILESHADERPROC)				(GLuint shader);
typedef GLuint		(APIENTRYP PFNGLCREATEPROGRAMPROC)				(void);
typedef GLuint		(APIENTRYP PFNGLCREATESHADERPROC)				(GLenum type);
typedef void		(APIENTRYP PFNGLDELETEBUFFERSPROC)				(GLsizei n, const GLuint *buffers);
typedef void		(APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)			(GLsizei, const GLuint *);
typedef void		(APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)			(GLsizei n, const GLuint *arrays);
typedef void		(APIENTRYP PFNGLDELETEPROGRAMPROC)				(GLuint program);
typedef void		(APIENTRYP PFNGLDELETESHADERPROC)				(GLuint shader);
typedef void		(APIENTRYP PFNGLDETACHSHADERPROC)				(GLuint program, GLuint shader);
typedef void		(APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)	(GLuint index);
typedef void		(APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)		(GLenum, GLenum, GLenum, GLuint, GLint);
typedef void		(APIENTRYP PFNGLGENBUFFERSPROC)					(GLsizei n, GLuint *buffers);
typedef void		(APIENTRYP PFNGLGENERATEMIPMAPPROC)				(GLenum);
typedef void		(APIENTRYP PFNGLGENFRAMEBUFFERSPROC)			(int, GLuint*);
typedef void		(APIENTRYP PFNGLGENVERTEXARRAYSPROC)			(GLsizei n, GLuint *arrays);
typedef void		(APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)			(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void		(APIENTRYP PFNGLGETPROGRAMIVPROC)				(GLuint program, GLenum pname, GLint *params);
typedef void		(APIENTRYP PFNGLGETSHADERINFOLOGPROC)			(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void		(APIENTRYP PFNGLGETSHADERIVPROC)				(GLuint shader, GLenum pname, GLint *params);
typedef GLuint		(APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC)		(GLuint program, const GLchar* uniformBlockName);
typedef GLint		(APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)			(GLuint program, const GLchar* name);
typedef void		(APIENTRYP PFNGLLINKPROGRAMPROC)				(GLuint program);
typedef void *		(APIENTRYP PFNGLMAPBUFFERPROC)					(GLenum target, GLenum access);
typedef void *		(APIENTRYP PFNGLMAPBUFFERRANGEPROC)				(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void		(APIENTRYP PFNGLSHADERSOURCEPROC)				(GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length);
typedef void		(APIENTRYP PFNGLUNIFORM1FPROC)					(GLint location, GLfloat v0);
typedef void		(APIENTRYP PFNGLUNIFORM1IPROC)					(GLint location, GLint v0);
typedef void		(APIENTRYP PFNGLUNIFORM3FVPROC)					(GLint location, GLsizei count, GLfloat* v0);
typedef void		(APIENTRYP PFNGLUNIFORM4FVPROC)					(GLint location, GLsizei count, GLfloat* v0);
typedef void		(APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC)		(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
typedef void		(APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)			(GLint location, GLsizei count, GLboolean transpose, GLfloat* value);
typedef void		(APIENTRYP PFNGLUNIFORMMATRIX3X2FVPROC)			(GLint location, GLsizei count, GLboolean transpose, GLfloat* value);
typedef void		(APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)			(GLint location, GLsizei count, GLboolean transpose, GLfloat* value);
typedef void		(APIENTRYP PFNGLUNIFORMMATRIX4X2FVPROC)			(GLint location, GLsizei count, GLboolean transpose, GLfloat* value);
typedef void		(APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC)			(GLint location, GLsizei count, GLboolean transpose, GLfloat* value);
typedef GLboolean	(APIENTRYP PFNGLUNMAPBUFFERPROC)				(GLenum target);
typedef void		(APIENTRYP PFNGLUSEPROGRAMPROC)					(GLuint program);
typedef void		(APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)		(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
typedef void		(APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC)		(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);

extern PFNGLACTIVETEXTUREPROC					glActiveTexture;
#endif

extern PFNGLGETATTRIBLOCATIONPROC				glGetAttribLocation;
extern PFNGLBINDATTRIBLOCATIONPROC				glBindAttribLocation;
extern PFNGLATTACHSHADERPROC					glAttachShader;
extern PFNGLBINDBUFFERPROC						glBindBuffer;
extern PFNGLBINDBUFFERBASEPROC					glBindBufferBase;
extern PFNGLBINDFRAMEBUFFERPROC					glBindFramebuffer;
extern PFNGLBINDVERTEXARRAYPROC					glBindVertexArray;
extern PFNGLBUFFERDATAPROC						glBufferData;
extern PFNGLBUFFERSUBDATAPROC					glBufferSubData;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC			glCheckFramebufferStatus;
extern PFNGLCOMPILESHADERPROC					glCompileShader;
extern PFNGLCREATEPROGRAMPROC					glCreateProgram;
extern PFNGLCREATESHADERPROC					glCreateShader;
extern PFNGLDELETEBUFFERSPROC					glDeleteBuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC				glDeleteFramebuffers;
extern PFNGLDELETEVERTEXARRAYSPROC				glDeleteVertexArrays;
extern PFNGLDELETEPROGRAMPROC					glDeleteProgram;
extern PFNGLDELETESHADERPROC					glDeleteShader;
extern PFNGLDETACHSHADERPROC					glDetachShader;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC			glEnableVertexAttribArray;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC			glFramebufferTexture2D;
extern PFNGLGENBUFFERSPROC						glGenBuffers;
extern PFNGLGENERATEMIPMAPPROC					glGenerateMipmap;
extern PFNGLGENFRAMEBUFFERSPROC					glGenFramebuffers;
extern PFNGLGENVERTEXARRAYSPROC					glGenVertexArrays;
extern PFNGLGETPROGRAMINFOLOGPROC				glGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC					glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC				glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC						glGetShaderiv;
extern PFNGLGETUNIFORMBLOCKINDEXPROC			glGetUniformBlockIndex;
extern PFNGLGETUNIFORMLOCATIONPROC				glGetUniformLocation;
extern PFNGLLINKPROGRAMPROC						glLinkProgram;
extern PFNGLMAPBUFFERPROC						glMapBuffer;
extern PFNGLMAPBUFFERRANGEPROC					glMapBufferRange;
extern PFNGLSHADERSOURCEPROC					glShaderSource;
extern PFNGLUNIFORM1FPROC						glUniform1f;
extern PFNGLUNIFORM1IPROC						glUniform1i;
extern PFNGLUNIFORM3FVPROC						glUniform3fv;
extern PFNGLUNIFORM4FVPROC						glUniform4fv;
extern PFNGLUNIFORMBLOCKBINDINGPROC				glUniformBlockBinding;
extern PFNGLUNIFORMMATRIX3FVPROC				glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC				glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX4FVPROC				glUniformMatrix4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC				glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC				glUniformMatrix4x3fv;
extern PFNGLUNMAPBUFFERPROC						glUnmapBuffer;
extern PFNGLUSEPROGRAMPROC						glUseProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC				glVertexAttribPointer;
extern PFNGLVERTEXATTRIBIPOINTERPROC			glVertexAttribIPointer;

namespace gge { namespace system {

void											LoadGLFunctions();

} }

#ifdef WIN32
#	undef APIENTRY
#	undef WINGDIAPI
#endif