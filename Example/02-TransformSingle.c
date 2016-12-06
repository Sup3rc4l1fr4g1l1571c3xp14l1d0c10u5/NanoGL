#include <NanoGL/NanoGL.h>

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 02-TransformSingle");

	// 画面を消去した際の背景色を設定
	Video.SetClearColor(Video.RGB(128,128,255));

	// 処理
	while (Video.Drawing()) {

		///////////////////////////////////////
		// 移動・拡縮・回転を使う
		///////////////////////////////////////

		//
		// 画面中央(256, 256)が中心で幅80,高さ80の正方形を拡大縮小させながら中心で回転させて描く
		// 

		//
		// 手順は「変形方法を設定」→「図形を描画」で行う
		//

		// 変形方法の設定
		// 
		// 変形はすべて原点座標中心で行われるため、図形を変形させるのは
		// 
		//   1. 回転の中心位置が原点座標になるように移動
		//   2. 拡縮・回転などの変形処理を実行する
		//   3. 1で行った移動を打ち消す移動を適用
		// 
		// という流れで行う.今回の場合は
		// 
		// 1. 正方形の中心が原点座標に来るように移動させる
		//    → 横に-256、縦に-256の平行移動を適用する
		//    → Video.Translate(-256, -256);	
		// 
		// 2. 拡大・縮小を行う
		//    → Video.Scale(横方向拡大率, 縦方向拡大率);
		// 
		// 3. 回転させる
		//    → Video.Rotate(Video.DegToRad(角度(0〜360)));
		// 
		// 4. 正方形の中心が元の位置に戻るように移動させる
		//    → 横に256、縦に256の平行移動を適用する
		//    → Video.Translate(256, 256);	
		//
		// となる。
		// 
		// ！！！！！重要事項！！！！！
		// 
		// 変形処理は数学における行列の掛け算なので交換法則が成り立ちません（Ａ×ＢとＢ×Ａでは結果が違う）
		// このライブラリでは行列の持ち方の関係で上記の １→２→３→４ を行う場合、には４→３→２→１の順で行う必要があります。

		// 図形の定義を開始
		Video.BeginPath();

		Video.ResetTransform();				// 変形のリセット

		// ３秒で１回転するように現在の回転角度をプログラム起動からの経過時間で算出
		double degree = 360.0 * Video.GetTime() / 3.0;

		// 角度に合わせて大きさが変化するようにsin関数でスケールを算出
		double scale = sin(Video.DegToRad(degree)) * 0.5 + 1.0;

		Video.Translate(256, 256);				// 4の処理
		Video.Rotate(Video.DegToRad(degree));	// 3の処理
		Video.Scale(scale, scale);				// 2の処理
		Video.Translate(-256, -256);			// 1の処理

		// 画面中央に正方形を配置する。
		// 上で指定した変形が適用されるので、で実際には画面中央に回転した四角形が描かれる
		Video.Rect(256 - 64, 256 - 64, 128, 128);

		// 図形の定義を終了
		Video.ClosePath();

		// 塗りつぶしで用いる色を 赤255, 緑0, 青0, 不透明度255 に設定
		Video.FillColor(Video.RGBA(255, 0, 0, 255));

		// 図形を描く
		Video.Fill();

	}

	// 処理が終ったので何もすることがない

}
