#include <NanoGL/NanoGL.h>
#include <string.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/dump.h>
#include <mruby/variable.h>
#include <mruby/hash.h>

////////////////////////////////////////////////////////////////////////////////
// NanoGL.Video 
////////////////////////////////////////////////////////////////////////////////

static bool ArrayToColor(mrb_state *mrb, mrb_value array, Color *color)
{
	if (!mrb_array_p(array)) {
		return false;
	}
	int len = RARRAY_LEN(array);
	if (len < 3) {
		return false;
	}
	uint8_t r = (uint8_t)mrb_fixnum(mrb_ary_ref(mrb, array, 0));
	uint8_t g = (uint8_t)mrb_fixnum(mrb_ary_ref(mrb, array, 1));
	uint8_t b = (uint8_t)mrb_fixnum(mrb_ary_ref(mrb, array, 2));
	uint8_t a = (len == 3) ? 0xFF : (uint8_t)mrb_fixnum(mrb_ary_ref(mrb, array, 3));
	*color = Video.RGBA(r, g, b, a);
	return true;
}

static bool ArrayToPaint(mrb_state *mrb, mrb_value array, Paint *paint)
{
	if (!mrb_array_p(array)) {
		return false;
	}
	int len = RARRAY_LEN(array);
	if (len != 19) {
		return false;
	}

	int n = 0;
	for (int i = 0; i < sizeof(paint->xform) / sizeof(paint->xform[0]); i++) {
		paint->xform[i] = (float)mrb_float(mrb_ary_ref(mrb, array, n++));
	}

	for (int i = 0; i < sizeof(paint->extent) / sizeof(paint->extent[0]); i++) {
		paint->extent[i] = (float)mrb_float(mrb_ary_ref(mrb, array, n++));
	}

	paint->radius = (float)mrb_float(mrb_ary_ref(mrb, array, n++));
	paint->feather = (float)mrb_float(mrb_ary_ref(mrb, array, n++));
	for (int i = 0; i < sizeof(paint->innerColor.rgba) / sizeof(paint->innerColor.rgba[0]); i++) {
		paint->innerColor.rgba[i] = (uint8_t)mrb_fixnum(mrb_ary_ref(mrb, array, n++));
	}
	for (int i = 0; i < sizeof(paint->outerColor.rgba) / sizeof(paint->outerColor.rgba[0]); i++) {
		paint->outerColor.rgba[i] = (uint8_t)mrb_fixnum(mrb_ary_ref(mrb, array, n++));
	}
	paint->image = mrb_fixnum(mrb_ary_ref(mrb, array, n));
	return true;
}


static mrb_value PaintToArray(mrb_state *mrb, Paint *paint)
{
	mrb_value array = mrb_ary_new_capa(mrb, 19);
	int n = 0;
	for (int i = 0; i < sizeof(paint->xform) / sizeof(paint->xform[0]); i++) {
		mrb_ary_set(mrb, array, n++, mrb_float_value(mrb, paint->xform[i]));
	}

	for (int i = 0; i < sizeof(paint->extent) / sizeof(paint->extent[0]); i++) {
		mrb_ary_set(mrb, array, n++, mrb_float_value(mrb, paint->extent[i]));
	}

	mrb_ary_set(mrb, array, n++, mrb_float_value(mrb, paint->radius));
	mrb_ary_set(mrb, array, n++, mrb_float_value(mrb, paint->feather));

	for (int i = 0; i < sizeof(paint->innerColor.rgba) / sizeof(paint->innerColor.rgba[0]); i++) {
		mrb_ary_set(mrb, array, n++, mrb_float_value(mrb, paint->innerColor.rgba[i]));
	}
	for (int i = 0; i < sizeof(paint->outerColor.rgba) / sizeof(paint->outerColor.rgba[0]); i++) {
		mrb_ary_set(mrb, array, n++, mrb_float_value(mrb, paint->outerColor.rgba[i]));
	}
	mrb_ary_set(mrb, array, n++, mrb_fixnum_value(paint->image));
	return array;
}

static bool MRBArrayToCArrayF(mrb_state *mrb, mrb_value array, float carray[], size_t cnt) {
	if (cnt >(size_t)RARRAY_LEN(array)) {
		return false;
	}
	for (size_t i = 0; i < cnt; i++) {
		carray[i] = (float)mrb_float(mrb_ary_ref(mrb, array, i));
	}
	return true;

}

static mrb_value CArrayToMRBArrayF(mrb_state *mrb, float carray[], size_t cnt) {
	mrb_value array = mrb_ary_new_capa(mrb, cnt);
	for (size_t i = 0; i < cnt; i++) {
		mrb_ary_set(mrb, array, i, mrb_float_value(mrb, carray[i]));
	}
	return array;
}

static mrb_value CArrayToMRBArrayI(mrb_state *mrb, int carray[], size_t cnt) {
	mrb_value array = mrb_ary_new_capa(mrb, cnt);
	for (size_t i = 0; i < cnt; i++) {
		mrb_ary_set(mrb, array, i, mrb_fixnum_value(carray[i]));
	}
	return array;
}

static mrb_value mrb_NanoGL_Video_SetWindowTitle(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *title;
	mrb_int ntitle;
	mrb_get_args(mrb, "s", &title, &ntitle);
	Video.SetWindowTitleUTF8(title);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_SetSize(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int width, height;
	mrb_get_args(mrb, "ii", &width, &height);
	Video.SetSize(width, height);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Drawing(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.Drawing();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_GetTime(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float t = (mrb_float)Video.GetTime();
	return mrb_float_value(mrb, t);
}

static mrb_value mrb_NanoGL_Video_Sleep(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float sec;
	mrb_get_args(mrb, "f", &sec);
	double sleeptime = Video.Sleep((double)sec);
	return mrb_float_value(mrb, (mrb_float)sleeptime);
}

static mrb_value mrb_NanoGL_Video_SetClearColor(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value color;
	mrb_get_args(mrb, "A", &color);
	Color c;
	if (ArrayToColor(mrb, color, &c)) {
		Video.SetClearColor(c);
	}
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Save(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.Save();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Restore(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.Restore();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Reset(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.Reset();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_StrokeColor(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value color;
	mrb_get_args(mrb, "A", &color);
	Color c;
	if (ArrayToColor(mrb, color, &c)) {
		Video.StrokeColor(c);
	}
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_StrokePaint(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value array;
	mrb_get_args(mrb, "A", &array);
	Paint p;
	if (ArrayToPaint(mrb, array, &p)) {
		Video.StrokePaint(p);
	}
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_FillColor(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value array;
	mrb_get_args(mrb, "A", &array);
	Color c;
	if (ArrayToColor(mrb, array, &c)) {
		Video.FillColor(c);
	}
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_FillPaint(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value array;
	mrb_get_args(mrb, "A", &array);
	Paint p;
	if (ArrayToPaint(mrb, array, &p)) {
		Video.FillPaint(p);
	}
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_MiterLimit(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float limit;
	mrb_get_args(mrb, "f", &limit);
	Video.MiterLimit((float)limit);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_StrokeWidth(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float width;
	mrb_get_args(mrb, "f", &width);
	Video.StrokeWidth((float)width);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_LineCap(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int style;
	mrb_get_args(mrb, "i", &style);
	Video.LineCap(style);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_LineJoin(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int style;
	mrb_get_args(mrb, "i", &style);
	Video.LineJoin(style);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_GlobalAlpha(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float alpha;
	mrb_get_args(mrb, "f", &alpha);
	Video.GlobalAlpha((float)alpha);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_ResetTransform(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.ResetTransform();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Transform(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value array;
	mrb_get_args(mrb, "A", &array);
	float xform[6];
	if (MRBArrayToCArrayF(mrb, array, xform, 6)) {
		Video.Transform(xform[0], xform[1], xform[2], xform[3], xform[4], xform[5]);
	}
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Translate(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	mrb_get_args(mrb, "ff", &x, &y);
	Video.Translate((float)x, (float)y);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Rotate(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float angle;
	mrb_get_args(mrb, "f", &angle);
	Video.Rotate((float)angle);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_SkewX(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float angle;
	mrb_get_args(mrb, "f", &angle);
	Video.SkewX((float)angle);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_SkewY(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float angle;
	mrb_get_args(mrb, "f", &angle);
	Video.SkewY((float)angle);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Scale(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	mrb_get_args(mrb, "ff", &x, &y);
	Video.Scale((float)x, (float)y);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_TranslateWorld(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	mrb_get_args(mrb, "ff", &x, &y);
	Video.TranslateWorld((float)x, (float)y);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_RotateWorld(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float angle;
	mrb_get_args(mrb, "f", &angle);
	Video.RotateWorld((float)angle);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_SkewXWorld(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float angle;
	mrb_get_args(mrb, "f", &angle);
	Video.SkewXWorld((float)angle);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_SkewYWorld(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float angle;
	mrb_get_args(mrb, "f", &angle);
	Video.SkewYWorld((float)angle);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_ScaleWorld(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	mrb_get_args(mrb, "ff", &x, &y);
	Video.ScaleWorld((float)x, (float)y);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_CurrentTransform(mrb_state *mrb, mrb_value self) {
	(void)self;
	float xform[6];
	Video.CurrentTransform(xform);
	return CArrayToMRBArrayF(mrb, xform, 6);

}

static mrb_value mrb_NanoGL_Video_TransformIdentity(mrb_state *mrb, mrb_value self) {
	(void)self;
	float dst[6];
	Video.TransformIdentity(dst);
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformTranslate(mrb_state *mrb, mrb_value self) {
	(void)self;
	float dst[6];
	mrb_float sx, sy;
	mrb_get_args(mrb, "ff", &sx, &sy);
	Video.TransformTranslate(dst, (float)sx, (float)sy);
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformRotate(mrb_state *mrb, mrb_value self) {
	(void)self;
	float dst[6];
	mrb_float a;
	mrb_get_args(mrb, "f", &a);
	Video.TransformRotate(dst, (float)a);
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformSkewX(mrb_state *mrb, mrb_value self) {
	(void)self;
	float dst[6];
	mrb_float a;
	mrb_get_args(mrb, "f", &a);
	Video.TransformSkewX(dst, (float)a);
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformSkewY(mrb_state *mrb, mrb_value self) {
	(void)self;
	float dst[6];
	mrb_float a;
	mrb_get_args(mrb, "f", &a);
	Video.TransformSkewY(dst, (float)a);
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformScale(mrb_state *mrb, mrb_value self) {
	(void)self;
	float dst[6];
	mrb_float sx, sy;
	mrb_get_args(mrb, "ff", &sx, &sy);
	Video.TransformScale(dst, (float)sx, (float)sy);
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformMultiply(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value array1, array2;
	mrb_get_args(mrb, "AA", &array1, &array2);
	float dst[6];
	float src[6];
	if (MRBArrayToCArrayF(mrb, array1, dst, 6) && MRBArrayToCArrayF(mrb, array2, src, 6)) {
		Video.TransformMultiply(dst, src);
	}
	else
	{
		Video.TransformIdentity(dst);
	}
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformPremultiply(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value array1, array2;
	mrb_get_args(mrb, "AA", &array1, &array2);
	float dst[6];
	float src[6];
	if (MRBArrayToCArrayF(mrb, array1, dst, 6) && MRBArrayToCArrayF(mrb, array2, src, 6)) {
		Video.TransformPremultiply(dst, src);
	}
	else
	{
		Video.TransformIdentity(dst);
	}
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformInverse(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value array;
	mrb_get_args(mrb, "A", &array);
	float dst[6];
	float src[6];
	if (MRBArrayToCArrayF(mrb, array, src, 6)) {
		Video.TransformInverse(dst, src);
	}
	else
	{
		Video.TransformIdentity(dst);
	}
	return CArrayToMRBArrayF(mrb, dst, 6);
}

static mrb_value mrb_NanoGL_Video_TransformPoint(mrb_state *mrb, mrb_value self) {
	(void)self;
	float dst[2], xform[6], src[2];
	mrb_value array1, array2;
	mrb_get_args(mrb, "AA", &array1, &array2);

	if (MRBArrayToCArrayF(mrb, array1, xform, 6) && MRBArrayToCArrayF(mrb, array2, src, 2))
	{
		Video.TransformPoint(&dst[0], &dst[1], xform, src[0], src[1]);
	}
	else
	{
		dst[0] = dst[1] = 0;
	}
	return CArrayToMRBArrayF(mrb, dst, 2);
}

static mrb_value mrb_NanoGL_Video_DegToRad(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float deg;
	mrb_get_args(mrb, "f", &deg);
	float rad = Video.DegToRad((float)deg);
	return mrb_float_value(mrb, rad);
}

static mrb_value mrb_NanoGL_Video_RadToDeg(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float rad;
	mrb_get_args(mrb, "f", &rad);
	float deg = Video.RadToDeg((float)rad);
	return mrb_float_value(mrb, deg);
}

static mrb_value mrb_NanoGL_Video_CreateImage(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *filename;
	mrb_int nfilename;
	mrb_int imageFlags;
	mrb_get_args(mrb, "si", &filename, &nfilename, &imageFlags);
	int id = Video.CreateImageUTF8(filename, imageFlags);
	return mrb_fixnum_value(id);
}

static mrb_value mrb_NanoGL_Video_CreateImageMem(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int imageFlags;
	unsigned char *data;
	mrb_int datalen;
	mrb_int ndata;
	mrb_get_args(mrb, "isi", &imageFlags, &data, &datalen, &ndata);
	int id = Video.CreateImageMem(imageFlags, data, ndata);
	return mrb_fixnum_value(id);
}

static mrb_value mrb_NanoGL_Video_CreateImageRGBA(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int w, h;
	mrb_int imageFlags;
	unsigned char *data;
	mrb_int datalen;
	mrb_get_args(mrb, "iiis", &w, &h, &imageFlags, &data, &datalen);
	int id = Video.CreateImageRGBA(w, h, imageFlags, data);
	return mrb_fixnum_value(id);
}

static mrb_value mrb_NanoGL_Video_UpdateImage(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int image;
	unsigned char *data;
	mrb_int datalen;
	mrb_get_args(mrb, "is", &image, &data, &datalen);
	Video.UpdateImage(image, data);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_ImageSize(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int image;
	mrb_get_args(mrb, "i", &image);
	int size[2];
	Video.ImageSize(image, &size[0], &size[1]);
	return CArrayToMRBArrayI(mrb, size, 2);
}

static mrb_value mrb_NanoGL_Video_DeleteImage(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int image;
	mrb_get_args(mrb, "i", &image);
	Video.DeleteImage(image);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_LinearGradient(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float sx, sy, ex, ey;
	mrb_value array1, array2;
	mrb_get_args(mrb, "ffffAA", &sx, &sy, &ex, &ey, &array1, &array2);
	Color icol, ocol;
	if (ArrayToColor(mrb, array1, &icol) && ArrayToColor(mrb, array2, &ocol)) {
		Paint p = Video.LinearGradient((float)sx, (float)sy, (float)ex, (float)ey, icol, ocol);
		return PaintToArray(mrb, &p);
	}
	else {
		return mrb_nil_value();
	}
}

static mrb_value mrb_NanoGL_Video_BoxGradient(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y, w, h, r, f;
	mrb_value array1, array2;
	mrb_get_args(mrb, "ffffffAA", &x, &y, &w, &h, &r, &f, &array1, &array2);
	Color icol, ocol;
	if (ArrayToColor(mrb, array1, &icol) && ArrayToColor(mrb, array2, &ocol)) {
		Paint p = Video.BoxGradient((float)x, (float)y, (float)w, (float)h, (float)r, (float)f, icol, ocol);
		return PaintToArray(mrb, &p);
	}
	else {
		return mrb_nil_value();
	}
}

static mrb_value mrb_NanoGL_Video_RadialGradient(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float cx, cy, inr, outr;
	mrb_value array1, array2;
	mrb_get_args(mrb, "ffffAA", &cx, &cy, &inr, &outr, &array1, &array2);
	Color icol, ocol;
	if (ArrayToColor(mrb, array1, &icol) && ArrayToColor(mrb, array2, &ocol)) {
		Paint p = Video.RadialGradient((float)cx, (float)cy, (float)inr, (float)outr, icol, ocol);
		return PaintToArray(mrb, &p);
	}
	else {
		return mrb_nil_value();
	}
}

static mrb_value mrb_NanoGL_Video_ImagePattern(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int image;
	mrb_float ox, oy, ex, ey, angle, alpha;
	mrb_get_args(mrb, "ffffffif", &ox, &oy, &ex, &ey, &angle, &image, &alpha);
	Paint p = Video.ImagePattern((float)ox, (float)oy, (float)ex, (float)ey, (float)angle, image, (float)alpha);
	return PaintToArray(mrb, &p);
}

static mrb_value mrb_NanoGL_Video_ImagePatternWithFillColor(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int image;
	mrb_float cx, cy, w, h, angle;
	mrb_get_args(mrb, "ffffffi", &cx, &cy, &w, &h, &angle, &image);
	Paint p = Video.ImagePatternWithFillColor((float)cx, (float)cy, (float)w, (float)h, (float)angle, image);
	return PaintToArray(mrb, &p);
}

static mrb_value mrb_NanoGL_Video_Scissor(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y, w, h;
	mrb_get_args(mrb, "ffff", &x, &y, &w, &h);
	Video.Scissor((float)x, (float)y, (float)w, (float)h);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_IntersectScissor(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y, w, h;
	mrb_get_args(mrb, "ffff", &x, &y, &w, &h);
	Video.IntersectScissor((float)x, (float)y, (float)w, (float)h);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_ResetScissor(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.ResetScissor();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_BeginPath(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.BeginPath();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_MoveTo(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	mrb_get_args(mrb, "ff", &x, &y);
	Video.MoveTo((float)x, (float)y);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_LineTo(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	mrb_get_args(mrb, "ff", &x, &y);
	Video.LineTo((float)x, (float)y);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_BezierTo(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  c1x, c1y, c2x, c2y, x, y;
	mrb_get_args(mrb, "ffffff", &c1x, &c1y, &c2x, &c2y, &x, &y);
	Video.BezierTo((float)c1x, (float)c1y, (float)c2x, (float)c2y, (float)x, (float)y);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_QuadTo(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  cx, cy, x, y;
	mrb_get_args(mrb, "ffff", &cx, &cy, &x, &y);
	Video.QuadTo((float)cx, (float)cy, (float)x, (float)y);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_ArcTo(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  cx, cy, x, y, r;
	mrb_get_args(mrb, "ffff", &cx, &cy, &x, &y, &r);
	Video.ArcTo((float)cx, (float)cy, (float)x, (float)y, (float)r);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_ClosePath(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.ClosePath();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_PathWinding(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int dir;
	mrb_get_args(mrb, "i", &dir);
	Video.PathWinding(dir);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Arc(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  cx, cy, r, a0, a1;
	mrb_int dir;
	mrb_get_args(mrb, "fffffi", &cx, &cy, &r, &a0, &a1, &dir);
	Video.Arc((float)cx, (float)cy, (float)r, (float)a0, (float)a1, (float)dir);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Rect(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  x, y, w, h;
	mrb_get_args(mrb, "ffff", &x, &y, &w, &h);
	Video.Rect((float)x, (float)y, (float)w, (float)h);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_RoundedRect(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  x, y, w, h, r;
	mrb_get_args(mrb, "fffff", &x, &y, &w, &h, &r);
	Video.RoundedRect((float)x, (float)y, (float)w, (float)h, (float)r);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Ellipse(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  cx, cy, rx, ry;
	mrb_get_args(mrb, "ffff", &cx, &cy, &rx, &ry);
	Video.Ellipse((float)cx, (float)cy, (float)rx, (float)ry);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Circle(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  cx, cy, r;
	mrb_get_args(mrb, "fff", &cx, &cy, &r);
	Video.Circle((float)cx, (float)cy, (float)r);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Fill(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.Fill();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Stroke(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	Video.Stroke();
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_CreateFont(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *name, *filename;
	mrb_int nname, nfilename;
	mrb_get_args(mrb, "ss", &name, &nname, &filename, &nfilename);
	int n = Video.CreateFontUTF8(name, filename);
	return mrb_fixnum_value(n);
}

static mrb_value mrb_NanoGL_Video_CreateFontMem(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *name;
	unsigned char *data;
	mrb_int nname, ndata;
	mrb_get_args(mrb, "ss", &name, &nname, &data, &ndata);
	int n = Video.CreateFontMemUTF8(name, data, ndata, 0);
	return mrb_fixnum_value(n);
}

static mrb_value mrb_NanoGL_Video_FindFont(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *name;
	mrb_int nname;

	mrb_get_args(mrb, "s", &name, &nname);
	int n = Video.FindFontUTF8(name);

	return mrb_fixnum_value(n);
}

static mrb_value mrb_NanoGL_Video_FontSize(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  size;
	mrb_get_args(mrb, "f", &size);
	Video.FontSize((float)size);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_FontBlur(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  blur;
	mrb_get_args(mrb, "f", &blur);
	Video.FontBlur((float)blur);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_TextLetterSpacing(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  spacing;
	mrb_get_args(mrb, "f", &spacing);
	Video.TextLetterSpacing((float)spacing);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_TextLineHeight(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float  lineHeight;
	mrb_get_args(mrb, "f", &lineHeight);
	Video.TextLineHeight((float)lineHeight);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_TextAlign(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int align;
	mrb_get_args(mrb, "i", &align);
	Video.TextAlign(align);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_FontFaceId(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int font;
	mrb_get_args(mrb, "i", &font);
	Video.FontFaceId(font);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_FontFace(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *font;
	mrb_int nfont;

	mrb_get_args(mrb, "s", &font, &nfont);
	Video.FontFaceUTF8(font);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_Text(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	char *str;
	mrb_get_args(mrb, "ffz", &x, &y, &str);
	Video.TextUTF8((float)x, (float)y, str, NULL);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_TextBox(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y, breakRowWidth;
	char *str;
	mrb_get_args(mrb, "fffz", &x, &y, &breakRowWidth, &str);
	Video.TextBoxUTF8((float)x, (float)y, (float)breakRowWidth, str, NULL);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_TextBounds(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	char *str;
	mrb_get_args(mrb, "ffz", &x, &y, &str);
	float bounds[4];
	Video.TextBoundsUTF8((float)x, (float)y, str, NULL, bounds);

	mrb_value array = mrb_ary_new_capa(mrb, 4);
	int n = 0;
	for (int i = 0; i < sizeof(bounds) / sizeof(bounds[0]); i++) {
		mrb_ary_set(mrb, array, n++, mrb_float_value(mrb, (mrb_float)bounds[i]));
	}

	return array;
}

static mrb_value mrb_NanoGL_Video_TextBoxBounds(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y, breakRowWidth;
	char *str;
	mrb_get_args(mrb, "fffz", &x, &y, &breakRowWidth, &str);
	float bounds[4];
	Video.TextBoxBoundsUTF8((float)x, (float)y, (float)breakRowWidth, str, NULL, bounds);

	mrb_value array = mrb_ary_new_capa(mrb, 4);
	int n = 0;
	for (int i = 0; i < sizeof(bounds) / sizeof(bounds[0]); i++) {
		mrb_ary_set(mrb, array, n++, mrb_float_value(mrb, (mrb_float)bounds[i]));
	}

	return array;
}

static mrb_value mrb_NanoGL_Video_TextGlyphPositions(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float x, y;
	char *str;
	mrb_int maxPositions;
	mrb_get_args(mrb, "ffzi", &x, &y, &str, &maxPositions);

	GlyphPosition *positions = calloc(maxPositions, sizeof(GlyphPosition));
	int ret = Video.TextGlyphPositionsUTF8((float)x, (float)y, str, NULL, positions, maxPositions);

	mrb_value array = mrb_ary_new_capa(mrb, ret);
	for (int i = 0; i<ret; i++)
	{
		mrb_value hash = mrb_hash_new_capa(mrb, 4);
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "str")), mrb_fixnum_value(String.Utf8.Count(str, positions[i].str)));
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "x")), mrb_float_value(mrb, positions[i].x));
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "minx")), mrb_float_value(mrb, positions[i].minx));
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "maxx")), mrb_float_value(mrb, positions[i].maxx));
		mrb_ary_set(mrb, array, i, hash);
	}
	free(positions);
	return array;
}

static mrb_value mrb_NanoGL_Video_TextMetrics(mrb_state *mrb, mrb_value self) {
	(void)self;
	float ascender, descender, lineh;
	Video.TextMetrics(&ascender, &descender, &lineh);
	mrb_value array = mrb_ary_new_capa(mrb, 3);
	mrb_ary_set(mrb, array, 0, mrb_float_value(mrb, ascender));
	mrb_ary_set(mrb, array, 1, mrb_float_value(mrb, descender));
	mrb_ary_set(mrb, array, 2, mrb_float_value(mrb, lineh));
	return array;

}

static mrb_value mrb_NanoGL_Video_TextBreakLines(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *string;
	mrb_float breakRowWidth;
	mrb_int maxRows;
	mrb_get_args(mrb, "zfi", &string, &breakRowWidth, &maxRows);

	TextRow *rows = calloc(maxRows, sizeof(TextRow));
	int ret = Video.TextBreakLinesUTF8(string, NULL, (float)breakRowWidth, rows, maxRows);

	mrb_value array = mrb_ary_new_capa(mrb, ret);
	for (int i = 0; i<ret; i++) {
		mrb_value hash = mrb_hash_new_capa(mrb, 6);
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "str")), mrb_fixnum_value(String.Utf8.Count(string, rows[i].start)));
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "end")), mrb_fixnum_value(String.Utf8.Count(string, rows[i].end)));
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "next")), mrb_fixnum_value(String.Utf8.Count(string, rows[i].next)));
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "width")), mrb_float_value(mrb, rows[i].width));
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "minx")), mrb_float_value(mrb, rows[i].minx));
		mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, "maxx")), mrb_float_value(mrb, rows[i].maxx));
		mrb_ary_set(mrb, array, i, hash);
	}
	free(rows);
	return array;
}

static mrb_value mrb_NanoGL_Video_DrawImage(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int image;
	mrb_float sx, sy, sw, sh, dx, dy, dw, dh, angle, alpha;
	mrb_get_args(mrb, "iffffffffff", &image, &sx, &sy, &sw, &sh, &dx, &dy, &dw, &dh, &angle, &alpha);

	Video.DrawImage(image, (float)sx, (float)sy, (float)sw, (float)sh, (float)dx, (float)dy, (float)dw, (float)dh, (float)angle, (float)alpha);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_Video_DrawImageWithFillColor(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int image;
	mrb_float sx, sy, sw, sh, dx, dy, dw, dh, angle;
	mrb_get_args(mrb, "ifffffffff", &image, &sx, &sy, &sw, &sh, &dx, &dy, &dw, &dh, &angle);

	Video.DrawImageWithFillColor(image, (float)sx, (float)sy, (float)sw, (float)sh, (float)dx, (float)dy, (float)dw, (float)dh, (float)angle);
	return mrb_nil_value();
}

//static mrb_value mrb_NanoGL_Video_FormatText(mrb_state *mrb, mrb_value self) {
//  ruby の書式指定機能を使って整形したうえでText()命令を呼べばよいので不要
//}

static void mrb_nanogl_videomodule_install(mrb_state *mrb, struct RClass *parent)
{
	struct RClass *mNanoGL_Video = mrb_define_module_under(mrb, parent, "Video");
	mrb_define_module_function(mrb, mNanoGL_Video, "SetWindowTitle", mrb_NanoGL_Video_SetWindowTitle, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "SetSize", mrb_NanoGL_Video_SetSize, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "Drawing", mrb_NanoGL_Video_Drawing, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "GetTime", mrb_NanoGL_Video_GetTime, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "Sleep", mrb_NanoGL_Video_Sleep, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "SetClearColor", mrb_NanoGL_Video_SetClearColor, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "Save", mrb_NanoGL_Video_Save, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "Restore", mrb_NanoGL_Video_Restore, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "Reset", mrb_NanoGL_Video_Reset, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "StrokeColor", mrb_NanoGL_Video_StrokeColor, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "StrokePaint", mrb_NanoGL_Video_StrokePaint, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "FillColor", mrb_NanoGL_Video_FillColor, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "FillPaint", mrb_NanoGL_Video_FillPaint, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "MiterLimit", mrb_NanoGL_Video_MiterLimit, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "StrokeWidth", mrb_NanoGL_Video_StrokeWidth, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "LineCap", mrb_NanoGL_Video_LineCap, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "LineJoin", mrb_NanoGL_Video_LineJoin, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "GlobalAlpha", mrb_NanoGL_Video_GlobalAlpha, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "ResetTransform", mrb_NanoGL_Video_ResetTransform, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "Transform", mrb_NanoGL_Video_Transform, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "Translate", mrb_NanoGL_Video_Translate, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "Rotate", mrb_NanoGL_Video_Rotate, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "SkewX", mrb_NanoGL_Video_SkewX, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "SkewY", mrb_NanoGL_Video_SkewY, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "Scale", mrb_NanoGL_Video_Scale, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "TranslateWorld", mrb_NanoGL_Video_TranslateWorld, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "RotateWorld", mrb_NanoGL_Video_RotateWorld, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "SkewXWorld", mrb_NanoGL_Video_SkewXWorld, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "SkewYWorld", mrb_NanoGL_Video_SkewYWorld, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "ScaleWorld", mrb_NanoGL_Video_ScaleWorld, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "CurrentTransform", mrb_NanoGL_Video_CurrentTransform, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformIdentity", mrb_NanoGL_Video_TransformIdentity, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformTranslate", mrb_NanoGL_Video_TransformTranslate, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformRotate", mrb_NanoGL_Video_TransformRotate, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformSkewX", mrb_NanoGL_Video_TransformSkewX, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformSkewY", mrb_NanoGL_Video_TransformSkewY, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformScale", mrb_NanoGL_Video_TransformScale, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformMultiply", mrb_NanoGL_Video_TransformMultiply, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformPremultiply", mrb_NanoGL_Video_TransformPremultiply, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformInverse", mrb_NanoGL_Video_TransformInverse, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "TransformPoint", mrb_NanoGL_Video_TransformPoint, MRB_ARGS_REQ(3));
	mrb_define_module_function(mrb, mNanoGL_Video, "DegToRad", mrb_NanoGL_Video_DegToRad, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "RadToDeg", mrb_NanoGL_Video_RadToDeg, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "CreateImage", mrb_NanoGL_Video_CreateImage, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "CreateImageMem", mrb_NanoGL_Video_CreateImageMem, MRB_ARGS_REQ(3));
	mrb_define_module_function(mrb, mNanoGL_Video, "CreateImageRGBA", mrb_NanoGL_Video_CreateImageRGBA, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "UpdateImage", mrb_NanoGL_Video_UpdateImage, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "ImageSize", mrb_NanoGL_Video_ImageSize, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "DeleteImage", mrb_NanoGL_Video_DeleteImage, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "LinearGradient", mrb_NanoGL_Video_LinearGradient, MRB_ARGS_REQ(6));
	mrb_define_module_function(mrb, mNanoGL_Video, "BoxGradient", mrb_NanoGL_Video_BoxGradient, MRB_ARGS_REQ(8));
	mrb_define_module_function(mrb, mNanoGL_Video, "RadialGradient", mrb_NanoGL_Video_RadialGradient, MRB_ARGS_REQ(6));
	mrb_define_module_function(mrb, mNanoGL_Video, "ImagePattern", mrb_NanoGL_Video_ImagePattern, MRB_ARGS_REQ(8));
	mrb_define_module_function(mrb, mNanoGL_Video, "ImagePatternWithFillColor", mrb_NanoGL_Video_ImagePatternWithFillColor, MRB_ARGS_REQ(7));
	mrb_define_module_function(mrb, mNanoGL_Video, "Scissor", mrb_NanoGL_Video_Scissor, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "IntersectScissor", mrb_NanoGL_Video_IntersectScissor, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "ResetScissor", mrb_NanoGL_Video_ResetScissor, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "BeginPath", mrb_NanoGL_Video_BeginPath, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "MoveTo", mrb_NanoGL_Video_MoveTo, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "LineTo", mrb_NanoGL_Video_LineTo, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "BezierTo", mrb_NanoGL_Video_BezierTo, MRB_ARGS_REQ(6));
	mrb_define_module_function(mrb, mNanoGL_Video, "QuadTo", mrb_NanoGL_Video_QuadTo, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "ArcTo", mrb_NanoGL_Video_ArcTo, MRB_ARGS_REQ(5));
	mrb_define_module_function(mrb, mNanoGL_Video, "ClosePath", mrb_NanoGL_Video_ClosePath, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "PathWinding", mrb_NanoGL_Video_PathWinding, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "Arc", mrb_NanoGL_Video_Arc, MRB_ARGS_REQ(6));
	mrb_define_module_function(mrb, mNanoGL_Video, "Rect", mrb_NanoGL_Video_Rect, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "RoundedRect", mrb_NanoGL_Video_RoundedRect, MRB_ARGS_REQ(5));
	mrb_define_module_function(mrb, mNanoGL_Video, "Ellipse", mrb_NanoGL_Video_Ellipse, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "Circle", mrb_NanoGL_Video_Circle, MRB_ARGS_REQ(3));
	mrb_define_module_function(mrb, mNanoGL_Video, "Fill", mrb_NanoGL_Video_Fill, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "Stroke", mrb_NanoGL_Video_Stroke, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "CreateFont", mrb_NanoGL_Video_CreateFont, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "CreateFontMem", mrb_NanoGL_Video_CreateFontMem, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Video, "FindFont", mrb_NanoGL_Video_FindFont, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "FontSize", mrb_NanoGL_Video_FontSize, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "FontBlur", mrb_NanoGL_Video_FontBlur, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "TextLetterSpacing", mrb_NanoGL_Video_TextLetterSpacing, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "TextLineHeight", mrb_NanoGL_Video_TextLineHeight, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "TextAlign", mrb_NanoGL_Video_TextAlign, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "FontFaceId", mrb_NanoGL_Video_FontFaceId, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "FontFace", mrb_NanoGL_Video_FontFace, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Video, "Text", mrb_NanoGL_Video_Text, MRB_ARGS_REQ(3));
	mrb_define_module_function(mrb, mNanoGL_Video, "TextBox", mrb_NanoGL_Video_TextBox, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "TextBounds", mrb_NanoGL_Video_TextBounds, MRB_ARGS_REQ(3));
	mrb_define_module_function(mrb, mNanoGL_Video, "TextBoxBounds", mrb_NanoGL_Video_TextBoxBounds, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "TextGlyphPositions", mrb_NanoGL_Video_TextGlyphPositions, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, mNanoGL_Video, "TextMetrics", mrb_NanoGL_Video_TextMetrics, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Video, "TextBreakLines", mrb_NanoGL_Video_TextBreakLines, MRB_ARGS_REQ(3));
	mrb_define_module_function(mrb, mNanoGL_Video, "DrawImage", mrb_NanoGL_Video_DrawImage, MRB_ARGS_REQ(10));
	mrb_define_module_function(mrb, mNanoGL_Video, "DrawImageWithFillColor", mrb_NanoGL_Video_DrawImageWithFillColor, MRB_ARGS_REQ(9));
	//mrb_define_module_function(mrb, mNanoGL_Video, "FormatText", mrb_NanoGL_Video_FormatText, MRB_ARGS_REQ(2));

	struct RClass *mNanoGL_Video_ImageFlags = mrb_define_module_under(mrb, mNanoGL_Video, "ImageFlags");
	mrb_define_const(mrb, mNanoGL_Video_ImageFlags, "IMAGE_GENERATE_MIPMAPS", mrb_fixnum_value(IMAGE_GENERATE_MIPMAPS));
	mrb_define_const(mrb, mNanoGL_Video_ImageFlags, "IMAGE_REPEATX", mrb_fixnum_value(IMAGE_REPEATX));
	mrb_define_const(mrb, mNanoGL_Video_ImageFlags, "IMAGE_REPEATY", mrb_fixnum_value(IMAGE_REPEATY));
	mrb_define_const(mrb, mNanoGL_Video_ImageFlags, "IMAGE_FLIPY", mrb_fixnum_value(IMAGE_FLIPY));
	mrb_define_const(mrb, mNanoGL_Video_ImageFlags, "IMAGE_PREMULTIPLIED", mrb_fixnum_value(IMAGE_PREMULTIPLIED));
	mrb_define_const(mrb, mNanoGL_Video_ImageFlags, "IMAGE_RESIZE_NEAREST", mrb_fixnum_value(IMAGE_RESIZE_NEAREST));

	struct RClass *mNanoGL_Video_LineCapStyle = mrb_define_module_under(mrb, mNanoGL_Video, "LineCapStyle");
	mrb_define_const(mrb, mNanoGL_Video_LineCapStyle, "CAP_BUTT", mrb_fixnum_value(CAP_BUTT));
	mrb_define_const(mrb, mNanoGL_Video_LineCapStyle, "CAP_ROUND", mrb_fixnum_value(CAP_ROUND));
	mrb_define_const(mrb, mNanoGL_Video_LineCapStyle, "CAP_SQUARE", mrb_fixnum_value(CAP_SQUARE));
	mrb_define_const(mrb, mNanoGL_Video_LineCapStyle, "CAP_BEVEL", mrb_fixnum_value(CAP_BEVEL));
	mrb_define_const(mrb, mNanoGL_Video_LineCapStyle, "CAP_MITER", mrb_fixnum_value(CAP_MITER));

	struct RClass *mNanoGL_Video_WindingStyle = mrb_define_module_under(mrb, mNanoGL_Video, "WindingStyle");
	mrb_define_const(mrb, mNanoGL_Video_WindingStyle, "WINDING_CCW", mrb_fixnum_value(WINDING_CCW));
	mrb_define_const(mrb, mNanoGL_Video_WindingStyle, "WINDING_CW", mrb_fixnum_value(WINDING_CW));

	struct RClass *mNanoGL_Video_SolidityStyle = mrb_define_module_under(mrb, mNanoGL_Video, "SolidityStyle");
	mrb_define_const(mrb, mNanoGL_Video_SolidityStyle, "SOLIDITY_SOLID", mrb_fixnum_value(SOLIDITY_SOLID));
	mrb_define_const(mrb, mNanoGL_Video_SolidityStyle, "SOLIDITY_HOLE", mrb_fixnum_value(SOLIDITY_HOLE));

	struct RClass *mNanoGL_Video_AlignFlags = mrb_define_module_under(mrb, mNanoGL_Video, "AlignFlags");
	mrb_define_const(mrb, mNanoGL_Video_AlignFlags, "ALIGN_LEFT", mrb_fixnum_value(ALIGN_LEFT));
	mrb_define_const(mrb, mNanoGL_Video_AlignFlags, "ALIGN_CENTER", mrb_fixnum_value(ALIGN_CENTER));
	mrb_define_const(mrb, mNanoGL_Video_AlignFlags, "ALIGN_RIGHT", mrb_fixnum_value(ALIGN_RIGHT));
	mrb_define_const(mrb, mNanoGL_Video_AlignFlags, "ALIGN_TOP", mrb_fixnum_value(ALIGN_TOP));
	mrb_define_const(mrb, mNanoGL_Video_AlignFlags, "ALIGN_MIDDLE", mrb_fixnum_value(ALIGN_MIDDLE));
	mrb_define_const(mrb, mNanoGL_Video_AlignFlags, "ALIGN_BOTTOM", mrb_fixnum_value(ALIGN_BOTTOM));
	mrb_define_const(mrb, mNanoGL_Video_AlignFlags, "ALIGN_BASELINE", mrb_fixnum_value(ALIGN_BASELINE));

}

////////////////////////////////////////////////////////////////////////////////
// NanoGL.Mouse
////////////////////////////////////////////////////////////////////////////////

static mrb_value mrb_NanoGL_Mouse_IsLeftButtonDown(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	mrb_bool n = Mouse.IsLeftButtonDown();
	return mrb_bool_value(n);
}

static mrb_value mrb_NanoGL_Mouse_IsRightButtonDown(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	mrb_bool n = Mouse.IsRightButtonDown();
	return mrb_bool_value(n);
}

static mrb_value mrb_NanoGL_Mouse_IsMiddleButtonDown(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	mrb_bool n = Mouse.IsMiddleButtonDown();
	return mrb_bool_value(n);
}

static mrb_value mrb_NanoGL_Mouse_GetCursorPos(mrb_state *mrb, mrb_value self) {
	(void)mrb;
	(void)self;
	double x, y;
	Mouse.GetCursorPos(&x, &y);
	mrb_value array = mrb_ary_new_capa(mrb, 2);
	mrb_ary_set(mrb, array, 0, mrb_float_value(mrb, x));
	mrb_ary_set(mrb, array, 1, mrb_float_value(mrb, y));
	return array;
}

static void mrb_nanogl_mousemodule_install(mrb_state *mrb, struct RClass *parent)
{
	struct RClass *mNanoGL_Mouse = mrb_define_module_under(mrb, parent, "Mouse");
	mrb_define_module_function(mrb, mNanoGL_Mouse, "IsRightButtonDown", mrb_NanoGL_Mouse_IsRightButtonDown, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Mouse, "IsLeftButtonDown", mrb_NanoGL_Mouse_IsLeftButtonDown, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Mouse, "IsMiddleButtonDown", mrb_NanoGL_Mouse_IsMiddleButtonDown, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_Mouse, "GetCursorPos", mrb_NanoGL_Mouse_GetCursorPos, MRB_ARGS_NONE());
}

////////////////////////////////////////////////////////////////////////////////
// NanoGL.Keyboard
////////////////////////////////////////////////////////////////////////////////

static mrb_value mrb_NanoGL_Keyboard_IsKeyDown(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value v;
	mrb_bool n = FALSE;
	mrb_get_args(mrb, "o", &v);
	if (mrb_fixnum_p(v)) {
		mrb_int key = mrb_fixnum(v);
		n = Keyboard.IsKeyDown((enum Keycode)key);
	} else if (mrb_float_p(v)) {
		mrb_int key = (mrb_int)mrb_float(v);
		n = Keyboard.IsKeyDown((enum Keycode)key);
	} else if (mrb_string_p(v)) {
		mrb_int key = *RSTRING_PTR(v);
		n = Keyboard.IsKeyDown((enum Keycode)key);
	}
	return mrb_bool_value(n);
}

static void mrb_nanogl_keyboardmodule_install(mrb_state *mrb, struct RClass *parent)
{
	struct RClass *mNanoGL_Keyboard = mrb_define_module_under(mrb, parent, "Keyboard");
	mrb_define_module_function(mrb, mNanoGL_Keyboard, "IsKeyDown", mrb_NanoGL_Keyboard_IsKeyDown, MRB_ARGS_REQ(1));

	struct RClass *mNanoGL_Keyboard_KeyCode = mrb_define_module_under(mrb, mNanoGL_Keyboard, "KeyCode");
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_SPACE", mrb_fixnum_value(KBKEY_SPACE));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_APOSTROPHE", mrb_fixnum_value(KBKEY_APOSTROPHE));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_COMMA", mrb_fixnum_value(KBKEY_COMMA));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_MINUS", mrb_fixnum_value(KBKEY_MINUS));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_PERIOD", mrb_fixnum_value(KBKEY_PERIOD));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_SLASH", mrb_fixnum_value(KBKEY_SLASH));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_0", mrb_fixnum_value(KBKEY_0));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_1", mrb_fixnum_value(KBKEY_1));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_2", mrb_fixnum_value(KBKEY_2));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_3", mrb_fixnum_value(KBKEY_3));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_4", mrb_fixnum_value(KBKEY_4));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_5", mrb_fixnum_value(KBKEY_5));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_6", mrb_fixnum_value(KBKEY_6));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_7", mrb_fixnum_value(KBKEY_7));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_8", mrb_fixnum_value(KBKEY_8));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_9", mrb_fixnum_value(KBKEY_9));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_SEMICOLON", mrb_fixnum_value(KBKEY_SEMICOLON));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_EQUAL", mrb_fixnum_value(KBKEY_EQUAL));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_A", mrb_fixnum_value(KBKEY_A));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_B", mrb_fixnum_value(KBKEY_B));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_C", mrb_fixnum_value(KBKEY_C));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_D", mrb_fixnum_value(KBKEY_D));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_E", mrb_fixnum_value(KBKEY_E));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F", mrb_fixnum_value(KBKEY_F));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_G", mrb_fixnum_value(KBKEY_G));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_H", mrb_fixnum_value(KBKEY_H));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_I", mrb_fixnum_value(KBKEY_I));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_J", mrb_fixnum_value(KBKEY_J));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_K", mrb_fixnum_value(KBKEY_K));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_L", mrb_fixnum_value(KBKEY_L));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_M", mrb_fixnum_value(KBKEY_M));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_N", mrb_fixnum_value(KBKEY_N));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_O", mrb_fixnum_value(KBKEY_O));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_P", mrb_fixnum_value(KBKEY_P));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_Q", mrb_fixnum_value(KBKEY_Q));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_R", mrb_fixnum_value(KBKEY_R));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_S", mrb_fixnum_value(KBKEY_S));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_T", mrb_fixnum_value(KBKEY_T));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_U", mrb_fixnum_value(KBKEY_U));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_V", mrb_fixnum_value(KBKEY_V));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_W", mrb_fixnum_value(KBKEY_W));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_X", mrb_fixnum_value(KBKEY_X));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_Y", mrb_fixnum_value(KBKEY_Y));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_Z", mrb_fixnum_value(KBKEY_Z));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_LEFT_BRACKET", mrb_fixnum_value(KBKEY_LEFT_BRACKET));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_BACKSLASH", mrb_fixnum_value(KBKEY_BACKSLASH));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_RIGHT_BRACKET", mrb_fixnum_value(KBKEY_RIGHT_BRACKET));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_GRAVE_ACCENT", mrb_fixnum_value(KBKEY_GRAVE_ACCENT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_WORLD_1", mrb_fixnum_value(KBKEY_WORLD_1));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_WORLD_2", mrb_fixnum_value(KBKEY_WORLD_2));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_ESCAPE", mrb_fixnum_value(KBKEY_ESCAPE));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_ENTER", mrb_fixnum_value(KBKEY_ENTER));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_TAB", mrb_fixnum_value(KBKEY_TAB));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_BACKSPACE", mrb_fixnum_value(KBKEY_BACKSPACE));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_INSERT", mrb_fixnum_value(KBKEY_INSERT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_DELETE", mrb_fixnum_value(KBKEY_DELETE));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_RIGHT", mrb_fixnum_value(KBKEY_RIGHT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_LEFT", mrb_fixnum_value(KBKEY_LEFT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_DOWN", mrb_fixnum_value(KBKEY_DOWN));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_UP", mrb_fixnum_value(KBKEY_UP));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_PAGE_UP", mrb_fixnum_value(KBKEY_PAGE_UP));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_PAGE_DOWN", mrb_fixnum_value(KBKEY_PAGE_DOWN));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_HOME", mrb_fixnum_value(KBKEY_HOME));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_END", mrb_fixnum_value(KBKEY_END));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_CAPS_LOCK", mrb_fixnum_value(KBKEY_CAPS_LOCK));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_SCROLL_LOCK", mrb_fixnum_value(KBKEY_SCROLL_LOCK));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_NUM_LOCK", mrb_fixnum_value(KBKEY_NUM_LOCK));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_PRINT_SCREEN", mrb_fixnum_value(KBKEY_PRINT_SCREEN));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_PAUSE", mrb_fixnum_value(KBKEY_PAUSE));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F1", mrb_fixnum_value(KBKEY_F1));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F2", mrb_fixnum_value(KBKEY_F2));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F3", mrb_fixnum_value(KBKEY_F3));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F4", mrb_fixnum_value(KBKEY_F4));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F5", mrb_fixnum_value(KBKEY_F5));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F6", mrb_fixnum_value(KBKEY_F6));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F7", mrb_fixnum_value(KBKEY_F7));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F8", mrb_fixnum_value(KBKEY_F8));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F9", mrb_fixnum_value(KBKEY_F9));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F10", mrb_fixnum_value(KBKEY_F10));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F11", mrb_fixnum_value(KBKEY_F11));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F12", mrb_fixnum_value(KBKEY_F12));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F13", mrb_fixnum_value(KBKEY_F13));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F14", mrb_fixnum_value(KBKEY_F14));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F15", mrb_fixnum_value(KBKEY_F15));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F16", mrb_fixnum_value(KBKEY_F16));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F17", mrb_fixnum_value(KBKEY_F17));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F18", mrb_fixnum_value(KBKEY_F18));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F19", mrb_fixnum_value(KBKEY_F19));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F20", mrb_fixnum_value(KBKEY_F20));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F21", mrb_fixnum_value(KBKEY_F21));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F22", mrb_fixnum_value(KBKEY_F22));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F23", mrb_fixnum_value(KBKEY_F23));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F24", mrb_fixnum_value(KBKEY_F24));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_F25", mrb_fixnum_value(KBKEY_F25));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_0", mrb_fixnum_value(KBKEY_KP_0));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_1", mrb_fixnum_value(KBKEY_KP_1));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_2", mrb_fixnum_value(KBKEY_KP_2));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_3", mrb_fixnum_value(KBKEY_KP_3));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_4", mrb_fixnum_value(KBKEY_KP_4));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_5", mrb_fixnum_value(KBKEY_KP_5));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_6", mrb_fixnum_value(KBKEY_KP_6));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_7", mrb_fixnum_value(KBKEY_KP_7));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_8", mrb_fixnum_value(KBKEY_KP_8));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_9", mrb_fixnum_value(KBKEY_KP_9));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_DECIMAL", mrb_fixnum_value(KBKEY_KP_DECIMAL));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_DIVIDE", mrb_fixnum_value(KBKEY_KP_DIVIDE));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_MULTIPLY", mrb_fixnum_value(KBKEY_KP_MULTIPLY));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_SUBTRACT", mrb_fixnum_value(KBKEY_KP_SUBTRACT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_ADD", mrb_fixnum_value(KBKEY_KP_ADD));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_ENTER", mrb_fixnum_value(KBKEY_KP_ENTER));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_KP_EQUAL", mrb_fixnum_value(KBKEY_KP_EQUAL));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_LEFT_SHIFT", mrb_fixnum_value(KBKEY_LEFT_SHIFT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_LEFT_CONTROL", mrb_fixnum_value(KBKEY_LEFT_CONTROL));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_LEFT_ALT", mrb_fixnum_value(KBKEY_LEFT_ALT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_LEFT_SUPER", mrb_fixnum_value(KBKEY_LEFT_SUPER));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_RIGHT_SHIFT", mrb_fixnum_value(KBKEY_RIGHT_SHIFT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_RIGHT_CONTROL", mrb_fixnum_value(KBKEY_RIGHT_CONTROL));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_RIGHT_ALT", mrb_fixnum_value(KBKEY_RIGHT_ALT));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_RIGHT_SUPER", mrb_fixnum_value(KBKEY_RIGHT_SUPER));
	mrb_define_const(mrb, mNanoGL_Keyboard_KeyCode, "KBKEY_MENU", mrb_fixnum_value(KBKEY_MENU));

}

////////////////////////////////////////////////////////////////////////////////
// NanoGL.FrameRate
////////////////////////////////////////////////////////////////////////////////

static mrb_value mrb_NanoGL_FrameRate_SetFrameRate(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int fps;
	mrb_get_args(mrb, "i", &fps);
	FrameRate.SetFrameRate(fps);
	return mrb_nil_value();
}

static mrb_value mrb_NanoGL_FrameRate_GetFrameRate(mrb_state *mrb, mrb_value self) {
	(void)self;
	unsigned int rate = FrameRate.GetFrameRate();
	return mrb_fixnum_value(rate);
}

static mrb_value mrb_NanoGL_FrameRate_GetRealFrameRate(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_float rate = (mrb_float)FrameRate.GetRealFrameRate();
	return mrb_float_value(mrb, rate);
}

static mrb_value mrb_NanoGL_FrameRate_GetCPUPower(mrb_state *mrb, mrb_value self) {
	(void)self;
	unsigned int rate = FrameRate.GetCPUPower();
	return mrb_fixnum_value(rate);
}

static void mrb_nanogl_frameratemodule_install(mrb_state *mrb, struct RClass *parent)
{
	struct RClass *mNanoGL_FrameRate = mrb_define_module_under(mrb, parent, "FrameRate");
	mrb_define_module_function(mrb, mNanoGL_FrameRate, "SetFrameRate", mrb_NanoGL_FrameRate_SetFrameRate, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_FrameRate, "GetFrameRate", mrb_NanoGL_FrameRate_GetFrameRate, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_FrameRate, "GetRealFrameRate", mrb_NanoGL_FrameRate_GetRealFrameRate, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, mNanoGL_FrameRate, "GetCPUPower", mrb_NanoGL_FrameRate_GetCPUPower, MRB_ARGS_NONE());
}

////////////////////////////////////////////////////////////////////////////////
// NanoGL.Sound
////////////////////////////////////////////////////////////////////////////////

static mrb_value mrb_NanoGL_Sound_ChannelCreate(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int channelId;
	char *filepath;
	mrb_int nfilepath;
	mrb_get_args(mrb, "is", &channelId, &filepath, &nfilepath);

	mrb_bool n = Sound.ChannelCreateUTF8(channelId, filepath);

	return mrb_bool_value(n);
}

static mrb_value mrb_NanoGL_Sound_ChannelFree(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int channelId;
	mrb_get_args(mrb, "i", &channelId);
	mrb_bool n = Sound.ChannelFree(channelId);
	return mrb_bool_value(n);
}

static mrb_value mrb_NanoGL_Sound_ChannelPlay(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int channelId;
	mrb_bool loop;
	mrb_get_args(mrb, "ib", &channelId, &loop);
	mrb_bool n = Sound.ChannelPlay(channelId, loop);
	return mrb_bool_value(n);
}

static mrb_value mrb_NanoGL_Sound_ChannelStop(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_int channelId;
	mrb_get_args(mrb, "i", &channelId);
	mrb_bool n = Sound.ChannelStop(channelId);
	return mrb_bool_value(n);
}

static void mrb_nanogl_soundmodule_install(mrb_state *mrb, struct RClass *parent)
{
	struct RClass *mNanoGL_Sound = mrb_define_module_under(mrb, parent, "Sound");
	mrb_define_module_function(mrb, mNanoGL_Sound, "ChannelCreate", mrb_NanoGL_Sound_ChannelCreate, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Sound, "ChannelFree", mrb_NanoGL_Sound_ChannelFree, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, mNanoGL_Sound, "ChannelPlay", mrb_NanoGL_Sound_ChannelPlay, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Sound, "ChannelStop", mrb_NanoGL_Sound_ChannelStop, MRB_ARGS_REQ(1));
}

////////////////////////////////////////////////////////////////////////////////
// NanoGL.System
////////////////////////////////////////////////////////////////////////////////

static mrb_value mrb_NanoGL_System_Quit(mrb_state *mrb, mrb_value self) {
	(void)self;
	(void)mrb;
	System.Quit();
	return mrb_nil_value();
}

static void mrb_nanogl_systemmodule_install(mrb_state *mrb, struct RClass *parent)
{
	struct RClass *mNanoGL_System = mrb_define_module_under(mrb, parent, "System");
	mrb_define_module_function(mrb, mNanoGL_System, "Quit", mrb_NanoGL_System_Quit, MRB_ARGS_NONE());
}

////////////////////////////////////////////////////////////////////////////////
// NanoGL.Dialog
////////////////////////////////////////////////////////////////////////////////

static mrb_value mrb_NanoGL_Dialog_OpenFileDialog(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *title;
	int titlelen;
	char *filter;
	int filterlen;
	string_t path;

	mrb_get_args(mrb, "ss", &title, &titlelen, &filter, &filterlen);

	path = Dialog.OpenFileDialogUTF8(title, filter);
	if (path.c_str == NULL) {
		return mrb_nil_value();
	} else {
		mrb_value v = mrb_str_new_cstr(mrb, path.c_str);
		String.Free(path);
		return v;
	}
}

static mrb_value mrb_NanoGL_Dialog_SaveFileDialog(mrb_state *mrb, mrb_value self) {
	(void)self;
	char *title;
	int titlelen;
	char *filter;
	int filterlen;
	string_t path;

	mrb_get_args(mrb, "ss", &title, &titlelen, &filter, &filterlen);

	path = Dialog.SaveFileDialogUTF8(title, filter);
	if (path.c_str == NULL) {
		return mrb_nil_value();
	} else {
		mrb_value v = mrb_str_new_cstr(mrb, path.c_str);
		String.Free(path);
		return v;
	}
}

static void mrb_nanogl_dialogmodule_install(mrb_state *mrb, struct RClass *parent)
{
	struct RClass *mNanoGL_Dialog = mrb_define_module_under(mrb, parent, "Dialog");
	mrb_define_module_function(mrb, mNanoGL_Dialog, "OpenFileDialog", mrb_NanoGL_Dialog_OpenFileDialog, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, mNanoGL_Dialog, "SaveFileDialog", mrb_NanoGL_Dialog_SaveFileDialog, MRB_ARGS_REQ(2));
}

//////////////////////////////////////////////////////////////////////////////////
// gem init
//////////////////////////////////////////////////////////////////////////////////

void mrb_nanogl_gem_init(mrb_state* mrb)
{
	// Install NanoGL
	struct RClass *mNanoGL = mrb_define_module(mrb, "NanoGL");

	// NanoGL.Video
	mrb_nanogl_videomodule_install(mrb, mNanoGL);
	
	// NanoGL.Mouse
	mrb_nanogl_mousemodule_install(mrb, mNanoGL);
	
	// NanoGL.Keyboard
	mrb_nanogl_keyboardmodule_install(mrb, mNanoGL);
	
	// NanoGL.Sound
	mrb_nanogl_soundmodule_install(mrb, mNanoGL);
	
	// NanoGL.FrameRate
	mrb_nanogl_frameratemodule_install(mrb, mNanoGL);

	// NanoGL.System
	mrb_nanogl_systemmodule_install(mrb, mNanoGL);

	// NanoGL.Dialog
	mrb_nanogl_dialogmodule_install(mrb, mNanoGL);
}

void mrb_nanogl_gem_final(mrb_state *mrb) {	

}
