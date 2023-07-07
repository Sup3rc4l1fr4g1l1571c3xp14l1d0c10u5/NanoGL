#ifndef _NANOGL_NANOVG_EXTRA_H
#define _NANOGL_NANOVG_EXTRA_H

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>
#include <nanovg_gl_utils.h>

void nvgTranslateWorld(NVGcontext* ctx, float x, float y);
void nvgRotateWorld(NVGcontext* ctx, float angle);
void nvgSkewXWorld(NVGcontext* ctx, float angle);
void nvgSkewYWorld(NVGcontext* ctx, float angle);
void nvgScaleWorld(NVGcontext* ctx, float x, float y);

NVGpaint nvgImagePatternWithFillColor(NVGcontext* ctx,
								float cx, float cy, float w, float h, float angle,
								int image);	

/*
 * html5 canvasの drawImageメソッドを模したもの
 */
void nvgExDrawImage(NVGcontext* ctx, int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle, float alpha);
void nvgExDrawImageWithFillColor(NVGcontext* ctx, int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle);

#endif
