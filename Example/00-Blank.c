#include <NanoGL/NanoGL.h>

// �Q�[���̃��C������
void start(void)
{
	// ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
	Video.SetSize(512, 512);

	// �^�C�g����ݒ�
	Video.SetWindowTitle("[Example] 00-Blank");

	// ��ʂ����������ۂ̔w�i�F��ݒ�
	Video.SetClearColor(Video.RGB(0,255,0));

	// ����
	while (Video.Drawing()) {
	}
	
	// �������I�����̂ŉ������邱�Ƃ��Ȃ�

}
