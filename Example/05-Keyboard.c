#include <NanoGL/NanoGL.h>

// �Q�[���̃��C������
void start(void)
{
	// ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
	Video.SetSize(512, 512);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 05-Keyboard");

	// ��ʂ����������ۂ̔w�i�F��ݒ�
	Video.SetClearColor(Video.RGB(128,128,255));

	// �摜���S���W���i�[����ϐ���p�ӂ��A�����ʒu����ʒ��S�Ƃ���
	float x = 512 / 2.0f;
	float y = 512 / 2.0f;
	// �摜�̉�]�p�x�ƃT�C�Y������ϐ���p�ӂ���
	float degree = 0;
	float scale = 1;

	// �v���O�����̒u���Ă���f�B���N�g������Ƃ���
	// image �t�H���_���� Image.png �Ƃ����t�@�C����ǂݍ��݁A�摜ID�� image	�ɐݒ肷��
	// �摜�I�v�V�����͎w�肵�Ȃ��̂� 0 ������
	int image = Video.CreateImage("./image/Image.png", 0);

	// �摜�T�C�Y���擾���ĕϐ� width �� �ϐ� height �ɓ����
	int width, height;
	Video.ImageSize(image, &width, &height);

	// ����
	while (Video.Drawing()) {

		// �L�[���͂����o
		if (Keyboard.IsKeyDown(KBKEY_UP))
		{
			// ��L�[��������Ă���̂ŉ�ʏ������Y���W���X�V
			y -= 1.0f;
		}
		if (Keyboard.IsKeyDown(KBKEY_DOWN))
		{
			// ���L�[��������Ă���̂ŉ�ʉ�������Y���W���X�V
			y += 1.0f;
		}
		if (Keyboard.IsKeyDown(KBKEY_LEFT))
		{
			// ���L�[��������Ă���̂ŉ�ʍ�������X���W���X�V
			x -= 1.0f;
		}
		if (Keyboard.IsKeyDown(KBKEY_RIGHT))
		{
			// �E�L�[��������Ă���̂ŉ�ʉE������X���W���X�V
			x += 1.0f;
		}
		if (Keyboard.IsKeyDown('Z'))
		{
			// Z�L�[��������Ă���̂Ŕ����v���ɉ�]
			degree -= 1;
		}
		if (Keyboard.IsKeyDown('X'))
		{
			// X�L�[��������Ă���̂Ŏ��v���ɉ�]
			degree += 1;
		}
		if (Keyboard.IsKeyDown('A'))
		{
			// A�L�[��������Ă���̂Ŋg�傷��
			scale = min(scale + 0.1, 5);
		}
		if (Keyboard.IsKeyDown('S'))
		{
			// X�L�[��������Ă���̂ŏk������
			scale = max(scale - 0.1, 0.1);
		}

		// �摜�ʒu(x,y)�ɕό`���݂ŕ`�悷��
		Video.BeginPath();

		Video.Reset();
		Video.Translate(x, y);
		Video.Rotate(Video.DegToRad(degree));
		Video.Scale(scale, scale);
		Video.DrawImage(image, 0, 0, width, height, -64, -64, 128, 128, 0, 1.0f);

		Video.ClosePath();
	}

	// �������I�����̂ŉ������邱�Ƃ��Ȃ�

}
