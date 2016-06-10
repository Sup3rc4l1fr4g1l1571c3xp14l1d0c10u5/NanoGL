#include <NanoGL/NanoGL.h>

// ボールの情報を表現する構造体
struct ball
{
	float x;	// ボールのX座標
	float y;	// ボールのY座標
	float sx;	// ボールのX方向移動速度
	float sy;	// ボールのY方向移動速度
};

// ボール情報を1000個作る
struct ball balls[1000];

// ゲームのメイン処理
void start(void)
{
	// 画面サイズを縦横 512 ピクセル に 設定
	Video.SetSize(512, 512);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 08-Sprite");

	// プログラムの置いてあるディレクトリを基準として
	// image フォルダ内の ball.png というファイルを読み込み、画像IDを img_ballに設定する
	// 画像オプションは指定しないので 0 を入れる
	int img_ball = Video.CreateImage("./image/ball.png", 0);

	// 画像サイズを取得して変数 width と 変数 height に入れる
	int width, height;
	Video.ImageSize(img_ball, &width, &height);

	// ボールの位置と移動方向を設定する
	for (int i=0; i<1000; i++)
	{
		// 移動方向をラジアンで算出
		float rad = Video.DegToRad(360.0f * i / 1000);

		balls[i].x = 512 / 2;
		balls[i].y = 512 / 2;
		balls[i].sx = (float)cos(rad) * 2.0f;	// 移動方向を元に横方向移動量をcos関数で算出し、速度を乗算
		balls[i].sy = (float)sin(rad) * 2.0f;	// 移動方向を元に縦方向移動量をsin関数で算出し、速度を乗算
	}

	// 処理
	while (Video.Drawing()) {

		// ボールひとつづつ移動処理を行う
		for (int i = 0; i<1000; i++)
		{
			// ボールの現在位置に移動速度を加算して次の位置に移動
			balls[i].x = balls[i].x + balls[i].sx;
			balls[i].y = balls[i].y + balls[i].sy;

			//画面からはみ出そうとしているボールは符号を反転させることで移動方向を逆にして壁で反射したように見せる

			// 画面の左右からはみ出している場合
			if ((balls[i].x < 0) || (512 <= balls[i].x))
			{
				balls[i].sx = balls[i].sx * -1;
			}

			// 画面の上下からはみ出している場合
			if ((balls[i].y < 0) || (512 <= balls[i].y))
			{
				balls[i].sy = balls[i].sy * -1;
			}
		}


		// ボールひとつづつ描画する
		for (int i = 0; i < 1000; i++)
		{
			// ボールの位置にボールの画像の中心がくるように位置を調整して描画
			Video.DrawImage(img_ball, 0, 0, width, height, balls[i].x - width / 2, balls[i].y - height / 2, width, height, 0, 1.0f);
		}
	}

	// 処理が終ったので何もすることがない

}
