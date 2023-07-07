#include <NanoGL.h>

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 11-Dialog");

	// 画面を消去した際の背景色を設定
	Video.SetClearColor(Video.RGB(128,128,255));

	// プログラムの置いてあるディレクトリを基準として
	// Fontフォルダ内のIPAexfont00301フォルダからipaexg.ttfというフォントファイルを読み込み、"font"という名前を付ける
	Video.CreateFont("font", "./Font/IPAexfont00301/ipaexg.ttf");

	// 読み込んだフォントを現在利用するフォントに設定
	Video.FontFace("font");

	// ファイル名を入れる文字列
	string_t filename = String.Create(NULL);

	// 処理
	int lbdown = 0;
	int rbdown = 0;
	while (Video.Drawing()) {
		// マウス左クリックでファイルを開くダイアログボックスを表示
		if (Mouse.IsLeftButtonDown()) {
			lbdown++;
		} else {
			lbdown = 0;
		}
		if (lbdown == 1) {
			String.Free(filename);
			filename = Dialog.OpenFileDialog("ファイルを選んでください", "JPEG|*.jpg;*jpeg\nGIF|*.gif\nすべてのファイル|*.*");
		}
		// マウス右クリックでファイル保存ダイアログボックスを表示
		if (Mouse.IsRightButtonDown()) {
			rbdown++;
		} else {
			rbdown = 0;
		}
		if (rbdown == 1) {
			String.Free(filename);
			filename = Dialog.SaveFileDialog("保存先ファイル名を入力してください", "JPEG|*.jpg;*jpeg\nGIF|*.gif\nすべてのファイル|*.*");
		}

		// テキスト描画は図形ではないのでFillColor命令でテキスト色を指定してからテキスト描画命令を用いる
		Video.FontSize(12);								// フォントの大きさを高さ12ptに設定
		Video.FillColor(Video.RGBA(0, 0, 255, 255));	// テキストの色を緑色に設定
		Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);	// 基準位置に文字列の縦横中心がくるように設定
		if (filename.c_str != NULL) {
			Video.Text(512 / 2, 512 / 2, filename.c_str, NULL);
		}

		Video.Fill();
	}

	// 処理が終ったので何もすることがない

}
