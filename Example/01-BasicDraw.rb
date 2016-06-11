require 'NanoGL'
include NanoGL

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 01-BasicDraw")

# 画面を消去した際の背景色を設定
Video.SetClearColor([128,128,255])

# 更新処理
Video.Draw do

	#
	# 四角形を描画する例
	#
	
	# 図形の定義
	Video.Path do
		# 図形に四角形(左上座標100,200,幅150,高さ50を追加
		Video.Rect(100, 200, 150, 50)
	end
	
	# 塗りつぶし処理で用いる色を 赤255, 緑0, 青128, 不透明度255 に設定
	Video.FillColor([255, 0, 128, 255])
	
	# 図形を塗りつぶす
	Video.Fill()


	#
	# 丸(楕円)を描画する例
	#

	# 図形の定義
	Video.Path do
		# 図形に楕円(中心座標300, 200, 横方向半径80,縦方向半径50を追加
		Video.Ellipse(300, 200, 80, 50)
	end
	
	# 線の描画で用いる色を 赤0, 緑255, 青128, 不透明度255 に設定
	Video.StrokeColor([0, 255, 128, 255])
	
	# 図形の輪郭線を描く
	Video.Stroke()


	#
	# 三角形を描画する例
	#

	# 図形の定義
	Video.Path do
		# ペンの始点を座標 (200, 50) に設定
		Video.MoveTo(200, 50)
		# ペンの位置を座標(350, 100)に動かして線を引く
		Video.LineTo(350, 100)
		# ペンの位置を座標(150, 200)に動かして線を引く
		Video.LineTo(150, 200)
		# 図形の定義を終わらせると自動的に現在位置(150,200)と始点(200, 50)の間に線が引かれる
	end

	# 線の描画で用いる色を 赤0, 緑255, 青0, 不透明度255 に設定
	Video.StrokeColor([0, 255, 0, 255])

	# 線の太さを3pixelに設定
	Video.StrokeWidth(3)

	# 塗りつぶし処理で用いる色を 赤255, 緑255, 青255, 不透明度255 に設定
	Video.FillColor([255, 255, 255, 255])

	# 図形のを塗りつぶす
	Video.Fill()

	# 図形の輪郭線を描く
	Video.Stroke()

end

