require 'NanoGL'
include NanoGL

# 画面サイズを縦横 512 ピクセル に 設定
Video.SetSize(512, 512)

# タイトルを設定
Video.SetWindowTitle("[Example] 03-TransformMulti")

# 画面を消去した際の背景色を設定
Video.SetClearColor([128,128,255])

# 更新処理
Video.Draw do

	#
	# 変形は複数の図形にも適用できる
	#

	# 図形を定義
	Path do
		# 変形のリセット
		ResetTransform()

		# ３秒で１回転するように現在の回転角度をプログラム起動からの経過時間で算出
		degree = 360.0 * Video.GetTime() / 3.0	

		# 角度に合わせて大きさが変化するようにsin関数でスケールを算出
		scale = Math.sin(Video.DegToRad(degree)) * 0.5 + 1.0	

		Translate(256, 256)					# 4の処理
		Rotate(Video.DegToRad(degree))		# 3の処理
		Scale(scale, scale)					# 2の処理
		Translate(-256, -256)				# 1の処理

		# 画面上に４個の正方形を配置する。
		# 上で指定した変形が適用されるので、実際には画面中央を中心にそれぞれの四角形は整列したまま回転する
		Rect(256 - 128 - 32, 256 - 128 - 32, 64, 64)	
		Rect(256 + 128 - 32, 256 - 128 - 32, 64, 64)	
		Rect(256 - 128 - 32, 256 + 128 - 32, 64, 64)	
		Rect(256 + 128 - 32, 256 + 128 - 32, 64, 64)	
	end

	# 塗りつぶしで用いる色を 赤255, 緑0, 青0, 不透明度255 に設定
	FillColor([255, 0, 0, 255])	

	# 図形を描く
	Fill()	

end
