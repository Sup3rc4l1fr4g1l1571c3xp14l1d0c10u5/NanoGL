require 'NanoGL'
include NanoGL

# ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
Video.SetSize(512, 512)

# �^�C�g����ݒ�
Video.SetWindowTitle("[Example] 04-Image")

# ��ʂ����������ۂ̔w�i�F��ݒ�
Video.SetClearColor([128,128,255])

# �v���O�����̒u���Ă���f�B���N�g������Ƃ���
# image �t�H���_���� Image.png �Ƃ����t�@�C����ǂݍ��݁A�摜ID�� image �ɐݒ肷��
# �摜�I�v�V�����͎w�肵�Ȃ��̂� 0 ������
image = Video.CreateImage("./image/Image.png", 0)

# �摜�T�C�Y���擾���ĕϐ� width �� �ϐ� height �ɓ����
(width, height) = Video.ImageSize(image)

# �X�V����
Video.Draw do
	Video.DrawImage(image, 0, 0, width, height, 256-64, 256-64, 128, 128, 0, 1.0)
end

