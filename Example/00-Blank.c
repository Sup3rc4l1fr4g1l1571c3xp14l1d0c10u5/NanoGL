#include <NanoGL/NanoGL.h>

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 00-Blank");

	// 処理
	while (Video.Drawing()) {
	}
	
	// 処理が終ったので何もすることがない

}
