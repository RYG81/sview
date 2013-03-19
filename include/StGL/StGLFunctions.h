/**
 * Copyright © 2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLFunctions_h_
#define __StGLFunctions_h_

// required for correct APIENTRY definition
#if(defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__))
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#ifndef APIENTRY
    #define APIENTRY
#endif
#ifndef APIENTRYP
    #define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
    #define GLAPI extern
#endif

// exclude modern definitions - use own glext.h header!
#define GL_GLEXT_LEGACY

#ifndef __glext_h_
    #define __glext_h_
    #define ST_GLEXT_PRE
#endif

// include main OpenGL header provided with system
#if(defined(__APPLE__))
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

#ifdef ST_GLEXT_PRE
    #undef __glext_h_
#endif

// GL version can be defined by system gl.h header
#undef GL_VERSION_1_2
#undef GL_VERSION_1_3
#undef GL_VERSION_1_4
#undef GL_VERSION_1_5
#undef GL_VERSION_2_0
#undef GL_VERSION_2_1
#undef GL_VERSION_3_0
#undef GL_VERSION_3_1
#undef GL_VERSION_3_2
#undef GL_VERSION_3_3
#undef GL_VERSION_4_0
#undef GL_VERSION_4_1
#undef GL_VERSION_4_2
#undef GL_VERSION_4_3

#include <StGL/StGLExt.h>

#include <stTypes.h>

/**
 * Mega structure defines the list of ALL OpenGL functions!
 */
struct ST_LOCAL StGLFunctions {

        public: //! @name OpenGL 1.2

    PFNGLBLENDCOLORPROC               glBlendColor;
    PFNGLBLENDEQUATIONPROC            glBlendEquation;
    PFNGLDRAWRANGEELEMENTSPROC        glDrawRangeElements;
    PFNGLTEXIMAGE3DPROC               glTexImage3D;
    PFNGLTEXSUBIMAGE3DPROC            glTexSubImage3D;
    PFNGLCOPYTEXSUBIMAGE3DPROC        glCopyTexSubImage3D;

        public: //! @name OpenGL 1.3

    PFNGLACTIVETEXTUREPROC            glActiveTexture;
    PFNGLSAMPLECOVERAGEPROC           glSampleCoverage;
    PFNGLCOMPRESSEDTEXIMAGE3DPROC     glCompressedTexImage3D;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC     glCompressedTexImage2D;
    PFNGLCOMPRESSEDTEXIMAGE1DPROC     glCompressedTexImage1D;
    PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC  glCompressedTexSubImage3D;
    PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC  glCompressedTexSubImage2D;
    PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC  glCompressedTexSubImage1D;
    PFNGLGETCOMPRESSEDTEXIMAGEPROC    glGetCompressedTexImage;

    PFNGLCLIENTACTIVETEXTUREPROC      glClientActiveTexture;
    PFNGLMULTITEXCOORD1DPROC          glMultiTexCoord1d;
    PFNGLMULTITEXCOORD1DVPROC         glMultiTexCoord1dv;
    PFNGLMULTITEXCOORD1FPROC          glMultiTexCoord1f;
    PFNGLMULTITEXCOORD1FVPROC         glMultiTexCoord1fv;
    PFNGLMULTITEXCOORD1IPROC          glMultiTexCoord1i;
    PFNGLMULTITEXCOORD1IVPROC         glMultiTexCoord1iv;
    PFNGLMULTITEXCOORD1SPROC          glMultiTexCoord1s;
    PFNGLMULTITEXCOORD1SVPROC         glMultiTexCoord1sv;
    PFNGLMULTITEXCOORD2DPROC          glMultiTexCoord2d;
    PFNGLMULTITEXCOORD2DVPROC         glMultiTexCoord2dv;
    PFNGLMULTITEXCOORD2FPROC          glMultiTexCoord2f;
    PFNGLMULTITEXCOORD2FVPROC         glMultiTexCoord2fv;
    PFNGLMULTITEXCOORD2IPROC          glMultiTexCoord2i;
    PFNGLMULTITEXCOORD2IVPROC         glMultiTexCoord2iv;
    PFNGLMULTITEXCOORD2SPROC          glMultiTexCoord2s;
    PFNGLMULTITEXCOORD2SVPROC         glMultiTexCoord2sv;
    PFNGLMULTITEXCOORD3DPROC          glMultiTexCoord3d;
    PFNGLMULTITEXCOORD3DVPROC         glMultiTexCoord3dv;
    PFNGLMULTITEXCOORD3FPROC          glMultiTexCoord3f;
    PFNGLMULTITEXCOORD3FVPROC         glMultiTexCoord3fv;
    PFNGLMULTITEXCOORD3IPROC          glMultiTexCoord3i;
    PFNGLMULTITEXCOORD3IVPROC         glMultiTexCoord3iv;
    PFNGLMULTITEXCOORD3SPROC          glMultiTexCoord3s;
    PFNGLMULTITEXCOORD3SVPROC         glMultiTexCoord3sv;
    PFNGLMULTITEXCOORD4DPROC          glMultiTexCoord4d;
    PFNGLMULTITEXCOORD4DVPROC         glMultiTexCoord4dv;
    PFNGLMULTITEXCOORD4FPROC          glMultiTexCoord4f;
    PFNGLMULTITEXCOORD4FVPROC         glMultiTexCoord4fv;
    PFNGLMULTITEXCOORD4IPROC          glMultiTexCoord4i;
    PFNGLMULTITEXCOORD4IVPROC         glMultiTexCoord4iv;
    PFNGLMULTITEXCOORD4SPROC          glMultiTexCoord4s;
    PFNGLMULTITEXCOORD4SVPROC         glMultiTexCoord4sv;
    PFNGLLOADTRANSPOSEMATRIXFPROC     glLoadTransposeMatrixf;
    PFNGLLOADTRANSPOSEMATRIXDPROC     glLoadTransposeMatrixd;
    PFNGLMULTTRANSPOSEMATRIXFPROC     glMultTransposeMatrixf;
    PFNGLMULTTRANSPOSEMATRIXDPROC     glMultTransposeMatrixd;

        public: //! @name OpenGL 1.4

    PFNGLBLENDFUNCSEPARATEPROC        glBlendFuncSeparate;
    PFNGLMULTIDRAWARRAYSPROC          glMultiDrawArrays;
    PFNGLMULTIDRAWELEMENTSPROC        glMultiDrawElements;
    PFNGLPOINTPARAMETERFPROC          glPointParameterf;
    PFNGLPOINTPARAMETERFVPROC         glPointParameterfv;
    PFNGLPOINTPARAMETERIPROC          glPointParameteri;
    PFNGLPOINTPARAMETERIVPROC         glPointParameteriv;

        public: //! @name OpenGL 1.5

    PFNGLGENQUERIESPROC               glGenQueries;
    PFNGLDELETEQUERIESPROC            glDeleteQueries;
    PFNGLISQUERYPROC                  glIsQuery;
    PFNGLBEGINQUERYPROC               glBeginQuery;
    PFNGLENDQUERYPROC                 glEndQuery;
    PFNGLGETQUERYIVPROC               glGetQueryiv;
    PFNGLGETQUERYOBJECTIVPROC         glGetQueryObjectiv;
    PFNGLGETQUERYOBJECTUIVPROC        glGetQueryObjectuiv;
    PFNGLBINDBUFFERPROC               glBindBuffer;
    PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
    PFNGLGENBUFFERSPROC               glGenBuffers;
    PFNGLISBUFFERPROC                 glIsBuffer;
    PFNGLBUFFERDATAPROC               glBufferData;
    PFNGLBUFFERSUBDATAPROC            glBufferSubData;
    PFNGLGETBUFFERSUBDATAPROC         glGetBufferSubData;
    PFNGLMAPBUFFERPROC                glMapBuffer;
    PFNGLUNMAPBUFFERPROC              glUnmapBuffer;
    PFNGLGETBUFFERPARAMETERIVPROC     glGetBufferParameteriv;
    PFNGLGETBUFFERPOINTERVPROC        glGetBufferPointerv;

        public: //! @name OpenGL 2.0

    PFNGLBLENDEQUATIONSEPARATEPROC    glBlendEquationSeparate;
    PFNGLDRAWBUFFERSPROC              glDrawBuffers;
    PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
    PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate;
    PFNGLSTENCILMASKSEPARATEPROC      glStencilMaskSeparate;
    PFNGLATTACHSHADERPROC             glAttachShader;
    PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
    PFNGLCOMPILESHADERPROC            glCompileShader;
    PFNGLCREATEPROGRAMPROC            glCreateProgram;
    PFNGLCREATESHADERPROC             glCreateShader;
    PFNGLDELETEPROGRAMPROC            glDeleteProgram;
    PFNGLDELETESHADERPROC             glDeleteShader;
    PFNGLDETACHSHADERPROC             glDetachShader;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
    PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
    PFNGLGETACTIVEATTRIBPROC          glGetActiveAttrib;
    PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform;
    PFNGLGETATTACHEDSHADERSPROC       glGetAttachedShaders;
    PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
    PFNGLGETPROGRAMIVPROC             glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
    PFNGLGETSHADERIVPROC              glGetShaderiv;
    PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
    PFNGLGETSHADERSOURCEPROC          glGetShaderSource;
    PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
    PFNGLGETUNIFORMFVPROC             glGetUniformfv;
    PFNGLGETUNIFORMIVPROC             glGetUniformiv;
    PFNGLGETVERTEXATTRIBDVPROC        glGetVertexAttribdv;
    PFNGLGETVERTEXATTRIBFVPROC        glGetVertexAttribfv;
    PFNGLGETVERTEXATTRIBIVPROC        glGetVertexAttribiv;
    PFNGLGETVERTEXATTRIBPOINTERVPROC  glGetVertexAttribPointerv;
    PFNGLISPROGRAMPROC                glIsProgram;
    PFNGLISSHADERPROC                 glIsShader;
    PFNGLLINKPROGRAMPROC              glLinkProgram;
    PFNGLSHADERSOURCEPROC             glShaderSource;
    PFNGLUSEPROGRAMPROC               glUseProgram;
    PFNGLUNIFORM1FPROC                glUniform1f;
    PFNGLUNIFORM2FPROC                glUniform2f;
    PFNGLUNIFORM3FPROC                glUniform3f;
    PFNGLUNIFORM4FPROC                glUniform4f;
    PFNGLUNIFORM1IPROC                glUniform1i;
    PFNGLUNIFORM2IPROC                glUniform2i;
    PFNGLUNIFORM3IPROC                glUniform3i;
    PFNGLUNIFORM4IPROC                glUniform4i;
    PFNGLUNIFORM1FVPROC               glUniform1fv;
    PFNGLUNIFORM2FVPROC               glUniform2fv;
    PFNGLUNIFORM3FVPROC               glUniform3fv;
    PFNGLUNIFORM4FVPROC               glUniform4fv;
    PFNGLUNIFORM1IVPROC               glUniform1iv;
    PFNGLUNIFORM2IVPROC               glUniform2iv;
    PFNGLUNIFORM3IVPROC               glUniform3iv;
    PFNGLUNIFORM4IVPROC               glUniform4iv;
    PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv;
    PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
    PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
    PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
    PFNGLVERTEXATTRIB1DPROC           glVertexAttrib1d;
    PFNGLVERTEXATTRIB1DVPROC          glVertexAttrib1dv;
    PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f;
    PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv;
    PFNGLVERTEXATTRIB1SPROC           glVertexAttrib1s;
    PFNGLVERTEXATTRIB1SVPROC          glVertexAttrib1sv;
    PFNGLVERTEXATTRIB2DPROC           glVertexAttrib2d;
    PFNGLVERTEXATTRIB2DVPROC          glVertexAttrib2dv;
    PFNGLVERTEXATTRIB2FPROC           glVertexAttrib2f;
    PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv;
    PFNGLVERTEXATTRIB2SPROC           glVertexAttrib2s;
    PFNGLVERTEXATTRIB2SVPROC          glVertexAttrib2sv;
    PFNGLVERTEXATTRIB3DPROC           glVertexAttrib3d;
    PFNGLVERTEXATTRIB3DVPROC          glVertexAttrib3dv;
    PFNGLVERTEXATTRIB3FPROC           glVertexAttrib3f;
    PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv;
    PFNGLVERTEXATTRIB3SPROC           glVertexAttrib3s;
    PFNGLVERTEXATTRIB3SVPROC          glVertexAttrib3sv;
    PFNGLVERTEXATTRIB4NBVPROC         glVertexAttrib4Nbv;
    PFNGLVERTEXATTRIB4NIVPROC         glVertexAttrib4Niv;
    PFNGLVERTEXATTRIB4NSVPROC         glVertexAttrib4Nsv;
    PFNGLVERTEXATTRIB4NUBPROC         glVertexAttrib4Nub;
    PFNGLVERTEXATTRIB4NUBVPROC        glVertexAttrib4Nubv;
    PFNGLVERTEXATTRIB4NUIVPROC        glVertexAttrib4Nuiv;
    PFNGLVERTEXATTRIB4NUSVPROC        glVertexAttrib4Nusv;
    PFNGLVERTEXATTRIB4BVPROC          glVertexAttrib4bv;
    PFNGLVERTEXATTRIB4DPROC           glVertexAttrib4d;
    PFNGLVERTEXATTRIB4DVPROC          glVertexAttrib4dv;
    PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f;
    PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv;
    PFNGLVERTEXATTRIB4IVPROC          glVertexAttrib4iv;
    PFNGLVERTEXATTRIB4SPROC           glVertexAttrib4s;
    PFNGLVERTEXATTRIB4SVPROC          glVertexAttrib4sv;
    PFNGLVERTEXATTRIB4UBVPROC         glVertexAttrib4ubv;
    PFNGLVERTEXATTRIB4UIVPROC         glVertexAttrib4uiv;
    PFNGLVERTEXATTRIB4USVPROC         glVertexAttrib4usv;
    PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;

        public: //! @name OpenGL 2.1

    PFNGLUNIFORMMATRIX2X3FVPROC       glUniformMatrix2x3fv;
    PFNGLUNIFORMMATRIX3X2FVPROC       glUniformMatrix3x2fv;
    PFNGLUNIFORMMATRIX2X4FVPROC       glUniformMatrix2x4fv;
    PFNGLUNIFORMMATRIX4X2FVPROC       glUniformMatrix4x2fv;
    PFNGLUNIFORMMATRIX3X4FVPROC       glUniformMatrix3x4fv;
    PFNGLUNIFORMMATRIX4X3FVPROC       glUniformMatrix4x3fv;

        public: //! @name GL_ARB_framebuffer_object (added to OpenGL 3.0 core)

    PFNGLISRENDERBUFFERPROC                      glIsRenderbuffer;
    PFNGLBINDRENDERBUFFERPROC                    glBindRenderbuffer;
    PFNGLDELETERENDERBUFFERSPROC                 glDeleteRenderbuffers;
    PFNGLGENRENDERBUFFERSPROC                    glGenRenderbuffers;
    PFNGLRENDERBUFFERSTORAGEPROC                 glRenderbufferStorage;
    PFNGLGETRENDERBUFFERPARAMETERIVPROC          glGetRenderbufferParameteriv;
    PFNGLISFRAMEBUFFERPROC                       glIsFramebuffer;
    PFNGLBINDFRAMEBUFFERPROC                     glBindFramebuffer;
    PFNGLDELETEFRAMEBUFFERSPROC                  glDeleteFramebuffers;
    PFNGLGENFRAMEBUFFERSPROC                     glGenFramebuffers;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC              glCheckFramebufferStatus;
    PFNGLFRAMEBUFFERTEXTURE1DPROC                glFramebufferTexture1D;
    PFNGLFRAMEBUFFERTEXTURE2DPROC                glFramebufferTexture2D;
    PFNGLFRAMEBUFFERTEXTURE3DPROC                glFramebufferTexture3D;
    PFNGLFRAMEBUFFERRENDERBUFFERPROC             glFramebufferRenderbuffer;
    PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
    PFNGLGENERATEMIPMAPPROC                      glGenerateMipmap;
    PFNGLBLITFRAMEBUFFERPROC                     glBlitFramebuffer;
    PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC      glRenderbufferStorageMultisample;
    PFNGLFRAMEBUFFERTEXTURELAYERPROC             glFramebufferTextureLayer;

        public: //! @name GL_ARB_vertex_array_object (added to OpenGL 3.0 core)

    PFNGLBINDVERTEXARRAYPROC             glBindVertexArray;
    PFNGLDELETEVERTEXARRAYSPROC          glDeleteVertexArrays;
    PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays;
    PFNGLISVERTEXARRAYPROC               glIsVertexArray;

        public: //! @name GL_ARB_map_buffer_range (added to OpenGL 3.0 core)

    PFNGLMAPBUFFERRANGEPROC              glMapBufferRange;
    PFNGLFLUSHMAPPEDBUFFERRANGEPROC      glFlushMappedBufferRange;

        public: //! @name OpenGL 3.0

    PFNGLCOLORMASKIPROC                  glColorMaski;
    PFNGLGETBOOLEANI_VPROC               glGetBooleani_v;
    PFNGLGETINTEGERI_VPROC               glGetIntegeri_v;
    PFNGLENABLEIPROC                     glEnablei;
    PFNGLDISABLEIPROC                    glDisablei;
    PFNGLISENABLEDIPROC                  glIsEnabledi;
    PFNGLBEGINTRANSFORMFEEDBACKPROC      glBeginTransformFeedback;
    PFNGLENDTRANSFORMFEEDBACKPROC        glEndTransformFeedback;
    PFNGLBINDBUFFERRANGEPROC             glBindBufferRange;
    PFNGLBINDBUFFERBASEPROC              glBindBufferBase;
    PFNGLTRANSFORMFEEDBACKVARYINGSPROC   glTransformFeedbackVaryings;
    PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
    PFNGLCLAMPCOLORPROC                  glClampColor;
    PFNGLBEGINCONDITIONALRENDERPROC      glBeginConditionalRender;
    PFNGLENDCONDITIONALRENDERPROC        glEndConditionalRender;
    PFNGLVERTEXATTRIBIPOINTERPROC        glVertexAttribIPointer;
    PFNGLGETVERTEXATTRIBIIVPROC          glGetVertexAttribIiv;
    PFNGLGETVERTEXATTRIBIUIVPROC         glGetVertexAttribIuiv;
    PFNGLVERTEXATTRIBI1IPROC             glVertexAttribI1i;
    PFNGLVERTEXATTRIBI2IPROC             glVertexAttribI2i;
    PFNGLVERTEXATTRIBI3IPROC             glVertexAttribI3i;
    PFNGLVERTEXATTRIBI4IPROC             glVertexAttribI4i;
    PFNGLVERTEXATTRIBI1UIPROC            glVertexAttribI1ui;
    PFNGLVERTEXATTRIBI2UIPROC            glVertexAttribI2ui;
    PFNGLVERTEXATTRIBI3UIPROC            glVertexAttribI3ui;
    PFNGLVERTEXATTRIBI4UIPROC            glVertexAttribI4ui;
    PFNGLVERTEXATTRIBI1IVPROC            glVertexAttribI1iv;
    PFNGLVERTEXATTRIBI2IVPROC            glVertexAttribI2iv;
    PFNGLVERTEXATTRIBI3IVPROC            glVertexAttribI3iv;
    PFNGLVERTEXATTRIBI4IVPROC            glVertexAttribI4iv;
    PFNGLVERTEXATTRIBI1UIVPROC           glVertexAttribI1uiv;
    PFNGLVERTEXATTRIBI2UIVPROC           glVertexAttribI2uiv;
    PFNGLVERTEXATTRIBI3UIVPROC           glVertexAttribI3uiv;
    PFNGLVERTEXATTRIBI4UIVPROC           glVertexAttribI4uiv;
    PFNGLVERTEXATTRIBI4BVPROC            glVertexAttribI4bv;
    PFNGLVERTEXATTRIBI4SVPROC            glVertexAttribI4sv;
    PFNGLVERTEXATTRIBI4UBVPROC           glVertexAttribI4ubv;
    PFNGLVERTEXATTRIBI4USVPROC           glVertexAttribI4usv;
    PFNGLGETUNIFORMUIVPROC               glGetUniformuiv;
    PFNGLBINDFRAGDATALOCATIONPROC        glBindFragDataLocation;
    PFNGLGETFRAGDATALOCATIONPROC         glGetFragDataLocation;
    PFNGLUNIFORM1UIPROC                  glUniform1ui;
    PFNGLUNIFORM2UIPROC                  glUniform2ui;
    PFNGLUNIFORM3UIPROC                  glUniform3ui;
    PFNGLUNIFORM4UIPROC                  glUniform4ui;
    PFNGLUNIFORM1UIVPROC                 glUniform1uiv;
    PFNGLUNIFORM2UIVPROC                 glUniform2uiv;
    PFNGLUNIFORM3UIVPROC                 glUniform3uiv;
    PFNGLUNIFORM4UIVPROC                 glUniform4uiv;
    PFNGLTEXPARAMETERIIVPROC             glTexParameterIiv;
    PFNGLTEXPARAMETERIUIVPROC            glTexParameterIuiv;
    PFNGLGETTEXPARAMETERIIVPROC          glGetTexParameterIiv;
    PFNGLGETTEXPARAMETERIUIVPROC         glGetTexParameterIuiv;
    PFNGLCLEARBUFFERIVPROC               glClearBufferiv;
    PFNGLCLEARBUFFERUIVPROC              glClearBufferuiv;
    PFNGLCLEARBUFFERFVPROC               glClearBufferfv;
    PFNGLCLEARBUFFERFIPROC               glClearBufferfi;
    PFNGLGETSTRINGIPROC                  glGetStringi;

        public: //! @name GL_ARB_uniform_buffer_object (added to OpenGL 3.1 core)

    PFNGLGETUNIFORMINDICESPROC           glGetUniformIndices;
    PFNGLGETACTIVEUNIFORMSIVPROC         glGetActiveUniformsiv;
    PFNGLGETACTIVEUNIFORMNAMEPROC        glGetActiveUniformName;
    PFNGLGETUNIFORMBLOCKINDEXPROC        glGetUniformBlockIndex;
    PFNGLGETACTIVEUNIFORMBLOCKIVPROC     glGetActiveUniformBlockiv;
    PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC   glGetActiveUniformBlockName;
    PFNGLUNIFORMBLOCKBINDINGPROC         glUniformBlockBinding;

        public: //! @name GL_ARB_copy_buffer (added to OpenGL 3.1 core)

    PFNGLCOPYBUFFERSUBDATAPROC           glCopyBufferSubData;

        public: //! @name OpenGL 3.1

    PFNGLDRAWARRAYSINSTANCEDPROC         glDrawArraysInstanced;
    PFNGLDRAWELEMENTSINSTANCEDPROC       glDrawElementsInstanced;
    PFNGLTEXBUFFERPROC                   glTexBuffer;
    PFNGLPRIMITIVERESTARTINDEXPROC       glPrimitiveRestartIndex;

        public: //! @name GL_ARB_draw_elements_base_vertex (added to OpenGL 3.2 core)

    PFNGLDRAWELEMENTSBASEVERTEXPROC      glDrawElementsBaseVertex;
    PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
    PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
    PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;

        public: //! @name GL_ARB_provoking_vertex (added to OpenGL 3.2 core)

    PFNGLPROVOKINGVERTEXPROC             glProvokingVertex;

        public: //! @name GL_ARB_sync (added to OpenGL 3.2 core)

    PFNGLFENCESYNCPROC                   glFenceSync;
    PFNGLISSYNCPROC                      glIsSync;
    PFNGLDELETESYNCPROC                  glDeleteSync;
    PFNGLCLIENTWAITSYNCPROC              glClientWaitSync;
    PFNGLWAITSYNCPROC                    glWaitSync;
    PFNGLGETINTEGER64VPROC               glGetInteger64v;
    PFNGLGETSYNCIVPROC                   glGetSynciv;

        public: //! @name GL_ARB_texture_multisample (added to OpenGL 3.2 core)

    PFNGLTEXIMAGE2DMULTISAMPLEPROC       glTexImage2DMultisample;
    PFNGLTEXIMAGE3DMULTISAMPLEPROC       glTexImage3DMultisample;
    PFNGLGETMULTISAMPLEFVPROC            glGetMultisamplefv;
    PFNGLSAMPLEMASKIPROC                 glSampleMaski;

        public: //! @name OpenGL 3.2

    PFNGLGETINTEGER64I_VPROC             glGetInteger64i_v;
    PFNGLGETBUFFERPARAMETERI64VPROC      glGetBufferParameteri64v;
    PFNGLFRAMEBUFFERTEXTUREPROC          glFramebufferTexture;

        public: //! @name GL_ARB_blend_func_extended (added to OpenGL 3.3 core)

    PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
    PFNGLGETFRAGDATAINDEXPROC            glGetFragDataIndex;

        public: //! @name GL_ARB_sampler_objects (added to OpenGL 3.3 core)

    PFNGLGENSAMPLERSPROC                 glGenSamplers;
    PFNGLDELETESAMPLERSPROC              glDeleteSamplers;
    PFNGLISSAMPLERPROC                   glIsSampler;
    PFNGLBINDSAMPLERPROC                 glBindSampler;
    PFNGLSAMPLERPARAMETERIPROC           glSamplerParameteri;
    PFNGLSAMPLERPARAMETERIVPROC          glSamplerParameteriv;
    PFNGLSAMPLERPARAMETERFPROC           glSamplerParameterf;
    PFNGLSAMPLERPARAMETERFVPROC          glSamplerParameterfv;
    PFNGLSAMPLERPARAMETERIIVPROC         glSamplerParameterIiv;
    PFNGLSAMPLERPARAMETERIUIVPROC        glSamplerParameterIuiv;
    PFNGLGETSAMPLERPARAMETERIVPROC       glGetSamplerParameteriv;
    PFNGLGETSAMPLERPARAMETERIIVPROC      glGetSamplerParameterIiv;
    PFNGLGETSAMPLERPARAMETERFVPROC       glGetSamplerParameterfv;
    PFNGLGETSAMPLERPARAMETERIUIVPROC     glGetSamplerParameterIuiv;

        public: //! @name GL_ARB_timer_query (added to OpenGL 3.3 core)

    PFNGLQUERYCOUNTERPROC                glQueryCounter;
    PFNGLGETQUERYOBJECTI64VPROC          glGetQueryObjecti64v;
    PFNGLGETQUERYOBJECTUI64VPROC         glGetQueryObjectui64v;

        public: //! @name GL_ARB_vertex_type_2_10_10_10_rev (added to OpenGL 3.3 core)

    PFNGLVERTEXP2UIPROC                  glVertexP2ui;
    PFNGLVERTEXP2UIVPROC                 glVertexP2uiv;
    PFNGLVERTEXP3UIPROC                  glVertexP3ui;
    PFNGLVERTEXP3UIVPROC                 glVertexP3uiv;
    PFNGLVERTEXP4UIPROC                  glVertexP4ui;
    PFNGLVERTEXP4UIVPROC                 glVertexP4uiv;
    PFNGLTEXCOORDP1UIPROC                glTexCoordP1ui;
    PFNGLTEXCOORDP1UIVPROC               glTexCoordP1uiv;
    PFNGLTEXCOORDP2UIPROC                glTexCoordP2ui;
    PFNGLTEXCOORDP2UIVPROC               glTexCoordP2uiv;
    PFNGLTEXCOORDP3UIPROC                glTexCoordP3ui;
    PFNGLTEXCOORDP3UIVPROC               glTexCoordP3uiv;
    PFNGLTEXCOORDP4UIPROC                glTexCoordP4ui;
    PFNGLTEXCOORDP4UIVPROC               glTexCoordP4uiv;
    PFNGLMULTITEXCOORDP1UIPROC           glMultiTexCoordP1ui;
    PFNGLMULTITEXCOORDP1UIVPROC          glMultiTexCoordP1uiv;
    PFNGLMULTITEXCOORDP2UIPROC           glMultiTexCoordP2ui;
    PFNGLMULTITEXCOORDP2UIVPROC          glMultiTexCoordP2uiv;
    PFNGLMULTITEXCOORDP3UIPROC           glMultiTexCoordP3ui;
    PFNGLMULTITEXCOORDP3UIVPROC          glMultiTexCoordP3uiv;
    PFNGLMULTITEXCOORDP4UIPROC           glMultiTexCoordP4ui;
    PFNGLMULTITEXCOORDP4UIVPROC          glMultiTexCoordP4uiv;
    PFNGLNORMALP3UIPROC                  glNormalP3ui;
    PFNGLNORMALP3UIVPROC                 glNormalP3uiv;
    PFNGLCOLORP3UIPROC                   glColorP3ui;
    PFNGLCOLORP3UIVPROC                  glColorP3uiv;
    PFNGLCOLORP4UIPROC                   glColorP4ui;
    PFNGLCOLORP4UIVPROC                  glColorP4uiv;
    PFNGLSECONDARYCOLORP3UIPROC          glSecondaryColorP3ui;
    PFNGLSECONDARYCOLORP3UIVPROC         glSecondaryColorP3uiv;
    PFNGLVERTEXATTRIBP1UIPROC            glVertexAttribP1ui;
    PFNGLVERTEXATTRIBP1UIVPROC           glVertexAttribP1uiv;
    PFNGLVERTEXATTRIBP2UIPROC            glVertexAttribP2ui;
    PFNGLVERTEXATTRIBP2UIVPROC           glVertexAttribP2uiv;
    PFNGLVERTEXATTRIBP3UIPROC            glVertexAttribP3ui;
    PFNGLVERTEXATTRIBP3UIVPROC           glVertexAttribP3uiv;
    PFNGLVERTEXATTRIBP4UIPROC            glVertexAttribP4ui;
    PFNGLVERTEXATTRIBP4UIVPROC           glVertexAttribP4uiv;

        public: //! @name OpenGL 3.3

    PFNGLVERTEXATTRIBDIVISORPROC         glVertexAttribDivisor;

        public: //! @name GL_ARB_draw_indirect (added to OpenGL 4.0 core)

    PFNGLDRAWARRAYSINDIRECTPROC          glDrawArraysIndirect;
    PFNGLDRAWELEMENTSINDIRECTPROC        glDrawElementsIndirect;

        public: //! @name GL_ARB_gpu_shader_fp64 (added to OpenGL 4.0 core)

    PFNGLUNIFORM1DPROC                   glUniform1d;
    PFNGLUNIFORM2DPROC                   glUniform2d;
    PFNGLUNIFORM3DPROC                   glUniform3d;
    PFNGLUNIFORM4DPROC                   glUniform4d;
    PFNGLUNIFORM1DVPROC                  glUniform1dv;
    PFNGLUNIFORM2DVPROC                  glUniform2dv;
    PFNGLUNIFORM3DVPROC                  glUniform3dv;
    PFNGLUNIFORM4DVPROC                  glUniform4dv;
    PFNGLUNIFORMMATRIX2DVPROC            glUniformMatrix2dv;
    PFNGLUNIFORMMATRIX3DVPROC            glUniformMatrix3dv;
    PFNGLUNIFORMMATRIX4DVPROC            glUniformMatrix4dv;
    PFNGLUNIFORMMATRIX2X3DVPROC          glUniformMatrix2x3dv;
    PFNGLUNIFORMMATRIX2X4DVPROC          glUniformMatrix2x4dv;
    PFNGLUNIFORMMATRIX3X2DVPROC          glUniformMatrix3x2dv;
    PFNGLUNIFORMMATRIX3X4DVPROC          glUniformMatrix3x4dv;
    PFNGLUNIFORMMATRIX4X2DVPROC          glUniformMatrix4x2dv;
    PFNGLUNIFORMMATRIX4X3DVPROC          glUniformMatrix4x3dv;
    PFNGLGETUNIFORMDVPROC                glGetUniformdv;

        public: //! @name GL_ARB_shader_subroutine (added to OpenGL 4.0 core)

    PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC   glGetSubroutineUniformLocation;
    PFNGLGETSUBROUTINEINDEXPROC             glGetSubroutineIndex;
    PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC   glGetActiveSubroutineUniformiv;
    PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
    PFNGLGETACTIVESUBROUTINENAMEPROC        glGetActiveSubroutineName;
    PFNGLUNIFORMSUBROUTINESUIVPROC          glUniformSubroutinesuiv;
    PFNGLGETUNIFORMSUBROUTINEUIVPROC        glGetUniformSubroutineuiv;
    PFNGLGETPROGRAMSTAGEIVPROC              glGetProgramStageiv;

        public: //! @name GL_ARB_tessellation_shader (added to OpenGL 4.0 core)

    PFNGLPATCHPARAMETERIPROC             glPatchParameteri;
    PFNGLPATCHPARAMETERFVPROC            glPatchParameterfv;

        public: //! @name GL_ARB_transform_feedback2 (added to OpenGL 4.0 core)

    PFNGLBINDTRANSFORMFEEDBACKPROC       glBindTransformFeedback;
    PFNGLDELETETRANSFORMFEEDBACKSPROC    glDeleteTransformFeedbacks;
    PFNGLGENTRANSFORMFEEDBACKSPROC       glGenTransformFeedbacks;
    PFNGLISTRANSFORMFEEDBACKPROC         glIsTransformFeedback;
    PFNGLPAUSETRANSFORMFEEDBACKPROC      glPauseTransformFeedback;
    PFNGLRESUMETRANSFORMFEEDBACKPROC     glResumeTransformFeedback;
    PFNGLDRAWTRANSFORMFEEDBACKPROC       glDrawTransformFeedback;

        public: //! @name GL_ARB_transform_feedback3 (added to OpenGL 4.0 core)

    PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
    PFNGLBEGINQUERYINDEXEDPROC           glBeginQueryIndexed;
    PFNGLENDQUERYINDEXEDPROC             glEndQueryIndexed;
    PFNGLGETQUERYINDEXEDIVPROC           glGetQueryIndexediv;

        public: //! @name OpenGL 4.0

    PFNGLMINSAMPLESHADINGPROC            glMinSampleShading;
    PFNGLBLENDEQUATIONIPROC              glBlendEquationi;
    PFNGLBLENDEQUATIONSEPARATEIPROC      glBlendEquationSeparatei;
    PFNGLBLENDFUNCIPROC                  glBlendFunci;
    PFNGLBLENDFUNCSEPARATEIPROC          glBlendFuncSeparatei;

        public: //! @name GL_ARB_ES2_compatibility (added to OpenGL 4.1 core)

    PFNGLRELEASESHADERCOMPILERPROC       glReleaseShaderCompiler;
    PFNGLSHADERBINARYPROC                glShaderBinary;
    PFNGLGETSHADERPRECISIONFORMATPROC    glGetShaderPrecisionFormat;
    PFNGLDEPTHRANGEFPROC                 glDepthRangef;
    PFNGLCLEARDEPTHFPROC                 glClearDepthf;

        public: //! @name GL_ARB_get_program_binary (added to OpenGL 4.1 core)

    PFNGLGETPROGRAMBINARYPROC            glGetProgramBinary;
    PFNGLPROGRAMBINARYPROC               glProgramBinary;
    PFNGLPROGRAMPARAMETERIPROC           glProgramParameteri;

        public: //! @name GL_ARB_separate_shader_objects (added to OpenGL 4.1 core)

    PFNGLUSEPROGRAMSTAGESPROC            glUseProgramStages;
    PFNGLACTIVESHADERPROGRAMPROC         glActiveShaderProgram;
    PFNGLCREATESHADERPROGRAMVPROC        glCreateShaderProgramv;
    PFNGLBINDPROGRAMPIPELINEPROC         glBindProgramPipeline;
    PFNGLDELETEPROGRAMPIPELINESPROC      glDeleteProgramPipelines;
    PFNGLGENPROGRAMPIPELINESPROC         glGenProgramPipelines;
    PFNGLISPROGRAMPIPELINEPROC           glIsProgramPipeline;
    PFNGLGETPROGRAMPIPELINEIVPROC        glGetProgramPipelineiv;
    PFNGLPROGRAMUNIFORM1IPROC            glProgramUniform1i;
    PFNGLPROGRAMUNIFORM1IVPROC           glProgramUniform1iv;
    PFNGLPROGRAMUNIFORM1FPROC            glProgramUniform1f;
    PFNGLPROGRAMUNIFORM1FVPROC           glProgramUniform1fv;
    PFNGLPROGRAMUNIFORM1DPROC            glProgramUniform1d;
    PFNGLPROGRAMUNIFORM1DVPROC           glProgramUniform1dv;
    PFNGLPROGRAMUNIFORM1UIPROC           glProgramUniform1ui;
    PFNGLPROGRAMUNIFORM1UIVPROC          glProgramUniform1uiv;
    PFNGLPROGRAMUNIFORM2IPROC            glProgramUniform2i;
    PFNGLPROGRAMUNIFORM2IVPROC           glProgramUniform2iv;
    PFNGLPROGRAMUNIFORM2FPROC            glProgramUniform2f;
    PFNGLPROGRAMUNIFORM2FVPROC           glProgramUniform2fv;
    PFNGLPROGRAMUNIFORM2DPROC            glProgramUniform2d;
    PFNGLPROGRAMUNIFORM2DVPROC           glProgramUniform2dv;
    PFNGLPROGRAMUNIFORM2UIPROC           glProgramUniform2ui;
    PFNGLPROGRAMUNIFORM2UIVPROC          glProgramUniform2uiv;
    PFNGLPROGRAMUNIFORM3IPROC            glProgramUniform3i;
    PFNGLPROGRAMUNIFORM3IVPROC           glProgramUniform3iv;
    PFNGLPROGRAMUNIFORM3FPROC            glProgramUniform3f;
    PFNGLPROGRAMUNIFORM3FVPROC           glProgramUniform3fv;
    PFNGLPROGRAMUNIFORM3DPROC            glProgramUniform3d;
    PFNGLPROGRAMUNIFORM3DVPROC           glProgramUniform3dv;
    PFNGLPROGRAMUNIFORM3UIPROC           glProgramUniform3ui;
    PFNGLPROGRAMUNIFORM3UIVPROC          glProgramUniform3uiv;
    PFNGLPROGRAMUNIFORM4IPROC            glProgramUniform4i;
    PFNGLPROGRAMUNIFORM4IVPROC           glProgramUniform4iv;
    PFNGLPROGRAMUNIFORM4FPROC            glProgramUniform4f;
    PFNGLPROGRAMUNIFORM4FVPROC           glProgramUniform4fv;
    PFNGLPROGRAMUNIFORM4DPROC            glProgramUniform4d;
    PFNGLPROGRAMUNIFORM4DVPROC           glProgramUniform4dv;
    PFNGLPROGRAMUNIFORM4UIPROC           glProgramUniform4ui;
    PFNGLPROGRAMUNIFORM4UIVPROC          glProgramUniform4uiv;
    PFNGLPROGRAMUNIFORMMATRIX2FVPROC     glProgramUniformMatrix2fv;
    PFNGLPROGRAMUNIFORMMATRIX3FVPROC     glProgramUniformMatrix3fv;
    PFNGLPROGRAMUNIFORMMATRIX4FVPROC     glProgramUniformMatrix4fv;
    PFNGLPROGRAMUNIFORMMATRIX2DVPROC     glProgramUniformMatrix2dv;
    PFNGLPROGRAMUNIFORMMATRIX3DVPROC     glProgramUniformMatrix3dv;
    PFNGLPROGRAMUNIFORMMATRIX4DVPROC     glProgramUniformMatrix4dv;
    PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC   glProgramUniformMatrix2x3fv;
    PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC   glProgramUniformMatrix3x2fv;
    PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC   glProgramUniformMatrix2x4fv;
    PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC   glProgramUniformMatrix4x2fv;
    PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC   glProgramUniformMatrix3x4fv;
    PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC   glProgramUniformMatrix4x3fv;
    PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC   glProgramUniformMatrix2x3dv;
    PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC   glProgramUniformMatrix3x2dv;
    PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC   glProgramUniformMatrix2x4dv;
    PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC   glProgramUniformMatrix4x2dv;
    PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC   glProgramUniformMatrix3x4dv;
    PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC   glProgramUniformMatrix4x3dv;
    PFNGLVALIDATEPROGRAMPIPELINEPROC     glValidateProgramPipeline;
    PFNGLGETPROGRAMPIPELINEINFOLOGPROC   glGetProgramPipelineInfoLog;

        public: //! @name GL_ARB_vertex_attrib_64bit (added to OpenGL 4.1 core)

    PFNGLVERTEXATTRIBL1DPROC             glVertexAttribL1d;
    PFNGLVERTEXATTRIBL2DPROC             glVertexAttribL2d;
    PFNGLVERTEXATTRIBL3DPROC             glVertexAttribL3d;
    PFNGLVERTEXATTRIBL4DPROC             glVertexAttribL4d;
    PFNGLVERTEXATTRIBL1DVPROC            glVertexAttribL1dv;
    PFNGLVERTEXATTRIBL2DVPROC            glVertexAttribL2dv;
    PFNGLVERTEXATTRIBL3DVPROC            glVertexAttribL3dv;
    PFNGLVERTEXATTRIBL4DVPROC            glVertexAttribL4dv;
    PFNGLVERTEXATTRIBLPOINTERPROC        glVertexAttribLPointer;
    PFNGLGETVERTEXATTRIBLDVPROC          glGetVertexAttribLdv;

        public: //! @name GL_ARB_viewport_array (added to OpenGL 4.1 core)

    PFNGLVIEWPORTARRAYVPROC              glViewportArrayv;
    PFNGLVIEWPORTINDEXEDFPROC            glViewportIndexedf;
    PFNGLVIEWPORTINDEXEDFVPROC           glViewportIndexedfv;
    PFNGLSCISSORARRAYVPROC               glScissorArrayv;
    PFNGLSCISSORINDEXEDPROC              glScissorIndexed;
    PFNGLSCISSORINDEXEDVPROC             glScissorIndexedv;
    PFNGLDEPTHRANGEARRAYVPROC            glDepthRangeArrayv;
    PFNGLDEPTHRANGEINDEXEDPROC           glDepthRangeIndexed;
    PFNGLGETFLOATI_VPROC                 glGetFloati_v;
    PFNGLGETDOUBLEI_VPROC                glGetDoublei_v;

        public: //! @name OpenGL 4.1

    //

        public: //! @name GL_ARB_base_instance (added to OpenGL 4.2 core)

    PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC             glDrawArraysInstancedBaseInstance;
    PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC           glDrawElementsInstancedBaseInstance;
    PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;

        public: //! @name GL_ARB_transform_feedback_instanced (added to OpenGL 4.2 core)

    PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC              glDrawTransformFeedbackInstanced;
    PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC        glDrawTransformFeedbackStreamInstanced;

        public: //! @name GL_ARB_internalformat_query (added to OpenGL 4.2 core)

    PFNGLGETINTERNALFORMATIVPROC                         glGetInternalformativ;

        public: //! @name GL_ARB_shader_atomic_counters (added to OpenGL 4.2 core)

    PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC              glGetActiveAtomicCounterBufferiv;

        public: //! @name GL_ARB_shader_image_load_store (added to OpenGL 4.2 core)

    PFNGLBINDIMAGETEXTUREPROC                            glBindImageTexture;
    PFNGLMEMORYBARRIERPROC                               glMemoryBarrier;

        public: //! @name GL_ARB_texture_storage (added to OpenGL 4.2 core)

    PFNGLTEXSTORAGE1DPROC                                glTexStorage1D;
    PFNGLTEXSTORAGE2DPROC                                glTexStorage2D;
    PFNGLTEXSTORAGE3DPROC                                glTexStorage3D;
    PFNGLTEXTURESTORAGE1DEXTPROC                         glTextureStorage1DEXT;
    PFNGLTEXTURESTORAGE2DEXTPROC                         glTextureStorage2DEXT;
    PFNGLTEXTURESTORAGE3DEXTPROC                         glTextureStorage3DEXT;

        public: //! @name OpenGL 4.2

        public: //! @name OpenGL 4.3

#if(defined(_WIN32) || defined(__WIN32__))
        public: //! @name wgl extensions

    typedef const char* (WINAPI *wglGetExtensionsStringARB_t)(HDC theDeviceContext);
    wglGetExtensionsStringARB_t wglGetExtensionsStringARB;

    typedef BOOL        (WINAPI *wglSwapIntervalEXT_t)(int theInterval);
    wglSwapIntervalEXT_t wglSwapIntervalEXT;
#elif(defined(__APPLE__))
        public: //! @name CGL extensions

#else
        public: //! @name glX extensions

    // GLX_EXT_swap_control
    //typedef int         (*glXSwapIntervalEXT_t)(Display* theDisplay, GLXDrawable theDrawable, int theInterval);
    typedef int         (*glXSwapIntervalEXT_t)();
    glXSwapIntervalEXT_t glXSwapIntervalEXT;

    typedef int         (*glXSwapIntervalSGI_t)(int theInterval);
    glXSwapIntervalSGI_t glXSwapIntervalSGI;
#endif

};

#endif // __StGLFunctions_h_
