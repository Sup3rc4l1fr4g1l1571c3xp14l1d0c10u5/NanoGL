require 'NanoGL'
include NanoGL

class Ball
	attr_reader :image	# �{�[���̉摜��ID
	attr_reader :x	# �{�[����X���W
	attr_reader :y	# �{�[����Y���W
	attr_reader :sx	# �{�[����X�����ړ����x
	attr_reader :sy	# �{�[����Y�����ړ����x

	@@image = nil

	def initialize(x,y,sx,sy)
		if @@image == nil
			# �v���O�����̒u���Ă���f�B���N�g������Ƃ���
			# image �t�H���_���� ball.png �Ƃ����t�@�C����ǂݍ��݁A�摜ID�� img_ball�ɐݒ肷��
			# �摜�I�v�V�����͎w�肵�Ȃ��̂� 0 ������
			@@image = Video.CreateImage("./image/ball.png", 0)

			# �摜�T�C�Y���擾���ĕϐ� width �� �ϐ� height �ɓ����
			(@@width, @@height) = Video.ImageSize(@@image)

		end
		@x = x
		@y = y
		@sx = sx
		@sy = sy
	end

	def update
		# �{�[���̌��݈ʒu�Ɉړ����x�����Z���Ď��̈ʒu�Ɉړ�
		@x += @sx
		@y += @sy

		#��ʂ���͂ݏo�����Ƃ��Ă���{�[���͈ړ����x�̐����𔽓]�����邱�Ƃňړ��������t�ɂ��ĕǂŔ��˂����悤�Ɍ�����

		# ��ʂ̍��E����͂ݏo���Ă���ꍇ
		if @x < 0 || 512 <= @x
			@sx = @sx * -1
		end

		# ��ʂ̏㉺����͂ݏo���Ă���ꍇ
		if @y < 0 || 512 <= @y
			@sy = @sy * -1
		end
	end
	
	def draw
		Video.DrawImage(@@image, 0, 0, @@width, @@height, @x - @@width / 2, @y - @@height / 2, @@width, @@height, 0, 1.0)
	end
end

# ��ʃT�C�Y���c�� 512 �s�N�Z�� �� �ݒ�
Video.SetSize(512, 512)

# �^�C�g����ݒ�
Video.SetWindowTitle("[Example] 08-Sprite")

# �{�[���𐶐����Ĉʒu�ƈړ�������ݒ肷��
balls = []
100.times do |i|
	rad = Video.DegToRad(360.0 * i / 100)
	balls << Ball.new(512 / 2, 512 / 2, Math.cos(rad) * 2, Math.sin(rad) * 2)
end

# ����
Video.Draw do
	# �{�[���ЂƂÂړ������ƕ`����s��
	balls.each(&:update)
	balls.each(&:draw)
end

