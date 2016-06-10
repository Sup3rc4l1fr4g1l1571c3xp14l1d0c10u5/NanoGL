#include <NanoGL/NanoGL.h>

// �{�[���̏���\������\����
struct ball
{
	float x;	// �{�[����X���W
	float y;	// �{�[����Y���W
	float sx;	// �{�[����X�����ړ����x
	float sy;	// �{�[����Y�����ړ����x
};

// �{�[������1000���
struct ball balls[1000];

// �Q�[���̃��C������
void start(void)
{
	// ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
	Video.SetSize(512, 512);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 08-Sprite");

	// �v���O�����̒u���Ă���f�B���N�g������Ƃ���
	// image �t�H���_���� ball.png �Ƃ����t�@�C����ǂݍ��݁A�摜ID�� img_ball�ɐݒ肷��
	// �摜�I�v�V�����͎w�肵�Ȃ��̂� 0 ������
	int img_ball = Video.CreateImage("./image/ball.png", 0);

	// �摜�T�C�Y���擾���ĕϐ� width �� �ϐ� height �ɓ����
	int width, height;
	Video.ImageSize(img_ball, &width, &height);

	// �{�[���̈ʒu�ƈړ�������ݒ肷��
	for (int i=0; i<1000; i++)
	{
		// �ړ����������W�A���ŎZ�o
		float rad = Video.DegToRad(360.0f * i / 1000);

		balls[i].x = 512 / 2;
		balls[i].y = 512 / 2;
		balls[i].sx = (float)cos(rad) * 2.0f;	// �ړ����������ɉ������ړ��ʂ�cos�֐��ŎZ�o���A���x����Z
		balls[i].sy = (float)sin(rad) * 2.0f;	// �ړ����������ɏc�����ړ��ʂ�sin�֐��ŎZ�o���A���x����Z
	}

	// ����
	while (Video.Drawing()) {

		// �{�[���ЂƂÂړ��������s��
		for (int i = 0; i<1000; i++)
		{
			// �{�[���̌��݈ʒu�Ɉړ����x�����Z���Ď��̈ʒu�Ɉړ�
			balls[i].x = balls[i].x + balls[i].sx;
			balls[i].y = balls[i].y + balls[i].sy;

			//��ʂ���͂ݏo�����Ƃ��Ă���{�[���͕����𔽓]�����邱�Ƃňړ��������t�ɂ��ĕǂŔ��˂����悤�Ɍ�����

			// ��ʂ̍��E����͂ݏo���Ă���ꍇ
			if ((balls[i].x < 0) || (512 <= balls[i].x))
			{
				balls[i].sx = balls[i].sx * -1;
			}

			// ��ʂ̏㉺����͂ݏo���Ă���ꍇ
			if ((balls[i].y < 0) || (512 <= balls[i].y))
			{
				balls[i].sy = balls[i].sy * -1;
			}
		}


		// �{�[���ЂƂÂ`�悷��
		for (int i = 0; i < 1000; i++)
		{
			// �{�[���̈ʒu�Ƀ{�[���̉摜�̒��S������悤�Ɉʒu�𒲐����ĕ`��
			Video.DrawImage(img_ball, 0, 0, width, height, balls[i].x - width / 2, balls[i].y - height / 2, width, height, 0, 1.0f);
		}
	}

	// �������I�����̂ŉ������邱�Ƃ��Ȃ�

}
