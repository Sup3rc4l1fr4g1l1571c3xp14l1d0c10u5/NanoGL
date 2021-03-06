#include <nanovg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * html5 canvasの drawImageメソッドを模したもの
 */
void nvgExDrawImage(NVGcontext* ctx, int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle, float alpha);
void nvgExDrawImageWithFillColor(NVGcontext* ctx, int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle);

#ifdef __cplusplus
}
#endif
