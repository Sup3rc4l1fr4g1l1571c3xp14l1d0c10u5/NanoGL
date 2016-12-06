#include "nanovg-extra-apis.h"


// html5 canvasの drawImageメソッドを模したもの
void nvgExDrawImage(NVGcontext* ctx, int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle, float alpha) {
	int w, h;
	float s[6], t[6], c[6];
	NVGpaint p;

	nvgImageSize(ctx, image, &w, &h);
	nvgSave(ctx);
	nvgBeginPath(ctx);
	nvgTransformScale(s, dw/sw,dh/sh);
	nvgTransformTranslate(t, dx, dy);
	p = nvgImagePattern(ctx, -sx, -sy, (float)w, (float)h, angle, image, alpha); // (-sx,-sy)を左上, 幅w, 高さhで画像imgを敷き詰めたマスクを作る
	
	// scale * translate * current  を算出
	nvgCurrentTransform(ctx, c);
	nvgTransformMultiply(s, t);
	nvgTransformMultiply(s, c);

	nvgResetTransform(ctx);
	nvgTransform(ctx, s[0], s[1], s[2], s[3], s[4], s[5]);
	nvgRect(ctx, 0, 0, sw, sh);
	nvgFillPaint(ctx, p);
	nvgFill(ctx);
	nvgRestore(ctx);
}

void nvgExDrawImageWithFillColor(NVGcontext* ctx, int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle) {
	int w, h;
	float s[6], t[6], c[6];
	NVGpaint p;

	nvgImageSize(ctx, image, &w, &h);
	nvgSave(ctx);
	nvgBeginPath(ctx);
	nvgTransformScale(s, dw/sw,dh/sh);
	nvgTransformTranslate(t, dx, dy);
	p = nvgImagePatternWithFillColor(ctx, -sx, -sy, (float)w, (float)h, angle, image); // (-sx,-sy)を左上, 幅w, 高さhで画像imgを敷き詰めたマスクを作る
	
	// scale * translate * current  を算出
	nvgCurrentTransform(ctx, c);
	nvgTransformMultiply(s, t);
	nvgTransformMultiply(s, c);

	nvgResetTransform(ctx);
	nvgTransform(ctx, s[0], s[1], s[2], s[3], s[4], s[5]);
	nvgRect(ctx, 0, 0, sw, sh);
	nvgFillPaint(ctx, p);
	nvgFill(ctx);
	nvgRestore(ctx);
}

