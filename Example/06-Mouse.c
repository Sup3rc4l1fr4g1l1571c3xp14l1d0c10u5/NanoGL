#include <NanoGL/NanoGL.h>

// �Q�[���̃��C������
void start(void)
{
	// ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
	Video.SetSize(512, 512);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 06-Mouse");

	// ����
	while (Video.Drawing()) {
		// �}�E�X�̃J�[�\���ʒu���擾
		double x, y;
		Mouse.GetCursorPos(&x, &y);

		// �}�E�X�̃J�[�\���ʒu�Ɏl�p�`��\������
		Video.BeginPath();
		Video.Rect(x - 25, y - 25, 50, 50);
		Video.ClosePath();

		// �}�E�X�̉����ɉ����ĐF��ς���
		if (Mouse.IsLeftButtonDown()) {
			// ���{�^����������Ă���
			Video.FillColor(Video.RGBA(255, 0, 0, 255));
		}
		else if (Mouse.IsRightButtonDown()) {
			// �E�{�^����������Ă���
			Video.FillColor(Video.RGBA(0, 0, 255, 255));
		}
		else if (Mouse.IsMiddleButtonDown()) {
			// �^�񒆃{�^����������Ă���
			Video.FillColor(Video.RGBA(0, 255, 0, 255));
		}
		else
		{
			// �Ȃɂ��{�^����������Ă��Ȃ�
			Video.FillColor(Video.RGBA(255, 255, 255, 255));
		}

		Video.Fill();
	}

	// �������I�����̂ŉ������邱�Ƃ��Ȃ�

}
