#include "../Config.h"

#if defined(_WIN32)
#include <windows.h>
#define msleep(x) Sleep(x)
#elif defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#define msleep(x) usleep((x)*(1000))
#define _strcmpi strcasecmp
#define _strdup strdup
#endif
// GLAD
#include <glad/gl.h>

// GLFW (include after glad)
#include <GLFW/glfw3.h>

// NanoGL
#include "./nanovg-extra.h"

// STB
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "Video.h"
#include "FpsTimer.h"
#include "../Debug/Debug.h"
#include "../System/System.h"
#include "../String/String.h"
#include "../String/CharCodeHelper.h"
#include "../Math/Math.h"

#define  MAT4_IMPLEMENTATION
#include "../Math/mat4.h"

static GLFWwindow* glfwWindow;
static NVGcontext* nvgContext;

#if defined(_MSC_VER) && (_MSC_VER <= 1800)
# define inline __inline
#endif

#define JugglingCastImpl(x,y) \
static inline y x##2##y(x _x) { return *((y*)&_x); } \
static inline x y##2##x(y _y) { return *((x*)&_y); }

#define JugglingCast(x, y, v)	x##2##y(v)

JugglingCastImpl(NVGcolor, Color)
JugglingCastImpl(NVGpaint, Paint)

//// Video ////

static bool Video_Initialize_(void) {
	if (glfwInit() == GL_FALSE) {
		Debug.PrintError("Failed to initialize GLFW.");
		return false;
	}

	// Using OpenGL 4.0 Core Profile Only (disable backward compatibility)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// disable window resize by user
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create window
	glfwWindow = glfwCreateWindow(640, 480, "", NULL, NULL);
	if (glfwWindow == NULL) {
		Debug.PrintError("Failed to create window.");
		glfwTerminate();
		return false;
	}

	// Create openGL's context, and bind to created window
	glfwMakeContextCurrent(glfwWindow);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		puts("Failed to initialize OpenGL context");
		return -1;
	}

	// Create NanoVG's context
	nvgContext = nvgCreateGL3(NVG_STENCIL_STROKES);
	if (nvgContext == NULL) {
		Debug.PrintError("Failed to create NanoVG's Context.");
		glfwTerminate();
		return false;
	}

	return true;
}

static bool Video_Initialize(void) {
	Debug.PushBanner("**Error in Video.Initialize");
	bool ret = Video_Initialize_();
	FpsTimer.Initialize();
	Joystick.Initialize();
	Debug.PopBanner();
	return ret;
}

static void Video_Finalize(void) {
	Joystick.Finalize();
	FpsTimer.Finalize();
	if (nvgContext != NULL) {
		nvgDeleteGL3(nvgContext);
		nvgContext = NULL;
	}
	glfwTerminate();
}

static void Video_SetWindowTitleUTF8(const char* title) {
	glfwSetWindowTitle(glfwWindow, title);
}

static void Video_SetWindowTitle(const char* title) {
#if defined(_WIN32)
	if (is_contains_not_ascii(title, NULL)) {
		utf8_t* utf8title = sjis2utf8(title, NULL);
		glfwSetWindowTitle(glfwWindow, utf8title);
		free(utf8title);
	} else {
		glfwSetWindowTitle(glfwWindow, title);
	}
#else
	glfwSetWindowTitle(glfwWindow, title);
#endif
}

static void Video_SetSize(int width, int height) {
	glfwSetWindowSize(glfwWindow, max(width, 1), max(height, 1));
}

static void Video_GetSize(int* width, int* height) {
	glfwGetWindowSize(glfwWindow, width, height);
}

static void Video_BeginDraw(void) {
	int ww, wh;
	glfwGetWindowSize(glfwWindow, &ww, &wh);
	int fbw, fbh;
	glfwGetFramebufferSize(glfwWindow, &fbw, &fbh);

	glViewport(0, 0, fbw, fbh);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	float pxRatio = fbw * 1.0f / ww;
	nvgBeginFrame(nvgContext, (float)ww, (float)wh, pxRatio);

	nvgReset(nvgContext);
}

static void Video_EndDraw(void) {
	nvgEndFrame(nvgContext);
	glfwSwapBuffers(glfwWindow);
}

static void Video_BeginDrawEx(void) {
	int ww, wh;
	glfwGetWindowSize(glfwWindow, &ww, &wh);
	int fbw, fbh;
	glfwGetFramebufferSize(glfwWindow, &fbw, &fbh);

	glViewport(0, 0, fbw, fbh);

	float pxRatio = fbw * 1.0f / ww;
	nvgBeginFrame(nvgContext, (float)ww, (float)wh, pxRatio);
	nvgReset(nvgContext);

}

static void Video_ResetDrawEx(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

static void Video_EndDrawEx(void) {
	nvgEndFrame(nvgContext);
}

static void Video_UpdateDrawEx(void) {
	glfwSwapBuffers(glfwWindow);
}

static bool Video_Drawing(void) {
	static bool need_finish = false;

	if (need_finish) {
		Video_EndDraw();
		need_finish = false;
	}

	FpsTimer.WaitFrame();
	glfwPollEvents();
	Mouse.Update();
	Joystick.Update();
	if (glfwWindowShouldClose(glfwWindow) != GL_FALSE) {
		System.Quit();
	}

	Video_BeginDraw();
	need_finish = true;

	return true;
}

static bool Video_Loop(void) {
	FpsTimer.WaitFrame();
	glfwPollEvents();
	Mouse.Update();
	Joystick.Update();
	if (glfwWindowShouldClose(glfwWindow) != GL_FALSE) {
		System.Quit();
	}

	return true;
}

static double Video_GetTime(void) {
	return glfwGetTime();
}

static double Video_Sleep(double sec) {
	double start = glfwGetTime();
	int ms = (int)(sec * 1000);
	msleep(ms);
	double end = glfwGetTime();
	return end - start;
}

static void Video_SetFrameRate(unsigned int fps) {
	FpsTimer.SetFPS(fps);
}

static unsigned int Video_GetFrameRate(void) {
	return FpsTimer.GetFPS();
}

static float Video_GetRealFrameRate() {
	return FpsTimer.GetRealFPS();
}

static unsigned int Video_GetCPUPower(void) {
	return FpsTimer.GetCPUPower();
}

// opengl api's
static void Video_SetClearColor(Color color) {
	// Sets the color of the erase of screen to be used by glClear function
	glClearColor(color.s.r, color.s.g, color.s.b, color.s.a);
}

// nanovg api's

static void Video_BeginFrame(int windowWidth, int windowHeight, float devicePixelRatio) {
	nvgBeginFrame(nvgContext, (float)windowWidth, (float)windowHeight, devicePixelRatio);
}

static void Video_CancelFrame(void) {
	nvgCancelFrame(nvgContext);
}

static void Video_EndFrame(void) {
	nvgEndFrame(nvgContext);
}

static void Video_GlobalCompositeOperation(enum CompositeOperation op) {
	nvgGlobalCompositeOperation(nvgContext, op);
}

static void Video_GlobalCompositeBlendFunc(enum BlendFactor sfactor, enum BlendFactor dfactor) {
	nvgGlobalCompositeBlendFunc(nvgContext, sfactor, dfactor);
}

static void Video_GlobalCompositeBlendFuncSeparate(enum BlendFactor srcRGB, enum BlendFactor dstRGB, enum BlendFactor srcAlpha, enum BlendFactor dstAlpha) {
	nvgGlobalCompositeBlendFuncSeparate(nvgContext, srcRGB, dstRGB, srcAlpha, dstAlpha);
}

static Color Video_RGB(unsigned char r, unsigned char g, unsigned char b) {
	return JugglingCast(NVGcolor, Color, nvgRGB(r, g, b));
}

static Color Video_RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	return JugglingCast(NVGcolor, Color, nvgRGBA(r, g, b, a));
}

static Color Video_LerpRGBA(Color c0, Color c1, float u) {
	return JugglingCast(NVGcolor, Color, nvgLerpRGBA(JugglingCast(Color, NVGcolor, c0), JugglingCast(Color, NVGcolor, c1), u));
}

static Color Video_TransRGBA(Color c0, unsigned char a) {
	return JugglingCast(NVGcolor, Color, nvgTransRGBA(JugglingCast(Color, NVGcolor, c0), a));
}

static Color Video_TransRGBAf(Color c0, float a) {
	return JugglingCast(NVGcolor, Color, nvgTransRGBAf(JugglingCast(Color, NVGcolor, c0), a));
}

static Color Video_HSL(float h, float s, float l) {
	return JugglingCast(NVGcolor, Color, nvgHSL(h, s, l));
}

static Color Video_HSLA(float h, float s, float l, unsigned char a) {
	return JugglingCast(NVGcolor, Color, nvgHSLA(h, s, l, a));
}

static void Video_Save(void) {
	nvgSave(nvgContext);
}

static void Video_Restore(void) {
	nvgRestore(nvgContext);
}

static void Video_Reset(void) {
	nvgReset(nvgContext);
}

static void Video_StrokeColor(Color color) {
	nvgStrokeColor(nvgContext, JugglingCast(Color, NVGcolor, color));
}

static void Video_StrokePaint(Paint paint) {
	nvgStrokePaint(nvgContext, JugglingCast(Paint, NVGpaint, paint));
}

static void Video_FillColor(Color color) {
	nvgFillColor(nvgContext, JugglingCast(Color, NVGcolor, color));
}

static void Video_FillPaint(Paint paint) {
	nvgFillPaint(nvgContext, JugglingCast(Paint, NVGpaint, paint));
}

static void Video_MiterLimit(float limit) {
	nvgMiterLimit(nvgContext, limit);
}

static void Video_StrokeWidth(float size) {
	nvgStrokeWidth(nvgContext, size);
}

static void Video_LineCap(enum LineCap cap) {
	nvgLineCap(nvgContext, cap);
}

static void Video_LineJoin(enum LineCap join) {
	nvgLineJoin(nvgContext, join);
}

static void Video_GlobalAlpha(float alpha) {
	nvgGlobalAlpha(nvgContext, alpha);
}

static void Video_ResetTransform(void) {
	nvgResetTransform(nvgContext);
}

static void Video_Transform(float a, float b, float c, float d, float e, float f) {
	nvgTransform(nvgContext, a, b, c, d, e, f);
}

static void Video_Translate(float x, float y) {
	nvgTranslate(nvgContext, x, y);
}

static void Video_Rotate(float angle) {
	nvgRotate(nvgContext, angle);
}

static void Video_SkewX(float angle) {
	nvgSkewX(nvgContext, angle);
}

static void Video_SkewY(float angle) {
	nvgSkewY(nvgContext, angle);
}

static void Video_Scale(float x, float y) {
	nvgScale(nvgContext, x, y);
}

static void Video_TranslateWorld(float x, float y) {
	nvgTranslateWorld(nvgContext, x, y);
}

static void Video_RotateWorld(float angle) {
	nvgRotateWorld(nvgContext, angle);
}

static void Video_SkewXWorld(float angle) {
	nvgSkewXWorld(nvgContext, angle);
}

static void Video_SkewYWorld(float angle) {
	nvgSkewYWorld(nvgContext, angle);
}

static void Video_ScaleWorld(float x, float y) {
	nvgScaleWorld(nvgContext, x, y);
}

static void Video_CurrentTransform(float* xform) {
	nvgCurrentTransform(nvgContext, xform);
}

static void Video_TransformIdentity(float* dst) {
	nvgTransformIdentity(dst);
}

static void Video_TransformTranslate(float* dst, float tx, float ty) {
	nvgTransformTranslate(dst, tx, ty);
}

static void Video_TransformScale(float* dst, float sx, float sy) {
	nvgTransformScale(dst, sx, sy);
}

static void Video_TransformRotate(float* dst, float a) {
	nvgTransformRotate(dst, a);
}

static void Video_TransformSkewX(float* dst, float a) {
	nvgTransformSkewX(dst, a);
}

static void Video_TransformSkewY(float* dst, float a) {
	nvgTransformSkewY(dst, a);
}

static void Video_TransformMultiply(float* dst, const float* src) {
	nvgTransformMultiply(dst, src);
}

static void Video_TransformPremultiply(float* dst, const float* src) {
	nvgTransformPremultiply(dst, src);
}

static int Video_TransformInverse(float* dst, const float* src) {
	return nvgTransformInverse(dst, src);
}

static void Video_TransformPoint(float* dstx, float* dsty, const float* xform, float srcx, float srcy) {
	nvgTransformPoint(dstx, dsty, xform, srcx, srcy);
}

static float Video_DegToRad(float deg) {
	return nvgDegToRad(deg);
}

static float Video_RadToDeg(float rad) {
	return nvgRadToDeg(rad);
}

static int Video_CreateImageUTF8_(const char* filename, int imageFlags) {
	if (filename == NULL) {
		Debug.PrintError("The argument `filename` is NULL.");
		return 0;
	}
	int ret;
#if defined(_WIN32)
	if (is_contains_not_ascii(filename, NULL)) {
		char* sjisfilename = utf82sjis(filename, NULL);
		ret = nvgCreateImage(nvgContext, sjisfilename, imageFlags);
		if (ret == 0) {
			Debug.PrintError("Failed to load image file %s. File is not exist, or not supported format.", sjisfilename);
		}
		free(sjisfilename);
	} else {
		ret = nvgCreateImage(nvgContext, filename, imageFlags);
	}
#else
	ret = nvgCreateImage(nvgContext, filename, imageFlags);
#endif
	if (ret == 0) {
		Debug.PrintError("Failed to load image file %s. File is not exist, or not supported format.", filename);
		return 0;
	}
	return ret;
}

static int Video_CreateImageUTF8(const char* filename, enum ImageFlags imageFlags) {
	Debug.PushBanner("**Error in Video.CreateImageUTF8");
	int ret = Video_CreateImageUTF8_(filename, imageFlags);
	Debug.PopBanner();
	return ret;
}

static int Video_CreateImage_(const char* filename, int imageFlags) {
	if (filename == NULL) {
		Debug.PrintError("The argument `filename` is NULL.");
		return 0;
	}
	int ret;
#if defined(_WIN32)
	if (is_contains_not_ascii(filename, NULL)) {
		utf8_t* utf8filename = sjis2utf8(filename, NULL);
		ret = nvgCreateImage(nvgContext, utf8filename, imageFlags);
		free(utf8filename);
	} else {
		ret = nvgCreateImage(nvgContext, filename, imageFlags);
	}
#else
	ret = nvgCreateImage(nvgContext, filename, imageFlags);
#endif
	if (ret == 0) {
		Debug.PrintError("Failed to load image file %s. File is not exist, or not supported format.", filename);
		return 0;
	}
	return ret;
}

// [red, green, blue, alpha]並び
static unsigned char* Video_LoadImageData(const char* filename, int* width, int* height) {
	if (filename == NULL) {
		Debug.PrintError("The argument `filename` is NULL.");
		return NULL;
	}
	stbi_set_unpremultiply_on_load(1);
	stbi_convert_iphone_png_to_rgb(1);
	int n;
	unsigned char* ret = stbi_load(filename, width, height, &n, 4);
	if (ret == NULL) {
		return NULL;
	}
	return ret;
}

static unsigned char* Video_LoadImageDataUTF8_(const char* filename, int* width, int* height) {
	if (filename == NULL) {
		Debug.PrintError("The argument `filename` is NULL.");
		return NULL;
	}
	stbi_set_unpremultiply_on_load(1);
	stbi_convert_iphone_png_to_rgb(1);
	int n;
	unsigned char* ret;

#if defined(_WIN32)
	if (is_contains_not_ascii(filename, NULL)) {
		char* sjisfilename = utf82sjis(filename, NULL);
		ret = stbi_load(sjisfilename, width, height, &n, 4);
		if (ret == NULL) {
			Debug.PrintError("Failed to load image file %s. File is not exist, or not supported format.", sjisfilename);
		}
		free(sjisfilename);
	} else {
		ret = stbi_load(filename, width, height, &n, 4);
	}
#else
	ret = stbi_load(filename, width, height, &n, 4);
#endif

	if (ret == NULL) {
		Debug.PrintError("Failed to load image file %s. File is not exist, or not supported format.", filename);
		return NULL;
	}
	return ret;
}

static unsigned char* Video_LoadImageDataUTF8(const char* filename, int* width, int* height) {
	Debug.PushBanner("**Error in Video.LoadImageDataUTF8");
	unsigned char* ret = Video_LoadImageDataUTF8_(filename, width, height);
	Debug.PopBanner();
	return ret;
}

static void Video_FreeImageData(unsigned char* img) {
	if (img != NULL) {
		stbi_image_free(img);
	}
}

static int Video_CreateImage(const char* filename, enum ImageFlags imageFlags) {
	Debug.PushBanner("**Error in Video.CreateImage");
	int ret = Video_CreateImage_(filename, imageFlags);
	Debug.PopBanner();
	return ret;
}

static int Video_CreateImageMem_(enum ImageFlags imageFlags, unsigned char* data, int ndata) {
	if (data == NULL) {
		Debug.PrintError("The argument `data` is NULL.");
		return 0;
	}
	if (ndata <= 0) {
		Debug.PrintError("The argument `ndata` is less than or equal to 0.");
		return 0;
	}
	int ret = nvgCreateImageMem(nvgContext, imageFlags, data, ndata);
	if (ret == 0) {
		Debug.PrintError("Failed to load image data. Bad arguments or unsupported format data.");
		return 0;
	}
	return ret;

}

static int Video_CreateImageMem(enum ImageFlags imageFlags, unsigned char* data, int ndata) {
	Debug.PushBanner("**Error in Video.CreateImageMem");
	int ret = Video_CreateImageMem_(imageFlags, data, ndata);
	Debug.PopBanner();
	return ret;
}

static int Video_CreateImageRGBA_(int w, int h, enum ImageFlags imageFlags, const unsigned char* data) {
	if (data == NULL) {
		Debug.PrintError("The argument `data` is NULL.");
		return 0;
	}
	if (w <= 0) {
		Debug.PrintError("The argument `w` is less than or equal to 0.");
		return 0;
	}
	if (h <= 0) {
		Debug.PrintError("The argument `h` is less than or equal to 0.");
		return 0;
	}
	int ret = nvgCreateImageRGBA(nvgContext, w, h, imageFlags, data);
	if (ret == 0) {
		Debug.PrintError("Failed to load image data. Bad arguments or unsupported format data.");
		return 0;
	}
	return ret;
}

static int Video_CreateImageRGBA(int w, int h, enum ImageFlags imageFlags, const unsigned char* data) {
	Debug.PushBanner("**Error in Video.CreateImageRGBA");
	int ret = Video_CreateImageRGBA_(w, h, imageFlags, data);
	Debug.PopBanner();
	return ret;
}

static void Video_UpdateImage(int image, const unsigned char* data) {
	nvgUpdateImage(nvgContext, image, data);
}

static void Video_ImageSize(int image, int* w, int* h) {
	nvgImageSize(nvgContext, image, w, h);
}

static void Video_DeleteImage(int image) {
	nvgDeleteImage(nvgContext, image);
}

static Paint Video_LinearGradient(float sx, float sy, float ex, float ey, Color icol, Color ocol) {
	return  JugglingCast(NVGpaint, Paint, nvgLinearGradient(nvgContext, sx, sy, ex, ey, JugglingCast(Color, NVGcolor, icol), JugglingCast(Color, NVGcolor, ocol)));
}

static Paint Video_BoxGradient(float x, float y, float w, float h, float r, float f, Color icol, Color ocol) {
	return JugglingCast(NVGpaint, Paint, nvgBoxGradient(nvgContext, x, y, w, h, r, f, JugglingCast(Color, NVGcolor, icol), JugglingCast(Color, NVGcolor, ocol)));
}

static Paint Video_RadialGradient(float cx, float cy, float inr, float outr, Color icol, Color ocol) {
	return JugglingCast(NVGpaint, Paint, nvgRadialGradient(nvgContext, cx, cy, inr, outr, JugglingCast(Color, NVGcolor, icol), JugglingCast(Color, NVGcolor, ocol)));
}

static Paint Video_ImagePattern(float ox, float oy, float ex, float ey, float angle, int image, float alpha) {
	return JugglingCast(NVGpaint, Paint, nvgImagePattern(nvgContext, ox, oy, ex, ey, angle, image, alpha));
}

static Paint Video_ImagePatternWithFillColor(float cx, float cy, float w, float h, float angle, int image) {
	return JugglingCast(NVGpaint, Paint, nvgImagePatternWithFillColor(nvgContext, cx, cy, w, h, angle, image));
}

static void Video_Scissor(float x, float y, float w, float h) {
	nvgScissor(nvgContext, x, y, w, h);
}

static void Video_IntersectScissor(float x, float y, float w, float h) {
	nvgIntersectScissor(nvgContext, x, y, w, h);
}

static void Video_ResetScissor(void) {
	nvgResetScissor(nvgContext);
}

static void Video_BeginPath(void) {
	nvgBeginPath(nvgContext);
}

static void Video_MoveTo(float x, float y) {
	nvgMoveTo(nvgContext, x, y);
}

static void Video_LineTo(float x, float y) {
	nvgLineTo(nvgContext, x, y);
}

static void Video_BezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y) {
	nvgBezierTo(nvgContext, c1x, c1y, c2x, c2y, x, y);
}

static void Video_QuadTo(float cx, float cy, float x, float y) {
	nvgQuadTo(nvgContext, cx, cy, x, y);
}

static void Video_ArcTo(float x1, float y1, float x2, float y2, float radius) {
	nvgArcTo(nvgContext, x1, y1, x2, y2, radius);
}

static void Video_ClosePath(void) {
	nvgClosePath(nvgContext);
}

static void Video_PathWinding(enum Solidity dir) {
	nvgPathWinding(nvgContext, dir);
}

static void Video_Arc(float cx, float cy, float r, float a0, float a1, enum Winding dir) {
	nvgArc(nvgContext, cx, cy, r, a0, a1, dir);
}

static void Video_Rect(float x, float y, float w, float h) {
	nvgRect(nvgContext, x, y, w, h);
}

static void Video_RoundedRect(float x, float y, float w, float h, float r) {
	nvgRoundedRect(nvgContext, x, y, w, h, r);
}

static void Video_RoundedRectVarying(float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft) {
	nvgRoundedRectVarying(nvgContext, x, y, w, h, radTopLeft, radTopRight, radBottomRight, radBottomLeft);
}

static void Video_Ellipse(float cx, float cy, float rx, float ry) {
	nvgEllipse(nvgContext, cx, cy, rx, ry);
}

static void Video_Circle(float cx, float cy, float r) {
	nvgCircle(nvgContext, cx, cy, r);
}

static void Video_Fill(void) {
	nvgFill(nvgContext);
}

static void Video_Stroke(void) {
	nvgStroke(nvgContext);
}

static int Video_CreateFontUTF8_(const char* name, const char* filename) {
	if (filename == NULL) {
		Debug.PrintError("The argument `filename` is NULL.");
		return -1;
	}
	if (name == NULL) {
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (is_contains_not_ascii(name, NULL) || is_contains_not_ascii(filename, NULL)) {
		char* sjisname = utf82sjis(name, NULL);
		char* sjisfilename = utf82sjis(filename, NULL);
		ret = nvgCreateFont(nvgContext, sjisname, sjisfilename);
		free(sjisname);
		free(sjisfilename);
	} else {
		ret = nvgCreateFont(nvgContext, name, filename);
	}
#else
	ret = nvgCreateFont(nvgContext, name, filename);
#endif
	if (ret == -1) {
		Debug.PrintError("Failed to load font file %s. File is not exist, or not supported format.", filename);
		return -1;
	}
	return ret;
}

static int Video_CreateFontUTF8(const char* name, const char* filename) {
	Debug.PushBanner("**Error in Video.CreateFontUTF8");
	int ret = Video_CreateFontUTF8_(name, filename);
	Debug.PopBanner();
	return ret;
}

static int Video_CreateFont_(const char* name, const char* filename) {
	if (filename == NULL) {
		Debug.PrintError("The argument `filename` is NULL.");
		return -1;
	}
	if (name == NULL) {
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (is_contains_not_ascii(name, NULL) || is_contains_not_ascii(filename, NULL)) {
		utf8_t* utf8name = sjis2utf8(name, NULL);
		utf8_t* utf8filename = sjis2utf8(filename, NULL);
		ret = nvgCreateFont(nvgContext, utf8name, utf8filename);
		free(utf8name);
		free(utf8filename);
	} else {
		ret = nvgCreateFont(nvgContext, name, filename);
	}
#else
	ret = nvgCreateFont(nvgContext, name, filename);
#endif
	if (ret == -1) {
		Debug.PrintError("Failed to load font file %s. File is not exist, or not supported format.", filename);
		return -1;
	}
	return ret;
}

static int Video_CreateFont(const char* name, const char* filename) {
	Debug.PushBanner("**Error in Video.CreateFont");
	int ret = Video_CreateFont_(name, filename);
	Debug.PopBanner();
	return ret;
}

static int Video_CreateFontMemUTF8_(const char* name, unsigned char* data, int ndata, int freeData) {
	if (name == NULL) {
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	if (data == NULL) {
		Debug.PrintError("The argument `data` is NULL.");
		return -1;
	}
	if (ndata <= 0) {
		Debug.PrintError("The argument `ndata` is less than or equal to 0.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (is_contains_not_ascii(name, NULL)) {
		char* sjisname = utf82sjis(name, NULL);
		ret = nvgCreateFontMem(nvgContext, sjisname, data, ndata, freeData);
		free(sjisname);
	} else {
		ret = nvgCreateFontMem(nvgContext, name, data, ndata, freeData);
	}
#else
	ret = nvgCreateFontMem(nvgContext, name, data, ndata, freeData);
#endif
	if (ret == -1) {
		Debug.PrintError("Failed to load font data. Bad arguments or unsupported format data.");
		return -1;
	}
	return ret;
}

static int Video_CreateFontMemUTF8(const char* name, unsigned char* data, int ndata, int freeData) {
	Debug.PushBanner("**Error in Video.CreateFontMemUTF8");
	int ret = Video_CreateFontMemUTF8_(name, data, ndata, freeData);
	Debug.PopBanner();
	return ret;
}

static int Video_CreateFontMem_(const char* name, unsigned char* data, int ndata, int freeData) {
	if (name == NULL) {
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	if (data == NULL) {
		Debug.PrintError("The argument `data` is NULL.");
		return -1;
	}
	if (ndata <= 0) {
		Debug.PrintError("The argument `ndata` is less than or equal to 0.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (is_contains_not_ascii(name, NULL)) {
		utf8_t* utf8name = sjis2utf8(name, NULL);
		ret = nvgCreateFontMem(nvgContext, utf8name, data, ndata, freeData);
		free(utf8name);
	} else {
		ret = nvgCreateFontMem(nvgContext, name, data, ndata, freeData);
	}
#else
	ret = nvgCreateFontMem(nvgContext, name, data, ndata, freeData);
#endif
	if (ret == -1) {
		Debug.PrintError("Failed to load font data. Bad arguments or unsupported format data.");
		return -1;
	}
	return ret;
}

static int Video_CreateFontMem(const char* name, unsigned char* data, int ndata, int freeData) {
	Debug.PushBanner("**Error in Video.CreateFontMem");
	int ret = Video_CreateFontMem_(name, data, ndata, freeData);
	Debug.PopBanner();
	return ret;
}

static int Video_FindFontUTF8_(const char* name) {
	if (name == NULL) {
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (is_contains_not_ascii(name, NULL)) {
		char* sjisname = utf82sjis(name, NULL);
		ret = nvgFindFont(nvgContext, sjisname);
		free(sjisname);
	} else {
		ret = nvgFindFont(nvgContext, name);
	}
#else
	ret = nvgFindFont(nvgContext, name);
#endif
	return ret;
}

static int Video_FindFontUTF8(const char* name) {
	Debug.PushBanner("**Error in Video.FindFontUTF8");
	int ret = Video_FindFontUTF8_(name);
	Debug.PopBanner();
	return ret;
}

static int Video_FindFont_(const char* name) {
	if (name == NULL) {
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	return nvgFindFont(nvgContext, name);
}

static int Video_FindFont(const char* name) {
	Debug.PushBanner("**Error in Video.FindFont");
	int ret;
#if defined(_WIN32)
	if (is_contains_not_ascii(name, NULL)) {
		utf8_t* utf8name = sjis2utf8(name, NULL);
		ret = Video_FindFont_(utf8name);
		free(utf8name);
	} else {
		ret = Video_FindFont_(name);
	}
#else
	ret = Video_FindFont_(name);
#endif
	Debug.PopBanner();
	return ret;
}

static void Video_AddFallbackFontId(int baseFont, int fallbackFont) {
	nvgAddFallbackFontId(nvgContext, baseFont, fallbackFont);
}

static void Video_AddFallbackFont(const char* baseFont, const char* fallbackFont) {
	nvgAddFallbackFont(nvgContext, baseFont, fallbackFont);
}

static void Video_AddFallbackFontUTF8(const char* baseFont, const char* fallbackFont) {
	Debug.PushBanner("**Error in Video.AddFallbackFontUTF8");
#if defined(_WIN32)
	if (is_contains_not_ascii(baseFont, NULL) || is_contains_not_ascii(fallbackFont, NULL)) {
		utf8_t* utf8baseFont = sjis2utf8(baseFont, NULL);
		utf8_t* utf8fallbackFont = sjis2utf8(fallbackFont, NULL);
		Video_AddFallbackFont(utf8baseFont, utf8fallbackFont);
		free(utf8baseFont);
	} else {
		Video_AddFallbackFont(baseFont, fallbackFont);
	}
#else
	Video_AddFallbackFont(baseFont, fallbackFont);
#endif
	Debug.PopBanner();
}

static void Video_FontSize(float size) {
	nvgFontSize(nvgContext, size);
}

static void Video_FontBlur(float blur) {
	nvgFontBlur(nvgContext, blur);
}

static void Video_TextLetterSpacing(float spacing) {
	nvgTextLetterSpacing(nvgContext, spacing);
}

static void Video_TextLineHeight(float lineHeight) {
	nvgTextLineHeight(nvgContext, lineHeight);
}

static void Video_TextAlign(enum Align align) {
	nvgTextAlign(nvgContext, align);
}

static void Video_FontFaceId(int font) {
	nvgFontFaceId(nvgContext, font);
}

static void Video_FontFaceUTF8(const char* font) {
	nvgFontFace(nvgContext, font);
}

static void Video_FontFace(const char* font) {
#if defined(_WIN32)
	if (is_contains_not_ascii(font, NULL)) {
		utf8_t* utf8font = sjis2utf8(font, NULL);
		nvgFontFace(nvgContext, utf8font);
		free(utf8font);
	} else {
		nvgFontFace(nvgContext, font);
	}
#else
	nvgFontFace(nvgContext, font);
#endif
}

static float Video_TextUTF8(float x, float y, const char* utf8str, const char* end) {
	Debug.PushBanner("**Error in Video.TextUTF8");
	float ret = 0.0;
	if (utf8str == NULL) {
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	ret = nvgText(nvgContext, x, y, utf8str, end);
EXIT:
	Debug.PopBanner();
	return ret;
}

static float Video_Text(float x, float y, const char* string, const char* end) {
	Debug.PushBanner("**Error in Video.Text");
	float ret = 0.0;
	if (string == NULL) {
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (is_contains_not_ascii(string, end)) {
		utf8_t* utf8str = sjis2utf8(string, end);
		ret = Video_TextUTF8(x, y, utf8str, NULL);
		free(utf8str);
	} else {
		ret = nvgText(nvgContext, x, y, string, end);
	}
#else
	ret = nvgText(nvgContext, x, y, string, end);
#endif
EXIT:
	Debug.PopBanner();
	return ret;
}

static void Video_TextBoxUTF8(float x, float y, float breakRowWidth, const char* utf8str, const char* end) {
	Debug.PushBanner("**Error in Video.TextBoxUTF8");
	if (utf8str == NULL) {
		Debug.PrintError("The argument `utf8str` is NULL.");
	} else {
		nvgTextBox(nvgContext, x, y, breakRowWidth, utf8str, end);
	}
	Debug.PopBanner();
}

static void Video_TextBox(float x, float y, float breakRowWidth, const char* string, const char* end) {
	Debug.PushBanner("**Error in Video.TextBox");
	if (string == NULL) {
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (is_contains_not_ascii(string, end)) {
		utf8_t* utf8str = sjis2utf8(string, end);
		Video_TextBoxUTF8(x, y, breakRowWidth, utf8str, NULL);
		free(utf8str);
	} else {
		nvgTextBox(nvgContext, x, y, breakRowWidth, string, end);
	}
#else
	nvgTextBox(nvgContext, x, y, breakRowWidth, string, end);
#endif
EXIT:
	Debug.PopBanner();
}

static float Video_TextBoundsUTF8(float x, float y, const char* utf8str, const char* end, float* bounds) {
	Debug.PushBanner("**Error in Video.TextBoundsUTF8");
	float ret = 0;
	if (utf8str == NULL) {
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	if (bounds == NULL) {
		Debug.PrintError("The argument `bounds` is NULL.");
		goto EXIT;
	}
	ret = nvgTextBounds(nvgContext, x, y, utf8str, end, bounds);
EXIT:
	Debug.PopBanner();
	return ret;
}

static float Video_TextBounds(float x, float y, const char* string, const char* end, float* bounds) {
	float ret = 0;
	Debug.PushBanner("**Error in Video.TextBounds");
	if (string == NULL) {
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
	if (bounds == NULL) {
		Debug.PrintError("The argument `bounds` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (is_contains_not_ascii(string, end)) {
		utf8_t* utf8str = sjis2utf8(string, end);
		ret = Video_TextBoundsUTF8(x, y, utf8str, NULL, bounds);
		free(utf8str);
	} else {
		ret = nvgTextBounds(nvgContext, x, y, string, end, bounds);
	}
#else
	ret = nvgTextBounds(nvgContext, x, y, string, end, bounds);
#endif
	Debug.PopBanner();
EXIT:
	return ret;
}

static void Video_TextBoxBoundsUTF8(float x, float y, float breakRowWidth, const char* utf8str, const char* end, float* bounds) {
	Debug.PushBanner("**Error in Video.TextBoxBoundsUTF8");
	if (utf8str == NULL) {
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	if (bounds == NULL) {
		Debug.PrintError("The argument `bounds` is NULL.");
		goto EXIT;
	}
	nvgTextBoxBounds(nvgContext, x, y, breakRowWidth, utf8str, end, bounds);
EXIT:
	Debug.PopBanner();
}

static void Video_TextBoxBounds(float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds) {
	Debug.PushBanner("**Error in Video.TextBoxBounds");
	if (string == NULL) {
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
	if (bounds == NULL) {
		Debug.PrintError("The argument `bounds` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (is_contains_not_ascii(string, end)) {
		utf8_t* utf8str = sjis2utf8(string, end);
		Video_TextBoxBoundsUTF8(x, y, breakRowWidth, utf8str, NULL, bounds);
		free(utf8str);
	} else {
		nvgTextBoxBounds(nvgContext, x, y, breakRowWidth, string, end, bounds);
	}
#else
	nvgTextBoxBounds(nvgContext, x, y, breakRowWidth, string, end, bounds);
#endif
EXIT:
	Debug.PopBanner();
}

static int Video_TextGlyphPositionsUTF8(float x, float y, const char* utf8str, const char* end, GlyphPosition* positions, int maxPositions) {
	Debug.PushBanner("**Error in Video.TextGlyphPositionsUTF8");
	int ret = 0;
	if (utf8str == NULL) {
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	if (positions == NULL) {
		Debug.PrintError("The argument `positions` is NULL.");
		goto EXIT;
	}
	if (maxPositions <= 0) {
		Debug.PrintError("The argument `maxPositions` must greater than 0. (You gave %d to the argument `maxPositions`)", maxPositions);
		goto EXIT;
	}

	ret = nvgTextGlyphPositions(nvgContext, x, y, utf8str, end, (NVGglyphPosition*)positions, maxPositions);
EXIT:
	Debug.PopBanner();
	return ret;
}

static int Video_TextGlyphPositions(float x, float y, const char* string, const char* end, GlyphPosition* positions, int maxPositions) {
	int ret = 0;
	if (string == NULL) {
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
	if (positions == NULL) {
		Debug.PrintError("The argument `positions` is NULL.");
		goto EXIT;
	}
	if (maxPositions <= 0) {
		Debug.PrintError("The argument `maxPositions` must greater than 0. (You gave %d to the argument `maxPositions`)", maxPositions);
		goto EXIT;
	}
#if defined(_WIN32)
	if (is_contains_not_ascii(string, end)) {
		utf8_t* utf8str = sjis2utf8(string, end);
		ret = nvgTextGlyphPositions(nvgContext, x, y, utf8str, NULL, (NVGglyphPosition*)positions, maxPositions);
		for (int i = 0; i < ret; i++) {
			positions[i].str = String.Sjis.Inc(string, String.Utf8.Count(utf8str, positions[i].str));
		}
		free(utf8str);
	} else {
		ret = Video_TextGlyphPositionsUTF8(x, y, string, end, positions, maxPositions);
	}
#else
	ret = nvgTextGlyphPositions(nvgContext, x, y, string, end, (NVGglyphPosition*)positions, maxPositions);
#endif
EXIT:
	Debug.PopBanner();
	return ret;
}

static void Video_TextMetrics(float* ascender, float* descender, float* lineh) {
	nvgTextMetrics(nvgContext, ascender, descender, lineh);
}

static int Video_TextBreakLinesUTF8(const char* utf8str, const char* end, float breakRowWidth, TextRow* rows, int maxRows) {
	Debug.PushBanner("**Error in Video.TextBreakLinesUTF8");
	int ret = 0;
	if (utf8str == NULL) {
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	if (rows == NULL) {
		Debug.PrintError("The argument `rows` is NULL.");
		goto EXIT;
	}
	if (maxRows <= 0) {
		Debug.PrintError("The argument `maxRows` must greater than 0. (You gave %d to the argument `maxRows`)", maxRows);
		goto EXIT;
	}
	ret = nvgTextBreakLines(nvgContext, utf8str, end, breakRowWidth, (NVGtextRow*)rows, maxRows);
EXIT:
	Debug.PopBanner();
	return ret;
}

static int Video_TextBreakLines(const char* string, const char* end, float breakRowWidth, TextRow* rows, int maxRows) {
	Debug.PushBanner("**Error in Video.TextBreakLinesUTF8");
	int ret = 0;
	if (string == NULL) {
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
	if (rows == NULL) {
		Debug.PrintError("The argument `rows` is NULL.");
		goto EXIT;
	}
	if (maxRows <= 0) {
		Debug.PrintError("The argument `maxRows` must greater than 0. (You gave %d to the argument `maxRows`)", maxRows);
		goto EXIT;
	}
#if defined(_WIN32)
	if (is_contains_not_ascii(string, end)) {
		utf8_t* utf8str = sjis2utf8(string, end);
		ret = Video_TextBreakLinesUTF8(utf8str, NULL, breakRowWidth, rows, maxRows);
		for (int i = 0; i < ret; i++) {
			rows[i].start = String.Sjis.Inc(string, String.Utf8.Count(utf8str, rows[i].start));
			rows[i].end = String.Sjis.Inc(string, String.Utf8.Count(utf8str, rows[i].end));
			rows[i].next = String.Sjis.Inc(string, String.Utf8.Count(utf8str, rows[i].next));
		}

		free(utf8str);
	} else {
		ret = nvgTextBreakLines(nvgContext, string, end, breakRowWidth, (NVGtextRow*)rows, maxRows);
	}
#else
	ret = nvgTextBreakLines(nvgContext, string, end, breakRowWidth, (NVGtextRow*)rows, maxRows);
#endif
EXIT:
	Debug.PopBanner();
	return ret;
}

static void Video_DrawImage_(int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle, float alpha) {
	if (image <= 0) {
		Debug.PrintError("The argument `image` is invalid value. (You gave %d to the argument `image`)", image);
		return;
	}
	nvgExDrawImage(nvgContext, image, sx, sy, sw, sh, dx, dy, dw, dh, angle, alpha);
}

static void Video_DrawImage(int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle, float alpha) {
	Debug.PushBanner("**Error in Video.DrawImage");
	Video_DrawImage_(image, sx, sy, sw, sh, dx, dy, dw, dh, angle, alpha);
	Debug.PopBanner();
}

static void Video_DrawImageWithFillColor_(int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle) {
	if (image <= 0) {
		Debug.PrintError("The argument `image` is invalid value. (You gave %d to the argument `image`)", image);
		return;
	}
	nvgExDrawImageWithFillColor(nvgContext, image, sx, sy, sw, sh, dx, dy, dw, dh, angle);
}

static void Video_DrawImageWithFillColor(int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle) {
	Debug.PushBanner("**Error in Video.DrawImage");
	Video_DrawImageWithFillColor_(image, sx, sy, sw, sh, dx, dy, dw, dh, angle);
	Debug.PopBanner();
}

static void Video_FormatText(float x, float y, const char* format, ...) {
	Debug.PushBanner("**Error in Video.FormatText");

	if (format == NULL) {
		Debug.PrintError("The argument `format` is NULL");
	} else {
		va_list args;
		va_start(args, format);
		string_t newBuf = String.FormatTextV(format, args);
		va_end(args);
		if (newBuf.c_str == NULL) {
			Debug.PrintError("There is not enough memory to do operation.");
		} else {
			Video_Text(x, y, newBuf.c_str, NULL);
			String.Free(newBuf);
		}
	}
	Debug.PopBanner();
}

#if 1

static void* Video_CreateFramebuffer(int w, int h, int imageFlags) {
	return (void*)nvgluCreateFramebuffer(nvgContext, w, h, imageFlags);
}

static void Video_DrawStartFramebuffer(void* fbo) {
	int winWidth, winHeight;
	glfwGetWindowSize(glfwWindow, &winWidth, &winHeight);

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(glfwWindow, &fbWidth, &fbHeight);

	float pxRatio = (float)fbWidth / (float)winWidth;

	NVGLUframebuffer* fb = (NVGLUframebuffer*)fbo;
	int fboWidth, fboHeight;
	nvgImageSize(nvgContext, fb->image, &fboWidth, &fboHeight);

	winWidth = (int)(fboWidth / pxRatio);
	winHeight = (int)(fboHeight / pxRatio);

	nvgluBindFramebuffer(fb);
	glViewport(0, 0, fboWidth, fboHeight);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	nvgBeginFrame(nvgContext, (float)winWidth, (float)winHeight, pxRatio);
}

static void Video_DrawEndFramebuffer(void) {
	nvgEndFrame(nvgContext);
	nvgluBindFramebuffer(NULL);
}

static void Video_DeleteFramebuffer(void* fb) {
	if (fb != NULL) {
		nvgluDeleteFramebuffer((NVGLUframebuffer*)fb);
	}
}

static int Video_GetFrameBufferImage(void* fb) {
	if (fb != NULL) {
		return ((NVGLUframebuffer*)fb)->image;
	}
	return 0;
}


static void unpremultiplyAlpha(unsigned char* image, int w, int h, int stride) {
	int x, y;

	// Unpremultiply
	for (y = 0; y < h; y++) {
		unsigned char* row = &image[y * stride];
		for (x = 0; x < w; x++) {
			int r = row[0], g = row[1], b = row[2], a = row[3];
			if (a != 0) {
				row[0] = (unsigned char)min(r * 255 / a, 255);
				row[1] = (unsigned char)min(g * 255 / a, 255);
				row[2] = (unsigned char)min(b * 255 / a, 255);
			}
			row += 4;
		}
	}

	// Defringe
	for (y = 0; y < h; y++) {
		unsigned char* row = &image[y * stride];
		for (x = 0; x < w; x++) {
			int r = 0, g = 0, b = 0, a = row[3], n = 0;
			if (a == 0) {
				if (x - 1 > 0 && row[-1] != 0) {
					r += row[-4];
					g += row[-3];
					b += row[-2];
					n++;
				}
				if (x + 1 < w && row[7] != 0) {
					r += row[4];
					g += row[5];
					b += row[6];
					n++;
				}
				if (y - 1 > 0 && row[-stride + 3] != 0) {
					r += row[-stride];
					g += row[-stride + 1];
					b += row[-stride + 2];
					n++;
				}
				if (y + 1 < h && row[stride + 3] != 0) {
					r += row[stride];
					g += row[stride + 1];
					b += row[stride + 2];
					n++;
				}
				if (n > 0) {
					row[0] = (unsigned char)(r / n);
					row[1] = (unsigned char)(g / n);
					row[2] = (unsigned char)(b / n);
				}
			}
			row += 4;
		}
	}
}

static void setAlpha(unsigned char* image, int w, int h, int stride, unsigned char a) {
	int x, y;
	for (y = 0; y < h; y++) {
		unsigned char* row = &image[y * stride];
		for (x = 0; x < w; x++)
			row[x * 4 + 3] = a;
	}
}

static void flipHorizontal(unsigned char* image, int w, int h, int stride) {
	int i = 0, j = h - 1, k;
	while (i < j) {
		unsigned char* ri = &image[i * stride];
		unsigned char* rj = &image[j * stride];
		for (k = 0; k < w * 4; k++) {
			unsigned char t = ri[k];
			ri[k] = rj[k];
			rj[k] = t;
		}
		i++;
		j--;
	}
}

bool Video_SaveScreenShot(int l, int t, int w, int h, bool premult, const char* name) {
	enum SaveMode {
		SM_None,
		SM_JPEG,
		SM_PNG,
		SM_BMP
	} mode = SM_None;
	{
		char* dot = strrchr(name, '.');
		if (_strcmpi(dot, ".jpg") == 0) {
			mode = SM_JPEG;
		} else if (_strcmpi(dot, ".png") == 0) {
			mode = SM_PNG;
		} else if (_strcmpi(dot, ".bmp") == 0) {
			mode = SM_BMP;
		} else {
			return false;
		}
	}

	unsigned char* image = (unsigned char*)malloc(w * h * 4);
	if (image == NULL) {
		return false;
	}
	glReadPixels(l, t, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image);
	if (premult)
		unpremultiplyAlpha(image, w, h, w * 4);
	else
		setAlpha(image, w, h, w * 4, 255);
	flipHorizontal(image, w, h, w * 4);

	{
		switch (mode) {
		case SM_JPEG:
			stbi_write_jpg(name, w, h, 4, image, 0);
			break;
		case SM_PNG:
			stbi_write_png(name, w, h, 4, image, w * 4);
			break;
		case SM_BMP:
			stbi_write_bmp(name, w, h, 0, image);
			break;
		default: break;
		}
	}
	free(image);
	return true;
}

bool Video_SaveScreenShotUTF8(int l, int t, int w, int h, bool premult, const char* name) {
	bool ret = false;
	Debug.PushBanner("**Error in Video.SaveScreenShot");
	if (name == NULL) {
		Debug.PrintError("The argument `name` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (is_contains_not_ascii(name, NULL)) {
		utf8_t* utf8str = sjis2utf8(name, NULL);
		ret = Video_SaveScreenShot(l, t, w, h, premult, utf8str);
		free(utf8str);
	} else {
		ret = Video_SaveScreenShot(l, t, w, h, premult, name);
	}
#else
	ret = Video_SaveScreenShot(l, t, w, h, premult, name);
#endif
EXIT:
	Debug.PopBanner();
	return ret;
}

#endif

const struct __tagVideoAPI Video = {
	Video_Initialize,
	Video_Finalize,
	Video_SetWindowTitleUTF8,
	Video_SetWindowTitle,
	Video_SetSize,
	Video_GetSize,
	Video_Drawing,
	Video_BeginDraw,
	Video_EndDraw,
	Video_BeginDrawEx,
	Video_ResetDrawEx,
	Video_EndDrawEx,
	Video_UpdateDrawEx,
	Video_Loop,
	Video_GetTime,
	Video_Sleep,
	Video_SetClearColor,
	Video_BeginFrame,
	Video_CancelFrame,
	Video_EndFrame,
	Video_GlobalCompositeOperation,
	Video_GlobalCompositeBlendFunc,
	Video_GlobalCompositeBlendFuncSeparate,
	Video_RGB,
	Video_RGBA,
	Video_LerpRGBA,
	Video_TransRGBA,
	Video_TransRGBAf,
	Video_HSL,
	Video_HSLA,
	Video_Save,
	Video_Restore,
	Video_Reset,
	Video_StrokeColor,
	Video_StrokePaint,
	Video_FillColor,
	Video_FillPaint,
	Video_MiterLimit,
	Video_StrokeWidth,
	Video_LineCap,
	Video_LineJoin,
	Video_GlobalAlpha,
	Video_ResetTransform,
	Video_Transform,
	Video_Translate,
	Video_Rotate,
	Video_SkewX,
	Video_SkewY,
	Video_Scale,
	Video_TranslateWorld,
	Video_RotateWorld,
	Video_SkewXWorld,
	Video_SkewYWorld,
	Video_ScaleWorld,
	Video_CurrentTransform,
	Video_TransformIdentity,
	Video_TransformTranslate,
	Video_TransformRotate,
	Video_TransformSkewX,
	Video_TransformSkewY,
	Video_TransformScale,
	Video_TransformMultiply,
	Video_TransformPremultiply,
	Video_TransformInverse,
	Video_TransformPoint,
	Video_DegToRad,
	Video_RadToDeg,
	Video_LoadImageData,
	Video_LoadImageDataUTF8,
	Video_FreeImageData,
	Video_CreateImageUTF8,
	Video_CreateImage,
	Video_CreateImageMem,
	Video_CreateImageRGBA,
	Video_UpdateImage,
	Video_ImageSize,
	Video_DeleteImage,
	Video_LinearGradient,
	Video_BoxGradient,
	Video_RadialGradient,
	Video_ImagePattern,
	Video_ImagePatternWithFillColor,
	Video_Scissor,
	Video_IntersectScissor,
	Video_ResetScissor,
	Video_BeginPath,
	Video_MoveTo,
	Video_LineTo,
	Video_BezierTo,
	Video_QuadTo,
	Video_ArcTo,
	Video_ClosePath,
	Video_PathWinding,
	Video_Arc,
	Video_Rect,
	Video_RoundedRect,
	Video_RoundedRectVarying,
	Video_Ellipse,
	Video_Circle,
	Video_Fill,
	Video_Stroke,
	Video_CreateFontUTF8,
	Video_CreateFont,
	Video_CreateFontMemUTF8,
	Video_CreateFontMem,
	Video_FindFontUTF8,
	Video_FindFont,
	Video_AddFallbackFontId,
	Video_AddFallbackFontUTF8,
	Video_AddFallbackFont,
	Video_FontSize,
	Video_FontBlur,
	Video_TextLetterSpacing,
	Video_TextLineHeight,
	Video_TextAlign,
	Video_FontFaceId,
	Video_FontFaceUTF8,
	Video_FontFace,
	Video_TextUTF8,
	Video_Text,
	Video_TextBoxUTF8,
	Video_TextBox,
	Video_TextBoundsUTF8,
	Video_TextBounds,
	Video_TextBoxBoundsUTF8,
	Video_TextBoxBounds,
	Video_TextGlyphPositionsUTF8,
	Video_TextGlyphPositions,
	Video_TextMetrics,
	Video_TextBreakLinesUTF8,
	Video_TextBreakLines,
	Video_DrawImage,
	Video_DrawImageWithFillColor,
	Video_FormatText,
	Video_CreateFramebuffer,
	Video_DrawStartFramebuffer,
	Video_DrawEndFramebuffer,
	Video_DeleteFramebuffer,
	Video_GetFrameBufferImage,
	Video_SaveScreenShotUTF8,
	Video_SaveScreenShot,
};

//// input ////

//
// Windows上において、glfwSetMouseButtonCallback は 
// マウス関連のウィンドウメッセージを処理するたびに呼び出される。(win32_windows.c:405)
// そのため、１フレーム中に複数個のマウス関連ウィンドウメッセージが送られていると、
// 複数回呼び出される。
// これはフレームベース駆動では都合が悪い動作の原因となるので本ライブラリ中では利用しない。
// 

typedef enum __tagButtonStatus {
	BS_StillUp = 0,
	BS_Down = 1,
	BS_StillDown = 2,
	BS_Up = 3,
} ButtonStatus;

typedef struct __tagMouseContext {
	struct {
		ButtonStatus Status;
	} LeftButton, RightButton, MiddleButton;
	struct {
		double X;
		double Y;
	} CursorPos;
} MouseContext;

static MouseContext mouseContext;

static void UpdateButtonStatus(ButtonStatus* status, bool isDown) {
	if (isDown) {
		switch (*status) {
		case BS_Up:
		case BS_StillUp:
		default:
			*status = BS_Down;
			break;
		case BS_Down:
		case BS_StillDown:
			*status = BS_StillDown;
			break;
		}
	} else {
		switch (*status) {
		case BS_Up:
		case BS_StillUp:
			*status = BS_StillUp;
			break;
		case BS_Down:
		case BS_StillDown:
		default:
			*status = BS_Up;
			break;
		}
	}
}

static void Mouse_Update(void) {
	UpdateButtonStatus(&mouseContext.LeftButton.Status, glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	UpdateButtonStatus(&mouseContext.RightButton.Status, glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	UpdateButtonStatus(&mouseContext.MiddleButton.Status, glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
	glfwGetCursorPos(glfwWindow, &mouseContext.CursorPos.X, &mouseContext.CursorPos.Y);
}

static bool Mouse_IsLeftButtonDown(void) {
	return mouseContext.LeftButton.Status == BS_Down ||
		mouseContext.LeftButton.Status == BS_StillDown;
}

static bool Mouse_IsRightButtonDown(void) {
	return mouseContext.RightButton.Status == BS_Down ||
		mouseContext.RightButton.Status == BS_StillDown;
}

static bool Mouse_IsMiddleButtonDown(void) {
	return mouseContext.MiddleButton.Status == BS_Down ||
		mouseContext.MiddleButton.Status == BS_StillDown;
}

static bool Mouse_IsLeftButtonPush(void) {
	return mouseContext.LeftButton.Status == BS_Down;
}

static bool Mouse_IsRightButtonPush(void) {
	return mouseContext.RightButton.Status == BS_Down;
}

static bool Mouse_IsMiddleButtonPush(void) {
	return mouseContext.MiddleButton.Status == BS_Down;
}

static bool Mouse_IsLeftButtonUp(void) {
	return mouseContext.LeftButton.Status == BS_Up ||
		mouseContext.LeftButton.Status == BS_StillUp;
}

static bool Mouse_IsRightButtonUp(void) {
	return mouseContext.RightButton.Status == BS_Up ||
		mouseContext.RightButton.Status == BS_StillUp;
}

static bool Mouse_IsMiddleButtonUp(void) {
	return mouseContext.MiddleButton.Status == BS_Up ||
		mouseContext.MiddleButton.Status == BS_StillUp;
}

static bool Mouse_IsLeftButtonRelease(void) {
	return mouseContext.LeftButton.Status == BS_Up;
}

static bool Mouse_IsRightButtonRelease(void) {
	return mouseContext.RightButton.Status == BS_Up;
}

static bool Mouse_IsMiddleButtonRelease(void) {
	return mouseContext.MiddleButton.Status == BS_Up;
}

static void Mouse_GetCursorPos(double* x, double* y) {
	*x = mouseContext.CursorPos.X;
	*y = mouseContext.CursorPos.Y;
}

const struct __tagMouseAPI Mouse = {
	Mouse_Update,
	Mouse_IsLeftButtonDown,
	Mouse_IsRightButtonDown,
	Mouse_IsMiddleButtonDown,
	Mouse_IsLeftButtonPush,
	Mouse_IsRightButtonPush,
	Mouse_IsMiddleButtonPush,
	Mouse_IsLeftButtonUp,
	Mouse_IsRightButtonUp,
	Mouse_IsMiddleButtonUp,
	Mouse_IsLeftButtonRelease,
	Mouse_IsRightButtonRelease,
	Mouse_IsMiddleButtonRelease,
	Mouse_GetCursorPos,
};

typedef struct __tagJoystickInfo {
	// ジョイスティックが接続されているかどうか
	bool Presented;

	// デバイス名
	char* Name;

	// 軸情報
	struct {
		int Count;
		float* RawStatus;
	} Axes;

	// ボタン情報
	struct {
		int Count;
		unsigned char* RawStatus;
		ButtonStatus* Status;
	} Buttons;
} JoystickInfo;

static struct __tagJoystickContext {
	bool Initialized;
	JoystickInfo Joysticks[GLFW_JOYSTICK_LAST];
} JoystickContext;

static int Joystick_MaxJoystickPort(void) {
	return GLFW_JOYSTICK_LAST;
}

static void Joystick_DisconnectOne(int i) {
	if (JoystickContext.Joysticks[i].Presented) {
		JoystickContext.Joysticks[i].Presented = false;
		if (JoystickContext.Joysticks[i].Name != NULL) {
			free(JoystickContext.Joysticks[i].Name);
			JoystickContext.Joysticks[i].Name = NULL;
		}
		JoystickContext.Joysticks[i].Axes.Count = 0;
		if (JoystickContext.Joysticks[i].Axes.RawStatus != NULL) {
			free(JoystickContext.Joysticks[i].Axes.RawStatus);
			JoystickContext.Joysticks[i].Axes.RawStatus = NULL;
		}
		JoystickContext.Joysticks[i].Buttons.Count = 0;
		if (JoystickContext.Joysticks[i].Buttons.RawStatus != NULL) {
			free(JoystickContext.Joysticks[i].Buttons.RawStatus);
			JoystickContext.Joysticks[i].Buttons.RawStatus = NULL;
		}
		if (JoystickContext.Joysticks[i].Buttons.Status != NULL) {
			free(JoystickContext.Joysticks[i].Buttons.Status);
			JoystickContext.Joysticks[i].Buttons.Status = NULL;
		}
	}
}

static void Joystick_ConnectOne(int i) {
	if (glfwJoystickPresent(i) == GL_TRUE) {
		if (JoystickContext.Joysticks[i].Presented == true) {
			Joystick_DisconnectOne(i);
		}
		JoystickContext.Joysticks[i].Presented = true;
		// ジョイスティック名を取得
		JoystickContext.Joysticks[i].Name = _strdup(glfwGetJoystickName(i));
		if (JoystickContext.Joysticks[i].Name == NULL) {
			Joystick_DisconnectOne(i);
			return;
		}
		// 軸の数を取得してバッファを作成
		int count;
		(void)glfwGetJoystickAxes(i, &count);
		JoystickContext.Joysticks[i].Axes.Count = count;
		JoystickContext.Joysticks[i].Axes.RawStatus = calloc(count, sizeof(float));
		if (JoystickContext.Joysticks[i].Axes.RawStatus == NULL) {
			Joystick_DisconnectOne(i);
			return;
		}
		// ボタンの数を取得してバッファを作成
		(void)glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
		JoystickContext.Joysticks[i].Buttons.Count = count;
		JoystickContext.Joysticks[i].Buttons.RawStatus = calloc(count, sizeof(unsigned char));
		if (JoystickContext.Joysticks[i].Buttons.RawStatus == NULL) {
			Joystick_DisconnectOne(i);
			return;
		}
		JoystickContext.Joysticks[i].Buttons.Status = calloc(count, sizeof(ButtonStatus));
		if (JoystickContext.Joysticks[i].Buttons.Status == NULL) {
			Joystick_DisconnectOne(i);
			return;
		}
	}
}

static void Joystick_UpdateOne(int i) {
	if (JoystickContext.Joysticks[i].Presented == true) {
		if (glfwJoystickPresent(i) == GL_FALSE) {
			Joystick_DisconnectOne(i);
			return;
		}
		{
			int count;
			const float* states = glfwGetJoystickAxes(i, &count);
			if (JoystickContext.Joysticks[i].Axes.Count != count) {
				Joystick_DisconnectOne(i);
				return;
			}
			memcpy(JoystickContext.Joysticks[i].Axes.RawStatus, states, count * sizeof(float));
		}

		{
			int count;
			const unsigned char* button_states = glfwGetJoystickButtons(i, &count);
			if (JoystickContext.Joysticks[i].Buttons.Count != count) {
				Joystick_DisconnectOne(i);
				return;
			}
			memcpy(JoystickContext.Joysticks[i].Buttons.RawStatus, button_states, count * sizeof(unsigned char));
			for (int j = 0; j < count; j++) {
				UpdateButtonStatus(&JoystickContext.Joysticks[i].Buttons.Status[j], JoystickContext.Joysticks[i].Buttons.RawStatus[j] != 0);
			}
		}
	}
}

static void Joystick_Connect(void) {
	for (int i = 0; i < GLFW_JOYSTICK_LAST; i++) {
		Joystick_UpdateOne(i);
	}
}

static void Joystick_Disconnect(void) {
	for (int i = 0; i < GLFW_JOYSTICK_LAST; i++) {
		Joystick_UpdateOne(i);
	}
}

static void Joystick_Initialize(void) {
	if (JoystickContext.Initialized == false) {
		memset(&JoystickContext, 0, sizeof(JoystickContext));
		Joystick_Connect();
		JoystickContext.Initialized = true;
	}
}

static void Joystick_Finalize(void) {
	if (JoystickContext.Initialized == true) {
		Joystick_Disconnect();
		memset(&JoystickContext, 0, sizeof(JoystickContext));
		JoystickContext.Initialized = false;
	}
}

static void Joystick_Update(void) {
	for (int i = 0; i < GLFW_JOYSTICK_LAST; i++) {
		Joystick_UpdateOne(i);
	}
}

static bool Joystick_IsPresented(int id) {
	if (id < 0 || GLFW_JOYSTICK_LAST <= id) {
		return false;
	}
	return JoystickContext.Joysticks[id].Presented;
}

static int Joystick_GetAxesCount(int id) {
	if (Joystick_IsPresented(id) == false) {
		return -1;
	}
	return JoystickContext.Joysticks[id].Axes.Count;
}

static float Joystick_GetAxesStatus(int id, int ax) {
	if (Joystick_IsPresented(id) == false) {
		return 0.0f;
	}
	if (ax < 0 || JoystickContext.Joysticks[id].Axes.Count <= ax) {
		return 0.0f;
	}
	return JoystickContext.Joysticks[id].Axes.RawStatus[ax];
}

static int Joystick_GetButtonCount(int id) {
	if (Joystick_IsPresented(id) == false) {
		return -1;
	}
	return JoystickContext.Joysticks[id].Buttons.Count;
}

static bool Joystick_IsButtonDown(int id, int btn) {
	if (Joystick_IsPresented(id) == false) {
		return false;
	}
	if (btn < 0 || JoystickContext.Joysticks[id].Buttons.Count <= btn) {
		return false;
	}
	return JoystickContext.Joysticks[id].Buttons.Status[btn] == BS_Down || JoystickContext.Joysticks[id].Buttons.Status[btn] == BS_StillDown;
}

static bool Joystick_IsButtonPush(int id, int btn) {
	if (Joystick_IsPresented(id) == false) {
		return false;
	}
	if (btn < 0 || JoystickContext.Joysticks[id].Buttons.Count <= btn) {
		return false;
	}
	return JoystickContext.Joysticks[id].Buttons.Status[btn] == BS_Down;
}

static bool Joystick_IsButtonUp(int id, int btn) {
	if (Joystick_IsPresented(id) == false) {
		return false;
	}
	if (btn < 0 || JoystickContext.Joysticks[id].Buttons.Count <= btn) {
		return false;
	}
	return JoystickContext.Joysticks[id].Buttons.Status[btn] == BS_Up || JoystickContext.Joysticks[id].Buttons.Status[btn] == BS_StillUp;
}

static bool Joystick_IsButtonRelease(int id, int btn) {
	if (Joystick_IsPresented(id) == false) {
		return false;
	}
	if (btn < 0 || JoystickContext.Joysticks[id].Buttons.Count <= btn) {
		return false;
	}
	return JoystickContext.Joysticks[id].Buttons.Status[btn] == BS_Up;
}

const struct __tagJoystickAPI Joystick = {
	Joystick_MaxJoystickPort,
	Joystick_Initialize,
	Joystick_Finalize,
	Joystick_Connect,
	Joystick_Disconnect,
	Joystick_Update,
	Joystick_IsPresented,
	Joystick_GetAxesCount,
	Joystick_GetAxesStatus,
	Joystick_GetButtonCount,
	Joystick_IsButtonDown,
	Joystick_IsButtonPush,
	Joystick_IsButtonUp,
	Joystick_IsButtonRelease,
};

static bool Keyboard_IsKeyDown(enum Keycode key) {
	return glfwGetKey(glfwWindow, key) == GLFW_RELEASE ? false : true;
}

const struct __tagKeyboardAPI Keyboard = {
	Keyboard_IsKeyDown,
};

//// FrameRate ////

static void FrameRate_SetFrameRate(unsigned int fps) {
	FpsTimer.SetFPS(fps);
}

static unsigned int FrameRate_GetFrameRate(void) {
	return FpsTimer.GetFPS();
}

static float FrameRate_GetRealFrameRate() {
	return FpsTimer.GetRealFPS();
}

static unsigned int FrameRate_GetCPUPower(void) {
	return FpsTimer.GetCPUPower();
}

const struct __tagFrameRateAPI FrameRate = {
	FrameRate_SetFrameRate,
	FrameRate_GetFrameRate,
	FrameRate_GetRealFrameRate,
	FrameRate_GetCPUPower,
};
