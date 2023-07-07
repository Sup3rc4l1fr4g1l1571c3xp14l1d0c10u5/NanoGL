#include <NanoGL.h>

////////////////////////////////////////////////////////////////////////////////
// 画面サイズの設定
////////////////////////////////////////////////////////////////////////////////
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 480

#define SCREEN_LEFT   0
#define SCREEN_RIGHT  (SCREEN_LEFT + SCREEN_WIDTH)
#define SCREEN_TOP    0
#define SCREEN_BOTTOM (SCREEN_TOP + SCREEN_HEIGHT)

////////////////////////////////////////////////////////////////////////////////
// 出現数上限の設定
////////////////////////////////////////////////////////////////////////////////
#define MAX_ENEMY		256		// 敵・敵弾合わせての最大出現数
#define MAX_SHOT		32		// プレイヤーの弾の最大出現数
#define MAX_PARTICLE	256		// パーティクルの最大出現数
#define MAX_HITAREA		3		// 敵が持つことができる当たり判定の最大数

////////////////////////////////////////////////////////////////////////////////
// 小型の敵からランダムに選んで球を打たせる「ランダム攻撃」の攻撃間隔などの設定で用いる変数
////////////////////////////////////////////////////////////////////////////////
static struct
{
	float	shot_speed;			// 弾速
	int		time;				// 最後の攻撃からの経過時間
	int		time_next;			// 次の攻撃を行う時間
	int		max_enemy;			// ランダム攻撃で選択可能なな敵の数
} random_fire;

static void reset_random_fire()
{
	random_fire.shot_speed = 2;
	random_fire.time = 0;
	random_fire.time_next = 60;
	random_fire.max_enemy = 0;
}

////////////////////////////////////////////////////////////////////////////////
// 読み込んだ画像のIDを入れる変数
////////////////////////////////////////////////////////////////////////////////
static int img_player;
static int img_shot_l;
static int img_shot_r;
static int img_enemy_shot;
static int img_enemy_s0;
static int img_enemy_s1;
static int img_enemy_m0;

////////////////////////////////////////////////////////////////////////////////
// 座標(x,y)が画面境界からpピクセル以上外側にあるか判定。
////////////////////////////////////////////////////////////////////////////////
static bool IsOutOfScreen(int x, int y, int p) {
	return (x < SCREEN_LEFT - p || SCREEN_RIGHT + p <= x || y < SCREEN_TOP - p || SCREEN_BOTTOM + p <= y);
}

////////////////////////////////////////////////////////////////////////////////
// 敵と敵弾を表現するデータ
// 最大 MAX_ENEMY 個
////////////////////////////////////////////////////////////////////////////////
static struct Enemy {
	bool use;	  // 使用中フラグ
	bool enemy;   // 敵として使われている場合はtrue
	float x;	  // X座標
	float y;	  // Y座標
	float s;	  // 移動速度
	float d;	  // 移動方向
	int   life;   // ライフ値
	void(*action)(struct Enemy *); // 動作
	int   time;   // 実行時間
	int   image;  // 画像番号
	int   score; // 得点
	bool	randomfire;	// ランダム砲火が可能か
	struct {
		float r; // 当たり判定の半径(0は未使用)
		float x, y; // 当たり判定の中心
	} cr[MAX_HITAREA];
} enemies[MAX_ENEMY];

// 現時点での敵の生存数
static int alive_enemy = 0;

// 未使用の敵データのIDを得る
static int find_free_enemy() {
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemies[i].use == false) { return i; }
	}
	return -1;
}

// 全ての敵・敵弾を「未使用」にし、敵の生存数も０にする
static void reset_enemies()
{
	alive_enemy = 0;
	for (int i = 0; i < MAX_ENEMY; i++) {
		enemies[i].use = false;
	}
}

// 敵をすべて描画する
static void draw_enemies()
{
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemies[i].use == false) { continue; }

		// 敵画像のサイズを得る
		int w, h;
		Video.ImageSize(enemies[i].image, &w, &h);


		// 念のため変形をリセット
		Video.ResetTransform();	

		// 変形処理を設定
		// （プログラム上では移動→回転の順序になっているが行列の掛け算なので演算結果は回転して移動となることに注意）

		// 表示したい座標への移動
		Video.Translate(enemies[i].x, enemies[i].y);
		// 回転を適用
		Video.Rotate(enemies[i].d - Video.DegToRad(90));

		// 画像を描画
		Video.DrawImage(enemies[i].image, 0, 0, w, h, -w / 2.0f, -h / 2.0f, w, h, 0, 1.0f);
		
		// 変形が終わったので変形をリセット
		Video.ResetTransform();
	}
}

////////////////////////////////////////////////////////////////////////////////
// プレイヤーの弾を管理する
// 最大 MAX_SHOT 個まで使える
////////////////////////////////////////////////////////////////////////////////
static struct Shot {
	bool use;	  // 使用中フラグ
	float x;	  // X座標
	float y;	  // Y座標
	float s;	  // 移動速度
	float d;	  // 移動方向
	float r;	  // 当り判定の半径
	int   image;  // 画像番号
} shots[MAX_SHOT];

// 未使用のプレイヤーの弾のIDを得る
static int find_free_shot() {
	for (int i = 0; i < MAX_SHOT; i++) {
		if (shots[i].use == false) { return i; }
	}
	return -1;
}

// 全ての弾を「未使用」にする
static void reset_shots()
{
	for (int i = 0; i < MAX_SHOT; i++) {
		shots[i].use = false;
	}
}

// プレイヤーの弾を移動させる
static void move_shots()
{
	for (int i = 0; i < MAX_SHOT; i++)
	{
		if (shots[i].use == false) { continue; }
		if (IsOutOfScreen(shots[i].x, shots[i].y, 16)) { shots[i].use = false; continue; }
		shots[i].x += cos(shots[i].d) * shots[i].s;
		shots[i].y += sin(shots[i].d) * shots[i].s;
	}
}

// プレイヤーの弾を描画する
static void draw_shots()
{
	for (int i = 0; i < MAX_SHOT; i++)
	{
		if (shots[i].use == false) { continue; }
		Video.ResetTransform();
		Video.Translate(shots[i].x, shots[i].y);
		Video.Rotate(shots[i].d + Video.DegToRad(90));
		int w, h;
		Video.ImageSize(shots[i].image, &w, &h);
		Video.DrawImage(shots[i].image, 0, 0, w, h, -w / 2.0f, -h / 2.0f, w, h, 0, 1.0f);
	}
	Video.ResetTransform();

}

////////////////////////////////////////////////////////////////////////////////
// プレイヤーを管理する
////////////////////////////////////////////////////////////////////////////////
static struct player {
	bool alive;	  // 生存フラグ
	float x;	  // X座標
	float y;	  // Y座標
	float r;	  // 当り判定の半径
	int reload;
} player;

static void reset_player()
{
	player.alive = true;
	player.x = SCREEN_LEFT + SCREEN_WIDTH  / 2;
	player.y = SCREEN_TOP  + (SCREEN_HEIGHT * 9) / 10;
	player.reload = 0;
}

// プレイヤーの動作を行う
static void move_player()
{
	// プレイヤーが生きているならプレイヤーの操作を反映させる
	if (player.alive) {

		// 十字キーの入力に応じて移動させる
		if (Keyboard.IsKeyDown(KBKEY_DOWN)) {
			player.y += 3;
		}
		if (Keyboard.IsKeyDown(KBKEY_UP)) {
			player.y -= 3;
		}
		if (Keyboard.IsKeyDown(KBKEY_LEFT)) {
			player.x -= 3;
		}
		if (Keyboard.IsKeyDown(KBKEY_RIGHT)) {
			player.x += 3;
		}

		// 移動した結果画面外に出た場合は画面内に戻す
		if (player.x < SCREEN_LEFT  ) { player.x = SCREEN_LEFT;   }
		if (player.x > SCREEN_RIGHT ) { player.x = SCREEN_RIGHT;  }
		if (player.y < SCREEN_TOP   ) { player.y = SCREEN_TOP;    }
		if (player.y > SCREEN_BOTTOM) { player.y = SCREEN_BOTTOM; }

		// 攻撃のリロードを管理する
		if (player.reload == 0) {
			// リロードが終わっている場合、'z' キーを押すと弾を撃つ
			if (Keyboard.IsKeyDown('Z')) {

				// 未使用の弾を探してパラメータを設定する

				// 左側の弾
				int i = find_free_shot();
				if (i != -1) {
					shots[i].use = true;
					shots[i].x = player.x - 8;
					shots[i].y = player.y;
					shots[i].d = Video.DegToRad(-90);
					shots[i].s = 12;
					shots[i].r = 12;
					shots[i].image = img_shot_l;
				}

				// 右側の弾
				int j = find_free_shot();
				if (j != -1) {
					shots[j].use = true;
					shots[j].x = player.x + 8;
					shots[j].y = player.y;
					shots[j].d = Video.DegToRad(-90);
					shots[j].s = 12;
					shots[j].r = 12;
					shots[j].image = img_shot_r;
				}
				player.reload = 4;
			}
		}
		else
		{
			// リロードが終わっていないのでリロード時間を１減らす
			player.reload -= 1;
		}

	}
}

static void draw_player()
{
	// プレイヤーが生きていれば描画する
	if (player.alive) {
		int w, h;
		Video.ImageSize(img_player, &w, &h);
		Video.DrawImage(img_player, 0, 0, w, h, player.x - w / 2.0f, player.y - h / 2.0f, w, h, 0, 1.0f);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 演出用のパーティクルを管理する
// 最大２５６個まで使える
////////////////////////////////////////////////////////////////////////////////
static struct particle {
	bool use;	  // 使用中フラグ
	float x;	  // X座標
	float y;	  // Y座標
	float s;	  // 移動速度
	float d;	  // 移動方向
	float r;	  // 半径
	Color c;	 // 色
	int   ttl;	// 消滅までの時間
	int   time;	// 生存時間
} particles[MAX_PARTICLE];

// パーティクルの色を決定するために、HSVカラー指定方法からRGBカラーを作る
static Color FromHsv(float h, float s, float v) {
	float r = v;
	float g = v;
	float b = v;
	if (s > 0.0) {
		h *= 6.0f;
		int i = (int)h;
		float f = h - i;
		switch (i)
		{
		default:
			g *= 1 - s * (1 - f);
			b *= 1 - s;
			break;
		case 1:
			r *= 1 - s * f;
			b *= 1 - s;
			break;
		case 2:
			r *= 1 - s;
			b *= 1 - s * (1 - f);
			break;
		case 3:
			r *= 1 - s;
			g *= 1 - s * f;
			break;
		case 4:
			r *= 1 - s * (1 - f);
			g *= 1 - s;
			break;
		case 5:
			g *= 1 - s;
			b *= 1 - s * f;
			break;
		}
	}
	return Video.RGB((int)(r * 255), (int)(g * 255), (int)(b * 255));
}

// 全パーティクルを「未使用」にする
static void reset_particles() {
	for (int i = 0; i < MAX_PARTICLE; i++) {
		particles[i].use = false;
	}
}
// パーティクルをひとつ作る
static void create_particle(float x, float y, float s, float d, float r, Color c, int ttl) {
	for (int i = 0; i < MAX_PARTICLE; i++) {
		if (particles[i].use == true) { continue; }
		particles[i].x = x;
		particles[i].y = y;
		particles[i].s = s;
		particles[i].d = d;
		particles[i].c = c;
		particles[i].r = r;
		particles[i].time = 0;
		particles[i].ttl = ttl;
		particles[i].use = true;
		return;
	}
}

// パーティクルを動かす
static void move_particles()
{
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particles[i].use == false) { continue; }
		particles[i].time += 1;
		if (particles[i].time == particles[i].ttl)
		{
			particles[i].use = false;
			continue;
		}
		particles[i].x += cos(particles[i].d) * particles[i].s;
		particles[i].y += sin(particles[i].d) * particles[i].s;
	}
}

static void draw_particles()
{
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particles[i].use == false) { continue; }
		Video.BeginPath();
		Video.Circle(particles[i].x, particles[i].y, particles[i].r * (1.0 - particles[i].time*1.0 / particles[i].ttl));
		Video.FillColor(particles[i].c);
		Video.Fill();
		Video.ClosePath();
	}

}

////////////////////////////////////////////////////////////////////////////////
// 敵の弾の動作（１）：直進する敵弾の動作
////////////////////////////////////////////////////////////////////////////////
static void action_b1(struct Enemy *e)
{
	e->x += cos(e->d) * e->s;
	e->y += sin(e->d) * e->s;
}

////////////////////////////////////////////////////////////////////////////////
// 敵弾を作る
////////////////////////////////////////////////////////////////////////////////
static void create_enemy_burret(float x, float y, float s, float d) {
	int i = find_free_enemy();
	if (i == -1) { return; }
	enemies[i].use = true;
	enemies[i].enemy = false;
	enemies[i].x = x;
	enemies[i].y = y;
	enemies[i].s = s;
	enemies[i].d = d;
	enemies[i].life = 1;
	enemies[i].action = action_b1;
	enemies[i].time = 0;
	enemies[i].image = img_enemy_shot;
	enemies[i].cr[0].r = 10;
	enemies[i].cr[0].x = 0;
	enemies[i].cr[0].y = 0;
	enemies[i].cr[1].r = 0;
	enemies[i].cr[2].r = 0;
	enemies[i].randomfire = false;

}

////////////////////////////////////////////////////////////////////////////////
// 小型の敵１を作る
////////////////////////////////////////////////////////////////////////////////
static void create_enemy_s0(float x, float y, float s, float d, void(*action)(struct Enemy*)) {
	int i = find_free_enemy();
	if (i == -1) { return; }
	enemies[i].use = true;
	enemies[i].enemy = true;
	enemies[i].x = x;
	enemies[i].y = y;
	enemies[i].s = s;
	enemies[i].d = d;
	enemies[i].life = 1;
	enemies[i].score = 9;
	enemies[i].action = action;
	enemies[i].time = 0;
	enemies[i].image = img_enemy_s0;
	enemies[i].cr[0].r = 14;
	enemies[i].cr[0].x = 0;
	enemies[i].cr[0].y = 0;
	enemies[i].cr[1].r = 0;
	enemies[i].cr[2].r = 0;
	enemies[i].randomfire = true;

}

////////////////////////////////////////////////////////////////////////////////
// 小型の敵２を作る
////////////////////////////////////////////////////////////////////////////////
static void create_enemy_s1(float x, float y, float s, float d, void(*action)(struct Enemy*)) {
	int i = find_free_enemy();
	if (i == -1) { return; }
	enemies[i].use = true;
	enemies[i].enemy = true;
	enemies[i].x = x;
	enemies[i].y = y;
	enemies[i].s = s;
	enemies[i].d = d;
	enemies[i].life = 7;
	enemies[i].score = 63;
	enemies[i].action = action;
	enemies[i].time = 0;
	enemies[i].image = img_enemy_s1;
	enemies[i].cr[0].r = 14;
	enemies[i].cr[0].x = 0;
	enemies[i].cr[0].y = 0;
	enemies[i].cr[1].r = 0;
	enemies[i].cr[2].r = 0;
	enemies[i].randomfire = true;

}

////////////////////////////////////////////////////////////////////////////////
// 敵の動作：設定された速度と方向で直進
////////////////////////////////////////////////////////////////////////////////
static void action_s0(struct Enemy *e)
{
	e->x += cos(e->d) * e->s;
	e->y += sin(e->d) * e->s;
}

////////////////////////////////////////////////////////////////////////////////
// 敵の動作：画面下に向かって左右にふらふら揺れながら直進
////////////////////////////////////////////////////////////////////////////////
static void action_s1(struct Enemy *e)
{
	e->x += cos(Video.DegToRad(e->time % 120 * 360 / 120)) * 2;
	e->y += e->s;
}

////////////////////////////////////////////////////////////////////////////////
// 敵の動作：設定された速度と方向で直進するが徐々に加速する
////////////////////////////////////////////////////////////////////////////////
static void action_s2(struct Enemy *e) {
	e->x += cos(e->d) * e->s;
	e->y += sin(e->d) * e->s;
	e->s *= 1.03;
}

////////////////////////////////////////////////////////////////////////////////
// 敵の動作：画面下に向かって直進してくるが、プレイヤーと横軸が合うと停止し、プレイヤーの向きに曲がってから加速する
////////////////////////////////////////////////////////////////////////////////
static void action_s3(struct Enemy *e) {
	e->y += e->s;
	if (e->y - 5 <= player.y && player.y <= e->y + 5) {
		e->s = 0.5;
		if (player.x > e->x)
		{
			e->d = Video.DegToRad(0);
		} else
		{
			e->d = Video.DegToRad(180);
		}
		e->action = action_s2;	//直進動作に切り替え
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵の動作：プレイヤーの方向に旋回しながら追尾する。距離が近くなると追尾を終了する
////////////////////////////////////////////////////////////////////////////////
static void action_s4(struct Enemy *e) {
	e->x += cos(e->d) * e->s;
	e->y += sin(e->d) * e->s;
	if (player.alive) {
		double d = (player.y - e->y) * (player.y - e->y) + (player.x - e->x) * (player.x - e->x);
		if (d < 60 * 60)
		{
			// 追尾終了
			//直進動作に切り替え
			e->action = action_s0;
		}
		else {
			double r = atan2(player.y - e->y, player.x - e->x);
			double vx = cos(e->d) * e->s;
			double vy = sin(e->d) * e->s;
			double tx = (player.x - e->x);
			double ty = (player.y - e->y);
			double s = vx * ty - vy * tx;
			double c = (vx * tx + vy * ty) / (sqrt(vx*vx + vy*vy) * sqrt(tx*tx + ty*ty));
			if (c > cos(Video.DegToRad(1.5)))
			{
				e->d = r;
			} else
			{
				e->d += (s > 0 ? 1 : -1)*Video.DegToRad(1.5);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 中型の敵を作る
////////////////////////////////////////////////////////////////////////////////
static void create_enemy_m0(float x, float y, float s, float d, void(*action)(struct Enemy*)) {
	int i = find_free_enemy();
	if (i == -1) { return; }
	enemies[i].use = true;
	enemies[i].enemy = true;
	enemies[i].x = x;
	enemies[i].y = y;
	enemies[i].s = s;
	enemies[i].d = d;
	enemies[i].life = 100;
	enemies[i].score = 900;
	enemies[i].action = action;
	enemies[i].time = 0;
	enemies[i].image = img_enemy_m0;
	enemies[i].cr[0].r = 32;
	enemies[i].cr[0].x = 0;
	enemies[i].cr[0].y = 0;
	enemies[i].cr[1].r = 24;
	enemies[i].cr[1].x = -48;
	enemies[i].cr[1].y = 0;
	enemies[i].cr[2].r = 24;
	enemies[i].cr[2].x = 48;
	enemies[i].cr[2].y = 0;
	enemies[i].randomfire = false;
}

////////////////////////////////////////////////////////////////////////////////
// 中型の敵の動作：
// 翼から真下に向かって角度固定の３方向に弾を撃つ
// 胴体からプレイヤーを狙う全方向弾を打つ
////////////////////////////////////////////////////////////////////////////////
static void action_m1(struct Enemy *e) {
	if (e->time < 40)
	{
		e->x += cos(e->d) * (40-e->time)/40 * e->s;
		e->y += sin(e->d) * (40 - e->time) / 40 * e->s;
	} else if (40 <= e->time && e->time < 340)
	{
		e->d = Video.DegToRad(90);
		e->x += cos(e->d) * 0.2;
		e->y += sin(e->d) * 0.2;

		int st = e->time - 40;
		if (st % 80 == 30 || st % 80 == 32 || st % 80 == 34 || st % 80 == 36 || st % 80 == 38 || st % 80 == 40)
		{
			float s = ((st % 80) - 30) * 0.1;
			for (int i = -1; i <= 1; i++) {
				create_enemy_burret(e->x - 51, e->y + 4, 2 + s + random_fire.shot_speed / 8, Video.DegToRad(90 + 20 * i));
				create_enemy_burret(e->x + 51, e->y + 4, 2 + s + random_fire.shot_speed / 8, Video.DegToRad(90 + 20 * i));
			}
		}
		if (st % 60 == 30)
		{
			float d = atan2(player.y - e->y, player.x - e->x);
			for (int i = 0; i <= 8; i++) {
				for (int j = 0; j <= 2; j++) {
					create_enemy_burret(e->x, e->y + 4, 3 + j * 0.2 + random_fire.shot_speed / 4, d + Video.DegToRad(360 * i / 8));
				}
			}
		}
	} else if (340 <= e->time)
	{
		e->d = Video.DegToRad(90);
		e->x += cos(e->d) * min(e->time - 340, 60) / 60 * -e->s;
		e->y += sin(e->d) * min(e->time - 340, 60) / 60 * -e->s;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現を管理する処理で使う変数
////////////////////////////////////////////////////////////////////////////////
static struct
{
	int  time;
	int  work;
	bool enable;
	int  action;
} spawn;

// 出現処理をリセットする
static void reset_spawn()
{
	spawn.time = 0;
	spawn.work = 0;
	spawn.enable = true;
	spawn.action = 0;
}

////////////////////////////////////////////////////////////////////////////////
// 敵の動作：登場後は静止してプレイヤーを狙う全方向弾を打つ。一定時間後に加速しながら突進する。
////////////////////////////////////////////////////////////////////////////////
static void action_s5(struct Enemy *e) {
	if (e->time < 40)
	{
		e->randomfire = false;
		e->x += cos(e->d) * (40 - e->time) / 40 * e->s;
		e->y += sin(e->d) * (40 - e->time) / 40 * e->s;
	} else if (e->time >= 40 && e->time < 280) {
		e->d = atan2(player.y - e->y, player.x - e->x);
		if ((e->time - 40) % 60 == 0) {
			for (int i = 0; i <= 8; i++) {
				create_enemy_burret(e->x, e->y + 4, 2 + random_fire.shot_speed / 4, e->d + Video.DegToRad(360 * i / 8));
			}
		}
	}
	else if (280 <= e->time)
	{
		e->s = 3;
		e->action = action_s4;
		e->randomfire = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：0.5秒なにもしない
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s0(void)
{
	// 0.5秒待つ
	if (spawn.time == 30) {
		spawn.enable = true;
	}
}


////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：画面上から画面下に向かって直進してくる敵を0.33秒間隔で１０体出現させる
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s1(void)
{
	// 
	if ((spawn.time % 20) == 0) {
		spawn.work += ((rand() % 13) + 5) * 11;
		create_enemy_s0((spawn.work % 240) + 40, -10, 4, Video.DegToRad(90), action_s0);
	}
	if (spawn.time == 20 * 10 - 1) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：画面上から画面下に向かって左右に揺れながら直進してくる敵を0.33秒間隔で１０体出現させる
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s2(void)
{
	if ((spawn.time % 20) == 0) {
		spawn.work += ((rand() % 19) + 5) * 11;
		create_enemy_s0((spawn.work % 220) + 50, -10, 2.5, Video.DegToRad(90), action_s1);
	}
	if (spawn.time == 20 * 20 - 1) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：画面左右からプレイヤーを追尾する敵を出現させる
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s3(void)
{
	if ((spawn.time % 10 == 0)) {
		int i = 4 - spawn.time / 10;
		create_enemy_s0(SCREEN_LEFT  - 10, i * 60, 3, Video.DegToRad(  0), action_s4);
		create_enemy_s0(SCREEN_RIGHT + 10, i * 60, 3, Video.DegToRad(180), action_s4);
	}
	if (spawn.time == 40-1) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：画面上部を左右に横断する敵を1/6秒間隔で作る
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s4(void)
{
	if (spawn.time % 10 == 0) {
		for (int i = 0; i < 3; i++) {
			create_enemy_s0(SCREEN_LEFT  - 10, 100 * i + 50, 8, Video.DegToRad(  0), action_s0);
			create_enemy_s0(SCREEN_RIGHT + 10, 100 * i     , 8, Video.DegToRad(180), action_s0);
		}
	}
	if (spawn.time == 100) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：画面両脇を上下に横断する敵を1/6秒間隔で作る
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s5(void)
{
	// 上下に横切る敵を出す
	if (spawn.time % 10 == 0) {
		create_enemy_s0(SCREEN_LEFT  + 25 * 3, SCREEN_TOP    - 10, 8, Video.DegToRad( 90), action_s0);
		create_enemy_s0(SCREEN_LEFT  + 25 * 1, SCREEN_BOTTOM + 10, 8, Video.DegToRad(-90), action_s0);
		create_enemy_s0(SCREEN_RIGHT - 25 * 1, SCREEN_BOTTOM + 10, 8, Video.DegToRad(-90), action_s0);
		create_enemy_s0(SCREEN_RIGHT - 25 * 3, SCREEN_TOP    - 10, 8, Video.DegToRad( 90), action_s0);
	}
	if (spawn.time == 100) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：
// 画面下に向かって直進し、プレイヤーと横軸が合うと停止し、曲がって加速する敵を
// 上から連続で降らせる
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s6(void)
{
	if ((spawn.time % 20) == 0) {
		spawn.work += ((rand() % 13) + 5) * 11;
		int x = SCREEN_LEFT + spawn.work % (SCREEN_WIDTH - 80) + 40;
		create_enemy_s0(  x, -10, 6, Video.DegToRad(90), action_s3);
	}
	if (spawn.time == 20 * 10 - 1) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：
// 中型の敵を上から出現させる
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_m0(void)
{
	if (spawn.time == 0) {
		spawn.work += ((rand() % 13) + 5) * 11;
		int x = SCREEN_LEFT + spawn.work % (SCREEN_WIDTH - 120) + 60;
		create_enemy_m0(x, -10, 4, Video.DegToRad(90), action_m1);
	}
	if (spawn.time == 20 * 10 - 1) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：
// 画面上から直進する敵とプレイヤーを追尾する敵を交互に出現させる
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s7(void)
{
	if ((spawn.time % 20) == 0) {
		spawn.work += ((rand() % 13) + 5) * 11;
		int x = SCREEN_LEFT + spawn.work % (SCREEN_WIDTH - 80) + 40;
		create_enemy_s0(x, -10, 4, Video.DegToRad(90), action_s0);
	}
	if ((spawn.time % 20) == 10) {
		spawn.work += ((rand() % 13) + 5) * 11;
		int x = SCREEN_LEFT + spawn.work % (SCREEN_WIDTH - 80) + 40;
		create_enemy_s0(x, -10, 5, Video.DegToRad(90), action_s4);
	}
	if (spawn.time == 30 * 10 - 1) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作：
// 静止して弾を打つ敵を出現させてから左右に揺れ画なら直進してくる敵を連続で降らせる
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s8(void)
{
	if (spawn.time % 10 == 0 && spawn.time / 10 <  3) {
		int i = spawn.time / 10;
		create_enemy_s1( SCREEN_LEFT -10, 0 + i * 60 + 30, 5 - i * 0.6, Video.DegToRad(  0+20), action_s5);
		create_enemy_s1( SCREEN_RIGHT+10, 0 + i * 60 + 30, 5 - i * 0.6, Video.DegToRad(180-20), action_s5);
	}
	if ((spawn.time % 20) == 0) {
		spawn.work += ((rand() % 13) + 5) * 11;
		int x = SCREEN_LEFT + spawn.work % (SCREEN_WIDTH - 80) + 40;
		create_enemy_s0(x, -10, 2.5, Video.DegToRad(90), action_s1);
	}
	if (spawn.time == 20 * 10 - 1) {
		spawn.enable = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 敵グループの出現動作を並べた表
// 関数ポインタという技法を使っているので理解するための難易度はやや高い
////////////////////////////////////////////////////////////////////////////////
static void(*spawn_table[])(void) = {
	spawn_enemy_s0,	// 0番目は使われない
	spawn_enemy_s1,
	spawn_enemy_s2,
	spawn_enemy_s0,
	spawn_enemy_s3,
	spawn_enemy_s0,
	spawn_enemy_s4,
	spawn_enemy_s0,
	spawn_enemy_s5,
	spawn_enemy_s6,
	spawn_enemy_s0,
	spawn_enemy_m0,	// 中型機出現
	spawn_enemy_s7,
	spawn_enemy_s8,
	NULL			// 終端を意味する NULL を入れておく
};

// 敵グループの出現を処理する
static void spawn_enemy()
{
	// 敵がいなくて、敵グループを作ってもいい場合は敵の出現テーブルを読み進めて実行するように設定する
	if (alive_enemy == 0 && spawn.enable == true) {
		spawn.action += 1;	// 出現テーブルで稼働させる要素の番号を+1する
		spawn.time = 0;		// 出現テーブルの動作時間を0にリセット
		spawn.enable = false;	// 敵の出現処理側から許可が出るまで敵の出現処理を更新しないようにする
		spawn.work = 0;			// 敵の出現処理内で使う用途を定めない変数を一応リセットしておく
	}

	if (spawn.enable == false) {
		// 敵の出現テーブルが終端まで読み進められていたら先頭に戻す。
		if (spawn_table[spawn.action] == NULL) {
			spawn.action = 0;
		}
		// 出現動作を実行して経過時間を進める
		spawn_table[spawn.action]();
		spawn.time += 1;
	}
}

// 敵の動作を行う
static void move_enemies() {
	// 生存している敵の数をリセットする
	alive_enemy = 0;
	// ランダム攻撃可能な敵の数もリセットしておく
	random_fire.max_enemy = 0;

	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemies[i].use == false) { continue; }

		// 画面外に出てしまった敵を消去
		if (IsOutOfScreen(enemies[i].x, enemies[i].y, 32)) { enemies[i].use = false; continue; }

		// 生存している敵の数を数えておく
		if (enemies[i].enemy) { alive_enemy++; }

		// ランダム攻撃可能敵の数を数えておく
		if (enemies[i].enemy && enemies[i].randomfire) { random_fire.max_enemy++; }

		// 敵ごとに設定されている動作を実行
		enemies[i].action(&enemies[i]);

		// 敵の動作時間を更新
		enemies[i].time += 1;
	}

	// ランダム攻撃可能な状況かを判定して可能なら変数onfireをtrueに設定する
	bool onfire = false;
	if (random_fire.time >= random_fire.time_next)
	{
		// 最後の攻撃からの経過時間 random_fire.time が次の攻撃を行う時間 random_fire.time_next 以上の場合、
		// 弾を撃ってほしい状況なので変数onfireをtrueに設定し、random_fire.timeを０にリセットする
		// また、次の攻撃までの時間を１引くことで速める（ただし20以下にはならないようにする）
		onfire = true;
		random_fire.time -= random_fire.time_next;
		random_fire.time_next = max(random_fire.time_next - 1, 20);
	}
	else
	{
		// 次の攻撃まで時間経過していないので経過時間を
		random_fire.time++;
	}

	// ランダム攻撃可能で生存している敵がいるならその中から敵を選んで弾を打たせる
	if (onfire && random_fire.max_enemy > 0)
	{
		// 敵を選ぶ
		int select = (int)(rand() * 1.0 * random_fire.max_enemy / RAND_MAX);

		// その敵を探して弾を打たせる
		for (int i = 0; i < MAX_ENEMY; i++) {
			if (enemies[i].use == false) { continue; }
			if (enemies[i].enemy == false) { continue; }
			if (enemies[i].randomfire == false) { continue; }

			if (select-- != 0) {
				continue;
			}
			double r = atan2(player.y - enemies[i].y, player.x - enemies[i].x);
			create_enemy_burret(enemies[i].x, enemies[i].y, random_fire.shot_speed, r);
			break;
		}
	}
}

// パーティクルを使って敵の爆発演出を作る
static void explod_enemy(struct Enemy *e)
{
	// ここでは敵の持つ当たり判定（円）の中心点をパーティクル生成位置の中心座標に、
	// 当たり判定の大きさをパーティクルの初期サイズに設定している
	for (int j = 0; j < MAX_HITAREA; j++) {	// 敵の当たり判定をすべてチェック
		if (e->cr[j].r == 0) { continue; }	// 未使用なので無視する

		// 中心座標に１６個のパーティクルを作成して四方八方に飛ばす
		for (int i = 0; i < 16; i++) {
			// パーティクルの色はHSV色空間と呼ばれるもの（ペイントソフトにある丸い色選択のアレ）を用いて鮮やかな色を選んでいる
			Color c = FromHsv(i / 8.0f, 0.5f, 1.0f);
			float dx = (4.0 * rand() / RAND_MAX - 2.0);
			float dy = (4.0 * rand() / RAND_MAX - 2.0);
			create_particle(e->x + e->cr[j].x, e->y + e->cr[j].y, dx, dy, e->cr[j].r, c, (rand() % 20) + 10);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 得点
////////////////////////////////////////////////////////////////////////////////
static int score;

static void draw_score()
{
	Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);

	Video.FontSize(24);
	Video.FillColor(FromHsv((float)(Video.GetTime()*0.05f), 0.5f, 1.0f));
	Video.FormatText(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + 24, "SCORE: %5.5d", score);
}

////////////////////////////////////////////////////////////////////////////////
// 当たり判定：円が重なっているか判定
////////////////////////////////////////////////////////////////////////////////
static bool hitcheck(float x1, float y1, float r1, float x2, float y2, float r2) {
	float len2 = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
	float d2 = (r1 + r2) * (r1 + r2);
	return len2 < d2;
}

////////////////////////////////////////////////////////////////////////////////
// 全体の当たり判定を実行
////////////////////////////////////////////////////////////////////////////////
static void hit_check()
{
	// プレイヤーの弾と敵の当たり判定を行う
	// プレイヤーの弾一つ一つに対して敵全てとの判定を行っていく
	for (int j = 0; j < MAX_SHOT; j++) {

		// プレイヤーの弾が命中したらtrueになる変数
		bool shot_hit = false;
		if (shots[j].use == false) { continue; }

		for (int i = 0; i < MAX_ENEMY; i++) {
			if (enemies[i].use == false) { continue; }
			if (enemies[i].enemy == false) { continue; }

			// 当たり判定領域と判定
			for (int k = 0; k < MAX_HITAREA; k++) {
				if (enemies[i].cr[k].r == 0) { continue; }

				if (hitcheck(shots[j].x, shots[j].y, shots[j].r, enemies[i].x + enemies[i].cr[k].x, enemies[i].y + enemies[i].cr[k].y, enemies[i].cr[k].r))
				{

					// 敵と衝突した プレイヤーの弾を未使用にする
					shots[j].use = false;

					// 衝突したので弾が当たったことを記録する変数をtrueにする
					shot_hit = true;

					//当たっているのでダメージを与える処理を行う
					enemies[i].life -= 1;

					// スコアを足す
					score += 1;

					// ダメージを与えた結果敵の耐久値が０になったら敵を壊す
					if (enemies[i].life == 0)
					{
						// 敵情報を未使用にする
						enemies[i].use = false;

						// 爆発音を鳴らす
						Sound.ChannelPlay(0, false);

						// 敵の弾の発射速度を少し速くする
						random_fire.shot_speed += 0.01f;

						// スコアを加算する
						score += enemies[i].score;

						// 敵の爆発演出を作る
						explod_enemy(&enemies[i]);

					}

					break;
				}
			}
		}

		// プレイヤーの弾が当たっていた場合、着弾点に小さなパーティクルを作る
		if (shot_hit == true)
		{
			double r = rand() / RAND_MAX / 8;
			for (int i = 0; i < 8; i++) {
				Color c = FromHsv(r + i / 8.0f, 0.5f, 1.0f);
				double d = shots[j].d + Video.DegToRad(rand() % 6 - 3);
				double s = shots[j].s * (rand() * 0.3 / RAND_MAX + 0.2);
				create_particle(shots[j].x, shots[j].y, s, d, 10, c, (rand() % 20) + 20);
			}
		}
	}

	// プレイヤーが生きていれば当たり判定
	if (player.alive)
	{
		for (int i = 0; i < MAX_ENEMY; i++) {
			if (enemies[i].use == false) { continue; }
			if (enemies[i].enemy == true) { continue; }
			// 当たり判定領域と判定
			for (int k = 0; k < MAX_HITAREA; k++) {
				if (enemies[i].cr[k].r == 0) { continue; }

				if (hitcheck(player.x, player.y, player.r, enemies[i].x + enemies[i].cr[k].x, enemies[i].y + enemies[i].cr[k].y, enemies[i].cr[k].r))
				{
					// 爆発音を鳴らす
					Sound.ChannelPlay(0, false);
					double r = rand() / RAND_MAX / 8;
					for (int i = 0; i < 8; i++) {
						Color c = FromHsv(r + i / 8.0f, 0.5f, 1.0f);
						float d = Video.DegToRad(i * 360 / 8);
						create_particle(player.x, player.y, rand()*3.0/RAND_MAX, d, 10, c, (rand() % 20) + 20);
					}
					player.alive = false;
					break;
				}
			}
			if (player.alive == false)
			{
				break;

			}
		}

	}
}

////////////////////////////////////////////////////////////////////////////////
// ゲームのデータをリセットする
////////////////////////////////////////////////////////////////////////////////
static void reset_game_datas()
{
	// 敵・敵弾を初期化
	reset_enemies();

	// パーティクルの初期化
	reset_particles();

	// プレイヤーの初期化
	reset_player();

	// プレイヤーの弾の初期化
	reset_shots();

	// ランダム攻撃の設定をリセット
	reset_random_fire();

	// 敵の出現をリセットする
	reset_spawn();

}

////////////////////////////////////////////////////////////////////////////////
// ゲームのメイン処理
////////////////////////////////////////////////////////////////////////////////
void start(void)
{
	//
	// 初期化処理
	//

	// 画面サイズを横 SCREEN_WIDTH 縦 SCREEN_HEIGHT ピクセル に 設定
	Video.SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// タイトルを設定
	Video.SetWindowTitle("[Example] 51-STG");

	// ゲームデータをリセットする
	reset_game_datas();

	// 画像の読み込み
	img_player = Video.CreateImage("./Image/player.png", 0);
	img_shot_l = Video.CreateImage("./Image/shot_l.png", 0);
	img_shot_r = Video.CreateImage("./Image/shot_r.png", 0);
	img_enemy_s0 = Video.CreateImage("./Image/enemy_s0.png", 0);
	img_enemy_s1 = Video.CreateImage("./Image/enemy_s1.png", 0);
	img_enemy_m0 = Video.CreateImage("./Image/enemy_m.png", 0);
	img_enemy_shot = Video.CreateImage("./Image/enemy_shot.png", 0);

	// 音源の読み込み
	Sound.ChannelCreate(0, "./Sound/explod.wav");
	Sound.ChannelCreate(1, "./Sound/bgm.ogg");

	// フォント読み込み
	Video.CreateFont("font", "./Font/IPAexfont00301/ipaexg.ttf");
	Video.FontFace("font");

	// 直前のZキーの状態を保持する変数
	bool prevZ;


	//
	// 準備完了なのでゲーム本体処理を開始する
	//
	for (;;) {
		// タイトル画面ではプレイヤーを消しておく
		player.alive = false;

		// タイトル画面
		prevZ = Keyboard.IsKeyDown('Z');
		while (Video.Drawing()) {

			// 当たり判定を行う
			hit_check();

			// 敵全体の動作を行う
			move_enemies();

			// パーティクルを動かす
			move_particles();

			// 敵の出現を行う
			spawn_enemy();

			// 敵を描画する
			draw_enemies();

			// パーティクルを描画する
			draw_particles();

			// ゲーム画面自体を全体的に暗くしておく
			Video.FillColor(Video.RGBA(0, 0, 0, 128));
			Video.BeginPath();
			Video.Rect(SCREEN_LEFT, SCREEN_TOP, SCREEN_WIDTH, SCREEN_HEIGHT);
			Video.ClosePath();
			Video.Fill();

			// タイトル文字などを表示
			Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);

			Video.FontSize(64);
			Video.FillColor(FromHsv((float)(Video.GetTime()*0.05f), 0.5f, 1.0f));
			Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 2 / 5, "Nano STG", NULL);

			Video.FontSize(16);
			Video.FillColor(Video.RGB(255, 255, 255));
			Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 4 / 5, "Push Z to Start.", NULL);

			// Zキーが押されたらタイトル画面を抜ける
			if (Keyboard.IsKeyDown('Z') && !prevZ)
			{
				break;
			}
			prevZ = Keyboard.IsKeyDown('Z');
		}

		// ゲームデータをリセットする
		reset_game_datas();
		score = 0;

		// BGM再生開始
		Sound.ChannelPlay(1, true);

		// ゲーム本体開始
		while (Video.Drawing()) {

			// 当たり判定を行う
			hit_check();

			// 敵全体の動作を行う
			move_enemies();

			// プレイヤーの弾の移動を行う
			move_shots();

			// プレイヤーを動かす
			move_player();

			// パーティクルを動かす
			move_particles();

			// 敵の出現を行う
			spawn_enemy();

			// 得点を描く
			draw_score();

			// 敵を描画する
			draw_enemies();

			// プレイヤーの弾を描画する
			draw_shots();

			// プレイヤーを描画する
			draw_player();

			// パーティクルを描画する
			draw_particles();

			// プレイヤーが死んでいたらゲームオーバー画面へ
			if (player.alive == false)
			{
				break;
			}
		}

		// ゲームオーバー画面へ行く前に1秒待機
		int t = 0;
		while (Video.Drawing()) {
			t++;

			// 当たり判定を行う
			hit_check();

			// 敵全体の動作を行う
			move_enemies();

			// プレイヤーの弾の移動を行う
			move_shots();

			// パーティクルを動かす
			move_particles();

			// 敵の出現を行う
			spawn_enemy();

			// 得点を描く
			draw_score();

			// 敵を描画する
			draw_enemies();

			// プレイヤーの弾を描画する
			draw_shots();

			// パーティクルを描画する
			draw_particles();

			if (t >= 60)
			{
				t = 0;
				break;
			}

		}

		// ゲームオーバー画面
		prevZ = Keyboard.IsKeyDown('Z');
		while (Video.Drawing()) {

			// 当たり判定を行う
			hit_check();

			// 敵全体の動作を行う
			move_enemies();

			// パーティクルを動かす
			move_particles();

			// 敵の出現を行う
			spawn_enemy();

			// 敵を描画する
			draw_enemies();

			// パーティクルを描画する
			draw_particles();

			// ゲーム画面自体を全体的に暗くしておく
			Video.FillColor(Video.RGBA(0, 0, 0, 128));
			Video.BeginPath();
			Video.Rect(SCREEN_LEFT, SCREEN_TOP, SCREEN_WIDTH, SCREEN_HEIGHT);
			Video.ClosePath();
			Video.Fill();

			// タイトル文字などを表示
			Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);

			Video.FontSize(64);
			Video.FillColor(Video.RGB(255, 0, 0));
			Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 2 / 10, "GAME", NULL);
			Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 4 / 10, "OVER", NULL);

			Video.FontSize(32);
			Video.FillColor(Video.RGB(255, 255, 255));
			Video.FormatText(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 6 / 10, "SCORE : %5.5d", score);

			// Zキーが押されたらタイトル画面を抜ける
			if (Keyboard.IsKeyDown('Z') && !prevZ)
			{
				break;
			}
			prevZ = Keyboard.IsKeyDown('Z');
		}
	}
}
