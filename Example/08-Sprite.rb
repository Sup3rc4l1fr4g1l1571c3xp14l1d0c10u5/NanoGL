require 'NanoGL'
include NanoGL

class Ball
	attr_reader :image	# ボールの画像のID
	attr_reader :x	# ボールのX座標
	attr_reader :y	# ボールのY座標
	attr_reader :sx	# ボールのX方向移動速度
	attr_reader :sy	# ボールのY方向移動速度

	@@image = nil

	def initialize(x,y,sx,sy)
		if @@image == nil
			# プログラムの置いてあるディレクトリを基準として
			# image フォルダ内の ball.png というファイルを読み込み、画像IDを img_ballに設定する
			# 画像オプションは指定しないので 0 を入れる
			@@image = Video.CreateImage("./image/ball.png", 0)

			# 画像サイズを取得して変数 width と 変数 height に入れる
			(@@width, @@height) = Video.ImageSize(@@image)

		end
		@x = x
		@y = y
		@sx = sx
		@sy = sy
	end

	def update
		# ボールの現在位置に移動速度を加算して次の位置に移動
		@x += @sx
		@y += @sy

		#画面からはみ出そうとしているボールは移動速度の正負を反転させることで移動方向を逆にして壁で反射したように見せる

		# 画面の左右からはみ出している場合
		if @x < 0 || 512 <= @x
			@sx = @sx * -1
		end

		# 画面の上下からはみ出している場合
		if @y < 0 || 512 <= @y
			@sy = @sy * -1
		end
	end
	
	def draw
		Video.DrawImage(@@image, 0, 0, @@width, @@height, @x - @@width / 2, @y - @@height / 2, @@width, @@height, 0, 1.0)
	end
end

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 08-Sprite")

# ボールを生成して位置と移動方向を設定する
balls = []
100.times do |i|
	rad = Video.DegToRad(360.0 * i / 100)
	balls << Ball.new(512 / 2, 512 / 2, Math.cos(rad) * 2, Math.sin(rad) * 2)
end

# 処理
Video.Draw do
	# ボールひとつづつ移動処理と描画を行う
	balls.each(&:update)
	balls.each(&:draw)
end

