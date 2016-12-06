require 'NanoGL'
include NanoGL

# ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
Video.SetSize(512, 512)

# �^�C�g����ݒ�
Video.SetWindowTitle("[Example] 05-Keyboard")

# ��ʂ����������ۂ̔w�i�F��ݒ�
Video.SetClearColor(Video.RGB(128,128,255))

# �摜���S���W���i�[����ϐ���p�ӂ��A�����ʒu����ʒ��S�Ƃ���
x = 512 / 2.0
y = 512 / 2.0

# �摜�̉�]�p�x�ƃT�C�Y������ϐ���p�ӂ���
degree = 0.0
scale = 1.0

# �v���O�����̒u���Ă���f�B���N�g������Ƃ���
# image �t�H���_���� Image	png �Ƃ����t�@�C����ǂݍ��݁A�摜ID�� image �ɐݒ肷��
# �摜�I�v�V�����͎w�肵�Ȃ��̂� 0 ������
image = Video.CreateImage("./image/Image.png", 0)

# �摜�T�C�Y���擾���ĕϐ� width �� �ϐ� height �ɓ����
(width, height) = Video.ImageSize(image)

# �X�V����
Video.Draw do

	# �L�[���͂����o
	if Keyboard.IsKeyDown(Keyboard::KeyCode::KBKEY_UP)
		# ��L�[��������Ă���̂ŉ�ʏ������Y���W���X�V
		y -= 1
	end
	if Keyboard.IsKeyDown(Keyboard::KeyCode::KBKEY_DOWN)
		# ���L�[��������Ă���̂ŉ�ʉ�������Y���W���X�V
		y += 1
	end
	if Keyboard.IsKeyDown(Keyboard::KeyCode::KBKEY_LEFT)
		# ���L�[��������Ă���̂ŉ�ʍ�������X���W���X�V
		x -= 1
	end
	if Keyboard.IsKeyDown(Keyboard::KeyCode::KBKEY_RIGHT)
		# �E�L�[��������Ă���̂ŉ�ʉE������X���W���X�V
		x += 1
	end
	if Keyboard.IsKeyDown('Z')
		# Z�L�[��������Ă���̂Ŕ����v���ɉ�]
		degree -= 1
	end
	if Keyboard.IsKeyDown('X')
		# X�L�[��������Ă���̂Ŏ��v���ɉ�]
		degree += 1
	end
	if Keyboard.IsKeyDown('A')
		# A�L�[��������Ă���̂Ŋg�傷��
		scale = [scale+0.1, 5].min
	end
	if Keyboard.IsKeyDown('S')
		# X�L�[��������Ă���̂ŏk������
		scale = [scale-0.1, 0.1].max
	end

	# �摜�ʒu(x,y)�ɕό`���݂ŕ`�悷��
	Video.Path do
		Video.Reset()
		Video.Translate(x, y)
		Video.Rotate(Video.DegToRad(degree))
		Video.Scale(scale, scale)
		Video.DrawImage(image, 0, 0, width, height, -64, -64, 128, 128, 0, 1)
	end
end

