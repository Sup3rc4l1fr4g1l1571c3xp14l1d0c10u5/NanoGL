require 'NanoGL'
include NanoGL

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 04-Image")

# 画面を消去した際の背景色を設定
Video.SetClearColor([128,128,255])

# プログラムの置いてあるディレクトリを基準として
# image フォルダ内の Image.png というファイルを読み込み、画像IDを image に設定する
# 画像オプションは指定しないので 0 を入れる
image = Video.CreateImage("./image/Image.png", 0)

# 画像サイズを取得して変数 width と 変数 height に入れる
(width, height) = Video.ImageSize(image)

# 更新処理
Video.Draw do
	Video.DrawImage(image, 0, 0, width, height, 256-64, 256-64, 128, 128, 0, 1.0)
end

