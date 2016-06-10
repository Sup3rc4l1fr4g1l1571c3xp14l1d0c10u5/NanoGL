#include <NanoGL/NanoGL.h>

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 03-TransformMulti");

	// 処理
	while (Video.Drawing()) {

		//
		// 変形は複数の図形にも適用できる
		//

		Video.BeginPath();

		Video.ResetTransform();// 変形のリセット

		// ３秒で１回転するように現在の回転角度をプログラム起動からの経過時間で算出
		double degree = 360.0 * Video.GetTime() / 3.0;

		// 角度に合わせて大きさが変化するようにsin関数でスケールを算出
		double scale = sin(Video.DegToRad(degree)) * 0.5 + 1.0;

		Video.Translate(256, 256);				// 4の処理
		Video.Rotate(Video.DegToRad(degree));	// 3の処理
		Video.Scale(scale, scale);				// 2の処理
		Video.Translate(-256, -256);			// 1の処理

		// 画面上に４個の正方形を配置する。

		// 上で指定した変形が適用されるので、実際には画面中央を中心にそれぞれの四角形は整列したまま回転する
		Video.Rect(256 - 128 - 32, 256 - 128 - 32, 64, 64);
		Video.Rect(256 + 128 - 32, 256 - 128 - 32, 64, 64);
		Video.Rect(256 - 128 - 32, 256 + 128 - 32, 64, 64);
		Video.Rect(256 + 128 - 32, 256 + 128 - 32, 64, 64);

		// 図形の定義を終了
		Video.ClosePath();

		// 塗りつぶしで用いる色を 赤255, 緑0, 青0, 不透明度255 に設定
		Video.FillColor(Video.RGBA(255, 0, 0, 255));

		// 図形を描く
		Video.Fill();

	}

	// 処理が終ったので何もすることがない

}
