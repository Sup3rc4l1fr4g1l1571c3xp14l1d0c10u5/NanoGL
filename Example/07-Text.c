#include <NanoGL.h>

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 07-Text");

	// 画面を消去した際の背景色を設定
	Video.SetClearColor(Video.RGB(128,128,255));

	// プログラムの置いてあるディレクトリを基準として
	// fontフォルダ内のIPAexfont00301フォルダからipaexg.ttfというフォントファイルを読み込み、"font"という名前を付ける
	Video.CreateFont("font", "./Font/IPAexfont00301/ipaexg.ttf");

	// 読み込んだフォントを現在利用するフォントに設定
	Video.FontFace("font");

	// 処理
	while (Video.Drawing()) {
		// プログラムを起動してからの経過時間を表示する

		// テキスト描画は図形ではないのでFillColor命令でテキスト色を指定してからテキスト描画命令を用いる
		Video.FontSize(48);								// フォントの大きさを高さ48ptに設定
		Video.FillColor(Video.RGBA(0, 0, 255, 255));	// テキストの色を緑色に設定
		Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);	// 基準位置に文字列の縦横中心がくるように設定
		Video.FormatText(512 / 2, 512 / 2, "現在 %3.3f 秒経過", Video.GetTime());

		Video.Fill();
	}

	// 処理が終ったので何もすることがない

}
