#ifndef _NANOGL_VIDEO_H
#define _NANOGL_VIDEO_H

#include "../Config.h"
#include <stdbool.h>

// begin of copy from nanovg.h

union Color {
	float rgba[4];
	struct {
		float r, g, b, a;
	} s;
};

/**
 * @brief 色をRGBA各0.0..1.0で表現
 */
typedef union Color Color;

struct Paint {
	float xform[6];
	float extent[2];
	float radius;
	float feather;
	Color innerColor;
	Color outerColor;
	int image;
};

/**
 * @brief 描画パターン
 */
typedef struct Paint Paint;


/**
 * @biref 回転方向
 */
enum Winding {
	WINDING_CCW = 1,			//! 反時計回り
	WINDING_CW = 2,				//! 時計回り
};

/**
 * @biref 描画する形状の特性
 */
enum Solidity {
	SOLIDITY_SOLID = 1,			//! 描画する形状は面として定義
	SOLIDITY_HOLE = 2,			//! 描画する形状は穴として定義
};

/**
 * 余談：
 * パスを用いてドーナツのような穴の開いた形状を塗りつぶして描画する方法
 *
 * Video.BeginPath();
 *
 * Video.Circle(120, 120, 250);        // ドーナツの本体となる形状を設定
 * Video.PathWinding(SOLIDITY_SOLID);  // 直前に設定した描画形状を面に設定
 * 
 * Video.Circle(120, 120, 50);        // ドーナツの穴となる形状を設定
 * Video.PathWinding(SOLIDITY_HOLE);   // 直前に設定した描画形状を穴に設定
 *
 * Video.FillColor(Video.RGB(255,255,255));
 * Video.Fill();
 *
 * Video.ClosePath();
 */

/**
* @biref 線分の終端の処理
*/
enum LineCap {
	CAP_BUTT,	//! 線の終点でぶつ切り
	CAP_ROUND,	//! 線の幅の半分の長さを半径とする半円
	CAP_SQUARE,	//! 線の幅の半分の長さだけはみ出る
	CAP_BEVEL,	//! マイター距離を無効にして交点まで押し出す
	CAP_MITER,	//! マイター距離を有効にする
};

/**
 * (主にテキストの)整列指定フラグ.
 */
enum Align {
	ALIGN_LEFT   = 1 << 0,	//! [水平方向] 左寄せ（デフォルト値)
	ALIGN_CENTER = 1 << 1,	//! [水平方向] 中央寄せ
	ALIGN_RIGHT  = 1 << 2,	//! [水平方向] 右寄せ

	ALIGN_TOP      = 1 << 3,	//! [垂直方向] 上寄せ
	ALIGN_MIDDLE   = 1 << 4,	//! [垂直方向] 中寄せ
	ALIGN_BOTTOM   = 1 << 5,	//! [垂直方向] 下寄せ
	ALIGN_BASELINE = 1 << 6,	//! [垂直方向] ベースライン寄せ（デフォルト値)
};

/**
* @brief 色合成要素
*/
enum BlendFactor {
	ZERO = 1 << 0,
	ONE = 1 << 1,
	SRC_COLOR = 1 << 2,
	ONE_MINUS_SRC_COLOR = 1 << 3,
	DST_COLOR = 1 << 4,
	ONE_MINUS_DST_COLOR = 1 << 5,
	SRC_ALPHA = 1 << 6,
	ONE_MINUS_SRC_ALPHA = 1 << 7,
	DST_ALPHA = 1 << 8,
	ONE_MINUS_DST_ALPHA = 1 << 9,
	SRC_ALPHA_SATURATE = 1 << 10,
};

/**
* @brief 色合成演算子
*/
enum CompositeOperation {
	SOURCE_OVER,
	SOURCE_IN,
	SOURCE_OUT,
	ATOP,
	DESTINATION_OVER,
	DESTINATION_IN,
	DESTINATION_OUT,
	DESTINATION_ATOP,
	LIGHTER,
	COPY,
	XOR,
};

/**
* @brief 画像合成用の情報
*/
struct CompositeOperationState {
	int srcRGB;
	int dstRGB;
	int srcAlpha;
	int dstAlpha;
};
typedef struct CompositeOperationState CompositeOperationState;

/**
 * @brief フォントのグリフ位置情報
 */
struct GlyphPosition {
	const char* str;	//! グリフに対応する文字位置
	float x;			//! 論理グリフのX座標
	float minx, maxx;	//! グリフ形状のX座標の最小値と最大値
};
typedef struct GlyphPosition GlyphPosition;

struct TextRow {
	const char* start;	// Pointer to the input text where the row starts.
	const char* end;	// Pointer to the input text where the row ends (one past the last character).
	const char* next;	// Pointer to the beginning of the next row.
	float width;		// Logical width of the row.
	float minx, maxx;	// Actual bounds of the row. Logical with and bounds can differ because of kerning and some parts over extending.
};
typedef struct TextRow TextRow;

enum ImageFlags {
	IMAGE_GENERATE_MIPMAPS = 1 << 0,	// 画像を読み込む際にミップマップデータを生成する。縮小描画を多用する画像だとやや速度が上がるかもしれない。
	IMAGE_REPEATX = 1 << 1,				// 画像の横方向範囲を逸脱して描画する際、画像が横に無限に連続しているように描画する。（ImagePattern()で画像幅を超えるサイズのパターンを描画する際に、画像を繰り返し横に並べて描画する）
	IMAGE_REPEATY = 1 << 2,				// 画像の縦方向範囲を逸脱して描画する際、画像が縦に無限に連続しているように描画する。（ImagePattern()で画像高さを超えるサイズのパターンを描画する際に、画像を繰り返し高に並べて描画する）
	IMAGE_FLIPY = 1 << 3,				// 画像を上下反転して描画する。（一部の画像形式は古いツールで出力すると上下反転で画像が格納されている場合があるため）
	IMAGE_PREMULTIPLIED = 1 << 4,		// αチャンネルを持つ画像の合成処理にプレマルチプライド方式を用いる。（Adobe AfterEffectからプレマルチプライド方式で出力した画像やTIFF形式などで必要）
	IMAGE_RESIZE_NEAREST = 1 << 5,		// 画像の拡大縮小にニアレストネイバー(nearest neighbor)法を使う。ドット絵などに使う。
};

// end of copy 

/**
 * @brief 描画命令
 */
extern const struct __tagVideoAPI {
	/**
	* @brief VideoAPIを初期化する
	* @retval true  初期化に成功
	* @retval false 初期化に失敗
	* @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
	*/
	bool(*Initialize)(void);

	/**
	 * @brief VideoAPIの終了処理を行う
	 * @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
	 */
	void(*Finalize)(void);

	/**
	 * @brief Video.SetWindowTitle()関数のUTF8専用版
	 */
	void(*SetWindowTitleUTF8)(const char *title);

	/**
	 * @brief ウィンドウタイトルを設定する
	 * @param title ウィンドウタイトル
	 */
	void(*SetWindowTitle)(const char *title);

	/**
	* @brief 画面サイズを設定する
	* @param width  幅
	* @param height 高さ
	*
	* 単位はピクセル
	*/
	void(*SetSize)(int width, int height);

	/**
	* @brief 画面サイズを取得する
	* @param width  幅
	* @param height 高さ
	*
	* 単位はピクセル
	*/
	void(*GetSize)(int *width, int *height);

	/**
	* @brief ゲームの状態更新・描画処理などが可能なるまで待つ
	* @return この関数は便宜上常にtrueを返す
	*
	* while (Video.Drawing()) { ゲームの状態更新・描画処理 } のように用いる
	* 画面が閉じられた場合にはプログラムは自動的に終了する
	*/
	bool(*Drawing)(void);

	/**
	* @brief Video.Loop内で明示的に画面上への描画を開始する際に呼び出す。
	*/
	void(*BeginDraw)(void);

	/**
	* @brief Video.Loop内で明示的に画面上への描画を終了する際に呼び出す。
	*/
	void(*EndDraw)(void);


	void (*BeginDrawEx)(void);
	void (*ResetDrawEx)(void);
	void (*EndDrawEx)(void);
	void (*UpdateDrawEx)(void);

	/**
	* @brief ゲームの状態更新が可能なるまで待つ
	* @return この関数は便宜上常にtrueを返す
	*
	* Drawing同様にwhile (Video.Loop()) { ゲームの状態更新・描画処理 } のように用いることができるが
	* 画面への描画処理は明示的に BeginDraw, EndDrawの間で行う必要がある。
	* フレームバッファオブジェクトを用いて実行中にテクスチャへのレンダリングや複数の合成演算を切り替えて描画を行う際に用いる。
	* 画面が閉じられた場合にはプログラムは自動的に終了する
	*/
	bool(*Loop)(void);

	/**
	 * @brief プログラム起動時からの経過秒数を取得
	 * @return 経過秒数を１秒=1.0として取得。 1ミリ秒=1.0/1000
	 */
	double(*GetTime)(void);

	/**
	 * @brief src秒スリープする
	 * @src  スリープ秒数(小数部分でミリ秒単位のスリープが可能)
	 * @return 実際のスリープ経過秒数
	 */
	double(*Sleep)(double sec);

	/**
	 * @brief Video.Drawing()が画面消去に用いる色を color に設定する
	 * @param color  設定する色
	 */
	void(*SetClearColor)(Color color);

	/**
	 * @brief Vide.Drawing()を使わずに全ての描画を管理する場合に用いる描画処理の開始
	 * @param windowWidth  画面幅
	 * @param windowHeight 画面高
	 * @param devicePixelRatio フレームバッファサイズ/画面サイズ で求めたサイズ比率
	 * @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
	 */
	void(*BeginFrame)(int windowWidth, int windowHeight, float devicePixelRatio);

	/**
	 * @brief Video.Drawing()を使わずに全ての描画を管理する場合に用いる描画処理のキャンセル
	 * @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
	 */
	void(*CancelFrame)(void);

	/**
	 * @brief Video.Drawing()を使わずに全ての描画を管理する場合に用いる描画処理の終了
	 * @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
	 */
	void(*EndFrame)(void);
	
	/**
	 * @brief BeginFrameからEndFrameの間で画像描画の際に使う合成処理を設定する。
	 * @param op 利用する合成処理 (意味はHTML5 CanvasのglobalCompositeOperationと同じ)
	 * @note 合成処理をBeginFrameからEndFrameまでの間で変更すると、予期しない描画結果になる。
	 * 理由は描画命令は一旦命令バッファにキャッシュされ、EndFrameが呼ばれるか蓄積された命令が閾値を超えた際に実行される。
	 * 合成処理はこの実行に適用されるので、途中で合成処理を変更すると変更前にキャッシュされている命令にまで影響が及ぶため。
	 */
	void (*GlobalCompositeOperation)(enum CompositeOperation op);

	/**
	* @brief BeginFrameからEndFrameの間で画像描画の際に使う転送元ピクセルと転送先ピクセルの合成で使う演算を設定する。
	* @param sfactor 転送元ピクセルの演算方法(意味はOpenGLのglBlendFuncと同じ)
	* @param dfactor 転送先ピクセルの演算方法(意味はOpenGLのglBlendFuncと同じ)
	* @note 合成処理をBeginFrameからEndFrameまでの間で変更すると、予期しない描画結果になる。
	* 理由は描画命令は一旦命令バッファにキャッシュされ、EndFrameが呼ばれるか蓄積された命令が閾値を超えた際に実行される。
	* 合成処理はこの実行に適用されるので、途中で合成処理を変更すると変更前にキャッシュされている命令にまで影響が及ぶため。
	*/
	void(*GlobalCompositeBlendFunc)(enum BlendFactor sfactor, enum BlendFactor dfactor);

	/**
	* @brief BeginFrameからEndFrameの間で画像描画の際に使う転送元ピクセルと転送先ピクセルの合成で使う演算を色(RGB)とα値(A)を別々に設定する。
	* @param srcRGB 転送元ピクセルのRGB部分の演算方法(意味はOpenGLのglBlendFuncと同じ)
	* @param dstRGB 転送先ピクセルのRGB部分の演算方法(意味はOpenGLのglBlendFuncと同じ)
	* @param srcAlpha 転送元ピクセルのα部分の演算方法(意味はOpenGLのglBlendFuncと同じ)
	* @param dstAlpha 転送先ピクセルのα部分の演算方法(意味はOpenGLのglBlendFuncと同じ)
	* @note 合成処理をBeginFrameからEndFrameまでの間で変更すると、予期しない描画結果になる。
	* 理由は描画命令は一旦命令バッファにキャッシュされ、EndFrameが呼ばれるか蓄積された命令が閾値を超えた際に実行される。
	* 合成処理はこの実行に適用されるので、途中で合成処理を変更すると変更前にキャッシュされている命令にまで影響が及ぶため。
	*/
	void(*GlobalCompositeBlendFuncSeparate)(enum BlendFactor srcRGB, enum BlendFactor dstRGB, enum BlendFactor srcAlpha, enum BlendFactor dstAlpha);

	/**
	 * @brief 色をRGB各256階調で指定して作成する。A(半透明度)は255固定となる
	 * @param r 赤の階調
	 * @param g 緑の階調
	 * @param b 青の階調
	 * @return 作成した色データ
	 */
	Color(*RGB)(unsigned char r, unsigned char g, unsigned char b);
	
	/**
	 * @brief 色をRGBA各256階調で指定して作成する。
	 * @param r 赤の階調
	 * @param g 緑の階調
	 * @param b 青の階調
	 * @param a 不透明度(255は完全不透明、0は完全透明)
	 * @return 作成した色データ
	 */
	Color(*RGBA)(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	/**
	* @brief 色c0と色c1を比率 u:1-u で合成した色を作成する。
	* @param c0 色1
	* @param c1 色2
	* @return 作成した色データ
	*/
	Color(*LerpRGBA)(Color c0, Color c1, float u);

	/**
	* @brief 色c0のα値をaに変えた色を作成する。
	* @param c0 色
	* @param a  α値(0..255)
	* @return 作成した色データ
	*/
	Color(*TransRGBA)(Color c0, unsigned char a);

	/**
	* @brief 色c0のα値をaに変えた色を作成する。
	* @param c0 色
	* @param a  α値(0.0..1.0)
	* @return 作成した色データ
	*/
	Color(*TransRGBAf)(Color c0, float a);

	/**
	* @brief 色をHSL各(0.0 .. 1.0)で指定して作成する。
	* @param h 色相(0.0 .. 1.0)
	* @param s 彩度(0.0 .. 1.0)
	* @param l 輝度(0.0 .. 1.0)
	* @return 作成した色データ
	*/
	Color(*HSL)(float h, float s, float l);

	/**
	* @brief 色をHSL各(0.0 .. 1.0)で指定して作成する。
	* @param h 色相(0.0 .. 1.0)
	* @param s 彩度(0.0 .. 1.0)
	* @param l 輝度(0.0 .. 1.0)
	* @param a 不透明度(255は完全不透明、0は完全透明)
	* @return 作成した色データ
	*/
	Color(*HSLA)(float h, float s, float l, unsigned char a);

	/**
	 * @brief 現在の描画設定をコピーしてスタックに積む
	 * @note  Video.Drawing()を呼び出すと描画設定もスタック状態もリセットされる
	 */
	void(*Save)(void);

	/**
	 * @brief 現在の描画設定をスタックから取り出して現在の設定にする
	 * @note  Video.Drawing()を呼び出すと描画設定もスタック状態もリセットされる
	 */
	void(*Restore)(void);

	/**
	 * @brief 現在の描画設定を初期値にリセットする
	 */
	void(*Reset)(void);

	/**
	 * @brief Video.Stroke()で描画する線の色を設定する
	 * @param color 線の色
	 */
	void(*StrokeColor)(Color color);

	/**
	 * @brief Video.Stroke()で描画する線のパターンを設定する
	 * @param paint 線のパターン
	 * @note  パターンはVideo.LinearGradient()/BoxGradient()/RadialGradient()/ImagePattern()で作成する
	 */
	void(*StrokePaint)(Paint paint);

	/**
	 * @brief Video.Fill()で描画する塗りつぶしの色を設定する
	 * @param color 塗りつぶしの色
	 */
	void(*FillColor)(Color color);

	/**
	 * @brief Video.Stroke()で描画する塗りつぶしのパターンを設定する
	 * @param paint 塗りつぶしのパターン
	 * @note  パターンはVideo.LinearGradient()/BoxGradient()/RadialGradient()/ImagePattern()で作成する
	 */
	void(*FillPaint)(Paint paint);
	
	/**
	 * @brief 折れ線の接合箇所のマイター限界（折れ線箇所を面取りしないで尖らせる）距離を指定する
	 * @param limit マイター距離
	 * @note  マイター距離についての解説は http://www.htmq.com/canvas/miterLimit.shtml を参照
	 */
	void(*MiterLimit)(float limit);

	/**
	 * @brief Video.Stroke()で描画する線の幅を設定する
	 * @param paint 線の幅
	 */
	void(*StrokeWidth)(float size);

	/**
	 * @brief Video.Stroke()で描画する線の終端の形状を指定する
	 * @param cap 線の終端の形状
	 * @note  終端の形状については enum LineCap の解説を参照
	 */
	void(*LineCap)(enum LineCap cap);

	/**
	* @brief 折れ線の接続形状を指定する
	* @param join 線の終端の形状（bevel, round, miterのいずれか）
	* @note  終端の形状については enum LineCap の解説を参照
	*/
	void(*LineJoin)(enum LineCap join);

	/**
	 * @brief 描画全体の不透明度を0.0～1.0で指定
	 * @param alpha 不透明度(1.0が完全不透明, 0.0が完全透明)
	 */
	void(*GlobalAlpha)(float alpha);

	/**
	 * @brief 現在の変形設定をリセットする（姿勢変換行列に単位行列を設定）
	 * | 1, 0, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 */
	void(*ResetTransform)(void);

	/**
	 * @brief 現在の変形設定に任意の2x3姿勢変換行列を設定する
	 * @param a,b,c,d,e,f 行列要素
	 * @note  各引数と行列要素の対応は以下の通り
	 * | a, c, e |
	 * | b, d, f |
	 * | 0, 0, 1 |
	 */
	void(*Transform)(float a, float b, float c, float d, float e, float f);

	/**
	 * @brief 現在の変形設定に移動(x, y)を右から適用
	 * @param x 横方向移動量
	 * @param y 縦方向移動量
	 * @note  各引数と行列要素の対応は以下の通り
	 * | 1, 0, x |
	 * | 0, 1, y |
	 * | 0, 0, 1 |
	 */
	void(*Translate)(float x, float y);

	/**
	 * @brief 現在の変形設定に 時計回りで angle ラジアンの回転を右から適用
	 * @param angle 時計回りでの回転
	 * @note  各引数と行列要素の対応は以下の通り
	 * | c,-s, 0 | 
	 * | s, c, 0 |
	 * | 0, 0, 1 |
	 * c = cos(angle), s = sin(angle)
	 */
	void(*Rotate)(float angle);

	/**
	 * @brief 現在の変形設定に angle ラジアンのX軸に沿った傾斜変形を右から適用
	 * @param angle 傾斜角度
	 * @note  各引数と行列要素の対応は以下の通り
	 * | 1, t, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 */
	void(*SkewX)(float angle);

	/**
	 * @brief 現在の変形設定に angle ラジアンのY軸に沿った傾斜変形を右から適用
	 * @param angle 傾斜角度
	 * @note  各引数と行列要素の対応は以下の通り
	 * | 1, 0, 0 |
	 * | t, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 */
	void(*SkewY)(float angle);

	/**
	 * @brief 現在の変形設定に 横 x 倍, 縦 y 倍のスケールを右から適用
	 * @param x 横方向スケール
	 * @param y 縦方向スケール
	 * @note  各引数と行列要素の対応は以下の通り
	 * | x, 0, 0 |
	 * | 0, y, 0 |
	 * | 0, 0, 1 |
	 */
	void(*Scale)(float x, float y);

	/**
	 * @brief 現在の変形設定に移動(x, y)を左から適用
	 * @param x 横方向移動量
	 * @param y 縦方向移動量
	 * @note  各引数と行列要素の対応は以下の通り
	 * | 1, 0, x |
	 * | 0, 1, y |
	 * | 0, 0, 1 |
	 */
	void(*TranslateWorld)(float x, float y);

	/**
	 * @brief 現在の変形設定に 時計回りで angle ラジアンの回転を左から適用
	 * @param angle 時計回りでの回転
	 * @note  各引数と行列要素の対応は以下の通り
	 * | c,-s, 0 | 
	 * | s, c, 0 |
	 * | 0, 0, 1 |
	 * c = cos(angle), s = sin(angle)
	 */
	void(*RotateWorld)(float angle);

	/**
	 * @brief 現在の変形設定に angle ラジアンのX軸に沿った傾斜変形を左から適用
	 * @param angle 傾斜角度
	 * @note  各引数と行列要素の対応は以下の通り
	 * | 1, t, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 */
	void(*SkewXWorld)(float angle);

	/**
	 * @brief 現在の変形設定に angle ラジアンのY軸に沿った傾斜変形を左から適用
	 * @param angle 傾斜角度
	 * @note  各引数と行列要素の対応は以下の通り
	 * | 1, 0, 0 |
	 * | t, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 */
	void(*SkewYWorld)(float angle);

	/**
	 * @brief 現在の変形設定に 横 x 倍, 縦 y 倍のスケールを左から適用
	 * @param x 横方向スケール
	 * @param y 縦方向スケール
	 * @note  各引数と行列要素の対応は以下の通り
	 * | x, 0, 0 |
	 * | 0, y, 0 |
	 * | 0, 0, 1 |
	 */
	void(*ScaleWorld)(float x, float y);

	/**
	 * @brief 現在の変形設定を配列xformに取り出す
	 * @param xform 6要素以上の配列。 
	 * @note  配列要素と行列要素の対応は以下の通り
	 * | a, c, e |
	 * | b, d, f |
	 * | 0, 0, 1 |
	 * xform = [a,b,c,d,e,f]
	 */
	void(*CurrentTransform)(float* xform);

	/**
	 * @brief 配列 dst に単位行列を設定
	 * @param dst 6要素以上の配列。 
	 * @note  配列要素と行列要素の対応は以下の通り
	 * | 1, 0, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 * dst = [1,0,0,1,0,0]
	 */
	void(*TransformIdentity)(float* dst);

	/**
	 * @brief 配列 dst に移動行列(x, y)を設定
	 * @param dst 6要素以上の配列。 
	 * @param x 横方向移動量
	 * @param y 縦方向移動量
	 * @note  配列要素と行列要素の対応は以下の通り
	 * | 1, 0, x |
	 * | 0, 1, y |
	 * | 0, 0, 1 |
	 * dst = [1,0,0,1,x,y]
	 */
	void(*TransformTranslate)(float* dst, float sx, float sy);

	/**
	 * @brief 配列 dst に時計回りで angle ラジアンの回転行列を設定
	 * @param dst 6要素以上の配列。 
	 * @param angle 時計回りでの回転
	 * @note  配列要素と行列要素の対応は以下の通り
	 * | c,-s, 0 | 
	 * | s, c, 0 |
	 * | 0, 0, 1 |
	 * c = cos(angle), s = sin(angle)
	 * dst = [c,s,-s,c,0,0]
	 */
	void(*TransformRotate)(float* dst, float a);

	/**
	 * @brief 配列 dst にangle ラジアンのX軸に沿った傾斜変形行列を設定
	 * @param dst 6要素以上の配列。 
	 * @param angle 傾斜角度
	 * @note  配列要素と行列要素の対応は以下の通り
	 * | 1, t, 0 |
	 * | 0, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 * dst = [1,0,t,1,0,0]
	 */
	void(*TransformSkewX)(float* dst, float a);

	/**
	 * @brief 配列 dst にangle ラジアンのY軸に沿った傾斜変形行列を設定
	 * @param dst 6要素以上の配列。 
	 * @param angle 傾斜角度
	 * @note  配列要素と行列要素の対応は以下の通り
	 * | 1, 0, 0 |
	 * | t, 1, 0 |
	 * | 0, 0, 1 |
	 * t = tan(angle)
	 * dst = [1,t,0,1,0,0]
	 */
	void(*TransformSkewY)(float* dst, float a);

	/**
	 * @brief 配列 dst に横 x 倍, 縦 y 倍のスケール行列を設定
	 * @param dst 6要素以上の配列。 
	 * @param x 横方向スケール
	 * @param y 縦方向スケール
	 * @note  各引数と行列要素の対応は以下の通り
	 * | x, 0, 0 |
	 * | 0, y, 0 |
	 * | 0, 0, 1 |
	 * dst = [x,0,0,y,0,0]
	 */
	void(*TransformScale)(float* dst, float sx, float sy);

	/**
	 * @brief 配列 dst と配列 src を行列と見なしてdst * src を行う。結果はdstに格納される。
	 * @param dst 6要素以上の配列。
	 * @param src 6要素以上の配列。
	 * @note 配列要素と行列要素の対応は Transform() 関数のものと同じ
	 */
	void(*TransformMultiply)(float* dst, const float* src);

	/**
	 * @brief 配列 dst と配列 src を行列と見なしてsrc * dst を行う。結果はdstに格納される。
	 * @param dst 6要素以上の配列。
	 * @param src 6要素以上の配列。
	 * @note 配列要素と行列要素の対応は Transform() 関数のものと同じ
	 */
	void(*TransformPremultiply)(float* dst, const float* src);

	/**
	* @brief 配列 dst を行列と見なして逆行列を求める。結果はdstに格納される。
	* @param dst 6要素以上の配列。
	* @param src 6要素以上の配列。
	* @retval 逆行列が存在する場合は～しない場合は～。
	*/
	int(*TransformInverse)(float* dst, const float* src);

	/**
	 * @brief 配列 xform を行列と見なして点(srcx, srcy)に変形行列 xform を適用した結果得られる点の座標を(dstx, dsty)に格納する。
	 * @param dstx 変形後の点のX座標を格納する変数のポインタ
	 * @param dsty 変形後の点のY座標を格納する変数のポインタ
	 * @param xform 変形に用いる行列としてみなす6要素以上の配列
	 * @param srcx 変形前の点のX座標
	 * @param srcy 変形前の点のY座標
	 */
	void(*TransformPoint)(float* dstx, float* dsty, const float* xform, float srcx, float srcy);

	/**
	* @brief 角度をラジアンに変換
	* @param deg 角度
	* @return degに対応するラジアン
	*/

	float(*DegToRad)(float deg);

	/**
	* @brief ラジアンを角度に変換
	* @param rad ラジアン
	* @return radに対応する角度
	*/
	float(*RadToDeg)(float rad);

	/**
	* @brief 画像を読み取ってRGBA配列とサイズを返す
	*/
	unsigned char* (*LoadImageData)(const char* filename, int *width, int *height);
	/**
	* @brief LoadImageDataのUTF8版
	*/
	unsigned char* (*LoadImageDataUTF8)(const char* filename, int *width, int *height);

	/**
	* @brief LoadImageDataで読み取ったデータを解放する
	*/
	void (*FreeImageData)(unsigned char *img);

	/**
	 * @brief CreateImageのUTF8版
	 */
	int(*CreateImageUTF8)(const char* filename, enum ImageFlags imageFlags);

	/**
	 * @brief 画像データをファイルから読み込む（対応する形式はJPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC）
	 * @param filename 画像ファイルのファイルパス
	 * @param imageFlags 画像を描画で用いる際のオプションを enum ImageFlags のゼロ個以上の組み合わせで指定
	 * @retval  == 0 読み込み失敗
	 * @retval   > 0 画像ID
	 */
	int(*CreateImage)(const char* filename, enum ImageFlags imageFlags);

	/**
	 * @brief 画像データをメモリ上から読み込む（対応する形式はJPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC）
	 * @param imageFlags 画像を描画で用いる際のオプションを enum ImageFlags のゼロ個以上の組み合わせで指定
	 * @param data 画像データのポインタ
	 * @param ndata 画像データのサイズ
	 * @retval  == 0 読み込み失敗
	 * @retval   > 0 画像ID
	 */
	int(*CreateImageMem)(enum ImageFlags imageFlags, unsigned char* data, int ndata);

	/**
	 * @brief 32bitRGBA画像データを読み込む
	 * @param w 画像の幅
	 * @param h 画像の高さ
	 * @param imageFlags 画像を描画で用いる際のオプションを enum ImageFlags のゼロ個以上の組み合わせで指定
	 * @param data 画像データのポインタ
	 * @retval  == 0 読み込み失敗
	 * @retval   > 0 画像ID
	 */
	int(*CreateImageRGBA)(int w, int h, enum ImageFlags imageFlags, const unsigned char* data);

	/**
	* @brief 32bitRGBA画像データ data で画像ID img の画像データを置き換える。画像サイズなどは変更できない
	* @param image 画像ID
	* @param data 画像データのポインタ
	* @note 動画やウェブカメラの画像などサイズは固定だが時間で変化する画像を描画する場合に用いる
	*/
	void(*UpdateImage)(int image, const unsigned char* data);

	/**
	* @brief 画像サイズを取得
	* @param image 画像ID
	* @param w 画像の幅の格納先ポインタ
	* @param h 画像の高さの格納先ポインタ
	*/
	void(*ImageSize)(int image, int* w, int* h);

	/**
	 * @brief 画像を破棄する
	 * @param image 画像ID
	 */
	void(*DeleteImage)(int image);

	/**
	 * @brief 線形グラデーションの塗りつぶしパターンを作成
	 * @param sx グラデーションの始点X座標
	 * @param sy グラデーションの始点Y座標
	 * @param ex グラデーションの終点X座標
	 * @param ey グラデーションの終点Y座標
	 * @param icol 始点側の色
	 * @param ocol 終点側の色
	 * @return 作成された塗りつぶしパターン
	 */
	Paint(*LinearGradient)(float sx, float sy, float ex, float ey, Color icol, Color ocol);

	/**
	 * @brief 矩形グラデーションの塗りつぶしパターンを作成
	 * @param x 矩形の左上点のX座標
	 * @param y 矩形の左上点のY座標
	 * @param w 矩形の幅
	 * @param h 矩形の高さ
	 * @param r 矩形の角の丸め半径
	 * @param f 矩形の境界のぼかし距離
	 * @param icol 始点（中央）側の色
	 * @param ocol 終点（境界）側の色
	 * @return 作成された塗りつぶしパターン
	 */
	Paint(*BoxGradient)(float x, float y, float w, float h, float r, float f, Color icol, Color ocol);

	/**
	 * @brief 円形グラデーションの塗りつぶしパターンを作成
	 * @param cx 円の中心点のX座標
	 * @param cy 円の中心点のY座標
	 * @param inr 内側の半径
	 * @param outr 外側の半径
	 * @param icol 内側の色
	 * @param ocol 外側の色
	 * @return 作成された塗りつぶしパターン
	 */
	Paint(*RadialGradient)(float cx, float cy, float inr, float outr, Color icol, Color ocol);

	/**
	* @brief 画像パターンを作成
	* @param ox パターンに使う画像領域（矩形）の左上点のX座標
	* @param oy パターンに使う画像領域（矩形）の左上点のY座標
	* @param ex パターンに使う画像領域（矩形）の幅
	* @param ey パターンに使う画像領域（矩形）の高さ
	* @param angle 画像の回転角度（回転中心がパターンの左上であることに注意）
	* @param image 画像ID
	* @param alpha 画像の不透明度(0.0～1.0で指定)
	* @return 作成された塗りつぶしパターン
	*/
	Paint(*ImagePattern)(float ox, float oy, float ex, float ey, float angle, int image, float alpha);

	/**
	* @brief 画像パターンを作成(画像の色としてFillColorに設定されている色を用いる)
	* @param ox パターンに使う画像領域（矩形）の左上点のX座標
	* @param oy パターンに使う画像領域（矩形）の左上点のY座標
	* @param ex パターンに使う画像領域（矩形）の幅
	* @param ey パターンに使う画像領域（矩形）の高さ
	* @param angle 画像の回転角度（回転中心がパターンの左上であることに注意）
	* @param image 画像ID
	* @return 作成された塗りつぶしパターン
	* この命令は特殊な使い方を想定している（爆発演出のパーティクルにテクスチャを使って赤色を追加するなど）。
	* 命令の構成にはこの先も少し手を入れる予定なので必要でない限り使用を避けてください。
	*/
	Paint(*ImagePatternWithFillColor)(float cx, float cy, float w, float h, float angle, int image);

	/**
	* @brief シザリング（クリッピング）領域を新規設定する
	* @param x シザリング（クリッピング）領域（矩形）の左上点のX座標
	* @param y シザリング（クリッピング）領域（矩形）の左上点のY座標
	* @param w シザリング（クリッピング）領域（矩形）の幅
	* @param h シザリング（クリッピング）領域（矩形）の高さ
	* シザリング（クリッピング）領域を設定するとその領域の外側には描画が行われなくなる。
	* 画面分割でプレイヤーごとに画面を描画する際に用いると無駄な描画を減らすことができる。
	*/
	void(*Scissor)(float x, float y, float w, float h);

	/**
	* @brief シザリング（クリッピング）領域を設定されている領域と引数で与えた領域の共通領域に更新する
	* @param x 重ねるシザリング（クリッピング）領域（矩形）の左上点のX座標
	* @param y 重ねるシザリング（クリッピング）領域（矩形）の左上点のY座標
	* @param w 重ねるシザリング（クリッピング）領域（矩形）の幅
	* @param h 重ねるシザリング（クリッピング）領域（矩形）の高さ
	*/
	void(*IntersectScissor)(float x, float y, float w, float h);

	/**
	* @brief シザリング（クリッピング）領域をリセットする。リセット後は画面全体が新しいシザリング領域になる
	*/
	void(*ResetScissor)(void);

	/**
	 * @brief 図形の描画を開始する
	 * 描画完了後は必ずVideo.EndFrame()を呼び出すこと。
	 */
	void(*BeginPath)(void);

	/**
	 * @brief 図形を描画するペンの座標を設定する
	 * @param x ペンのX座標
	 * @param y ペンのY座標
	 */
	void(*MoveTo)(float x, float y);

	/**
	 * @brief 現在のペン座標と指定した座標の間に辺を作成し、ペンを移動させる
	 * @param x 移動先のペンのX座標
	 * @param y 移動先のペンのY座標
	 */
	void(*LineTo)(float x, float y);

	/**
	 * @brief 現在のペン座標、制御点1、制御点2、指定した座標を用いて3次ベジェ曲線 (Cubic Bezier curve) を作成し、ペンを移動させる
	 * @param c1x 制御点1のX座標
	 * @param c1y 制御点1のY座標
	 * @param c2x 制御点2のX座標
	 * @param c2y 制御点2のy座標
	 * @param x 移動先のペンのX座標
	 * @param y 移動先のペンのY座標
	 * ベジェ曲線の開設は https://ja.wikipedia.org/wiki/%E3%83%99%E3%82%B8%E3%82%A7%E6%9B%B2%E7%B7%9A などを参照
	 */
	void(*BezierTo)(float c1x, float c1y, float c2x, float c2y, float x, float y);

	/**
	* @brief 現在のペン座標、制御点、指定した座標を用いて2次ベジェ曲線 (Quadratic Bezier curve)を作成し、ペンを移動させる
	* @param cx 制御点のX座標
	* @param cy 制御点のY座標
	* @param x 移動先のペンのX座標
	* @param y 移動先のペンのY座標
	* ベジェ曲線の開設は https://ja.wikipedia.org/wiki/%E3%83%99%E3%82%B8%E3%82%A7%E6%9B%B2%E7%B7%9A などを参照
	*/
	void(*QuadTo)(float cx, float cy, float x, float y);

	/**
	* @brief 現在のペン位置と制御点と移動先の点を元に半径radiusの円弧を描く
	* @param x1 制御点のX座標
	* @param y1 制御点のY座標
	* @param x2 移動先のペンのX座標
	* @param y2 移動先のペンのY座標
	* @param radius 円弧の半径
	* 円弧描画の解説は http://www.htmq.com/canvas/arcTo.shtml がまとまっている。
	*/
	void(*ArcTo)(float x1, float y1, float x2, float y2, float radius);

	/**
	* @brief 図形の描画を完了させる
	*/
	void(*ClosePath)(void);

	/**
	 * @brief 直前に追加した図形の形状の特性を穴or面で指定する
	 * @param dir 特性を示す enum Solidity 値
	 */
	void(*PathWinding)(enum Solidity dir);

	/**
	 * @brief 中心点 (cx, cy) で半径 r の円の円周上で時計回りで角度 a0 から a1 までの円弧を描く
	 * @param cx 円の中心点のX座標
	 * @param cy 円の中心点のY座標
	 * @param r  円の半径
	 * @param a1 円弧の始点角度
	 * @param a2 円弧の終点角度
	 * @param dir 始点と終点と結ぶ際の回転方向を示す enum Winding 値
	 */
	void(*Arc)(float cx, float cy, float r, float a0, float a1, enum Winding dir);

	/**
	 * @brief 左上座標(x,y) 幅 w 高さ h の矩形を描画する
	 * @param x 矩形の左上点のX座標
	 * @param y 矩形の左上点のY座標
	 * @param w 矩形の幅
	 * @param h 矩形の高さ
	 */
	void(*Rect)(float x, float y, float w, float h);

	/**
	 * @brief 左上座標(x,y) 幅 w 高さ h の矩形を半径 r で丸めた矩形を描画する
	 * @param x 矩形の左上点のX座標
	 * @param y 矩形の左上点のY座標
	 * @param w 矩形の幅
	 * @param h 矩形の高さ
	 * @param r 矩形の角の丸め半径
	 */
	void(*RoundedRect)(float x, float y, float w, float h, float r);

	/**
	* @brief 左上座標(x,y) 幅 w 高さ h の矩形それぞれの角をradTopLeft, radTopRight, radBottomRight, radBottomLeft で丸めた矩形を描画する
	* @param x 矩形の左上点のX座標
	* @param y 矩形の左上点のY座標
	* @param w 矩形の幅
	* @param h 矩形の高さ
	* @param radTopLeft 矩形の左上の丸め半径
	* @param radTopRight 矩形の右上の丸め半径
	* @param radBottomRight 矩形の左下の丸め半径
	* @param radBottomLeft 矩形の右下の丸め半径
	*/
	void(*RoundedRectVarying)(float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft);

	/**
	 * @brief 中心点 (cx, cy) で横半径 rx 縦半径 ry の楕円を描く
	 * @param cx 楕円の中心点のX座標
	 * @param cy 楕円の中心点のY座標
	 * @param rx 楕円の横半径
	 * @param ry 楕円の縦半径
	 */
	void(*Ellipse)(float cx, float cy, float rx, float ry);

	/**
	 * @brief 中心点 (cx, cy) で半径 r の円を描く
	 * @param cx 円の中心点のX座標
	 * @param cy 円の中心点のY座標
	 * @param r 円の半径
	 */
	void(*Circle)(float cx, float cy, float r);

	/**
	* @brief 描いた形状を塗りつぶす
	* 塗りつぶしは Video.FillColor() で設定した色、もしくは Video.FillPaint() で設定したパターンが使われる
	*/
	void(*Fill)(void);

	/**
	 * @brief 描いた形状の輪郭線を描画する
	 * 線は Video.StrokeColor() で設定した色、もしくは Video.StrokePaint() で設定したパターンを用いて Video.StrokeWidth()で設定された太さで描画される
	 */
	void(*Stroke)(void);

	/**
	 * @brief CreateFontのUTF8版
	 */
	int(*CreateFontUTF8)(const char* name, const char* filename);

	/**
	 * @brief TrueType形式のフォントをTTFファイル filename から読み込み、フォント名に名前 name を割り当てる
	 * @param name 読み込んだフォントに割り当てる名前
	 * @param filename 読み込むTTF形式のフォントファイル
	 * @retval == -1 読み込み失敗
	 * @retval >=  0 読み込んだフォントに割り当てられるフォントID
	 */
	int(*CreateFont)(const char* name, const char* filename);

	/**
	 * @brief CreateFontMemのUTF8版
	 */
	int(*CreateFontMemUTF8)(const char* name, unsigned char* data, int ndata, int freeData);

	/**
	 * @brief TrueType形式のフォントデータ data をメモリから読み込み、フォント名に名前 name を割り当てる
	 * @param name 読み込んだフォントに割り当てる名前
	 * @param data 読み込むTTF形式のフォントデータ
	 * @param ndata 読み込むTTF形式のフォントデータのサイズ
	 * @param freeData 通常時は使わない引数なので 0 を設定すること
	 * @retval == -1 読み込み失敗
	 * @retval >=  0 読み込んだフォントに割り当てられるフォントID
	 */
	int(*CreateFontMem)(const char* name, unsigned char* data, int ndata, int freeData);

	/**
	 * @brief FindFontのUTF8版
	 */
	int(*FindFontUTF8)(const char* name);

	/**
	 * @brief 既に読み込んだフォントの中から名前が name のものを探す
	 * @param name 探すフォントの名前
	 * @retval == -1 見つからなかった
	 * @retval >=  0 見つかったフォントに割り当てられているフォントID
	 */
	int(*FindFont)(const char* name);

	/**
	* @brief baseFontの代替えフォントをfallbackFontに設定する
	* @param baseFont ベースとするフォントのID
	* @param fallbackFont 代替えフォントとするフォントのID
	*/
	void (*AddFallbackFontId)(int baseFont, int fallbackFont);

	/**
	* @brief AddFallbackFontのUTF8版
	* @param baseFont ベースとするフォントの名前
	* @param fallbackFont 代替えフォントとするフォントの名前
	*/
	void(*AddFallbackFontUTF8)(const char* baseFont, const char* fallbackFont);

	/**
	* @brief baseFontの代替えフォントをfallbackFontに設定する
	* @param baseFont ベースとするフォントの名前
	* @param fallbackFont 代替えフォントとするフォントの名前
	*/
	void(*AddFallbackFont)(const char* baseFont, const char* fallbackFont);

	/**
	 * @brief 描画する文字のサイズを設定する
	 * @param size フォントサイズ
	 */
	void(*FontSize)(float size);

	/**
	* @brief 描画する文字のぼかし距離を設定する
	* @param size 境界をぼかす距離
	*/
	void(*FontBlur)(float blur);

	/**
	* @brief 描画する文字の文字間隔を設定する
	* @param size 文字間隔
	*/
	void(*TextLetterSpacing)(float spacing);

	/**
	* @brief 描画する文字の行高を設定する
	* @param size 行高
	*/
	void(*TextLineHeight)(float lineHeight);

	/**
	* @brief 描画する文字の配置を指定する
	* @param align 縦横それぞれを enum Align の組み合わせで指定する
	*/
	void(*TextAlign)(enum Align align);

	/**
	* @brief 文字の描画に使うフォントをフォントIDで指定する
	* @param font 描画に使うフォントのフォントID
	*/
	void(*FontFaceId)(int font);

	/**
	* @brief Video.FontFace()関数のUTF8専用版
	*/
	void(*FontFaceUTF8)(const char* font);

	/**
	* @brief 文字の描画に使うフォントをフォント名で指定する
	* @param font 描画に使うフォントのフォント名
	*/
	void(*FontFace)(const char* font);

	/**
	* @brief Video.Text()関数のUTF8専用版
	*/
	float(*TextUTF8)(float x, float y, const char* string, const char* end);

	/**
	* @brief 座標 (x, y)を基準点として文字列 string を描画する
	* @param x 基準点のX座標
	* @param y 基準点のY座標
	* @param string 描画する文字列の始点を示すポインタ
	* @param end    描画する文字列の終点を示すポインタ。与えた文字列すべてを描画したい場合は NULL を渡す。
	* @return 最後に描いた文字のX座標
	*/
	float(*Text)(float x, float y, const char* string, const char* end);

	/**
	* @brief Video.TextBox()関数のUTF8専用版
	*/
	void(*TextBoxUTF8)(float x, float y, float breakRowWidth, const char* string, const char* end);

	/**
	* @brief テキストを折り返す幅を breakRowWidth 、座標 (x, y)を基準点として文字列 string を描画する
	* @param x 基準点のX座標
	* @param y 基準点のY座標
	* @param breakRowWidth テキストを折り返す幅
	* @param string 描画する文字列の始点を示すポインタ
	* @param end    描画する文字列の終点を示すポインタ。与えた文字列すべてを描画したい場合は NULL を渡す。
	*/
	void(*TextBox)(float x, float y, float breakRowWidth, const char* string, const char* end);

	/**
	* @brief Video.TextBounds()関数のUTF8専用版
	*/
	float(*TextBoundsUTF8)(float x, float y, const char* string, const char* end, float* bounds);

	/**
	* @brief 座標 (x, y)を基準点として文字列 string を描画したときの描画領域を取得する（描画は行わない）
	* @param x 基準点のX座標
	* @param y 基準点のY座標
	* @param string 描画する文字列の始点を示すポインタ
	* @param end    描画する文字列の終点を示すポインタ。与えた文字列すべてを描画したい場合は NULL を渡す。
	* @param bounds 描画する文字列がぴったり入る領域の情報が格納される４要素以上の配列。[左上のX座標, 左上のY座標, 右上のX座標, 右上のY座標] の順で格納される
	* @return 最後に描いた文字のX座標
	*/
	float(*TextBounds)(float x, float y, const char* string, const char* end, float* bounds);

	/**
	* @brief Video.TextBoxBounds()関数のUTF8専用版
	*/
	void(*TextBoxBoundsUTF8)(float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds);

	/**
	* @brief 座標 (x, y)を基準点とし、テキストを折り返す幅を breakRowWidthとして文字列 string を描画したときの描画領域を取得する（描画は行わない）
	* @param x 基準点のX座標
	* @param y 基準点のY座標
	* @param breakRowWidth テキストを折り返す幅
	* @param string 描画する文字列の始点を示すポインタ
	* @param end    描画する文字列の終点を示すポインタ。与えた文字列すべてを描画したい場合は NULL を渡す。
	* @param bounds 描画する文字列がぴったり入る領域の情報が格納される４要素以上の配列。[左上のX座標, 左上のY座標, 右上のX座標, 右上のY座標] の順で格納される
	*/
	void(*TextBoxBounds)(float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds);

	/**
	* @brief Video.TextGlyphPositions()関数のUTF8専用版
	*/
	int(*TextGlyphPositionsUTF8)(float x, float y, const char* string, const char* end, GlyphPosition* positions, int maxPositions);

	/**
	* @brief 座標 (x, y)を基準点として文字列 string を描画したときの各文字の位置情報を maxPositions 個取得して 配列 positions に格納する。
	* @param x 基準点のX座標
	* @param y 基準点のY座標
	* @param string 描画する文字列の始点を示すポインタ
	* @param end    描画する文字列の終点を示すポインタ。与えた文字列すべてを描画したい場合は NULL を渡す。
	* @param positions 各文字の位置情報を格納する GlyphPosition 型変数配列。要素数はmaxPositions以上でなければならない。
	* @param maxPositions 取得する情報数の上限
	* @return 実際にpositionsに格納された要素の数
	* ゲーム内にテキスト入力用のGUIを作る場合などで使う
	*/
	int(*TextGlyphPositions)(float x, float y, const char* string, const char* end, GlyphPosition* positions, int maxPositions);

	/**
	* @brief 描画に用いるフォントのメトリクス情報(アセンダ、ディセンダ、行高さ)を取得する
	* @param ascender フォントのアセンダ（ベースラインから文字上部までの高さ）
	* @param descender フォントのディセンダ（ベースラインから文字下部までの高さ）
	* @param lineh フォントの行高さ
	*/
	void(*TextMetrics)(float* ascender, float* descender, float* lineh);

	/**
	* @brief Video.TextBreakLines()関数のUTF8専用版
	*/
	int(*TextBreakLinesUTF8)(const char* string, const char* end, float breakRowWidth, TextRow* rows, int maxRows);

	/**
	* @brief テキストを折り返す幅を breakRowWidthとして文字列 string を描画する際の画面上での各行の情報を得る
	* @param string 描画する文字列の始点を示すポインタ
	* @param end    描画する文字列の終点を示すポインタ。与えた文字列すべてを描画したい場合は NULL を渡す。
	* @param positions 各行の情報を格納する TextRow 型変数配列。要素数はmaxRows以上でなければならない。
	* @param maxPositions 取得する情報数の上限
	* @return 実際にpositionsに格納された要素の数
	* ゲーム内にテキスト入力用のGUIやクリック可能なテキストウィンドウを作る場合などで使う
	*/
	int(*TextBreakLines)(const char* string, const char* end, float breakRowWidth, TextRow* rows, int maxRows);

	/**
	* @brief 画像の左上座標(sx, sy), 幅sw, 高さsh で示される領域を画面の左上座標(dx,dy), 幅 dw, 高さ dh に角度 angle, 不透明度 alhpa で描画する
	* @param image 画像ID
	* @param sx 転送元領域の左上のX座標
	* @param sy 転送元領域の左上のY座標
	* @param sw 転送元領域の幅
	* @param sh 転送元領域の高さ
	* @param dx 転送先領域の左上のX座標
	* @param dy 転送先領域の左上のY座標
	* @param dw 転送先領域の幅
	* @param dh 転送先領域の高さ
	* @param angle 画像の回転角度（回転中心は画像の左上であることに注意）
	* @param alpha 不透明度(1.0が完全不透明, 0.0が完全透明)
	*/
	void(*DrawImage)(int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle, float alpha);

	/**
	* @brief 画像の左上座標(sx, sy), 幅sw, 高さsh で示される領域を画面の左上座標(dx,dy), 幅 dw, 高さ dh に角度 angleで描画する。描画色にはFillColorを用いる。
	* @param image 画像ID
	* @param sx 転送元領域の左上のX座標
	* @param sy 転送元領域の左上のY座標
	* @param sw 転送元領域の幅
	* @param sh 転送元領域の高さ
	* @param dx 転送先領域の左上のX座標
	* @param dy 転送先領域の左上のY座標
	* @param dw 転送先領域の幅
	* @param dh 転送先領域の高さ
	* @param angle 画像の回転角度（回転中心は画像の左上であることに注意）
	*/
	void(*DrawImageWithFillColor)(int image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float angle);

	/**
	* @brief 座標 (x, y)を基準点とし、printf同様に str を書式化文字列、その後に続く引数をパラメータとして用いて文字列を構築して描画する
	* @param x 基準点のX座標
	* @param y 基準点のY座標
	* @param str 書式化文字列 (printfのものと同じ)
	* @param ... パラメータ (printfのものと同じ)
	* スコアや制限時間の描画など使える。
	*/
	void(*FormatText)(float x, float y, const char *str, ... );

	/**
	 * @brief レンダリング対象として使えるフレームバッファオブジェクトを作る
	 * @param w 幅
	 * @param h 高さ
	 * @param imageFlags 画像を描画で用いる際のオプションを enum ImageFlags のゼロ個以上の組み合わせで指定
	 * @retval フレームバッファオブジェクトのハンドル（DeleteFramebufferで解放が必要）
	 */
	void* (*CreateFramebuffer)(int w, int h, int imageFlags);

	/**
	* @brief フレームバッファオブジェクト fb への描画を開始する。
	* @param fb レンダリング対象とするフレームバッファオブジェクトのハンドル
	*/
	void(*DrawStartFramebuffer)(void* fbo);

	/**
	* @brief フレームバッファオブジェクト fb への描画を開始する。
	* @param fb レンダリング対象とするフレームバッファオブジェクトのハンドル
	*/
	void(*DrawEndFramebuffer)(void);

	/**
	 * @brief フレームバッファオブジェクトを解放する
	 * @param fb 解放するフレームバッファオブジェクトのハンドル
	 */
	void(*DeleteFramebuffer)(void* fb);

	/**
	 * @brief フレームバッファオブジェクトに対応するDrawImageなどで使える画像ハンドルを取得する
	 * @param fb フレームバッファオブジェクトのハンドル
	 * @retval DrawImageなどで使える画像ハンドル
	 */
	int (*GetFrameBufferImage)(void* fb);

	/**
	* @brief SaveScreenShot関数のUTF8版
	* @param l スクリーンショット範囲の左座標
	* @param t スクリーンショット範囲の上座標
	* @param w スクリーンショット範囲の幅
	* @param h スクリーンショット範囲の高さ
	* @param premult プレマルチプライド方式を前提とした出力にするかどうか
	* @param name 保存するファイル名(拡張子で保存フォーマットを選択する)
	* @retval true 保存成功
	* @retval false 保存失敗
	*/
	bool(*SaveScreenShotUTF8)(int l, int t, int w, int h, bool premult, const char* name);

	/**
	* @brief 画面の座標(l,t)から幅w,高さhの範囲をスクリーンショットで保存する
	* @param l スクリーンショット範囲の左座標
	* @param t スクリーンショット範囲の上座標
	* @param w スクリーンショット範囲の幅
	* @param h スクリーンショット範囲の高さ
	* @param premult プレマルチプライド方式を前提とした出力にするかどうか
	* @param name 保存するファイル名(拡張子で保存フォーマットを選択する)
	* @retval true 保存成功
	* @retval false 保存失敗
	*/
	bool(*SaveScreenShot)(int l, int t, int w, int h, bool premult, const char* name);

} Video;

/**
 * @brief マウス情報
 */
extern const struct __tagMouseAPI {
	/**
	* @brief Video.Drawing()を使わずに入力を管理する場合に用いるマウス情報の更新命令。
	* @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
	*/
	void (*Update)(void);

	/**
	* @brief マウスの左ボタンが押されているかどうか調べる
	* @retval true 押されている
	* @retval false 押されていない
	*/
	bool(*IsLeftButtonDown)(void);

	/**
	* @brief マウスの右ボタンが押されているかどうか調べる
	* @retval true 押されている
	* @retval false 押されていない
	*/
	bool(*IsRightButtonDown)(void);

	/**
	* @brief マウスのホイールが押されているかどうか調べる
	* @retval true 押されている
	* @retval false 押されていない
	*/
	bool(*IsMiddleButtonDown)(void);

	/**
	* @brief マウスの左ボタンが押された瞬間かどうか調べる
	* @retval true 押された瞬間である
	* @retval false 押された瞬間ではない
	*/
	bool(*IsLeftButtonPush)(void);

	/**
	* @brief マウスの右ボタンが押された瞬間かどうか調べる
	* @retval true 押された瞬間である
	* @retval false 押された瞬間ではない
	*/
	bool(*IsRightButtonPush)(void);

	/**
	* @brief マウスのホイールが押された瞬間かどうか調べる
	* @retval true 押された瞬間である
	* @retval false 押された瞬間ではない
	*/
	bool(*IsMiddleButtonPush)(void);

	/**
	* @brief マウスの左ボタンが離されているかどうか調べる
	* @retval true 離されている
	* @retval false 離されていない
	*/
	bool(*IsLeftButtonUp)(void);

	/**
	* @brief マウスの右ボタンが離されているかどうか調べる
	* @retval true 離されている
	* @retval false 離されていない
	*/
	bool(*IsRightButtonUp)(void);

	/**
	* @brief マウスのホイールが離されているかどうか調べる
	* @retval true 離されている
	* @retval false 離されていない
	*/
	bool(*IsMiddleButtonUp)(void);

	/**
	* @brief マウスの左ボタンが離された瞬間かどうか調べる
	* @retval true 離された瞬間である
	* @retval false 離された瞬間ではない
	*/
	bool(*IsLeftButtonRelease)(void);

	/**
	* @brief マウスの右ボタンが離された瞬間かどうか調べる
	* @retval true 離された瞬間である
	* @retval false 離された瞬間ではない
	*/
	bool(*IsRightButtonRelease)(void);

	/**
	* @brief マウスのホイールが離された瞬間かどうか調べる
	* @retval true 離された瞬間である
	* @retval false 離された瞬間ではない
	*/
	bool(*IsMiddleButtonRelease)(void);

	/**
	* @brief 画面上におけるカーソルの位置を得る
	* @param x カーソルのx座標が格納される変数のポインタ
	* @param y カーソルのy座標が格納される変数のポインタ
	*/
	void(*GetCursorPos)(double *x, double *y);
} Mouse;

/**
* @brief ジョイスティック情報
*/
extern const struct __tagJoystickAPI {
	/**
	 * @brief ジョイスティックポート数を取得
	 * @retval ジョイスティックポート数
	 */
	int(*MaxJoystickPort)(void);

	/**
	* @brief ジョイスティック管理を初期化する
	*/
	void(*Initialize)(void);

	/**
	* @brief ジョイスティック管理を解放する
	*/
	void(*Finalize)(void);

	/**
	* @brief 接続されているジョイスティックを管理下に入れる
	*/
	void(*Connect)(void);

	/**
	* @brief 管理下にあるジョイスティックを全て管理下から外す
	*/
	void(*Disconnect)(void);

	/**
	* @brief 管理下のジョイスティックの情報を更新する
	*/
	void(*Update)(void);

	/**
	* @brief ポートid番にジョイスティックが接続されているかどうかを調べる
	* @param id 調べるポート番号
	* @retval true 接続されている
	* @retval false 接続されていない
	*/
	bool(*IsPresented)(int id);

	/**
	* @brief ポートid番のジョイスティックが提供する軸の数を調べる
	* @param id 調べるポート番号
	* @retval >= 0 軸の数
	* @retval <  0 ジョイスティックが接続されていない
	*/
	int(*GetAxesCount)(int id);

	/**
	* @brief ポートid番のジョイスティックが提供する軸axの状態を調べる
	* @param id 調べるポート番号
	* @param ax 調べる軸番号
	* @retval 軸の状態が[-1.0..1.0]の範囲で返る
	*/
	float(*GetAxesStatus)(int id, int ax);

	/**
	* @brief ポートid番のジョイスティックが提供するボタンの数を調べる
	* @param id 調べるポート番号
	* @retval >= 0 ボタンの数
	* @retval <  0 ジョイスティックが接続されていない
	*/
	int(*GetButtonCount)(int id);

	/**
	* @brief ポートid番のジョイスティックが提供するボタンbtnが押されているかどうか調べる
	* @retval true 押されている
	* @retval false 押されていない
	*/
	bool(*IsButtonDown)(int id, int btn);

	/**
	* @brief ポートid番のジョイスティックが提供するボタンbtnが押された瞬間かどうか調べる
	* @retval true 押された瞬間である
	* @retval false 押された瞬間ではない
	*/
	bool(*IsButtonPush)(int id, int btn);

	/**
	* @brief ポートid番のジョイスティックが提供するボタンbtnが離されているかどうか調べる
	* @retval true 離されている
	* @retval false 離されていない
	*/
	bool(*IsButtonUp)(int id, int btn);

	/**
	* @brief ポートid番のジョイスティックが提供するボタンbtnが離された瞬間かどうか調べる
	* @retval true 離された瞬間である
	* @retval false 離された瞬間ではない
	*/
	bool(*IsButtonRelease)(int id, int btn);
} Joystick;

/*
 * キーボードのキーを示すコード
 */
enum Keycode
{
	KBKEY_SPACE = 32,
	KBKEY_APOSTROPHE = 39,  /* ' */
	KBKEY_COMMA = 44,  /* , */
	KBKEY_MINUS = 45,  /* - */
	KBKEY_PERIOD = 46,  /* . */
	KBKEY_SLASH = 47,  /* / */
	KBKEY_0 = 48,
	KBKEY_1 = 49,
	KBKEY_2 = 50,
	KBKEY_3 = 51,
	KBKEY_4 = 52,
	KBKEY_5 = 53,
	KBKEY_6 = 54,
	KBKEY_7 = 55,
	KBKEY_8 = 56,
	KBKEY_9 = 57,
	KBKEY_SEMICOLON = 59,  /* ; */
	KBKEY_EQUAL = 61,  /* = */
	KBKEY_A = 65,
	KBKEY_B = 66,
	KBKEY_C = 67,
	KBKEY_D = 68,
	KBKEY_E = 69,
	KBKEY_F = 70,
	KBKEY_G = 71,
	KBKEY_H = 72,
	KBKEY_I = 73,
	KBKEY_J = 74,
	KBKEY_K = 75,
	KBKEY_L = 76,
	KBKEY_M = 77,
	KBKEY_N = 78,
	KBKEY_O = 79,
	KBKEY_P = 80,
	KBKEY_Q = 81,
	KBKEY_R = 82,
	KBKEY_S = 83,
	KBKEY_T = 84,
	KBKEY_U = 85,
	KBKEY_V = 86,
	KBKEY_W = 87,
	KBKEY_X = 88,
	KBKEY_Y = 89,
	KBKEY_Z = 90,
	KBKEY_LEFT_BRACKET = 91,  /* [ */
	KBKEY_BACKSLASH = 92,  /* \ */
	KBKEY_RIGHT_BRACKET = 93,  /* ] */
	KBKEY_GRAVE_ACCENT = 96,  /* ` */
	KBKEY_WORLD_1 = 161, /* non-US #1 */
	KBKEY_WORLD_2 = 162, /* non-US #2 */

	/* Function keys */
	KBKEY_ESCAPE = 256,
	KBKEY_ENTER = 257,
	KBKEY_TAB = 258,
	KBKEY_BACKSPACE = 259,
	KBKEY_INSERT = 260,
	KBKEY_DELETE = 261,
	KBKEY_RIGHT = 262,
	KBKEY_LEFT = 263,
	KBKEY_DOWN = 264,
	KBKEY_UP = 265,
	KBKEY_PAGE_UP = 266,
	KBKEY_PAGE_DOWN = 267,
	KBKEY_HOME = 268,
	KBKEY_END = 269,
	KBKEY_CAPS_LOCK = 280,
	KBKEY_SCROLL_LOCK = 281,
	KBKEY_NUM_LOCK = 282,
	KBKEY_PRINT_SCREEN = 283,
	KBKEY_PAUSE = 284,
	KBKEY_F1 = 290,
	KBKEY_F2 = 291,
	KBKEY_F3 = 292,
	KBKEY_F4 = 293,
	KBKEY_F5 = 294,
	KBKEY_F6 = 295,
	KBKEY_F7 = 296,
	KBKEY_F8 = 297,
	KBKEY_F9 = 298,
	KBKEY_F10 = 299,
	KBKEY_F11 = 300,
	KBKEY_F12 = 301,
	KBKEY_F13 = 302,
	KBKEY_F14 = 303,
	KBKEY_F15 = 304,
	KBKEY_F16 = 305,
	KBKEY_F17 = 306,
	KBKEY_F18 = 307,
	KBKEY_F19 = 308,
	KBKEY_F20 = 309,
	KBKEY_F21 = 310,
	KBKEY_F22 = 311,
	KBKEY_F23 = 312,
	KBKEY_F24 = 313,
	KBKEY_F25 = 314,
	KBKEY_KP_0 = 320,
	KBKEY_KP_1 = 321,
	KBKEY_KP_2 = 322,
	KBKEY_KP_3 = 323,
	KBKEY_KP_4 = 324,
	KBKEY_KP_5 = 325,
	KBKEY_KP_6 = 326,
	KBKEY_KP_7 = 327,
	KBKEY_KP_8 = 328,
	KBKEY_KP_9 = 329,
	KBKEY_KP_DECIMAL = 330,
	KBKEY_KP_DIVIDE = 331,
	KBKEY_KP_MULTIPLY = 332,
	KBKEY_KP_SUBTRACT = 333,
	KBKEY_KP_ADD = 334,
	KBKEY_KP_ENTER = 335,
	KBKEY_KP_EQUAL = 336,
	KBKEY_LEFT_SHIFT = 340,
	KBKEY_LEFT_CONTROL = 341,
	KBKEY_LEFT_ALT = 342,
	KBKEY_LEFT_SUPER = 343,
	KBKEY_RIGHT_SHIFT = 344,
	KBKEY_RIGHT_CONTROL = 345,
	KBKEY_RIGHT_ALT = 346,
	KBKEY_RIGHT_SUPER = 347,
	KBKEY_MENU = 348,
};

/**
* @brief キーボード情報
*/
extern const struct __tagKeyboardAPI {
	/**
	* @brief キーボードの指定したキーが押されているか調べる
	* @param key 調べたいキーを示す enum Keycode の値
	* @retval true 押されている
	* @retval false 押されていない
	* スペース、アポストロフィ、コンマ、マイナス、ピリオド、スラッシュ、セミコロン、イコール、
	* 数字の1～10、大文字のA～Z、ブラケット記号のキーについては 'A'のような表記でもよい
	*/
	bool(*IsKeyDown)(enum Keycode key);
} Keyboard;

/**
 * @brief 画面の更新間隔（フレームレート）情報
 */
extern const struct __tagFrameRateAPI {
	void(*SetFrameRate)(unsigned int fps);
	unsigned int(*GetFrameRate)(void);
	float(*GetRealFrameRate)();
	unsigned int(*GetCPUPower)(void);
} FrameRate;

#endif
