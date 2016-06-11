require 'NanoGL'
include NanoGL

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 06-Mouse")

# 画面を消去した際の背景色を設定
Video.SetClearColor([128,128,255])

# 更新処理
Video.Draw do
	# マウスのカーソル位置を取得
	(x, y) = Mouse.GetCursorPos()

	# マウスの押下に応じて色を変える
	if Mouse.IsLeftButtonDown()
		# 左ボタンが押されている
		color = [255, 0, 0, 255]
	elsif Mouse.IsRightButtonDown()
		# 右ボタンが押されている
		color = [0, 0, 255, 255]
	elsif Mouse.IsMiddleButtonDown()
		# 真ん中ボタンが押されている
		color = [0, 255, 0, 255]
	else
		# なにもボタンが押されていない
		color = [255, 255, 255, 255]
	end

	# マウスのカーソル位置に四角形を表示する
	Video.Path do
		Video.Rect(x - 25, y - 25, 50, 50);
	end
	Video.FillColor(color)
	Video.Fill()
end

