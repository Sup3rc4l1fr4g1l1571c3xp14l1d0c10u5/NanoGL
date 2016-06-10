require 'NanoGL'
include NanoGL

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 00-Blank")

# 画面を消去した際の背景色を設定
Video.SetClearColor([0,255,0])

# 更新処理
Video.Draw do

end

