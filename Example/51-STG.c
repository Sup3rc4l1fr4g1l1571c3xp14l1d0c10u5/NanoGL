#include <NanoGL/NanoGL.h>

////////////////////////////////////////////////////////////////////////////////
// ��ʃT�C�Y�̐ݒ�
////////////////////////////////////////////////////////////////////////////////
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 480

#define SCREEN_LEFT   0
#define SCREEN_RIGHT  (SCREEN_LEFT + SCREEN_WIDTH)
#define SCREEN_TOP    0
#define SCREEN_BOTTOM (SCREEN_TOP + SCREEN_HEIGHT)

////////////////////////////////////////////////////////////////////////////////
// �o��������̐ݒ�
////////////////////////////////////////////////////////////////////////////////
#define MAX_ENEMY		256		// �G�E�G�e���킹�Ă̍ő�o����
#define MAX_SHOT		32		// �v���C���[�̒e�̍ő�o����
#define MAX_PARTICLE	256		// �p�[�e�B�N���̍ő�o����
#define MAX_HITAREA		3		// �G�������Ƃ��ł��铖���蔻��̍ő吔

////////////////////////////////////////////////////////////////////////////////
// ���^�̓G���烉���_���ɑI��ŋ���ł�����u�����_���U���v�̍U���Ԋu�Ȃǂ̐ݒ�ŗp����ϐ�
////////////////////////////////////////////////////////////////////////////////
static struct
{
	float	shot_speed;			// �e��
	int		time;				// �Ō�̍U������̌o�ߎ���
	int		time_next;			// ���̍U�����s������
	int		max_enemy;			// �����_���U���őI���\�ȂȓG�̐�
} random_fire;

static void reset_random_fire()
{
	random_fire.shot_speed = 2;
	random_fire.time = 0;
	random_fire.time_next = 60;
	random_fire.max_enemy = 0;
}

////////////////////////////////////////////////////////////////////////////////
// �ǂݍ��񂾉摜��ID������ϐ�
////////////////////////////////////////////////////////////////////////////////
static int img_player;
static int img_shot_l;
static int img_shot_r;
static int img_enemy_shot;
static int img_enemy_s0;
static int img_enemy_s1;
static int img_enemy_m0;

////////////////////////////////////////////////////////////////////////////////
// ���W(x,y)����ʋ��E����p�s�N�Z���ȏ�O���ɂ��邩����B
////////////////////////////////////////////////////////////////////////////////
static bool IsOutOfScreen(int x, int y, int p) {
	return (x < SCREEN_LEFT - p || SCREEN_RIGHT + p <= x || y < SCREEN_TOP - p || SCREEN_BOTTOM + p <= y);
}

////////////////////////////////////////////////////////////////////////////////
// �G�ƓG�e��\������f�[�^
// �ő� MAX_ENEMY ��
////////////////////////////////////////////////////////////////////////////////
static struct Enemy {
	bool use;	  // �g�p���t���O
	bool enemy;   // �G�Ƃ��Ďg���Ă���ꍇ��true
	float x;	  // X���W
	float y;	  // Y���W
	float s;	  // �ړ����x
	float d;	  // �ړ�����
	int   life;   // ���C�t�l
	void(*action)(struct Enemy *); // ����
	int   time;   // ���s����
	int   image;  // �摜�ԍ�
	int   score; // ���_
	bool	randomfire;	// �����_���C�΂��\��
	struct {
		float r; // �����蔻��̔��a(0�͖��g�p)
		float x, y; // �����蔻��̒��S
	} cr[MAX_HITAREA];
} enemies[MAX_ENEMY];

// �����_�ł̓G�̐�����
static int alive_enemy = 0;

// ���g�p�̓G�f�[�^��ID�𓾂�
static int find_free_enemy() {
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemies[i].use == false) { return i; }
	}
	return -1;
}

// �S�Ă̓G�E�G�e���u���g�p�v�ɂ��A�G�̐��������O�ɂ���
static void reset_enemies()
{
	alive_enemy = 0;
	for (int i = 0; i < MAX_ENEMY; i++) {
		enemies[i].use = false;
	}
}

// �G�����ׂĕ`�悷��
static void draw_enemies()
{
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemies[i].use == false) { continue; }

		// �G�摜�̃T�C�Y�𓾂�
		int w, h;
		Video.ImageSize(enemies[i].image, &w, &h);


		// �O�̂��ߕό`�����Z�b�g
		Video.ResetTransform();	

		// �ό`������ݒ�
		// �i�v���O������ł͈ړ�����]�̏����ɂȂ��Ă��邪�s��̊|���Z�Ȃ̂ŉ��Z���ʂ͉�]���Ĉړ��ƂȂ邱�Ƃɒ��Ӂj

		// �\�����������W�ւ̈ړ�
		Video.Translate(enemies[i].x, enemies[i].y);
		// ��]��K�p
		Video.Rotate(enemies[i].d - Video.DegToRad(90));

		// �摜��`��
		Video.DrawImage(enemies[i].image, 0, 0, w, h, -w / 2.0f, -h / 2.0f, w, h, 0, 1.0f);
		
		// �ό`���I������̂ŕό`�����Z�b�g
		Video.ResetTransform();
	}
}

////////////////////////////////////////////////////////////////////////////////
// �v���C���[�̒e���Ǘ�����
// �ő� MAX_SHOT �܂Ŏg����
////////////////////////////////////////////////////////////////////////////////
static struct Shot {
	bool use;	  // �g�p���t���O
	float x;	  // X���W
	float y;	  // Y���W
	float s;	  // �ړ����x
	float d;	  // �ړ�����
	float r;	  // ���蔻��̔��a
	int   image;  // �摜�ԍ�
} shots[MAX_SHOT];

// ���g�p�̃v���C���[�̒e��ID�𓾂�
static int find_free_shot() {
	for (int i = 0; i < MAX_SHOT; i++) {
		if (shots[i].use == false) { return i; }
	}
	return -1;
}

// �S�Ă̒e���u���g�p�v�ɂ���
static void reset_shots()
{
	for (int i = 0; i < MAX_SHOT; i++) {
		shots[i].use = false;
	}
}

// �v���C���[�̒e���ړ�������
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

// �v���C���[�̒e��`�悷��
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
// �v���C���[���Ǘ�����
////////////////////////////////////////////////////////////////////////////////
static struct player {
	bool alive;	  // �����t���O
	float x;	  // X���W
	float y;	  // Y���W
	float r;	  // ���蔻��̔��a
	int reload;
} player;

static void reset_player()
{
	player.alive = true;
	player.x = SCREEN_LEFT + SCREEN_WIDTH  / 2;
	player.y = SCREEN_TOP  + (SCREEN_HEIGHT * 9) / 10;
	player.reload = 0;
}

// �v���C���[�̓�����s��
static void move_player()
{
	// �v���C���[�������Ă���Ȃ�v���C���[�̑���𔽉f������
	if (player.alive) {

		// �\���L�[�̓��͂ɉ����Ĉړ�������
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

		// �ړ��������ʉ�ʊO�ɏo���ꍇ�͉�ʓ��ɖ߂�
		if (player.x < SCREEN_LEFT  ) { player.x = SCREEN_LEFT;   }
		if (player.x > SCREEN_RIGHT ) { player.x = SCREEN_RIGHT;  }
		if (player.y < SCREEN_TOP   ) { player.y = SCREEN_TOP;    }
		if (player.y > SCREEN_BOTTOM) { player.y = SCREEN_BOTTOM; }

		// �U���̃����[�h���Ǘ�����
		if (player.reload == 0) {
			// �����[�h���I����Ă���ꍇ�A'z' �L�[�������ƒe������
			if (Keyboard.IsKeyDown('Z')) {

				// ���g�p�̒e��T���ăp�����[�^��ݒ肷��

				// �����̒e
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

				// �E���̒e
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
			// �����[�h���I����Ă��Ȃ��̂Ń����[�h���Ԃ��P���炷
			player.reload -= 1;
		}

	}
}

static void draw_player()
{
	// �v���C���[�������Ă���Ε`�悷��
	if (player.alive) {
		int w, h;
		Video.ImageSize(img_player, &w, &h);
		Video.DrawImage(img_player, 0, 0, w, h, player.x - w / 2.0f, player.y - h / 2.0f, w, h, 0, 1.0f);
	}
}

////////////////////////////////////////////////////////////////////////////////
// ���o�p�̃p�[�e�B�N�����Ǘ�����
// �ő�Q�T�U�܂Ŏg����
////////////////////////////////////////////////////////////////////////////////
static struct particle {
	bool use;	  // �g�p���t���O
	float x;	  // X���W
	float y;	  // Y���W
	float s;	  // �ړ����x
	float d;	  // �ړ�����
	float r;	  // ���a
	Color c;	 // �F
	int   ttl;	// ���ł܂ł̎���
	int   time;	// ��������
} particles[MAX_PARTICLE];

// �p�[�e�B�N���̐F�����肷�邽�߂ɁAHSV�J���[�w����@����RGB�J���[�����
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

// �S�p�[�e�B�N�����u���g�p�v�ɂ���
static void reset_particles() {
	for (int i = 0; i < MAX_PARTICLE; i++) {
		particles[i].use = false;
	}
}
// �p�[�e�B�N�����ЂƂ��
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

// �p�[�e�B�N���𓮂���
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
// �G�̒e�̓���i�P�j�F���i����G�e�̓���
////////////////////////////////////////////////////////////////////////////////
static void action_b1(struct Enemy *e)
{
	e->x += cos(e->d) * e->s;
	e->y += sin(e->d) * e->s;
}

////////////////////////////////////////////////////////////////////////////////
// �G�e�����
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
// ���^�̓G�P�����
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
// ���^�̓G�Q�����
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
// �G�̓���F�ݒ肳�ꂽ���x�ƕ����Œ��i
////////////////////////////////////////////////////////////////////////////////
static void action_s0(struct Enemy *e)
{
	e->x += cos(e->d) * e->s;
	e->y += sin(e->d) * e->s;
}

////////////////////////////////////////////////////////////////////////////////
// �G�̓���F��ʉ��Ɍ������č��E�ɂӂ�ӂ�h��Ȃ��璼�i
////////////////////////////////////////////////////////////////////////////////
static void action_s1(struct Enemy *e)
{
	e->x += cos(Video.DegToRad(e->time % 120 * 360 / 120)) * 2;
	e->y += e->s;
}

////////////////////////////////////////////////////////////////////////////////
// �G�̓���F�ݒ肳�ꂽ���x�ƕ����Œ��i���邪���X�ɉ�������
////////////////////////////////////////////////////////////////////////////////
static void action_s2(struct Enemy *e) {
	e->x += cos(e->d) * e->s;
	e->y += sin(e->d) * e->s;
	e->s *= 1.03;
}

////////////////////////////////////////////////////////////////////////////////
// �G�̓���F��ʉ��Ɍ������Ē��i���Ă��邪�A�v���C���[�Ɖ����������ƒ�~���A�v���C���[�̌����ɋȂ����Ă����������
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
		e->action = action_s2;	//���i����ɐ؂�ւ�
	}
}

////////////////////////////////////////////////////////////////////////////////
// �G�̓���F�v���C���[�̕����ɐ��񂵂Ȃ���ǔ�����B�������߂��Ȃ�ƒǔ����I������
////////////////////////////////////////////////////////////////////////////////
static void action_s4(struct Enemy *e) {
	e->x += cos(e->d) * e->s;
	e->y += sin(e->d) * e->s;
	if (player.alive) {
		double d = (player.y - e->y) * (player.y - e->y) + (player.x - e->x) * (player.x - e->x);
		if (d < 60 * 60)
		{
			// �ǔ��I��
			//���i����ɐ؂�ւ�
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
// ���^�̓G�����
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
// ���^�̓G�̓���F
// ������^���Ɍ������Ċp�x�Œ�̂R�����ɒe������
// ���̂���v���C���[��_���S�����e��ł�
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
// �G�O���[�v�̏o�����Ǘ����鏈���Ŏg���ϐ�
////////////////////////////////////////////////////////////////////////////////
static struct
{
	int  time;
	int  work;
	bool enable;
	int  action;
} spawn;

// �o�����������Z�b�g����
static void reset_spawn()
{
	spawn.time = 0;
	spawn.work = 0;
	spawn.enable = true;
	spawn.action = 0;
}

////////////////////////////////////////////////////////////////////////////////
// �G�̓���F�o���͐Î~���ăv���C���[��_���S�����e��łB��莞�Ԍ�ɉ������Ȃ���ːi����B
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
// �G�O���[�v�̏o������F0.5�b�Ȃɂ����Ȃ�
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s0(void)
{
	// 0.5�b�҂�
	if (spawn.time == 30) {
		spawn.enable = true;
	}
}


////////////////////////////////////////////////////////////////////////////////
// �G�O���[�v�̏o������F��ʏォ���ʉ��Ɍ������Ē��i���Ă���G��0.33�b�Ԋu�łP�O�̏o��������
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
// �G�O���[�v�̏o������F��ʏォ���ʉ��Ɍ������č��E�ɗh��Ȃ��璼�i���Ă���G��0.33�b�Ԋu�łP�O�̏o��������
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
// �G�O���[�v�̏o������F��ʍ��E����v���C���[��ǔ�����G���o��������
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
// �G�O���[�v�̏o������F��ʏ㕔�����E�ɉ��f����G��1/6�b�Ԋu�ō��
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
// �G�O���[�v�̏o������F��ʗ��e���㉺�ɉ��f����G��1/6�b�Ԋu�ō��
////////////////////////////////////////////////////////////////////////////////
static void spawn_enemy_s5(void)
{
	// �㉺�ɉ��؂�G���o��
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
// �G�O���[�v�̏o������F
// ��ʉ��Ɍ������Ē��i���A�v���C���[�Ɖ����������ƒ�~���A�Ȃ����ĉ�������G��
// �ォ��A���ō~�点��
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
// �G�O���[�v�̏o������F
// ���^�̓G���ォ��o��������
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
// �G�O���[�v�̏o������F
// ��ʏォ�璼�i����G�ƃv���C���[��ǔ�����G�����݂ɏo��������
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
// �G�O���[�v�̏o������F
// �Î~���Ēe��łG���o�������Ă��獶�E�ɗh���Ȃ璼�i���Ă���G��A���ō~�点��
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
// �G�O���[�v�̏o���������ׂ��\
// �֐��|�C���^�Ƃ����Z�@���g���Ă���̂ŗ������邽�߂̓�Փx�͂�⍂��
////////////////////////////////////////////////////////////////////////////////
static void(*spawn_table[])(void) = {
	spawn_enemy_s0,	// 0�Ԗڂ͎g���Ȃ�
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
	spawn_enemy_m0,	// ���^�@�o��
	spawn_enemy_s7,
	spawn_enemy_s8,
	NULL			// �I�[���Ӗ����� NULL �����Ă���
};

// �G�O���[�v�̏o������������
static void spawn_enemy()
{
	// �G�����Ȃ��āA�G�O���[�v������Ă������ꍇ�͓G�̏o���e�[�u����ǂݐi�߂Ď��s����悤�ɐݒ肷��
	if (alive_enemy == 0 && spawn.enable == true) {
		spawn.action += 1;	// �o���e�[�u���ŉғ�������v�f�̔ԍ���+1����
		spawn.time = 0;		// �o���e�[�u���̓��쎞�Ԃ�0�Ƀ��Z�b�g
		spawn.enable = false;	// �G�̏o�����������狖���o��܂œG�̏o���������X�V���Ȃ��悤�ɂ���
		spawn.work = 0;			// �G�̏o���������Ŏg���p�r���߂Ȃ��ϐ����ꉞ���Z�b�g���Ă���
	}

	if (spawn.enable == false) {
		// �G�̏o���e�[�u�����I�[�܂œǂݐi�߂��Ă�����擪�ɖ߂��B
		if (spawn_table[spawn.action] == NULL) {
			spawn.action = 0;
		}
		// �o����������s���Čo�ߎ��Ԃ�i�߂�
		spawn_table[spawn.action]();
		spawn.time += 1;
	}
}

// �G�̓�����s��
static void move_enemies() {
	// �������Ă���G�̐������Z�b�g����
	alive_enemy = 0;
	// �����_���U���\�ȓG�̐������Z�b�g���Ă���
	random_fire.max_enemy = 0;

	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemies[i].use == false) { continue; }

		// ��ʊO�ɏo�Ă��܂����G������
		if (IsOutOfScreen(enemies[i].x, enemies[i].y, 32)) { enemies[i].use = false; continue; }

		// �������Ă���G�̐��𐔂��Ă���
		if (enemies[i].enemy) { alive_enemy++; }

		// �����_���U���\�G�̐��𐔂��Ă���
		if (enemies[i].enemy && enemies[i].randomfire) { random_fire.max_enemy++; }

		// �G���Ƃɐݒ肳��Ă��铮������s
		enemies[i].action(&enemies[i]);

		// �G�̓��쎞�Ԃ��X�V
		enemies[i].time += 1;
	}

	// �����_���U���\�ȏ󋵂��𔻒肵�ĉ\�Ȃ�ϐ�onfire��true�ɐݒ肷��
	bool onfire = false;
	if (random_fire.time >= random_fire.time_next)
	{
		// �Ō�̍U������̌o�ߎ��� random_fire.time �����̍U�����s������ random_fire.time_next �ȏ�̏ꍇ�A
		// �e�������Ăق����󋵂Ȃ̂ŕϐ�onfire��true�ɐݒ肵�Arandom_fire.time���O�Ƀ��Z�b�g����
		// �܂��A���̍U���܂ł̎��Ԃ��P�������Ƃő��߂�i������20�ȉ��ɂ͂Ȃ�Ȃ��悤�ɂ���j
		onfire = true;
		random_fire.time -= random_fire.time_next;
		random_fire.time_next = max(random_fire.time_next - 1, 20);
	}
	else
	{
		// ���̍U���܂Ŏ��Ԍo�߂��Ă��Ȃ��̂Ōo�ߎ��Ԃ�
		random_fire.time++;
	}

	// �����_���U���\�Ő������Ă���G������Ȃ炻�̒�����G��I��Œe��ł�����
	if (onfire && random_fire.max_enemy > 0)
	{
		// �G��I��
		int select = (int)(rand() * 1.0 * random_fire.max_enemy / RAND_MAX);

		// ���̓G��T���Ēe��ł�����
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

// �p�[�e�B�N�����g���ēG�̔������o�����
static void explod_enemy(struct Enemy *e)
{
	// �����ł͓G�̎������蔻��i�~�j�̒��S�_���p�[�e�B�N�������ʒu�̒��S���W�ɁA
	// �����蔻��̑傫�����p�[�e�B�N���̏����T�C�Y�ɐݒ肵�Ă���
	for (int j = 0; j < MAX_HITAREA; j++) {	// �G�̓����蔻������ׂă`�F�b�N
		if (e->cr[j].r == 0) { continue; }	// ���g�p�Ȃ̂Ŗ�������

		// ���S���W�ɂP�U�̃p�[�e�B�N�����쐬���Ďl�������ɔ�΂�
		for (int i = 0; i < 16; i++) {
			// �p�[�e�B�N���̐F��HSV�F��ԂƌĂ΂����́i�y�C���g�\�t�g�ɂ���ۂ��F�I���̃A���j��p���đN�₩�ȐF��I��ł���
			Color c = FromHsv(i / 8.0f, 0.5f, 1.0f);
			float dx = (4.0 * rand() / RAND_MAX - 2.0);
			float dy = (4.0 * rand() / RAND_MAX - 2.0);
			create_particle(e->x + e->cr[j].x, e->y + e->cr[j].y, dx, dy, e->cr[j].r, c, (rand() % 20) + 10);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// ���_
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
// �����蔻��F�~���d�Ȃ��Ă��邩����
////////////////////////////////////////////////////////////////////////////////
static bool hitcheck(float x1, float y1, float r1, float x2, float y2, float r2) {
	float len2 = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
	float d2 = (r1 + r2) * (r1 + r2);
	return len2 < d2;
}

////////////////////////////////////////////////////////////////////////////////
// �S�̂̓����蔻������s
////////////////////////////////////////////////////////////////////////////////
static void hit_check()
{
	// �v���C���[�̒e�ƓG�̓����蔻����s��
	// �v���C���[�̒e���ɑ΂��ēG�S�ĂƂ̔�����s���Ă���
	for (int j = 0; j < MAX_SHOT; j++) {

		// �v���C���[�̒e������������true�ɂȂ�ϐ�
		bool shot_hit = false;
		if (shots[j].use == false) { continue; }

		for (int i = 0; i < MAX_ENEMY; i++) {
			if (enemies[i].use == false) { continue; }
			if (enemies[i].enemy == false) { continue; }

			// �����蔻��̈�Ɣ���
			for (int k = 0; k < MAX_HITAREA; k++) {
				if (enemies[i].cr[k].r == 0) { continue; }

				if (hitcheck(shots[j].x, shots[j].y, shots[j].r, enemies[i].x + enemies[i].cr[k].x, enemies[i].y + enemies[i].cr[k].y, enemies[i].cr[k].r))
				{

					// �G�ƏՓ˂��� �v���C���[�̒e�𖢎g�p�ɂ���
					shots[j].use = false;

					// �Փ˂����̂Œe�������������Ƃ��L�^����ϐ���true�ɂ���
					shot_hit = true;

					//�������Ă���̂Ń_���[�W��^���鏈�����s��
					enemies[i].life -= 1;

					// �X�R�A�𑫂�
					score += 1;

					// �_���[�W��^�������ʓG�̑ϋv�l���O�ɂȂ�����G����
					if (enemies[i].life == 0)
					{
						// �G���𖢎g�p�ɂ���
						enemies[i].use = false;

						// ��������炷
						Sound.ChannelPlay(0, false);

						// �G�̒e�̔��ˑ��x��������������
						random_fire.shot_speed += 0.01f;

						// �X�R�A�����Z����
						score += enemies[i].score;

						// �G�̔������o�����
						explod_enemy(&enemies[i]);

					}

					break;
				}
			}
		}

		// �v���C���[�̒e���������Ă����ꍇ�A���e�_�ɏ����ȃp�[�e�B�N�������
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

	// �v���C���[�������Ă���Γ����蔻��
	if (player.alive)
	{
		for (int i = 0; i < MAX_ENEMY; i++) {
			if (enemies[i].use == false) { continue; }
			if (enemies[i].enemy == true) { continue; }
			// �����蔻��̈�Ɣ���
			for (int k = 0; k < MAX_HITAREA; k++) {
				if (enemies[i].cr[k].r == 0) { continue; }

				if (hitcheck(player.x, player.y, player.r, enemies[i].x + enemies[i].cr[k].x, enemies[i].y + enemies[i].cr[k].y, enemies[i].cr[k].r))
				{
					// ��������炷
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
// �Q�[���̃f�[�^�����Z�b�g����
////////////////////////////////////////////////////////////////////////////////
static void reset_game_datas()
{
	// �G�E�G�e��������
	reset_enemies();

	// �p�[�e�B�N���̏�����
	reset_particles();

	// �v���C���[�̏�����
	reset_player();

	// �v���C���[�̒e�̏�����
	reset_shots();

	// �����_���U���̐ݒ�����Z�b�g
	reset_random_fire();

	// �G�̏o�������Z�b�g����
	reset_spawn();

}

////////////////////////////////////////////////////////////////////////////////
// �Q�[���̃��C������
////////////////////////////////////////////////////////////////////////////////
void start(void)
{
	//
	// ����������
	//

	// ��ʃT�C�Y���� SCREEN_WIDTH �c SCREEN_HEIGHT �s�N�Z�� �� �ݒ�
	Video.SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 51-STG");

	// �Q�[���f�[�^�����Z�b�g����
	reset_game_datas();

	// �摜�̓ǂݍ���
	img_player = Video.CreateImage("./image/player.png", 0);
	img_shot_l = Video.CreateImage("./image/shot_l.png", 0);
	img_shot_r = Video.CreateImage("./image/shot_r.png", 0);
	img_enemy_s0 = Video.CreateImage("./image/enemy_s0.png", 0);
	img_enemy_s1 = Video.CreateImage("./image/enemy_s1.png", 0);
	img_enemy_m0 = Video.CreateImage("./image/enemy_m.png", 0);
	img_enemy_shot = Video.CreateImage("./image/enemy_shot.png", 0);

	// �����̓ǂݍ���
	Sound.ChannelCreate(0, "./Sound/explod.wav");
	Sound.ChannelCreate(1, "./Sound/bgm.ogg");

	// �t�H���g�ǂݍ���
	Video.CreateFont("font", "./Font/IPAexfont00301/ipaexg.ttf");
	Video.FontFace("font");

	// ���O��Z�L�[�̏�Ԃ�ێ�����ϐ�
	bool prevZ;


	//
	// ���������Ȃ̂ŃQ�[���{�̏������J�n����
	//
	for (;;) {
		// �^�C�g����ʂł̓v���C���[�������Ă���
		player.alive = false;

		// �^�C�g�����
		prevZ = Keyboard.IsKeyDown('Z');
		while (Video.Drawing()) {

			// �����蔻����s��
			hit_check();

			// �G�S�̂̓�����s��
			move_enemies();

			// �p�[�e�B�N���𓮂���
			move_particles();

			// �G�̏o�����s��
			spawn_enemy();

			// �G��`�悷��
			draw_enemies();

			// �p�[�e�B�N����`�悷��
			draw_particles();

			// �Q�[����ʎ��̂�S�̓I�ɈÂ����Ă���
			Video.FillColor(Video.RGBA(0, 0, 0, 128));
			Video.BeginPath();
			Video.Rect(SCREEN_LEFT, SCREEN_TOP, SCREEN_WIDTH, SCREEN_HEIGHT);
			Video.ClosePath();
			Video.Fill();

			// �^�C�g�������Ȃǂ�\��
			Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);

			Video.FontSize(64);
			Video.FillColor(FromHsv((float)(Video.GetTime()*0.05f), 0.5f, 1.0f));
			Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 2 / 5, "Nano STG", NULL);

			Video.FontSize(16);
			Video.FillColor(Video.RGB(255, 255, 255));
			Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 4 / 5, "Push Z to Start.", NULL);

			// Z�L�[�������ꂽ��^�C�g����ʂ𔲂���
			if (Keyboard.IsKeyDown('Z') && !prevZ)
			{
				break;
			}
			prevZ = Keyboard.IsKeyDown('Z');
		}

		// �Q�[���f�[�^�����Z�b�g����
		reset_game_datas();
		score = 0;

		// BGM�Đ��J�n
		Sound.ChannelPlay(1, true);

		// �Q�[���{�̊J�n
		while (Video.Drawing()) {

			// �����蔻����s��
			hit_check();

			// �G�S�̂̓�����s��
			move_enemies();

			// �v���C���[�̒e�̈ړ����s��
			move_shots();

			// �v���C���[�𓮂���
			move_player();

			// �p�[�e�B�N���𓮂���
			move_particles();

			// �G�̏o�����s��
			spawn_enemy();

			// ���_��`��
			draw_score();

			// �G��`�悷��
			draw_enemies();

			// �v���C���[�̒e��`�悷��
			draw_shots();

			// �v���C���[��`�悷��
			draw_player();

			// �p�[�e�B�N����`�悷��
			draw_particles();

			// �v���C���[������ł�����Q�[���I�[�o�[��ʂ�
			if (player.alive == false)
			{
				break;
			}
		}

		// �Q�[���I�[�o�[��ʂ֍s���O��1�b�ҋ@
		int t = 0;
		while (Video.Drawing()) {
			t++;

			// �����蔻����s��
			hit_check();

			// �G�S�̂̓�����s��
			move_enemies();

			// �v���C���[�̒e�̈ړ����s��
			move_shots();

			// �p�[�e�B�N���𓮂���
			move_particles();

			// �G�̏o�����s��
			spawn_enemy();

			// ���_��`��
			draw_score();

			// �G��`�悷��
			draw_enemies();

			// �v���C���[�̒e��`�悷��
			draw_shots();

			// �p�[�e�B�N����`�悷��
			draw_particles();

			if (t >= 60)
			{
				t = 0;
				break;
			}

		}

		// �Q�[���I�[�o�[���
		prevZ = Keyboard.IsKeyDown('Z');
		while (Video.Drawing()) {

			// �����蔻����s��
			hit_check();

			// �G�S�̂̓�����s��
			move_enemies();

			// �p�[�e�B�N���𓮂���
			move_particles();

			// �G�̏o�����s��
			spawn_enemy();

			// �G��`�悷��
			draw_enemies();

			// �p�[�e�B�N����`�悷��
			draw_particles();

			// �Q�[����ʎ��̂�S�̓I�ɈÂ����Ă���
			Video.FillColor(Video.RGBA(0, 0, 0, 128));
			Video.BeginPath();
			Video.Rect(SCREEN_LEFT, SCREEN_TOP, SCREEN_WIDTH, SCREEN_HEIGHT);
			Video.ClosePath();
			Video.Fill();

			// �^�C�g�������Ȃǂ�\��
			Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);

			Video.FontSize(64);
			Video.FillColor(Video.RGB(255, 0, 0));
			Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 2 / 10, "GAME", NULL);
			Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 4 / 10, "OVER", NULL);

			Video.FontSize(32);
			Video.FillColor(Video.RGB(255, 255, 255));
			Video.FormatText(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 6 / 10, "SCORE : %5.5d", score);

			// Z�L�[�������ꂽ��^�C�g����ʂ𔲂���
			if (Keyboard.IsKeyDown('Z') && !prevZ)
			{
				break;
			}
			prevZ = Keyboard.IsKeyDown('Z');
		}
	}
}
