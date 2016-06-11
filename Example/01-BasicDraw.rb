require 'NanoGL'
include NanoGL

# ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
Video.SetSize(512, 512)

# �^�C�g����ݒ�
Video.SetWindowTitle("[Example] 01-BasicDraw")

# ��ʂ����������ۂ̔w�i�F��ݒ�
Video.SetClearColor([128,128,255])

# �X�V����
Video.Draw do

	#
	# �l�p�`��`�悷���
	#
	
	# �}�`�̒�`
	Video.Path do
		# �}�`�Ɏl�p�`(������W100,200,��150,����50��ǉ�
		Video.Rect(100, 200, 150, 50)
	end
	
	# �h��Ԃ������ŗp����F�� ��255, ��0, ��128, �s�����x255 �ɐݒ�
	Video.FillColor([255, 0, 128, 255])
	
	# �}�`��h��Ԃ�
	Video.Fill()


	#
	# ��(�ȉ~)��`�悷���
	#

	# �}�`�̒�`
	Video.Path do
		# �}�`�ɑȉ~(���S���W300, 200, ���������a80,�c�������a50��ǉ�
		Video.Ellipse(300, 200, 80, 50)
	end
	
	# ���̕`��ŗp����F�� ��0, ��255, ��128, �s�����x255 �ɐݒ�
	Video.StrokeColor([0, 255, 128, 255])
	
	# �}�`�̗֊s����`��
	Video.Stroke()


	#
	# �O�p�`��`�悷���
	#

	# �}�`�̒�`
	Video.Path do
		# �y���̎n�_�����W (200, 50) �ɐݒ�
		Video.MoveTo(200, 50)
		# �y���̈ʒu�����W(350, 100)�ɓ������Đ�������
		Video.LineTo(350, 100)
		# �y���̈ʒu�����W(150, 200)�ɓ������Đ�������
		Video.LineTo(150, 200)
		# �}�`�̒�`���I��点��Ǝ����I�Ɍ��݈ʒu(150,200)�Ǝn�_(200, 50)�̊Ԃɐ����������
	end

	# ���̕`��ŗp����F�� ��0, ��255, ��0, �s�����x255 �ɐݒ�
	Video.StrokeColor([0, 255, 0, 255])

	# ���̑�����3pixel�ɐݒ�
	Video.StrokeWidth(3)

	# �h��Ԃ������ŗp����F�� ��255, ��255, ��255, �s�����x255 �ɐݒ�
	Video.FillColor([255, 255, 255, 255])

	# �}�`�̂�h��Ԃ�
	Video.Fill()

	# �}�`�̗֊s����`��
	Video.Stroke()

end

