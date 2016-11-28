#include <NanoGL/NanoGL.h>

// �Q�[���̃��C������
void start(void)
{
	// ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
	Video.SetSize(512, 512);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 11-Dialog");

	// ��ʂ����������ۂ̔w�i�F��ݒ�
	Video.SetClearColor(Video.RGBA(128, 128, 255, 255));

	// �v���O�����̒u���Ă���f�B���N�g������Ƃ���
	// Font�t�H���_����IPAexfont00301�t�H���_����ipaexg.ttf�Ƃ����t�H���g�t�@�C����ǂݍ��݁A"font"�Ƃ������O��t����
	Video.CreateFont("font", "./Font/IPAexfont00301/ipaexg.ttf");

	// �ǂݍ��񂾃t�H���g�����ݗ��p����t�H���g�ɐݒ�
	Video.FontFace("font");

	// �t�@�C���������镶����
	string_t filename = String.Create(NULL);

	// ����
	while (Video.Drawing()) {
		// �}�E�X���N���b�N�Ńt�@�C�����J���_�C�A���O�{�b�N�X��\��
		if (Mouse.IsLeftButtonDown()) {
			String.Free(filename);
			filename = Dialog.OpenFileDialog("�t�@�C����I��ł�������", "");
		}
		// �}�E�X�E�N���b�N�Ńt�@�C���ۑ��_�C�A���O�{�b�N�X��\��
		if (Mouse.IsRightButtonDown()) {
			String.Free(filename);
			filename = Dialog.SaveFileDialog("�ۑ���t�@�C��������͂��Ă�������", "");
		}

		// �e�L�X�g�`��͐}�`�ł͂Ȃ��̂�FillColor���߂Ńe�L�X�g�F���w�肵�Ă���e�L�X�g�`�施�߂�p����
		Video.FontSize(12);								// �t�H���g�̑傫��������12pt�ɐݒ�
		Video.FillColor(Video.RGBA(0, 0, 255, 255));	// �e�L�X�g�̐F��ΐF�ɐݒ�
		Video.TextAlign(ALIGN_CENTER | ALIGN_MIDDLE);	// ��ʒu�ɕ�����̏c�����S������悤�ɐݒ�
		if (filename.c_str != NULL) {
			Video.Text(512 / 2, 512 / 2, filename.c_str, NULL);
		}

		Video.Fill();
	}

	// �������I�����̂ŉ������邱�Ƃ��Ȃ�

}
