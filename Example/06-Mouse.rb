require 'NanoGL'
include NanoGL

# ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
Video.SetSize(512, 512)

# �^�C�g����ݒ�
Video.SetWindowTitle("[Example] 06-Mouse")

# ��ʂ����������ۂ̔w�i�F��ݒ�
Video.SetClearColor(Video.RGB(128,128,255))

# �X�V����
Video.Draw do
	# �}�E�X�̃J�[�\���ʒu���擾
	(x, y) = Mouse.GetCursorPos()

	# �}�E�X�̉����ɉ����ĐF��ς���
	if Mouse.IsLeftButtonDown()
		# ���{�^����������Ă���
		color = Video.RGBA(255, 0, 0, 255)
	elsif Mouse.IsRightButtonDown()
		# �E�{�^����������Ă���
		color = Video.RGBA(0, 0, 255, 255)
	elsif Mouse.IsMiddleButtonDown()
		# �^�񒆃{�^����������Ă���
		color = Video.RGBA(0, 255, 0, 255)
	else
		# �Ȃɂ��{�^����������Ă��Ȃ�
		color = Video.RGBA(255, 255, 255, 255)
	end

	# �}�E�X�̃J�[�\���ʒu�Ɏl�p�`��\������
	Video.Path do
		Video.Rect(x - 25, y - 25, 50, 50);
	end
	Video.FillColor(color)
	Video.Fill()
end

