#include <nanovg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * html5 canvas‚Ì drawImageƒƒ\ƒbƒh‚ğ–Í‚µ‚½‚à‚Ì
 */
void nvgExDrawImage(NVGcontext* ctx, int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle, float alpha);
void nvgExDrawImageWithFillColor(NVGcontext* ctx, int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle);

#ifdef __cplusplus
}
#endif
