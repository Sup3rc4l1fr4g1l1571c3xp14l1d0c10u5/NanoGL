# 文字コードをUTF8にしてファイルを保存しないと
# 日本語を含む文字列は正常に表示されません

require 'NanoGL'
include NanoGL

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 11-Dialog")

# 画面を消去した際の背景色を設定
Video.SetClearColor(Video.RGB(128,128,255))

# プログラムの置いてあるディレクトリを基準として
# Fontフォルダ内のIPAexfont00301フォルダからipaexg.ttfというフォントファイルを読み込み、"font"という名前を付ける
Video.CreateFont("font", "./Font/IPAexfont00301/ipaexg.ttf");

# 読み込んだフォントを現在利用するフォントに設定
Video.FontFace("font");

filename = ""

# 更新処理
lbdown = 0
rbdown = 0
Video.Draw do
	# マウス左クリックでファイルを開くダイアログボックスを表示
	if Mouse.IsLeftButtonDown
		lbdown += 1
	else
		lbdown = 0
	end
	if lbdown == 1
		filename = Dialog.OpenFileDialog("ファイルを選んでください", "JPEG|*.jpg;*jpeg\nGIF|*.gif\nすべてのファイル|*.*")
	end
	# マウス右クリックでファイル保存ダイアログボックスを表示
	if Mouse.IsRightButtonDown
		rbdown += 1
	else
		rbdown = 0
	end
	if rbdown == 1
		filename = Dialog.SaveFileDialog("保存先ファイル名を入力してください", "JPEG|*.jpg;*jpeg\nGIF|*.gif\nすべてのファイル|*.*")
	end

	# テキスト描画は図形ではないのでFillColor命令でテキスト色を指定してからテキスト描画命令を用いる
	Video.FontSize(12)								# フォントの大きさを高さ48ptに設定
	Video.FillColor(Video.RGBA(0, 0, 255, 255))	# テキストの色を緑色に設定
	Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)	# 基準位置に文字列の縦横中心がくるように設定
	if filename != nil
		Video.Text(512 / 2, 512 / 2, filename)
	end
end




