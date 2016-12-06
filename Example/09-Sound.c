#include <NanoGL/NanoGL.h>

// �Q�[���̃��C������
void start(void)
{
	// ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
	Video.SetSize(512, 512);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 09-Sound");

	// ��ʂ����������ۂ̔w�i�F��ݒ�
	Video.SetClearColor(Video.RGB(128,128,255));

	// �v���O�����̒u���Ă���f�B���N�g������Ƃ���
	// Sound �t�H���_���� bgm.ogg �Ƃ����t�@�C���������ԍ�0�Ԃɓǂݍ���
	Sound.ChannelCreate(0, "./Sound/bgm.ogg");

	// �v���O�����̒u���Ă���f�B���N�g������Ƃ���
	// Sound �t�H���_���� beep.mp3 �Ƃ����t�@�C���������ԍ�1�Ԃɓǂݍ���
	Sound.ChannelCreate(1, "./Sound/beep.mp3");

	// BGM���Đ�
	// ���[�v�Đ�������̂Ń��[�v�t���O�� true �Ƃ���
	Sound.ChannelPlay(0, true);

	// �ЂƂO�̎��_�ł̃}�E�X�{�^���̏�Ԃ����Ă����ϐ���p��
	bool IsLeftButtonDowned = false;

	// ����
	while (Video.Drawing()) {

		// �}�E�X���N���b�N���ꂽ�i����������ꂽ�u�ԁj�������ʉ���炷
		if (Mouse.IsLeftButtonDown() && IsLeftButtonDowned == false)
		{
			// ���ʉ����Đ�
			// ���[�v�����Ȃ��̂Ń��[�v�t���O�� false �Ƃ���
			Sound.ChannelPlay(1, false);
		}
		IsLeftButtonDowned = Mouse.IsLeftButtonDown();

	}

	// �������I�����̂ŉ������邱�Ƃ��Ȃ�

}
