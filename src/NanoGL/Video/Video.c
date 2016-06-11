#if defined(_WIN32)
#include <windows.h>
#define msleep(x) Sleep(x)
#elif defined(__APPLE__)
#include <unistd.h>
#define msleep(x) usleep((x)*(1000))
#endif
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <nanovg.h>
#include <nanovg_gl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>

#include "Video.h"
#include "FpsTimer.h"
#include "nanovg-extra-apis.h"
#include "../Debug/Debug.h"
#include "../System/System.h"
#include "../String/String.h"
#include "../Math/Math.h"

static GLFWwindow *glfwWindow;
static NVGcontext *nvgContext;

#if defined(_WIN32)
typedef char utf8_t;
static char *utf82sjis(const utf8_t *pUtf8Str, const utf8_t *pEnd) {
	// Convert UTF8 to WideChar(UTF-16)
	int nUtf8ByteCount = pEnd - pUtf8Str;
	int nUtf16WordCount = MultiByteToWideChar(CP_UTF8, 0, pUtf8Str, nUtf8ByteCount, NULL, 0);
	wchar_t *pUtf16Str = calloc(nUtf16WordCount + 1, sizeof(wchar_t));
	if (pUtf16Str == NULL) { return NULL; }
	if (MultiByteToWideChar(CP_UTF8, 0, pUtf8Str, nUtf8ByteCount, pUtf16Str, nUtf16WordCount) != nUtf16WordCount) {
		free(pUtf16Str);
		return NULL;
	}

	// Convert WideChar(UTF-16) to UShiftJIS(CP923)
	int nConvBytes = WideCharToMultiByte(CP_ACP, 0, pUtf16Str, -1, NULL, 0, NULL, NULL);
	char *pSjisStr = calloc(nConvBytes + 1, sizeof(char));
	if (pSjisStr == NULL) {
		free(pUtf16Str);
		return false;
	}
	if (WideCharToMultiByte(CP_ACP, 0, pUtf16Str, -1, pSjisStr, nConvBytes, NULL, NULL) != nConvBytes) {
		free(pUtf16Str);
		free(pSjisStr);
		return false;
	}

	free(pUtf16Str);

	return pSjisStr;
}
static utf8_t *sjis2utf8(const char *pSjisStr, const char *pEnd) {
	// Convert ShiftJIS(CP923) to WideChar(UTF-16)
	int nConvBytes = pEnd == NULL ? -1 : (pEnd - pSjisStr);
	int nUtf16WordCount = MultiByteToWideChar(CP_ACP, 0, pSjisStr, nConvBytes, NULL, 0);
	wchar_t *pUtf16Str = calloc(nUtf16WordCount + 1, sizeof(wchar_t));
	if (pUtf16Str == NULL) { return NULL; }
	if (MultiByteToWideChar(CP_ACP, 0, pSjisStr, nConvBytes, pUtf16Str, nUtf16WordCount) != nUtf16WordCount) {
		free(pUtf16Str);
		return NULL;
	}

	// Convert WideChar(UTF-16) to ShiftJIS(CP923)
	int nUtf8WordCount = WideCharToMultiByte(CP_UTF8, 0, pUtf16Str, -1, NULL, 0, NULL, NULL);
	utf8_t *pUtf8Str = calloc(nUtf8WordCount + 1, sizeof(utf8_t));
	if (pUtf8Str == NULL) {
		free(pUtf16Str);
		return false;
	}
	if (WideCharToMultiByte(CP_UTF8, 0, pUtf16Str, -1, pUtf8Str, nUtf8WordCount, NULL, NULL) != nUtf8WordCount) {
		free(pUtf16Str);
		free(pUtf8Str);
		return false;
	}

	free(pUtf16Str);

	return pUtf8Str;
}

static bool IsContainsNotAscii(const char *str, const char *end)
{
	while (str != end && *str != '\0')
	{
		if (!isascii(*str++)) { return true; }
	}
	return false;
}

#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1800)
# define inline __inline
#endif

#define JugglingCastImpl(x,y) \
static inline y x##2##y(x _x) { return *((y*)&_x); } \
static inline x y##2##x(y _y) { return *((x*)&_y); }

#define JugglingCast(x, y, v)	x##2##y(v)

JugglingCastImpl(NVGcolor, Color)
JugglingCastImpl(NVGpaint, Paint)

static bool Video_Initialize_(void)
{
	if (glfwInit() == GL_FALSE)
	{
		Debug.PrintError("Failed to initialize GLFW.");
		return false;
	}

#if 0
	// Older VGA Driver (ie. Intel HD Graphics 4000 driver 8.15.10.2696 on Windows7)
	// need to disable FORWARD_COMPAT and OPENGL_CORE_PROFILE
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	// Using OpenGL 4.0 Core Profile Only (disable backward compatibility)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	
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

	// Initialize GLEW
	// You must place the glewInit() immediately after glfwMakeContextCurrent()! !! MUST !!
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		Debug.PrintError("Failed to initialize GLEW.");
		glfwTerminate();
		return false;
	}

	// Create NanoVG's context
	nvgContext = nvgCreateGL3(NVG_STENCIL_STROKES);
	if (nvgContext == NULL)
	{
		Debug.PrintError("Failed to create NanoVG's Context.");
		glfwTerminate();
		return false;
	}


	return true;
}

static bool Video_Initialize(void)
{
	Debug.PushBanner("**Error in Video.Initialize");
	bool ret = Video_Initialize_();
	FpsTimer.Initialize();
	Debug.PopBanner();
	return ret;
}

static void Video_Finalize(void)
{
	FpsTimer.Finalize();
	if (nvgContext != NULL) {
		nvgDeleteGL3(nvgContext);
		nvgContext = NULL;
	}
	glfwTerminate();
}

static void Video_SetWindowTitleUTF8(const char *title)
{
	glfwSetWindowTitle(glfwWindow, title);
}

static void Video_SetWindowTitle(const char *title)
{
#if defined(_WIN32)
	if (IsContainsNotAscii(title,NULL))
	{
		utf8_t *utf8title = sjis2utf8(title, NULL);
		glfwSetWindowTitle(glfwWindow, utf8title);
		free(utf8title);
	} else
	{
		glfwSetWindowTitle(glfwWindow, title);
	}
#else
	glfwSetWindowTitle(glfwWindow, title);
#endif
}

static void Video_SetSize(int width, int height)
{
	glfwSetWindowSize(glfwWindow, max(width, 1), max(height, 1));
}

static void Video_BeginDraw(void) {
	int ww, wh;
	glfwGetWindowSize(glfwWindow, &ww, &wh);
	int fbw, fbh;
	glfwGetFramebufferSize(glfwWindow, &fbw, &fbh);

	glViewport(0, 0, fbw, fbh);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	float pxRatio = fbw * 1.0f / ww;
	nvgBeginFrame(nvgContext, ww, wh, pxRatio);

	nvgReset(nvgContext);
}

static void Video_EndDraw(void) {
	nvgEndFrame(nvgContext);
	glfwSwapBuffers(glfwWindow);
}

static bool Video_Drawing(void) {
	static bool need_finish = false;

	if (need_finish) {
		Video_EndDraw();
		need_finish = false;
	}

	glfwPollEvents();
	if (glfwWindowShouldClose(glfwWindow) != GL_FALSE) {
		System.Quit();
	}
	FpsTimer.WaitFrame();

	Video_BeginDraw();
	need_finish = true;

	return true;
}

static double Video_GetTime(void)
{
	return glfwGetTime();
}

static double Video_Sleep(double sec)
{
	double start = glfwGetTime();
	int ms = (int)(sec * 1000);
	msleep(ms);
	double end = glfwGetTime();
	return end - start;
}

static void Video_SetFrameRate(unsigned int fps)
{
	FpsTimer.SetFPS(fps);
}

static unsigned int Video_GetFrameRate(void)
{
	return FpsTimer.GetFPS();
}

static float Video_GetRealFrameRate()
{
	return FpsTimer.GetRealFPS();
}

static unsigned int Video_GetCPUPower(void)
{
	return FpsTimer.GetCPUPower();
}

// opengl api's
static void Video_SetClearColor(Color color)
{
	// Sets the color of the erase of screen to be used by glClear function
	glClearColor(color.s.r, color.s.g, color.s.b, color.s.a);
}

// nanovg api's

static void Video_BeginFrame(int windowWidth, int windowHeight, float devicePixelRatio) {
	nvgBeginFrame(nvgContext, windowWidth, windowHeight, devicePixelRatio);
}

static void Video_CancelFrame(void) {
	nvgCancelFrame(nvgContext);
}

static void Video_EndFrame(void) {
	nvgEndFrame(nvgContext);
}

static Color Video_RGB(unsigned char r, unsigned char g, unsigned char b) {
	return JugglingCast(NVGcolor,Color,nvgRGB(r, g, b));
}

static Color Video_RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	return JugglingCast(NVGcolor,Color,nvgRGBA(r, g, b, a));
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
	nvgStrokeColor(nvgContext, JugglingCast(Color,NVGcolor,color));
}

static void Video_StrokePaint(Paint paint) {
	nvgStrokePaint(nvgContext, JugglingCast(Paint,NVGpaint,paint));
}

static void Video_FillColor(Color color) {
	nvgFillColor(nvgContext, JugglingCast(Color,NVGcolor,color));
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

#if defined(UNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD)
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

#endif
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
	if (filename == NULL)
	{
		Debug.PrintError("The argument `filename` is NULL.");
		return 0;
	}
	int ret;
#if defined(_WIN32)
	if (IsContainsNotAscii(filename, NULL))
	{
		char *sjisfilename = utf82sjis(filename, NULL);
		ret = nvgCreateImage(nvgContext, sjisfilename, imageFlags);
		if (ret == 0)
		{
			Debug.PrintError("Failed to load image file %s. File is not exist, or not supported format.", sjisfilename);
		}
		free(sjisfilename);
	}
	else
	{
		ret = nvgCreateImage(nvgContext, filename, imageFlags);
	}
#else
	ret = nvgCreateImage(nvgContext, filename, imageFlags);
#endif
	if (ret == 0)
	{
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
	if (filename == NULL)
	{
		Debug.PrintError("The argument `filename` is NULL.");
		return 0;
	}
	int ret;
#if defined(_WIN32)
	if (IsContainsNotAscii(filename, NULL))
	{
		utf8_t *utf8filename = sjis2utf8(filename, NULL);
		ret = nvgCreateImage(nvgContext, utf8filename, imageFlags);
		free(utf8filename);
	}
	else
	{
		ret = nvgCreateImage(nvgContext, filename, imageFlags);
	}
#else
	ret = nvgCreateImage(nvgContext, filename, imageFlags);
#endif
	if (ret == 0)
	{
		Debug.PrintError("Failed to load image file %s. File is not exist, or not supported format.", filename);
		return 0;
	}
	return ret;
}

static int Video_CreateImage(const char* filename, enum ImageFlags imageFlags) {
	Debug.PushBanner("**Error in Video.CreateImage");
	int ret = Video_CreateImage_(filename, imageFlags);
	Debug.PopBanner();
	return ret;
}

static int Video_CreateImageMem_(enum ImageFlags imageFlags, unsigned char* data, int ndata) {
	if (data == NULL)
	{
		Debug.PrintError("The argument `data` is NULL.");
		return 0;
	}
	if (ndata <= 0)
	{
		Debug.PrintError("The argument `ndata` is less than or equal to 0.");
		return 0;
	}
	int ret = nvgCreateImageMem(nvgContext, imageFlags, data, ndata);
	if (ret == 0)
	{
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
	if (data == NULL)
	{
		Debug.PrintError("The argument `data` is NULL.");
		return 0;
	}
	if (w <= 0)
	{
		Debug.PrintError("The argument `w` is less than or equal to 0.");
		return 0;
	}
	if (h <= 0)
	{
		Debug.PrintError("The argument `h` is less than or equal to 0.");
		return 0;
	}
	int ret = nvgCreateImageRGBA(nvgContext, w, h, imageFlags, data);
	if (ret == 0)
	{
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
	return  JugglingCast(NVGpaint, Paint, nvgLinearGradient(nvgContext, sx, sy, ex, ey, JugglingCast(Color,NVGcolor,icol), JugglingCast(Color,NVGcolor,ocol)));
}

static Paint Video_BoxGradient(float x, float y, float w, float h, float r, float f, Color icol, Color ocol) {
	return JugglingCast(NVGpaint, Paint, nvgBoxGradient(nvgContext, x, y, w, h, r, f, JugglingCast(Color,NVGcolor,icol), JugglingCast(Color,NVGcolor,ocol)));
}

static Paint Video_RadialGradient(float cx, float cy, float inr, float outr, Color icol, Color ocol) {
	return JugglingCast(NVGpaint, Paint, nvgRadialGradient(nvgContext, cx, cy, inr, outr, JugglingCast(Color,NVGcolor,icol), JugglingCast(Color,NVGcolor,ocol)));
}

static Paint Video_ImagePattern(float ox, float oy, float ex, float ey, float angle, int image, float alpha) {
	return JugglingCast(NVGpaint, Paint, nvgImagePattern(nvgContext, ox, oy, ex, ey, angle, image, alpha));
}

#if defined(UNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN)
static Paint Video_ImagePatternWithFillColor(float cx, float cy, float w, float h, float angle, int image) {
	return JugglingCast(NVGpaint, Paint, nvgImagePatternWithFillColor(nvgContext, cx, cy, w, h, angle, image));
}

#endif
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
	if (filename == NULL)
	{
		Debug.PrintError("The argument `filename` is NULL.");
		return -1;
	}
	if (name == NULL)
	{
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (IsContainsNotAscii(name, NULL) || IsContainsNotAscii(filename, NULL))
	{
		char *sjisname = utf82sjis(name, NULL);
		char *sjisfilename = utf82sjis(filename, NULL);
		ret = nvgCreateFont(nvgContext, sjisname, sjisfilename);
		free(sjisname);
		free(sjisfilename);
	}
	else
	{
		ret = nvgCreateFont(nvgContext, name, filename);
	}
#else
	ret = nvgCreateFont(nvgContext, name, filename);
#endif
	if (ret == -1)
	{
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
	if (filename == NULL)
	{
		Debug.PrintError("The argument `filename` is NULL.");
		return -1;
	}
	if (name == NULL)
	{
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (IsContainsNotAscii(name, NULL) || IsContainsNotAscii(filename, NULL))
	{
		utf8_t *utf8name = sjis2utf8(name, NULL);
		utf8_t *utf8filename = sjis2utf8(filename, NULL);
		ret = nvgCreateFont(nvgContext, utf8name, utf8filename);
		free(utf8name);
		free(utf8filename);
	}
	else
	{
		ret = nvgCreateFont(nvgContext, name, filename);
	}
#else
	ret = nvgCreateFont(nvgContext, name, filename);
#endif
	if (ret == -1)
	{
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
	if (name == NULL)
	{
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	if (data == NULL)
	{
		Debug.PrintError("The argument `data` is NULL.");
		return -1;
	}
	if (ndata <= 0)
	{
		Debug.PrintError("The argument `ndata` is less than or equal to 0.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (IsContainsNotAscii(name, NULL))
	{
		char *sjisname = utf82sjis(name, NULL);
		ret = nvgCreateFontMem(nvgContext, sjisname, data, ndata, freeData);
		free(sjisname);
	}
	else
	{
		ret = nvgCreateFontMem(nvgContext, name, data, ndata, freeData);
	}
#else
	ret = nvgCreateFontMem(nvgContext, name, data, ndata, freeData);
#endif
	if (ret == -1)
	{
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
	if (name == NULL)
	{
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	if (data == NULL)
	{
		Debug.PrintError("The argument `data` is NULL.");
		return -1;
	}
	if (ndata <= 0)
	{
		Debug.PrintError("The argument `ndata` is less than or equal to 0.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (IsContainsNotAscii(name, NULL))
	{
		utf8_t *utf8name = sjis2utf8(name, NULL);
		ret = nvgCreateFontMem(nvgContext, utf8name, data, ndata, freeData);
		free(utf8name);
	}
	else
	{
		ret = nvgCreateFontMem(nvgContext, name, data, ndata, freeData);
	}
#else
	ret = nvgCreateFontMem(nvgContext, name, data, ndata, freeData);
#endif
	if (ret == -1)
	{
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
	if (name == NULL)
	{
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	int ret;
#if defined(_WIN32)
	if (IsContainsNotAscii(name, NULL))
	{
		char *sjisname = utf82sjis(name, NULL);
		ret = nvgFindFont(nvgContext, sjisname);
		free(sjisname);
	}
	else
	{
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
	if (name == NULL)
	{
		Debug.PrintError("The argument `name` is NULL.");
		return -1;
	}
	return nvgFindFont(nvgContext, name);
}

static int Video_FindFont(const char* name) {
	Debug.PushBanner("**Error in Video.FindFont");
	int ret;
#if defined(_WIN32)
	if (IsContainsNotAscii(name, NULL))
	{
		utf8_t *utf8name = sjis2utf8(name, NULL);
		ret = Video_FindFont_(utf8name);
		free(utf8name);
	}
	else
	{
		ret = Video_FindFont_(name);
	}
#else
	ret = Video_FindFont_(name);
#endif
	Debug.PopBanner();
	return ret;
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
	if (IsContainsNotAscii(font, NULL))
	{
		utf8_t *utf8font = sjis2utf8(font, NULL);
		nvgFontFace(nvgContext, utf8font);
		free(utf8font);
	}
	else
	{
		nvgFontFace(nvgContext, font);
	}
#else
	nvgFontFace(nvgContext, font);
#endif
}

static float Video_TextUTF8(float x, float y, const char* utf8str, const char* end) {
	Debug.PushBanner("**Error in Video.TextUTF8");
	float ret = 0.0;
	if (utf8str == NULL)
	{
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
	if (string == NULL)
	{
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (IsContainsNotAscii(string,end))
	{
		utf8_t *utf8str = sjis2utf8(string, end);
		ret = Video_TextUTF8(x, y, utf8str, NULL);
		free(utf8str);
	} else
	{
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
	if (utf8str == NULL)
	{
		Debug.PrintError("The argument `utf8str` is NULL.");
	}
	else {
		nvgTextBox(nvgContext, x, y, breakRowWidth, utf8str, end);
	}
	Debug.PopBanner();
}

static void Video_TextBox(float x, float y, float breakRowWidth, const char* string, const char* end) {
	Debug.PushBanner("**Error in Video.TextBox");
	if (string == NULL)
	{
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (IsContainsNotAscii(string, end))
	{
		utf8_t *utf8str = sjis2utf8(string, end);
		Video_TextBoxUTF8(x, y, breakRowWidth, utf8str, NULL);
		free(utf8str);
	}
	else
	{
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
	if (utf8str == NULL)
	{
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	if (bounds == NULL)
	{
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
	if (string == NULL)
	{
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
	if (bounds == NULL)
	{
		Debug.PrintError("The argument `bounds` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (IsContainsNotAscii(string, end))
	{
		utf8_t *utf8str = sjis2utf8(string, end);
		ret = Video_TextBoundsUTF8(x, y, utf8str, NULL, bounds);
		free(utf8str);
	}
	else
	{
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
	if (utf8str == NULL)
	{
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	if (bounds == NULL)
	{
		Debug.PrintError("The argument `bounds` is NULL.");
		goto EXIT;
	}
	nvgTextBoxBounds(nvgContext, x, y, breakRowWidth, utf8str, end, bounds);
EXIT:
	Debug.PopBanner();
}

static void Video_TextBoxBounds(float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds) {
	Debug.PushBanner("**Error in Video.TextBoxBounds");
	if (string == NULL)
	{
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
	if (bounds == NULL)
	{
		Debug.PrintError("The argument `bounds` is NULL.");
		goto EXIT;
	}
#if defined(_WIN32)
	if (IsContainsNotAscii(string, end))
	{
		utf8_t *utf8str = sjis2utf8(string, end);
		Video_TextBoxBoundsUTF8(x, y, breakRowWidth, utf8str, NULL, bounds);
		free(utf8str);
	}
	else
	{
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
	if (utf8str == NULL)
	{
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	if (positions == NULL)
	{
		Debug.PrintError("The argument `positions` is NULL.");
		goto EXIT;
	}
	if (maxPositions <= 0)
	{
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
	if (string == NULL)
	{
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
	if (positions == NULL)
	{
		Debug.PrintError("The argument `positions` is NULL.");
		goto EXIT;
	}
	if (maxPositions <= 0)
	{
		Debug.PrintError("The argument `maxPositions` must greater than 0. (You gave %d to the argument `maxPositions`)", maxPositions);
		goto EXIT;
	}
#if defined(_WIN32)
	if (IsContainsNotAscii(string, end))
	{
		utf8_t *utf8str = sjis2utf8(string, end);
		ret = nvgTextGlyphPositions(nvgContext, x, y, utf8str, NULL, (NVGglyphPosition*)positions, maxPositions);
		for(int i=0; i<ret; i++)
		{
			positions[i].str = String.Sjis.Inc(string, String.Utf8.Count(utf8str, positions[i].str));
		}
		free(utf8str);
	}
	else
	{
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
	if (utf8str == NULL)
	{
		Debug.PrintError("The argument `utf8str` is NULL.");
		goto EXIT;
	}
	if (rows == NULL)
	{
		Debug.PrintError("The argument `rows` is NULL.");
		goto EXIT;
	}
	if (maxRows <= 0)
	{
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
	if (string == NULL)
	{
		Debug.PrintError("The argument `string` is NULL.");
		goto EXIT;
	}
	if (rows == NULL)
	{
		Debug.PrintError("The argument `rows` is NULL.");
		goto EXIT;
	}
	if (maxRows <= 0)
	{
		Debug.PrintError("The argument `maxRows` must greater than 0. (You gave %d to the argument `maxRows`)", maxRows);
		goto EXIT;
	}
#if defined(_WIN32)
	if (IsContainsNotAscii(string, end))
	{
		utf8_t *utf8str = sjis2utf8(string, end);
		ret = Video_TextBreakLinesUTF8(utf8str, NULL, breakRowWidth, rows, maxRows);
		for (int i = 0; i<ret; i++)
		{
			rows[i].start = String.Sjis.Inc(string, String.Utf8.Count(utf8str, rows[i].start));
			rows[i].end   = String.Sjis.Inc(string, String.Utf8.Count(utf8str, rows[i].end));
			rows[i].next  = String.Sjis.Inc(string, String.Utf8.Count(utf8str, rows[i].next));
		}

		free(utf8str);
	}
	else
	{
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
	if (image <= 0)
	{
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
	if (image <= 0)
	{
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

static void Video_FormatText(float x, float y, const char *format, ...)
{
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

const struct __tagVideoAPI Video = {
	Video_Initialize,
	Video_Finalize,
	Video_SetWindowTitleUTF8,
	Video_SetWindowTitle,
	Video_SetSize,
	Video_Drawing,
	Video_GetTime,
	Video_Sleep,
	Video_SetClearColor,
	Video_BeginFrame,
	Video_CancelFrame,
	Video_EndFrame,
	Video_RGB,
	Video_RGBA,
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
#if defined(UNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD)
	Video_TranslateWorld,
	Video_RotateWorld,
	Video_SkewXWorld,
	Video_SkewYWorld,
	Video_ScaleWorld,
#endif
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
#if defined(UNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN)
	Video_ImagePatternWithFillColor,
#endif
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
};

/// input ////

static bool Mouse_IsLeftButtonDown(void)
{
	return glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? true : false;
}

static bool Mouse_IsRightButtonDown(void)
{
	return glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ? true : false;
}

static bool Mouse_IsMiddleButtonDown(void)
{
	return glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ? true : false;
}

static void Mouse_GetCursorPos(double *x, double *y)
{
	glfwGetCursorPos(glfwWindow, x, y);
}

const struct __tagMouseAPI Mouse = {
	Mouse_IsLeftButtonDown,
	Mouse_IsRightButtonDown,
	Mouse_IsMiddleButtonDown,
	Mouse_GetCursorPos,
};

static bool Keyboard_IsKeyDown(enum Keycode key)
{
	return glfwGetKey(glfwWindow, key) == GLFW_RELEASE ? false : true;
}

const struct __tagKeyboardAPI Keyboard = {
	Keyboard_IsKeyDown,
};


//
//

static void FrameRate_SetFrameRate(unsigned int fps)
{
	FpsTimer.SetFPS(fps);
}

static unsigned int FrameRate_GetFrameRate(void)
{
	return FpsTimer.GetFPS();
}

static float FrameRate_GetRealFrameRate()
{
	return FpsTimer.GetRealFPS();
}

static unsigned int FrameRate_GetCPUPower(void)
{
	return FpsTimer.GetCPUPower();
}

const struct __tagFrameRateAPI FrameRate = {
	FrameRate_SetFrameRate,
	FrameRate_GetFrameRate,
	FrameRate_GetRealFrameRate,
	FrameRate_GetCPUPower,
};
