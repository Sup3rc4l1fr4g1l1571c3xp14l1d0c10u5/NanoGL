#include <NanoGL/NanoGL.h>

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 09-Sound");

	// プログラムの置いてあるディレクトリを基準として
	// Sound フォルダ内の beam-gun01.mp3 というファイルを音源番号0番に読み込む
	Sound.ChannelCreate(0, "./Sound/beam-gun01.mp3");

	// プログラムの置いてあるディレクトリを基準として
	// Sound フォルダ内の game_maoudamashii_5_town26.mp3 というファイルを音源番号1番に読み込む
	Sound.ChannelCreate(1, "./Sound/game_maoudamashii_5_town26.mp3");

	// BGMを再生
	// ループ再生させるのでループフラグは true とする
	Sound.ChannelPlay(1, true);

	// ひとつ前の時点でのマウスボタンの状態を入れておく変数を用意
	bool IsLeftButtonDowned = false;

	// 処理
	while (Video.Drawing()) {

		// マウスがクリックされた（押し下げられた瞬間）だけ効果音を鳴らす
		if (Mouse.IsLeftButtonDown() && IsLeftButtonDowned == false)
		{
			// 効果音を再生
			// ループさせないのでループフラグは false とする
			Sound.ChannelPlay(0, false);
		}
		IsLeftButtonDowned = Mouse.IsLeftButtonDown();

	}

	// 処理が終ったので何もすることがない

}
