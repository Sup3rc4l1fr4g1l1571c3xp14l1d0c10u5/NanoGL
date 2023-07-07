# 文字コードをUTF8にしてファイルを保存しないと
# 日本語を含む文字列は正常に表示されません

require 'NanoGL'
include NanoGL

# パズルの盤面の大きさ
PAZZLE_BOARD_WIDTH   = 512
PAZZLE_BOARD_HEIGHT  = 512

# スライドアニメの速度
SLIDE_SPEED_X = (PAZZLE_BOARD_WIDTH / 32).to_i
SLIDE_SPEED_Y = (PAZZLE_BOARD_HEIGHT / 32).to_i

# 盤面上のタイルを管理する
#
# 以下のように盤面のマスには番号が振られている
# ┌─┬─┬─┬─┐
# │ 0│ 1│ 2│ 3│
# ├─┼─┼─┼─┤
# │ 4│ 5│ 6│ 7│
# ├─┼─┼─┼─┤
# │ 8│ 9│10│11│
# ├─┼─┼─┼─┤
# │12│13│14│15│
# └─┴─┴─┴─┘
#
# 9番のマスにあるタイルを読み書きしたい場合は @tiles[9] を読み書きする
#

class Tile
	attr_reader :num	# タイルの数字
	# タイルがマスの中心から上下左右にどれだけずれているかを管理する
	# アニメーション処理で使う
	attr_reader :x	# 値が 0 の場合は 左右どちらにもずれていない　負の場合はタイルは左にずれている、正の場合はタイルは右にずれている
	attr_reader :y	# 値が 0 の場合は 上下どちらにもずれていない　負の場合はタイルは上にずれている、正の場合はタイルは下にずれている

	def initialize(num)
		@num = num
		@x = 0
		@y = 0
	end
	
	def set_animate(x,y)
		@x = x
		@y = y
	end
	
	def animate()
		if @x == 0 && @y == 0
			return false
		end

		# タイルのずれ量を０に近づける
		if (@x > 0) 
			@x = [@x - SLIDE_SPEED_X, 0].max
		elsif (@x < 0) 
			@x = [@x + SLIDE_SPEED_X, 0].min
		end
		if (@y > 0) 
			@y = [@y - SLIDE_SPEED_Y, 0].max
		elsif (@y < 0) 
			@y = [@y + SLIDE_SPEED_Y, 0].min
		end

		return true
	end
end

class Board
	def initialize(imageId, imageWidth, imageHeight)
		# タイルの画像のIDと幅、高さ
		@imageId = imageId
		@imageWidth = imageWidth
		@imageHeight = imageHeight
		@tiles = Array.new(16) { |i| Tile.new(i) }
		@x = 0
		@y = 0
	end

	#
	# 盤面を作成する。
	# ランダムに初期配置を作成すると解けない盤面となることがあるので
	# 解けない盤面のときは解ける盤面になるよう操作する
	# 
	def reset
		# 盤面上に0から15(空き)までのタイルをセルの番号順に並べる
		@tiles = Array.new(16) { |i| Tile.new(i) }

		# シャッフルを行う
		@tiles.shuffle!

		# シャッフル後のタイルの配置が元の状態に戻る（つまり答えがあるか？）を判定
		# 置換を用いた解説は http:#hp.vector.co.jp/authors/VA010128/math/puzzle/P15-1.html
		sum = 0
		16.times do |i|
			base = @tiles[i].num
			if base == 15
				sum += (i / 4).to_i + 1
				next
			else
				(i+1).upto(15) do |j|
					if @tiles[j].num == 15 
						next
					end
					sum += ((base > @tiles[j].num) ? 1 : 0)
				end
			end
		end

		# 答えのないタイル配置の場合、タイルの配置を左右反転させると答えのある配置になることが知られている
		# (逆に答えのあるタイルの配置を左右反転させると解けなくなる)
		if (sum % 2) == 1 
			4.times do |y|
				2.times do |x|
					@tiles[y * 4 + x], @tiles[y * 4 + (3 - x)] = @tiles[y * 4 + (3 - x)], @tiles[y * 4 + x]
				end
			end
		end
	end

	# 盤面を画面上に描画する
	def draw
		Video.Path do
			Video.Rect(0.0, 0.0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT)
		end
		Video.FillColor(Video.RGB(128, 128, 128))
		Video.Fill()
		
		@tiles.each_with_index do |tile, i|
			v = tile.num

			scx = v % 4
			scy = (v / 4).to_i
			sx  = (scx * @imageWidth / 4).to_i
			sy  = (scy * @imageHeight / 4).to_i
			sw  = ((scx + 1) * @imageWidth / 4).to_i - sx
			sh  = ((scy + 1) * @imageHeight / 4).to_i - sy

			dcx = i % 4
			dcy = (i / 4).to_i
			dx  = (dcx * PAZZLE_BOARD_WIDTH / 4).to_i
			dy  = (dcy * PAZZLE_BOARD_HEIGHT / 4).to_i
			dw  = ((dcx + 1) * PAZZLE_BOARD_WIDTH / 4).to_i - dx
			dh  = ((dcy + 1) * PAZZLE_BOARD_HEIGHT / 4).to_i - dy

			if (v != 15) 
				Video.DrawImage(@imageId, sx, sy, sw, sh, dx + tile.x, dy + tile.y, dw, dh, 0.0, 1.0)
			end
		end
	end

	# 盤面をクリックされたときにタイルを動かす
	def click(x, y)
		x = x.to_i
		y = y.to_i
		cx = (x * 4 / PAZZLE_BOARD_WIDTH).to_i
		cy = (y * 4 / PAZZLE_BOARD_HEIGHT).to_i
		if cx < 0 || 4 <= cx || cy < 0 || 4 <= cy
			return false
		end

		blankTileIndex = @tiles.index { |t| t.num == 15 }
		blankCellX = (blankTileIndex % 4)
		blankCellY = (blankTileIndex / 4).to_i
		blankTile = @tiles[blankTileIndex]

		if cx != blankCellX && cy == blankCellY 
			# 空白セルと水平方向が一致するセルを移動
			if cx > blankCellX 
				# 右から左に移動
				blankCellX.upto(cx-1) do |i|
					@tiles[blankCellY * 4 + i + 1].set_animate( (PAZZLE_BOARD_HEIGHT / 4).to_i, 0)
					@tiles[blankCellY * 4 + i] = @tiles[blankCellY * 4 + i + 1]
				end
				@tiles[blankCellY * 4 + cx] = blankTile
				return true
			elsif cx < blankCellX
				# 左から右に移動
				blankCellX.downto(cx+1) do |i|
					@tiles[blankCellY * 4 + i - 1].set_animate(-(PAZZLE_BOARD_HEIGHT / 4).to_i, 0)
					@tiles[blankCellY * 4 + i] = @tiles[blankCellY * 4 + i - 1]
				end
				@tiles[blankCellY * 4 + cx] = blankTile
				return true
			end
		elsif cx == blankCellX && cy != blankCellY
			# 空白セルと垂直方向が一致するセルを移動
			if cy > blankCellY
				# 下から上に移動
				blankCellY.upto(cy-1) do |i|
					@tiles[(i + 1) * 4 + blankCellX].set_animate(0,  (PAZZLE_BOARD_HEIGHT / 4).to_i)
					@tiles[i * 4 + blankCellX] = @tiles[(i + 1) * 4 + blankCellX]
				end
				@tiles[cy * 4 + blankCellX] = blankTile
				return true
			elsif cy < blankCellY
				# 上から下に移動
				blankCellY.downto(cy+1) do |i|
					@tiles[(i - 1) * 4 + blankCellX].set_animate(0, -(PAZZLE_BOARD_HEIGHT / 4).to_i)
					@tiles[i * 4 + blankCellX] = @tiles[(i - 1) * 4 + blankCellX]
				end
				@tiles[cy * 4 + blankCellX] = blankTile
				return true
			end
		end
		return false
	end

	# 盤面が完成したか判定する
	def complete?
		16.times.all? { |i| @tiles[i].num == i }
	end

	# 盤面のアニメーションを行う
	def animate
		# すべてのタイルがアニメしなければアニメーション完了
		return @tiles.inject(false) { |s, tile| s | tile.animate }
	end
end

#
# ゲームのメイン処理
#

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 50-15Puzzle")

# 効果音をロード
Sound.ChannelCreate(0, "./Sound/beep.mp3")

# TrueTypeフォントをロードして"ipa"という名前を割り当て
Video.CreateFont("ipa", "./Font/IPAexfont00301/ipaexg.ttf")
# 名前"ipa"を指定して描画に使用するフォントに指定
Video.FontFace("ipa")

# 画像ファイルを読み込み、サイズを取得
imageId = Video.CreateImage("./Image/Image.png", Video::ImageFlags::IMAGE_RESIZE_NEAREST)
(imageWidth, imageHeight) = Video.ImageSize(imageId)

# 盤面作成
board = Board.new(imageId, imageWidth, imageHeight)
board.reset()

# ボタン押下カウンタを用意して初期化
btn = 0

Video.Draw do
	# 盤面描画
	Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0, 1.0)

	Video.Path do
		Video.FillColor(Video.RGBA(255, 255, 255, 128))
		Video.Rect(0, (PAZZLE_BOARD_HEIGHT / 2).to_i - 24 / 2, PAZZLE_BOARD_WIDTH, 24)
		Video.Fill()
	end

	# メッセージ表示
	Video.Path do
		Video.FontSize(24)
		Video.FillColor(Video.RGB(0, 0, 0))
		Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)
		Video.Text( (PAZZLE_BOARD_WIDTH / 2).to_i, (PAZZLE_BOARD_HEIGHT / 2).to_i, "Click To Start")
		Video.Fill()
	end

	# クリックされたら遷移
	if Mouse.IsLeftButtonDown()
		btn += 1
	else
		btn = 0
	end

	if (btn == 1)
		Sound.ChannelPlay(0, false)
		break
	end
end

# ステージ開始前のアニメーション
start = Video.GetTime()
Video.Draw do
	# 盤面描画
	board.draw()

	# 上に元画像を表示
	diff = 1.0 - [1.0, Video.GetTime() - start].min
	Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0, diff)

	# メッセージ表示
	Video.Path do
		Video.FillColor(Video.RGB(0, 255, 0))
		Video.FontSize(80)
		Video.Save()
		Video.Translate((PAZZLE_BOARD_WIDTH / 2).to_i, (PAZZLE_BOARD_HEIGHT / 2).to_i)
		Video.Scale(diff, diff)
		Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)
		Video.Text(0, 0, "Ready?")
		Video.Restore()
	end

	# 1秒経過したら遷移
	if (Video.GetTime() - start >= 1.0) 

		break
	end
end


# ゲーム中
start = Video.GetTime()
step = 0
Video.Draw do
	diff = 1.0 - [1.0, Video.GetTime() - start].min

	# 盤面描画
	board.draw()

	if (diff > 0)
		# メッセージ表示
		Video.Path do
			Video.FillColor(Video.RGBA(0, 255, 0, (diff * 255).to_i))
			Video.FontSize(80)
			Video.Save()
			Video.Translate((PAZZLE_BOARD_WIDTH / 2).to_i, (PAZZLE_BOARD_HEIGHT / 2).to_i)
			Video.Scale((1 - diff) + 1, (1 - diff) + 1)
			Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)
			Video.Text(0, 0, "Go!")
			Video.Restore()
		end
	end

	if (Mouse.IsLeftButtonDown()) 
		btn += 1
	else
		btn = 0
	end

	if (board.animate == false)
		if (board.complete?)
			break
		elsif (btn == 1)
			# クリックされたらパネルを動かす
			(xpos, ypos) = Mouse.GetCursorPos()
			if (board.click(xpos, ypos))
				step += 1
				Sound.ChannelPlay(0, false)
			end
		end
	end

end

# クリア画面前のアニメーション
start = Video.GetTime()
Video.Draw do
	# 盤面描画
	board.draw()

	# 上に元画像をフェード付きで表示
	diff = [1.0, Video.GetTime() - start].min
	Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0, diff)

	# 1秒経過したら遷移
	if (Video.GetTime() - start >= 1.0)
		break
	end

end

# クリア画面前のアニメーション(2)
start = Video.GetTime()
Video.Draw do
	# 盤面描画
	board.draw()

	# 上に元画像を表示
	Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0, 1.0)

	diff = [1.0, Video.GetTime() - start].min
	Video.Path do
		Video.FillColor(Video.RGBA(255, 255, 255, (128 * diff).to_i))
		Video.Rect(0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT)
		Video.Fill()
	end

	# 2秒経過したら遷移
	if (Video.GetTime() - start >= 1.0)
		break
	end

end

# クリア画面
Video.Draw do
	# 元画像を表示
	Video.DrawImage(imageId, 0, 0, imageWidth, imageHeight, 0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT, 0.0, 1.0)

	Video.Path do
		Video.FillColor(Video.RGBA(255, 255, 255, 128))
		Video.Rect(0, 0, PAZZLE_BOARD_WIDTH, PAZZLE_BOARD_HEIGHT)
		Video.Fill()
	end

	# メッセージ表示
	Video.Path do
		Video.FillColor(Video.RGBA(0, 0, 0))
		Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)

		Video.FontSize(48)
		Video.Text(PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2 - 50, "Complete!")

		Video.FontSize(24)
		Video.Text(PAZZLE_BOARD_WIDTH / 2, PAZZLE_BOARD_HEIGHT / 2 + 50, "クリア手数 #{step} 回")
	end

	# クリックされたら遷移
	if (Mouse.IsLeftButtonDown())
		btn += 1
	else
		btn = 0
	end
	
	if (btn == 1) 
		break
	end

end

