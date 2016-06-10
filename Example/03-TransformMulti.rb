require 'NanoGL'
include NanoGL

# ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
Video.SetSize(512, 512)

# �^�C�g����ݒ�
Video.SetWindowTitle("[Example] 03-TransformMulti")

# ��ʂ����������ۂ̔w�i�F��ݒ�
Video.SetClearColor([128,128,255])

# �X�V����
Video.Draw do

	#
	# �ό`�͕����̐}�`�ɂ��K�p�ł���
	#

	# �}�`���`
	Path do
		# �ό`�̃��Z�b�g
		ResetTransform()

		# �R�b�łP��]����悤�Ɍ��݂̉�]�p�x���v���O�����N������̌o�ߎ��ԂŎZ�o
		degree = 360.0 * Video.GetTime() / 3.0	

		# �p�x�ɍ��킹�đ傫�����ω�����悤��sin�֐��ŃX�P�[�����Z�o
		scale = Math.sin(Video.DegToRad(degree)) * 0.5 + 1.0	

		Translate(256, 256)					# 4�̏���
		Rotate(Video.DegToRad(degree))		# 3�̏���
		Scale(scale, scale)					# 2�̏���
		Translate(-256, -256)				# 1�̏���

		# ��ʏ�ɂS�̐����`��z�u����B
		# ��Ŏw�肵���ό`���K�p�����̂ŁA���ۂɂ͉�ʒ����𒆐S�ɂ��ꂼ��̎l�p�`�͐��񂵂��܂܉�]����
		Rect(256 - 128 - 32, 256 - 128 - 32, 64, 64)	
		Rect(256 + 128 - 32, 256 - 128 - 32, 64, 64)	
		Rect(256 - 128 - 32, 256 + 128 - 32, 64, 64)	
		Rect(256 + 128 - 32, 256 + 128 - 32, 64, 64)	
	end

	# �h��Ԃ��ŗp����F�� ��255, ��0, ��0, �s�����x255 �ɐݒ�
	FillColor([255, 0, 0, 255])	

	# �}�`��`��
	Fill()	

end
