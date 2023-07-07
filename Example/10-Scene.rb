# 文字コードをUTF8にしてファイルを保存しないと
# 日本語を含む文字列は正常に表示されません

require 'NanoGL'
include NanoGL

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 10-Scene")

# 画面を消去した際の背景色を設定
Video.SetClearColor(Video.RGB(128,128,255))

# プログラムの置いてあるディレクトリを基準として
# fontフォルダ内のIPAexfont00301フォルダからipaexg.ttfというフォントファイルを読み込み、"font"という名前を付ける
Video.CreateFont("font", "./Font/IPAexfont00301/ipaexg.ttf")

# 読み込んだフォントを現在利用するフォントに設定
Video.FontFace("font")


# ３つのシーンを順番に繰り返す
loop do

	# シーン１
	# マウスボタンの状態を入れておく変数はシーンに入る前にリセットして(falseを入れて)おく
	isLeftButtonDowned = false
	Video.Draw do

		Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)
		Video.Text(512 / 2, 512 / 2, "吾輩は猫である。名前はまだ無い。")

		# マウスが離された瞬間に次のシーンに移動する
		if !Mouse.IsLeftButtonDown() && isLeftButtonDowned == true
			# この処理を終了して次に移る
			break
		end
		isLeftButtonDowned = Mouse.IsLeftButtonDown()
	end

	# シーン２
	# マウスボタンの状態を入れておく変数はシーンに入る前にリセットして(falseを入れて)おく
	isLeftButtonDowned = false
	Video.Draw do
		Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)
		Video.Text(512 / 2, 512 / 2, "どこで生れたかとんと見当がつかぬ")

		# マウスが離されたされたら次の処理に移動する
		if !Mouse.IsLeftButtonDown() && isLeftButtonDowned == true
			# この処理を終了して次に移る
			break
		end
		isLeftButtonDowned = Mouse.IsLeftButtonDown()
	end

	# シーン３
	# マウスボタンの状態を入れておく変数はシーンに入る前にリセットして(falseを入れて)おく
	isLeftButtonDowned = false
	Video.Draw do
		Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)
		Video.TextBox((512 - 320) / 2, 512 / 2, 320, "何でも薄暗いじめじめした所でニャーニャー泣いていた事だけは記憶している。")

		# マウスが離されたされたら次の処理に移動する
		if !Mouse.IsLeftButtonDown() && isLeftButtonDowned == true
			# この処理を終了して次に移る
			break
		end
		isLeftButtonDowned = Mouse.IsLeftButtonDown()
	end

	# シーン４
	# マウスボタンの状態を入れておく変数はシーンに入る前にリセットして(falseを入れて)おく
	isLeftButtonDowned = false
	# 「はい」の領域と「いいえ」の領域を決めておく
	rect_yes = { :left => 256-80-50, :top => 256-40, :width => 100, :height => 80}
	rect_no  = { :left => 256+80-50, :top => 256-40, :width => 100, :height => 80}

	loop = true
	Video.Draw do
		Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)
		Video.Text(256, 512 / 2-100, "もう一度読む？")
		Video.Path do
			Video.Rect(rect_yes[:left], rect_yes[:top], rect_yes[:width], rect_yes[:height])
			Video.Rect(rect_no[:left],   rect_no[:top], rect_no[:width], rect_no[:height])
		end
		Video.FillColor(Video.RGBA(255,255,255,64))
		Video.Fill()
		Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)
		Video.FillColor(Video.RGBA(0,0,0,255))
		Video.Text(rect_yes[:left]+rect_yes[:width]/2, rect_yes[:top]+rect_yes[:height]/2, "はい")
		Video.Text(rect_no[:left]+rect_no[:width]/2, rect_no[:top]+rect_no[:height]/2, "いいえ")

		# マウスが離されたされたら位置を判定して次の処理に移動する
		if !Mouse.IsLeftButtonDown() && isLeftButtonDowned == true
			(x, y) = Mouse.GetCursorPos()
			if rect_yes[:left] <= x && x <= rect_yes[:left] + rect_yes[:width] && rect_yes[:top] <= y && y <= rect_yes[:top] + rect_yes[:height]
				# はい側をクリック
				# ループを終了しない
				loop = true
				break
			end
			if rect_no[:left] <= x && x <= rect_no[:left] + rect_no[:width] && rect_no[:top] <= y && y <= rect_no[:top] + rect_no[:height]
				# いいえ側をクリック
				# この処理を終了して次に移る
				loop = false
				break
			end
		end
		isLeftButtonDowned = Mouse.IsLeftButtonDown()
	end
	if loop == false
		break
	end
end

