#include <NanoGL/NanoGL.h>

// �Q�[���̃��C������
void start(void)
{
	// ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
	Video.SetSize(512, 512);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 03-TransformMulti");

	// ����
	while (Video.Drawing()) {

		//
		// �ό`�͕����̐}�`�ɂ��K�p�ł���
		//

		Video.BeginPath();

		Video.ResetTransform();// �ό`�̃��Z�b�g

		// �R�b�łP��]����悤�Ɍ��݂̉�]�p�x���v���O�����N������̌o�ߎ��ԂŎZ�o
		double degree = 360.0 * Video.GetTime() / 3.0;

		// �p�x�ɍ��킹�đ傫�����ω�����悤��sin�֐��ŃX�P�[�����Z�o
		double scale = sin(Video.DegToRad(degree)) * 0.5 + 1.0;

		Video.Translate(256, 256);				// 4�̏���
		Video.Rotate(Video.DegToRad(degree));	// 3�̏���
		Video.Scale(scale, scale);				// 2�̏���
		Video.Translate(-256, -256);			// 1�̏���

		// ��ʏ�ɂS�̐����`��z�u����B

		// ��Ŏw�肵���ό`���K�p�����̂ŁA���ۂɂ͉�ʒ����𒆐S�ɂ��ꂼ��̎l�p�`�͐��񂵂��܂܉�]����
		Video.Rect(256 - 128 - 32, 256 - 128 - 32, 64, 64);
		Video.Rect(256 + 128 - 32, 256 - 128 - 32, 64, 64);
		Video.Rect(256 - 128 - 32, 256 + 128 - 32, 64, 64);
		Video.Rect(256 + 128 - 32, 256 + 128 - 32, 64, 64);

		// �}�`�̒�`���I��
		Video.ClosePath();

		// �h��Ԃ��ŗp����F�� ��255, ��0, ��0, �s�����x255 �ɐݒ�
		Video.FillColor(Video.RGBA(255, 0, 0, 255));

		// �}�`��`��
		Video.Fill();

	}

	// �������I�����̂ŉ������邱�Ƃ��Ȃ�

}
