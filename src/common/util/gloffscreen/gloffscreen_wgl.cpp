/*
 *  Offscreen OpenGL abstraction layer - WGL (windows) specific
 *
 *  Copyright (c) 2010 Intel
 *  Written by: 
 *    Gordon Williams <gordon.williams@collabora.co.uk>
 *    Ian Molton <ian.molton@collabora.co.uk>
 *  Copyright (c) 2013 Wayo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <wingdi.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include "wglext.h"

#include "gloffscreen.h"

/* In Windows, you must create a window *before* you can create a pbuffer or
 * get a context. So we create a hidden Window on startup(see glo_init/GloMain).
 *
 * Also, you can't share contexts that have different pixel formats, so we can't
 * just create a new context from the window. We must create a whole new PBuffer
 * just for a context :(
 */

struct GloMain {
    HINSTANCE             hInstance;
    HDC                   hDC;
    HWND                  hWnd; /* Our hidden window */
    HGLRC                 hContext;
};

struct GloMain glo;
int glo_inited = 0;

struct _GloContext {
    /* Pixel format returned by wglChoosePixelFormat */
    int                   wglPixelFormat;
    /* We need a pbuffer to make a context of the right pixelformat :( */
    HPBUFFERARB           hPBuffer;
    HDC                   hDC;
    HGLRC                 hContext;
};

#define GLO_WINDOW_CLASS "QEmuGLClass"

extern HWND g_hEmuWindow;

/* Initialise gloffscreen */
static void glo_init(void) {
    PIXELFORMATDESCRIPTOR pfd;

    if (glo_inited) {
        fprintf(stderr, "gloffscreen already inited\n");
        abort();
    }

    /* Grab An Instance For Our Window */
    glo.hInstance = GetModuleHandle(NULL);
    glo.hDC = GetDC(g_hEmuWindow);

    /* Create a pixel format */
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;
    unsigned int pixelFormat = ChoosePixelFormat(glo.hDC, &pfd);
    DescribePixelFormat(glo.hDC,
                    pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    if (!SetPixelFormat(glo.hDC, pixelFormat, &pfd))
        return;

    /* Create a tempoary OpenGL 2 context */
    glo.hContext = wglCreateContext(glo.hDC);
    if (glo.hContext == NULL) {
        fprintf(stderr, "Unable to create GL context\n");
        abort();
    }
    wglMakeCurrent(glo.hDC, glo.hContext);

    /* Initialize glew */
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Glew init failed.\n");
        abort();
    }
	
    if (!WGLEW_ARB_create_context
        || !WGLEW_ARB_pixel_format
        || !WGLEW_ARB_pbuffer) {
        fprintf(stderr, "Unable to load the required WGL extensions\n");
        abort();
    }

    glo_inited = 1;
}

/* Uninitialise gloffscreen */
static void glo_kill(void) {
    if (glo.hContext) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(glo.hContext);
        glo.hContext = NULL;
    }
    if (glo.hDC) {
        ReleaseDC(glo.hWnd, glo.hDC);
        glo.hDC = NULL;
    }
    if (glo.hWnd) {
        DestroyWindow(glo.hWnd);
        glo.hWnd = NULL;
    }
    UnregisterClass(GLO_WINDOW_CLASS, glo.hInstance);
}

GloContext *glo_context_create(void) {
    if (!glo_inited)
      glo_init();

    GloContext *context = (GloContext *)malloc(sizeof(GloContext));
    memset(context, 0, sizeof(GloContext));

    /* Create the context proper */
    const int ctx_attri[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

	context->hDC = glo.hDC;
    context->hContext = wglCreateContextAttribsARB(context->hDC, 0, ctx_attri);
    if (context->hContext == NULL) {
        printf("Unable to create GL context\n");
        exit(EXIT_FAILURE);
    }

    glo_set_current(context);

	printf("---Opengl---\n");
	printf("%s\n", glGetString(GL_VERSION));
	printf("%s\n", glGetString(GL_VENDOR));
	printf("%s\n", glGetString(GL_RENDERER));
    return context;
}

/* Set current context */
void glo_set_current(GloContext *context) {

    if (context == NULL) {
        wglMakeCurrent(NULL, NULL);
    } else {
        wglMakeCurrent(context->hDC, context->hContext);
    }
}

/* Destroy a previously created OpenGL context */
void glo_context_destroy(GloContext *context)
{
    if (!context) return;

    wglMakeCurrent(NULL, NULL);
    if (context->hPBuffer != NULL) {
        wglReleasePbufferDCARB(context->hPBuffer, context->hDC);
        wglDestroyPbufferARB(context->hPBuffer);
    }
    if (context->hDC != NULL) {
        ReleaseDC(glo.hWnd, context->hDC);
    }
    if (context->hContext) {
        wglDeleteContext(context->hContext);
    }
    free(context);
}


void glo_swap(GloContext *context)
{
	if (!context) { return; }

	SwapBuffers(context->hDC);

}
