#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "NanoGL/NanoGL.h"

// �p�Y���̔Ֆʂ̑傫��
#define PAZZLE_BOARD_WIDTH   512
#define PAZZLE_BOARD_HEIGHT  512

// �X���C�h�A�j���̑��x
#define SLIDE_SPEED_X (PAZZLE_BOARD_WIDTH / 32)	
#define SLIDE_SPEED_Y (PAZZLE_BOARD_HEIGHT / 32)	

// ��l�����𐶐�
static double urandom(void) {
	return ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);
}

// �Ֆʏ�̃^�C�����Ǘ�����
//
// �ȉ��̂悤�ɔՖʂ̃}�X�ɂ͔ԍ����U���Ă���
// ������������������
// �� 0�� 1�� 2�� 3��
// ������������������
// �� 4�� 5�� 6�� 7��
// ������������������
// �� 8�� 9��10��11��
// ������������������
// ��12��13��14��15��
// ������������������
// 9�Ԃ̃}�X�ɂ���^�C���̐�����ǂݏ����������ꍇ�� board[9] ��ǂݏ�������
static int board[16];

// �^�C�����}�X�̒��S����㉺���E�ɂǂꂾ������Ă��邩���Ǘ�����
// �A�j���[�V���������Ŏg��
static int board_x[16];	// �l�� 0 �̏ꍇ�� ���E�ǂ���ɂ�����Ă��Ȃ��@���̏ꍇ�̓^�C���͍��ɂ���Ă���A���̏ꍇ�̓^�C���͉E�ɂ���Ă���
static int board_y[16];	// �l�� 0 �̏ꍇ�� �㉺�ǂ���ɂ�����Ă��Ȃ��@���̏ꍇ�̓^�C���͏�ɂ���Ă���A���̏ꍇ�̓^�C���͉��ɂ���Ă���

// �^�C���̉摜��ID�ƕ��A����
static int imageId;
static int imageWidth, imageHeight;

//
// �ŏ��̔Ֆʂ��쐬����B
// �����_���ɏ����z�u���쐬����Ɖ����Ȃ��ՖʂƂȂ邱�Ƃ�����̂�
// �����Ȃ��Ֆʂ̂Ƃ��͉�����ՖʂɂȂ�悤���삷��
// 
void init_board(void)
{

	// �Ֆʏ��0����15(��)�܂ł̃^�C�����Z���̔ԍ����ɕ��ׂ�
	// �܂��A�^�C���̂���͂Ȃ��Ƃ���B
	for (int i = 0; i < 16; i++) {
		board[i] = i;
		board_x[i] = 0;
		board_y[i] = 0;
	}

	// Fisher-Yates shuffle�A���S���Y���ŃV���b�t�����s��
	for (int i = 16; i > 1; --i) {
		int a = i - 1;
		int b = (int)floor(urandom() * (i + 1));
		int tmp = board[a];
		board[a] = board[b];
		board[b] = tmp;
	}

	// �V���b�t����̃^�C���̔z�u�����̏�Ԃɖ߂�i�܂蓚�������邩�H�j�𔻒�
	// �u����p��������� http://hp.vector.co.jp/authors/VA010128/math/puzzle/P15-1.html
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

	// �����̂Ȃ��^�C���z�u�̏ꍇ�A�^�C���̔z�u�����E���]������Ɠ����̂���z�u�ɂȂ邱�Ƃ��m���Ă���
	// (�t�ɓ����̂���^�C���̔z�u�����E���]������Ɖ����Ȃ��Ȃ�)
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

// �Ֆʂ���ʏ�ɕ`�悷��
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

// �Ֆʂ��N���b�N���ꂽ�Ƃ��Ƀ^�C���𓮂���
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
		// �󔒃Z���Ɛ�����������v����Z�����ړ�
		if (cx > blankCellX) {
			// �E���獶�Ɉړ�
			for (int i = blankCellX; i < cx; i++) {
				board_x[blankCellY * 4 + i] = PAZZLE_BOARD_WIDTH / 4;
				board[blankCellY * 4 + i] = board[blankCellY * 4 + i + 1];
			}
			board[blankCellY * 4 + cx] = 15;
			return true;
		}
		else if (cx < blankCellX) {
			// ������E�Ɉړ�
			for (int i = blankCellX; i > cx; i--) {
				board_x[blankCellY * 4 + i] = -PAZZLE_BOARD_WIDTH / 4;
				board[blankCellY * 4 + i] = board[blankCellY * 4 + i - 1];
			}
			board[blankCellY * 4 + cx] = 15;
			return true;
		}
	}
	else if (cx == blankCellX && cy != blankCellY) {
		// �󔒃Z���Ɛ�����������v����Z�����ړ�
		if (cy > blankCellY) {
			// �������Ɉړ�
			for (int i = blankCellY; i < cy; i++) {
				board_y[i * 4 + blankCellX] = PAZZLE_BOARD_HEIGHT / 4;
				board[i * 4 + blankCellX] = board[(i + 1) * 4 + blankCellX];
			}
			board[cy * 4 + blankCellX] = 15;
			return true;
		}
		else if (cy < blankCellY) {
			// �ォ�牺�Ɉړ�
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

// �Ֆʂ��������������肷��
bool is_complete_board()
{
	for (int i = 0; i < 16; i++) {
		if (board[i] != i) {
			return false;
		}
	}
	return true;
}

// �Ֆʂ̃A�j���[�V�������s��
bool animate_board()
{
	// �^�C���̂���ʂ��O�ɋ߂Â���
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

	// ���ׂĂ̂���ʂ��O�ɂȂ�����A�j���[�V��������
	for (int i = 0; i < 16; i++) {
		if (board_x[i] || board_y[i]) {
			return true;
		}
	}
	return false;
}

// �Q�[���̃��C������
void start(void)
{
	// ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
	Video.SetSize(512, 512);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 50-15Puzzle");

	// ���ʉ������[�h
	Sound.ChannelCreate(0, "./Sound/beep.mp3");
	
	// TrueType�t�H���g�����[�h����"ipa"�Ƃ������O�����蓖��
	Video.CreateFont("ipa", "./Font/IPAexfont00301/ipaexg.ttf");
	// ���O"ipa"���w�肵�ĕ`��Ɏg�p����t�H���g�Ɏw��
	Video.FontFace("ipa");

	// �摜�t�@�C����ǂݍ��݁A�T�C�Y���擾
	imageId = Video.CreateImage("./Image/Image.png", IMAGE_RESIZE_NEAREST);
	Video.ImageSize(imageId, &imageWidth, &imageHeight);

	// �Ֆʍ쐬
	init_board();

	// �{�^�������J�E���^��p�ӂ��ď�����
	int btn = 0;

	while (Video.Drawing())
	{
		// �Ֆʕ`��
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, 1.0f);

		Video.BeginPath();
		Video.FillColor(Video.RGBA(255, 255, 255, 128));
		Video.Rect(0, PAZZLE_BOARD_HEIGHT / 2 - 24 / 2, PAZZLE_BOARD_WIDTH, 24);
		Video.Fill();
		Video.ClosePath();

		// ���b�Z�[�W�\��
		Video.BeginPath();
		Video.FontSize(24);
		Video.FillColor(Video.RGB(0, 0, 0));
		Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);
		Video.Text( PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2, "Click To Start", NULL);
		Video.Fill();
		Video.ClosePath();

		// �N���b�N���ꂽ��J��
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

	// �X�e�[�W�J�n�O�̃A�j���[�V����
	double start = Video.GetTime();
	while (Video.Drawing())
	{
		// �Ֆʕ`��
		draw_board();

		// ��Ɍ��摜��\��
		float diff = 1.0f - (float)min(1.0, Video.GetTime() - start);
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, diff);

		// ���b�Z�[�W�\��
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

		// 1�b�o�߂�����J��
		if (Video.GetTime() - start >= 1.0) {

			break;
		}

	}


	// �Q�[����
	start = Video.GetTime();
	int step = 0;
	while (Video.Drawing())
	{
		float diff = 1.0f - (float)min(1.0, Video.GetTime() - start);

		// �Ֆʕ`��
		draw_board();

		if (diff > 0)
		{
			// ���b�Z�[�W�\��
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
				// �N���b�N���ꂽ��p�l���𓮂���
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

	// �N���A��ʑO�̃A�j���[�V����
	start = Video.GetTime();
	while (Video.Drawing())
	{
		// �Ֆʕ`��
		draw_board();

		// ��Ɍ��摜��\��
		float diff = (float)min(1.0, Video.GetTime() - start);
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, diff);

		// 1�b�o�߂�����J��
		if (Video.GetTime() - start >= 1.0) {
			break;
		}

	}

	// �N���A��ʑO�̃A�j���[�V����(2)
	start = Video.GetTime();
	while (Video.Drawing())
	{
		// �Ֆʕ`��
		draw_board();

		// ��Ɍ��摜��\��
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, 1.0);

		float diff = (float)min(1.0, Video.GetTime() - start);
		Video.BeginPath();
		Video.FillColor(Video.RGBA(255, 255, 255, 128 * diff));
		Video.Rect(0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT);
		Video.Fill();
		Video.ClosePath();

		// 2�b�o�߂�����J��
		if (Video.GetTime() - start >= 1.0) {

			break;
		}

	}

	// �N���A���
	while (Video.Drawing())
	{
		// ���摜��\��
		Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0f, 1.0f);

		Video.BeginPath();
		Video.FillColor(Video.RGBA(255, 255, 255, 128));
		Video.Rect(0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT);
		Video.Fill();
		Video.ClosePath();

		// ���b�Z�[�W�\��
		Video.BeginPath();
		Video.FillColor(Video.RGB(0, 0, 0));
		Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);

		Video.FontSize(48);
		Video.Text(PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2 - 50, "Complete!", NULL);

		Video.FontSize(24);
		Video.FormatText(PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2 + 50, "�N���A�萔 %d ��", step);

		Video.ClosePath();

		// �N���b�N���ꂽ��J��
		if (Mouse.IsLeftButtonDown()) {
			btn++;
		}
		else
		{
			btn = 0;
		}
		if (btn == 1) { break; }

	}

	// ����ȏ�Ȃ��̂ŏI��
}
