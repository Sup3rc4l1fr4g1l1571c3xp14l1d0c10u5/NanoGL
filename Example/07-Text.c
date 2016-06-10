#include <NanoGL/NanoGL.h>

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 06-Mouse");

	// 処理
	while (Video.Drawing()) {
		// マウスのカーソル位置を取得
		double x, y;
		Mouse.GetCursorPos(&x, &y);

		// マウスのカーソル位置に四角形を表示する
		Video.BeginPath();
		Video.Rect(x - 25, y - 25, 50, 50);
		Video.ClosePath();

		// マウスの押下に応じて色を変える
		if (Mouse.IsLeftButtonDown()) {
			// 左ボタンが押されている
			Video.FillColor(Video.RGBA(255, 0, 0, 255));
		}
		else if (Mouse.IsRightButtonDown()) {
			// 右ボタンが押されている
			Video.FillColor(Video.RGBA(0, 0, 255, 255));
		}
		else if (Mouse.IsMiddleButtonDown()) {
			// 真ん中ボタンが押されている
			Video.FillColor(Video.RGBA(0, 255, 0, 255));
		}
		else
		{
			// なにもボタンが押されていない
			Video.FillColor(Video.RGBA(255, 255, 255, 255));
		}

		Video.Fill();
	}

	// 処理が終ったので何もすることがない

}
