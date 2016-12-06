#include <NanoGL/NanoGL.h>

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 05-Keyboard");

	// 画面を消去した際の背景色を設定
	Video.SetClearColor(Video.RGB(128,128,255));

	// 画像中心座標を格納する変数を用意し、初期位置を画面中心とする
	float x = 512 / 2.0f;
	float y = 512 / 2.0f;
	// 画像の回転角度とサイズを入れる変数を用意する
	float degree = 0;
	float scale = 1;

	// プログラムの置いてあるディレクトリを基準として
	// image フォルダ内の Image.png というファイルを読み込み、画像IDを image	に設定する
	// 画像オプションは指定しないので 0 を入れる
	int image = Video.CreateImage("./image/Image.png", 0);

	// 画像サイズを取得して変数 width と 変数 height に入れる
	int width, height;
	Video.ImageSize(image, &width, &height);

	// 処理
	while (Video.Drawing()) {

		// キー入力を検出
		if (Keyboard.IsKeyDown(KBKEY_UP))
		{
			// 上キーが押されているので画面上方向にY座標を更新
			y -= 1.0f;
		}
		if (Keyboard.IsKeyDown(KBKEY_DOWN))
		{
			// 下キーが押されているので画面下方向にY座標を更新
			y += 1.0f;
		}
		if (Keyboard.IsKeyDown(KBKEY_LEFT))
		{
			// 左キーが押されているので画面左方向にX座標を更新
			x -= 1.0f;
		}
		if (Keyboard.IsKeyDown(KBKEY_RIGHT))
		{
			// 右キーが押されているので画面右方向にX座標を更新
			x += 1.0f;
		}
		if (Keyboard.IsKeyDown('Z'))
		{
			// Zキーが押されているので反時計回りに回転
			degree -= 1;
		}
		if (Keyboard.IsKeyDown('X'))
		{
			// Xキーが押されているので時計回りに回転
			degree += 1;
		}
		if (Keyboard.IsKeyDown('A'))
		{
			// Aキーが押されているので拡大する
			scale = min(scale + 0.1, 5);
		}
		if (Keyboard.IsKeyDown('S'))
		{
			// Xキーが押されているので縮小する
			scale = max(scale - 0.1, 0.1);
		}

		// 画像位置(x,y)に変形込みで描画する
		Video.BeginPath();

		Video.Reset();
		Video.Translate(x, y);
		Video.Rotate(Video.DegToRad(degree));
		Video.Scale(scale, scale);
		Video.DrawImage(image, 0, 0, width, height, -64, -64, 128, 128, 0, 1.0f);

		Video.ClosePath();
	}

	// 処理が終ったので何もすることがない

}
