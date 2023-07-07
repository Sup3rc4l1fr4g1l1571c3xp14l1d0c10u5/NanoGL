#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <NanoGL.h>

// パズルの盤面の大きさ
#define PAZZLE_BOARD_WIDTH   512
#define PAZZLE_BOARD_HEIGHT  512

// スライドアニメの速度
#define SLIDE_SPEED_X (PAZZLE_BOARD_WIDTH / 32)	
#define SLIDE_SPEED_Y (PAZZLE_BOARD_HEIGHT / 32)	

// 一様乱数を生成
static double urandom(void) {
	return ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);
}

// 盤面上のタイルを管理する
//
// 以下のように盤面のマスには番号が振られている
// ┌─┬─┬─┬─┐
// │ 0│ 1│ 2│ 3│
// ├─┼─┼─┼─┤
// │ 4│ 5│ 6│ 7│
// ├─┼─┼─┼─┤
// │ 8│ 9│10│11│
// ├─┼─┼─┼─┤
// │12│13│14│15│
// └─┴─┴─┴─┘
// 9番のマスにあるタイルの数字を読み書きしたい場合は board[9] を読み書きする
static int board[16];

// タイルがマスの中心から上下左右にどれだけずれているかを管理する
// アニメーション処理で使う
static int board_x[16];	// 値が 0 の場合は 左右どちらにもずれていない　負の場合はタイルは左にずれている、正の場合はタイルは右にずれている
static int board_y[16];	// 値が 0 の場合は 上下どちらにもずれていない　負の場合はタイルは上にずれている、正の場合はタイルは下にずれている

// タイルの画像のIDと幅、高さ
static int imageId;
static int imageWidth, imageHeight;

//
// 最初の盤面を作成する。
// ランダムに初期配置を作成すると解けない盤面となることがあるので
// 解けない盤面のときは解ける盤面になるよう操作する
// 
void init_board(void)
{

	// 盤面上に0から15(空き)までのタイルをセルの番号順に並べる
	// また、タイルのずれはなしとする。
	for (int i = 0; i < 16; i++) {
		board[i] = i;
		board_x[i] = 0;
		board_y[i] = 0;
	}

	// Fisher-Yates shuffleアルゴリズムでシャッフルを行う
	for (int i = 16; i > 1; --i) {
		int a = i - 1;
		int b = (int)floor(urandom() * (i + 1));
		int tmp = board[a];
		board[a] = board[b];
		board[b] = tmp;
	}

	// シャッフル後のタイルの配置が元の状態に戻る（つまり答えがあるか？）を判定
	// 置換を用いた解説は http://hp.vector.co.jp/authors/VA010128/math/puzzle/P15-1.html
	int sum = 0;
	for (int i = 0; i < 16; i++) {
		int base = board[i];
		if (base == 15) {
			sum += i / 4 + 1;
			continue;
		}
		else {
			for (int j = i + 1; j < 16; j++) {
				if (board[j] == 15) {
					continue;
				}
				sum += (base > board[j]) ? 1 : 0;
			}
		}
	}

	// 答えのないタイル配置の場合、タイルの配置を左右反転させると答えのある配置になることが知られている
	// (逆に答えのあるタイルの配置を左右反転させると解けなくなる)
	if ((sum % 2) == 1) {
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 2; x++) {
				int tmp = board[y * 4 + x];
				board[y * 4 + x] = board[y * 4 + (3 - x)];
				board[y * 4 + (3 - x)] = tmp;
			}
		}
	}
}

// 盤面を画面上に描画する
void draw_board(void)
{
	Video.BeginPath();
	Video.FillColor(Video.RGB(128, 128, 128));
	Video.Rect(0.0f, 0.0f, (float)PAZZLE_BOARD_WIDTH, (float)PAZZLE_BOARD_HEIGHT);
	Video.Fill();
	Video.ClosePath();

	for (int i = 0; i < 16; i++) {
		int v = board[i];

		int scx = v % 4;
		int scy = (int)floor(v / 4);
		int sx = (int)floor(scx * imageWidth / 4);
		int sy = (int)floor(scy * imageHeight / 4);
		int sw = (int)floor((scx + 1) * imageWidth / 4) - sx;
		int sh = (int)floor((scy + 1) * imageHeight / 4) - sy;

		int dcx = i % 4;
		int dcy = (int)floor(i / 4);
		int dx = (int)floor(dcx * PAZZLE_BOARD_WIDTH / 4);
		int dy = (int)floor(dcy * PAZZLE_BOARD_HEIGHT / 4);
		int dw = (int)floor((dcx + 1) * PAZZLE_BOARD_WIDTH / 4) - dx;
		int dh = (int)floor((dcy + 1) * PAZZLE_BOARD_HEIGHT / 4) - dy;

		if (v != 15) {
			Video.DrawImage(imageId, (float)sx, (float)sy, (float)sw, (float)sh, (float)dx + board_x[i], (float)dy + board_y[i], (float)dw, (float)dh, 0.0f, 1.0f);
		}
	}
}

// 盤面をクリックされたときにタイルを動かす
bool click_board(int x, int y)
{
	int cx = x * 4 / PAZZLE_BOARD_WIDTH;
	int cy = y * 4 / PAZZLE_BOARD_HEIGHT;
	if (cx < 0 || 4 <= cx || cy < 0 || 4 <= cy) {
		return false;
	}
	int blankCell = -1;
	for (int i = 0; i < 16; i++) {
		if (board[i] == 15) {
			blankCell = i;
			break;
		}
	}

	int blankCellX = blankCell % 4;
	int blankCellY = blankCell / 4;

	if (cx != blankCellX && cy == blankCellY) {
		// 空白セルと水平方向が一致するセルを移動
		if (cx > blankCellX) {
			// 右から左に移動
			for (int i = blankCellX; i < cx; i++) {
				board_x[blankCellY * 4 + i] = PAZZLE_BOARD_WIDTH / 4;
				board[blankCellY * 4 + i] = board[blankCellY * 4 + i + 1];
			}
			board[blankCellY * 4 + cx] = 15;
			return true;
		}
		else if (cx < blankCellX) {
			// 左から右に移動
			for (int i = blankCellX; i > cx; i--) {
				board_x[blankCellY * 4 + i] = -PAZZLE_BOARD_WIDTH / 4;
				board[blankCellY * 4 + i] = board[blankCellY * 4 + i - 1];
			}
			board[blankCellY * 4 + cx] = 15;
			return true;
		}
	}
	else if (cx == blankCellX && cy != blankCellY) {
		// 空白セルと垂直方向が一致するセルを移動
		if (cy > blankCellY) {
			// 下から上に移動
			for (int i = blankCellY; i < cy; i++) {
				board_y[i * 4 + blankCellX] = PAZZLE_BOARD_HEIGHT / 4;
				board[i * 4 + blankCellX] = board[(i + 1) * 4 + blankCellX];
			}
			board[cy * 4 + blankCellX] = 15;
			return true;
		}
		else if (cy < blankCellY) {
			// 上から下に移動
			for (int i = blankCellY; i > cy; i--) {
				board_y[i * 4 + blankCellX] = -PAZZLE_BOARD_HEIGHT / 4;
				board[i * 4 + blankCellX] = board[(i - 1) * 4 + blankCellX];
			}
			board[cy * 4 + blankCellX] = 15;
			return true;
		}
	}
	return false;
}

// 盤面が完成したか判定する
bool is_complete_board()
{
	for (int i = 0; i < 16; i++) {
		if (board[i] != i) {
			return false;
		}
	}
	return true;
}

// 盤面のアニメーションを行う
bool animate_board()
{
	// タイルのずれ量を０に近づける
	for (int i = 0; i < 16; i++) {
		if (board_x[i] > 0) {
			board_x[i] = max(board_x[i] - SLIDE_SPEED_X, 0);
		}
		else if (board_x[i] < 0) {
			board_x[i] = min(board_x[i] + SLIDE_SPEED_X, 0);
		}
		if (board_y[i] > 0) {
			board_y[i] = max(board_y[i] - SLIDE_SPEED_Y, 0);
		}
		else if (board_y[i] < 0) {
			board_y[i] = min(board_y[i] + SLIDE_SPEED_Y, 0);
		}
	}

	// すべてのずれ量が０になったらアニメーション完了
	for (int i = 0; i < 16; i++) {
		if (board_x[i] || board_y[i]) {
			return true;
		}
	}
	return false;
}

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 50-15Puzzle");

	// 効果音をロード
	Sound.ChannelCreate(0, "./Sound/beep.mp3");
	
	// TrueTypeフォントをロードして"ipa"という名前を割り当て
	Video.CreateFont("ipa", "./Font/IPAexfont00301/ipaexg.ttf");
	// 名前"ipa"を指定して描画に使用するフォントに指定
	Video.FontFace("ipa");

	// 画像ファイルを読み込み、サイズを取得
	imageId = Video.CreateImage("./Image/Image.png", IMAGE_RESIZE_NEAREST);
	Video.ImageSize(imageId, &imageWidth, &imageHeight);

	// 盤面作成
	init_board();

	// ボタン押下カウンタを用意して初期化
	int btn = 0;

	while (Video.Drawing())
	{
		// 盤面描画
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, 1.0f);

		Video.BeginPath();
		Video.FillColor(Video.RGBA(255, 255, 255, 128));
		Video.Rect(0, PAZZLE_BOARD_HEIGHT / 2 - 24 / 2, PAZZLE_BOARD_WIDTH, 24);
		Video.Fill();
		Video.ClosePath();

		// メッセージ表示
		Video.BeginPath();
		Video.FontSize(24);
		Video.FillColor(Video.RGB(0, 0, 0));
		Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);
		Video.Text( PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2, "Click To Start", NULL);
		Video.Fill();
		Video.ClosePath();

		// クリックされたら遷移
		if (Mouse.IsLeftButtonDown()) {
			btn++;
		}
		else
		{
			btn = 0;
		}
		if (btn == 1)
		{
			Sound.ChannelPlay(0,false);
			break;
		}

	}

	// ステージ開始前のアニメーション
	double start = Video.GetTime();
	while (Video.Drawing())
	{
		// 盤面描画
		draw_board();

		// 上に元画像を表示
		float diff = 1.0f - (float)min(1.0, Video.GetTime() - start);
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, diff);

		// メッセージ表示
		Video.BeginPath();
		Video.FillColor(Video.RGB(0, 255, 0));
		Video.FontSize(80);
		Video.Save();
		Video.Translate(PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2);
		Video.Scale(diff, diff);
		Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);
		Video.Text(0, 0, "Ready?", NULL);
		Video.Restore();
		Video.ClosePath();

		// 1秒経過したら遷移
		if (Video.GetTime() - start >= 1.0) {

			break;
		}

	}


	// ゲーム中
	start = Video.GetTime();
	int step = 0;
	while (Video.Drawing())
	{
		float diff = 1.0f - (float)min(1.0, Video.GetTime() - start);

		// 盤面描画
		draw_board();

		if (diff > 0)
		{
			// メッセージ表示
			Video.BeginPath();
			Video.FillColor(Video.RGBA(0, 255, 0, (uint8_t)(diff * 255)));
			Video.FontSize(80);
			Video.Save();
			Video.Translate(PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2);
			Video.Scale((1 - diff) + 1, (1 - diff) + 1);
			Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);
			Video.Text(0, 0, "Go!", NULL);
			Video.Restore();

			Video.ClosePath();
		}

		if (Mouse.IsLeftButtonDown()) {
			btn++;
		}
		else
		{
			btn = 0;
		}

		if (animate_board() == false)
		{
			if (is_complete_board())
			{
				break;
			}
			else if (btn == 1) {
				// クリックされたらパネルを動かす
				double xpos, ypos;
				Mouse.GetCursorPos(&xpos, &ypos);
				if (click_board((int)xpos, (int)ypos))
				{
					step++;
					Sound.ChannelPlay(0,false);
				}
			}
		}

	}

	// クリア画面前のアニメーション
	start = Video.GetTime();
	while (Video.Drawing())
	{
		// 盤面描画
		draw_board();

		// 上に元画像を表示
		float diff = (float)min(1.0, Video.GetTime() - start);
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, diff);

		// 1秒経過したら遷移
		if (Video.GetTime() - start >= 1.0) {
			break;
		}

	}

	// クリア画面前のアニメーション(2)
	start = Video.GetTime();
	while (Video.Drawing())
	{
		// 盤面描画
		draw_board();

		// 上に元画像を表示
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, 1.0);

		float diff = (float)min(1.0, Video.GetTime() - start);
		Video.BeginPath();
		Video.FillColor(Video.RGBA(255, 255, 255, 128 * diff));
		Video.Rect(0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT);
		Video.Fill();
		Video.ClosePath();

		// 2秒経過したら遷移
		if (Video.GetTime() - start >= 1.0) {

			break;
		}

	}

	// クリア画面
	while (Video.Drawing())
	{
		// 元画像を表示
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, 1.0f);

		Video.BeginPath();
		Video.FillColor(Video.RGBA(255, 255, 255, 128));
		Video.Rect(0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT);
		Video.Fill();
		Video.ClosePath();

		// メッセージ表示
		Video.BeginPath();
		Video.FillColor(Video.RGB(0, 0, 0));
		Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);

		Video.FontSize(48);
		Video.Text(PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2 - 50, "Complete!", NULL);

		Video.FontSize(24);
		Video.FormatText(PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2 + 50, "クリア手数 %d 回", step);

		Video.ClosePath();

		// クリックされたら遷移
		if (Mouse.IsLeftButtonDown()) {
			btn++;
		}
		else
		{
			btn = 0;
		}
		if (btn == 1) { break; }

	}

	// これ以上ないので終了
}
