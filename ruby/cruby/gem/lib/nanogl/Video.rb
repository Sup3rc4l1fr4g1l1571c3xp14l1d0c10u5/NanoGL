module NanoGL
  module Video
    module VideoImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private
      class Matrix < FFI::Struct
        layout :m, [:float, 6]

        def self.from_a(a)
          6.times.inject(Matrix.new) { |s, x| s[:m][x] = (a[x] || 0.0); s }
        end

        def to_a()
          6.times.inject(Array.new) { |s, x| s[x] = self[:m][x]; s }
        end

        def to_s()
          "Matrix(#{self.to_a.join(', ')})"
        end
      end

      class Int < FFI::Struct
        layout :value, :int
      end

      class Float < FFI::Struct
        layout :value, :float
      end

      class Float4 < FFI::Struct
        layout :v, [:float, 4]

        def self.from_a(a)
          4.times.inject(Matrix.new) { |s, x| s[:v][x] = (a[x] || 0.0); s }
        end

        def to_a()
          4.times.inject(Array.new) { |s, x| s[x] = self[:v][x]; s }
        end

        def to_s()
          "Float4(#{self.to_a.join(', ')})"
        end
      end

      class Color < FFI::Struct
        layout :r, :float,
               :g, :float,
               :b, :float,
               :a, :float

        def self.from_a(a)
          c = Color.new
          c[:r] = a[0]/255.0
          c[:g] = a[1]/255.0
          c[:b] = a[2]/255.0
          c[:a] = (a[3] || 255)/255.0
          return c
        end

        def to_a()
          [self[:r], self[:g], self[:b], self[:a]].map { |x| (x*255).to_i }
        end

        def to_s()
          "Color(#{self.to_a.join(', ')})"
        end

      end

      class Paint < FFI::Struct
        DEFAULT = {
            :xform => [0]*6,
            :extent => [0]*2,
            :radius => 0,
            :feather => 0,
            :innerColor => [0, 0, 0, 0]*4,
            :outerColor => [0, 0, 0, 0]*4,
            :image => 0
        }

        layout :xform, Video::VideoImpl::Matrix.by_value,
               :extent, [:float, 2],
               :radius, :float,
               :feather, :float,
               :innerColor, Video::VideoImpl::Color.by_value,
               :outerColor, Video::VideoImpl::Color.by_value,
               :image, :int

        def self.from_h(h)
          h = DEFAULT.merge(h)
          [:xform, :extent, :radius, :feather, :innerColor, :outerColor, :image].inject(Paint.new) do |s, x|
            case x
              when :xform then
                s[x] = Matrix.from_a(h[x])
              when :extent then
                (s[x][0], s[x][1]) = h[x][0..1]
              when :innerColor, :outerColor then
                s[x] = Color.from_a(h[x])
              else
                s[x] = h[x]
            end
            s
          end
        end

        def to_h()
          [:xform, :extent, :radius, :feather, :innerColor, :outerColor, :image].inject({}) do |s, x|
            case x
              when :xform then
                s[x] = self[x].to_a()
              when :extent then
                s[x] = [self[x][0], self[x][1]]
              when :innerColor, :outerColor then
                s[x] = self[x].to_a()
              else
                s[x] = self[x]
            end
            s
          end
        end

        def to_s()
          "Paint(#{self.to_h})"
        end

      end

      Winding = enum(:WINDING_CCW, 1,
                     :WINDING_CW
      )

      Solidity = enum(:SOLIDITY_SOLID, 1,
                      :SOLIDITY_HOLE
      )

      LineCap = enum(:CAP_BUTT, # 線の終点でぶつ切り
                     :CAP_ROUND, # 線の幅の半分の長さを半径とする半円
                     :CAP_SQUARE, # 線の幅の半分の長さだけはみ出る
                     :CAP_BEVEL, # マイター距離を無効にして交点まで押し出す
                     :CAP_MITER # マイター距離を有効にする
      )

      ImageFlag = enum(:IMAGE_GENERATE_MIPMAPS, 1 << 0, # 画像を読み込む際にミップマップデータを生成する。縮小描画を多用する画像だとやや速度が上がるかもしれない。
                       :IMAGE_REPEATX, 1 << 1, # 画像の横方向範囲を逸脱して描画する際、画像が横に無限に連続しているように描画する。（ImagePattern()で画像幅を超えるサイズのパターンを描画する際に、画像を繰り返し横に並べて描画する）
                       :IMAGE_REPEATY, 1 << 2, # 画像の縦方向範囲を逸脱して描画する際、画像が縦に無限に連続しているように描画する。（ImagePattern()で画像高さを超えるサイズのパターンを描画する際に、画像を繰り返し高に並べて描画する）
                       :IMAGE_FLIPY, 1 << 3, # 画像を上下反転して描画する。（一部の画像形式は古いツールで出力すると上下反転で画像が格納されている場合があるため）
                       :IMAGE_PREMULTIPLIED, 1 << 4, # αチャンネルを持つ画像の合成処理にプレマルチプライド方式を用いる。（Adobe AfterEffectからプレマルチプライド方式で出力した画像やTIFF形式などで必要）
                       #if defined(UNOFFICIAL_HACK_ENABLE_NEAREST_RESIZE)
                       :IMAGE_RESIZE_NEAREST, 1 << 5, # 画像の拡大縮小にニアレストネイバー(nearest neighbor)法を使う。ドット絵などに使う。
      #endif
      )

      Align = enum(:ALIGN_LEFT, 1 << 0, # [水平方向] 左寄せ（デフォルト値)
                   :ALIGN_CENTER, 1 << 1, # [水平方向] 中央寄せ
                   :ALIGN_RIGHT, 1 << 2, # [水平方向] 右寄せ

                   :ALIGN_TOP, 1 << 3, # [垂直方向] 上寄せ
                   :ALIGN_MIDDLE, 1 << 4, # [垂直方向] 中寄せ
                   :ALIGN_BOTTOM, 1 << 5, # [垂直方向] 下寄せ
                   :ALIGN_BASELINE, 1 << 6 # [垂直方向] ベースライン寄せ（デフォルト値)
      )

      class GlyphPosition < FFI::Struct
        layout :str, :pointer,
               :x, :float,
               :minx, :float,
               :maxx, :float

        def to_s
          "{ :str => #{self[:str].to_i}, :x => #{self[:x]}, :minx => #{self[:minx]}, :maxx => #{self[:maxx]}"
        end
      end

      class TextRow < FFI::Struct
        layout :start, :pointer,
               :end, :pointer,
               :next, :pointer,
               :width, :float,
               :minx, :float,
               :maxx, :float
      end

      class VTable < FFI::Struct
        layout :Initialize, callback([], :bool),
               :Finalize, callback([], :void),
               :SetWindowTitleUTF8, callback([:string], :void),
               :SetWindowTitle, callback([:string], :void),
               :SetSize, callback([:int, :int], :void),
               :Drawing, callback([], :void),
               :GetTime, callback([], :double),
               :Wait, callback([:double], :double),
               :SetClearColor, callback([VideoImpl::Color.by_value], :void),
               :BeginFrame, callback([:int, :int, :float], :void),
               :CancelFrame, callback([], :void),
               :EndFrame, callback([], :void),
               :RGB, callback([:uint8, :uint8, :uint8], VideoImpl::Color.by_value),
               :RGBA, callback([:uint8, :uint8, :uint8, :uint8], VideoImpl::Color.by_value),
               :Save, callback([], :void),
               :Restore, callback([], :void),
               :Reset, callback([], :void),
               :StrokeColor, callback([VideoImpl::Color.by_value], :void),
               :StrokePaint, callback([VideoImpl::Paint.by_value], :void),
               :FillColor, callback([VideoImpl::Color.by_value], :void),
               :FillPaint, callback([VideoImpl::Paint.by_value], :void),
               :MiterLimit, callback([:float], :void),
               :StrokeWidth, callback([:float], :void),
               :LineCap, callback([LineCap], :void),
               :LineJoin, callback([LineCap], :void),
               :GlobalAlpha, callback([:float], :void),
               :ResetTransform, callback([], :void),
               :Transform, callback([:float, :float, :float, :float, :float, :float], :void),
               :Translate, callback([:float, :float], :void),
               :Rotate, callback([:float], :void),
               :SkewX, callback([:float], :void),
               :SkewY, callback([:float], :void),
               :Scale, callback([:float, :float], :void),
               #if defined(UNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD)
               :TranslateWorld, callback([:float, :float], :void),
               :RotateWorld, callback([:float], :void),
               :SkewXWorld, callback([:float], :void),
               :SkewYWorld, callback([:float], :void),
               :ScaleWorld, callback([:float, :float], :void),
               #endif
               :CurrentTransform, callback([VideoImpl::Matrix.by_ref], :void),
               :TransformIdentity, callback([VideoImpl::Matrix.by_ref], :void),
               :TransformTranslate, callback([VideoImpl::Matrix.by_ref, :float, :float], :void),
               :TransformRotate, callback([VideoImpl::Matrix.by_ref, :float], :void),
               :TransformSkewX, callback([VideoImpl::Matrix.by_ref, :float], :void),
               :TransformSkewY, callback([VideoImpl::Matrix.by_ref, :float], :void),
               :TransformScale, callback([VideoImpl::Matrix.by_ref, :float, :float], :void),
               :TransformMultiply, callback([VideoImpl::Matrix.by_ref, VideoImpl::Matrix.by_ref], :void),
               :TransformPremultiply, callback([VideoImpl::Matrix.by_ref, VideoImpl::Matrix.by_ref], :void),
               :TransformInverse, callback([VideoImpl::Matrix.by_ref, VideoImpl::Matrix.by_ref], :int),
               :TransformPoint, callback([VideoImpl::Float.by_ref, VideoImpl::Float.by_ref, VideoImpl::Matrix.by_ref, :float, :float], :void),
               :DegToRad, callback([:float], :float),
               :RadToDeg, callback([:float], :float),
               :CreateImageUTF8, callback([:string, ImageFlag], :int),
               :CreateImage, callback([:string, ImageFlag], :int),
               :CreateImageMem, callback([ImageFlag, :buffer_in, :int], :int),
               :CreateImageRGBA, callback([:int, :int, ImageFlag, :buffer_in], :int),
               :UpdateImage, callback([:int, :buffer_in], :void),
               :ImageSize, callback([:int, VideoImpl::Int.by_ref, VideoImpl::Int.by_ref], :void),
               :DeleteImage, callback([:int], :void),
               :LinearGradient, callback([:float, :float, :float, :float, VideoImpl::Color.by_value, VideoImpl::Color.by_value], VideoImpl::Paint.by_value),
               :BoxGradient, callback([:float, :float, :float, :float, :float, :float, VideoImpl::Color.by_value, VideoImpl::Color.by_value], VideoImpl::Paint.by_value),
               :RadialGradient, callback([:float, :float, :float, :float, VideoImpl::Color.by_value, VideoImpl::Color.by_value], VideoImpl::Paint.by_value),
               :ImagePattern, callback([:float, :float, :float, :float, :float, :int, :float], VideoImpl::Paint.by_value),
               #if defined(UNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN)
               :ImagePatternWithFillColor, callback([:float, :float, :float, :float, :float, :int], VideoImpl::Paint.by_value),
               #endif
               :Scissor, callback([:float, :float, :float, :float], :void),
               :IntersectScissor, callback([:float, :float, :float, :float], :void),
               :ResetScissor, callback([], :void),

               :BeginPath, callback([], :void),
               :MoveTo, callback([:float, :float], :void),
               :LineTo, callback([:float, :float], :void),
               :BezierTo, callback([:float, :float, :float, :float, :float, :float], :void),
               :QuadTo, callback([:float, :float, :float, :float], :void),
               :ArcTo, callback([:float, :float, :float, :float, :float], :void),
               :ClosePath, callback([], :void),
               :PathWinding, callback([Solidity], :void),
               :ArcTo, callback([:float, :float, :float, :float, :float, Winding], :void),
               :Rect, callback([:float, :float, :float, :float], :void),
               :RoundedRect, callback([:float, :float, :float, :float, :float, :float], :void),
               :Ellipse, callback([:float, :float, :float, :float], :void),
               :Circle, callback([:float, :float, :float], :void),
               :Fill, callback([], :void),
               :Stroke, callback([], :void),

               :CreateFontUTF8, callback([:string, :string], :int),
               :CreateFont, callback([:string, :string], :int),
               :CreateFontMemUTF8, callback([:string, :buffer_in, :int, :pointer], :int),
               :CreateFontMem, callback([:string, :buffer_in, :int, :pointer], :int),
               :FindFontUTF8, callback([:string], :int),
               :FindFont, callback([:string], :int),
               :FontSize, callback([:float], :void),
               :FontBlur, callback([:float], :void),
               :TextLetterSpacing, callback([:float], :void),
               :TextLineHeight, callback([:float], :void),

               :TextAlign, callback([Align], :void),
               :FontFaceId, callback([:int], :void),
               :FontFaceUTF8, callback([:string], :void),
               :FontFace, callback([:string], :void),

               :TextUTF8, callback([:float, :float, :string, :pointer], :float),
               :Text, callback([:float, :float, :string, :pointer], :float),

               :TextBoxUTF8, callback([:float, :float, :float, :string, :pointer], :void),
               :TextBox, callback([:float, :float, :float, :string, :pointer], :void),

               :TextBoundsUTF8, callback([:float, :float, :string, :pointer, VideoImpl::Float4.by_ref], :float),
               :TextBounds, callback([:float, :float, :string, :pointer, VideoImpl::Float4.by_ref], :float),

               :TextBoxBoundsUTF8, callback([:float, :float, :float, :string, :pointer, VideoImpl::Float4.by_ref], :void),
               :TextBoxBounds, callback([:float, :float, :float, :string, :pointer, VideoImpl::Float4.by_ref], :void),

               :TextGlyphPositionsUTF8, callback([:float, :float, :pointer, :pointer, :pointer, :int], :int),
               :TextGlyphPositions, callback([:float, :float, :pointer, :pointer, :pointer, :int], :int),

               :TextMetrics, callback([VideoImpl::Float.by_ref, VideoImpl::Float.by_ref, VideoImpl::Float.by_ref], :void),

               :TextBreakLinesUTF8, callback([:string, :pointer, :float, :pointer, :int], :int),
               :TextBreakLines, callback([:string, :pointer, :float, :pointer, :int], :int),

               :DrawImage, callback([:int, :float, :float, :float, :float, :float, :float, :float, :float, :float, :float], :void),
               :DrawImageWithFillColor, callback([:int, :float, :float, :float, :float, :float, :float, :float, :float, :float, :float], :void),

               :FormatText, callback([:float, :float, :string, :varargs], :void)
      end

      attach_function :GetVideoVTable, [], VideoImpl::VTable.by_ref
    end

    private
    VTABLE = VideoImpl::GetVideoVTable()

    public
    module WindingStyle
      WINDING_CCW = VideoImpl::Winding[:WINDING_CCW]
      WINDING_CW = VideoImpl::Winding[:WINDING_CW]
    end

    module SolidityStyle
      SOLIDITY_SOLID = VideoImpl::Solidity[:SOLIDITY_SOLID]
      SOLIDITY_HOLE = VideoImpl::Solidity[:SOLIDITY_HOLE]
    end

    module LineCapStyle
      CAP_BUTT = VideoImpl::LineCap[:CAP_BUTT]
      CAP_ROUND = VideoImpl::LineCap[:CAP_ROUND]
      CAP_SQUARE = VideoImpl::LineCap[:CAP_SQUARE]
      CAP_BEVEL = VideoImpl::LineCap[:CAP_BEVEL]
      CAP_MITER = VideoImpl::LineCap[:CAP_MITER]
    end

    module AlignFlags
      ALIGN_LEFT = VideoImpl::Align[:ALIGN_LEFT]
      ALIGN_CENTER = VideoImpl::Align[:ALIGN_CENTER]
      ALIGN_RIGHT = VideoImpl::Align[:ALIGN_RIGHT]

      ALIGN_TOP = VideoImpl::Align[:ALIGN_TOP]
      ALIGN_MIDDLE = VideoImpl::Align[:ALIGN_MIDDLE]
      ALIGN_BOTTOM = VideoImpl::Align[:ALIGN_BOTTOM]
      ALIGN_BASELINE = VideoImpl::Align[:ALIGN_BASELINE]
    end

    module ImageFlags
      IMAGE_GENERATE_MIPMAPS = VideoImpl::ImageFlag[:IMAGE_GENERATE_MIPMAPS]
      IMAGE_REPEATX = VideoImpl::ImageFlag[:IMAGE_REPEATX]
      IMAGE_REPEATY = VideoImpl::ImageFlag[:IMAGE_REPEATY]
      IMAGE_FLIPY = VideoImpl::ImageFlag[:IMAGE_FLIPY]
      IMAGE_PREMULTIPLIED = VideoImpl::ImageFlag[:IMAGE_PREMULTIPLIED]
      IMAGE_RESIZE_NEAREST = VideoImpl::ImageFlag[:IMAGE_RESIZE_NEAREST]
    end
    class << self

      #
      # @brief VTableを初期化する
      # @retval true  初期化に成功
      # @retval false 初期化に失敗
      # @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
      #
      def Initialize()
        VTABLE[:Initialize].call()
      end

      #
      # @brief VTableの終了処理を行う
      # @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
      #
      def Finalize()
        VTABLE[:Finalize].call()
      end

      #
      # @brief ウィンドウタイトルを設定する
      # @param title ウィンドウタイトル
      #
      def SetWindowTitle(title)
        VTABLE[:SetWindowTitleUTF8].call(title)
      end

      #
      # @brief 画面サイズを設定する
      # @param width  幅
      # @param height 高さ
      #
      # 単位はピクセル
      #
      def SetSize(width, height)
        VTABLE[:SetSize].call(width, height)
      end

      #
      # @brief ゲームの状態更新・描画処理などが可能なるまで待つ
      # @return この関数は便宜上常にtrueを返す
      #
      # while (Video.Drawing()) { ゲームの状態更新・描画処理 } のように用いる
      # 画面が閉じられた場合にはプログラムは自動的に終了する
      #
      def Drawing()
        VTABLE[:Drawing].call()
      end

      #
      # @brief プログラム起動時からの経過秒数を取得
      # @return 経過秒数を１秒=1.0として取得。 1ミリ秒=1.0/1000
      #
      def GetTime()
        VTABLE[:GetTime].call()
      end

      #
      # @brief プログラム起動時からの経過秒数を取得
      # @return 経過秒数を１秒=1.0として取得。 1ミリ秒=1.0/1000
      #
      def Wait(sec)
        VTABLE[:Wait].call(sec)
      end

      #
      # @brief Video.Drawing()が画面消去に用いる色を color に設定する
      # @param color  設定する色
      #
      def SetClearColor(color)
        VTABLE[:SetClearColor].call(VideoImpl::Color.from_a(color))
      end

      #
      # @brief Video.Drawing()を使わずに全ての描画を管理する場合に用いる描画処理の開始
      # @param windowWidth  画面幅
      # @param windowHeight 画面高
      # @param devicePixelRatio フレームバッファサイズ/画面サイズ で求めたサイズ比率
      # @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
      #
      def BeginFrame(windowWidth, windowHeight, devicePixelRatio)
        VTABLE[:BeginFrame].call(windowWidth, windowHeight, devicePixelRatio)
      end

      #
      # @brief Video.Drawing()を使わずに全ての描画を管理する場合に用いる描画処理のキャンセル
      # @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
      #
      def CancelFrame()
        VTABLE[:CancelFrame].call()
      end

      #
      # @brief Video.Drawing()を使わずに全ての描画を管理する場合に用いる描画処理の終了
      # @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
      #
      def EndFrame()
        VTABLE[:EndFrame].call()
      end

      #
      # @brief 色をRGB各256階調で指定して作成する。A(半透明度)は255固定となる
      # @param r 赤の階調
      # @param g 緑の階調
      # @param b 青の階調
      # @return 作成した色データ
      #
      def RGB(r, g, b)
        VTABLE[:RGB].call(r, g, b).to_a
      end

      #
      # @brief 色をRGBA各256階調で指定して作成する。
      # @param r 赤の階調
      # @param g 緑の階調
      # @param b 青の階調
      # @param a 不透明度(255は完全不透明、0は完全透明)
      # @return 作成した色データ
      #
      def RGBA(r, g, b, a)
        VTABLE[:RGBA].call(r, g, b, a).to_a
      end

      #
      # @brief 現在の描画設定をコピーしてスタックに積む
      # @note  Video.Drawing()を呼び出すと描画設定もスタック状態もリセットされる
      #
      def Save()
        VTABLE[:Save].call()
      end

      #
      # @brief 現在の描画設定をスタックから取り出して現在の設定にする
      # @note  Video.Drawing()を呼び出すと描画設定もスタック状態もリセットされる
      #
      def Restore()
        VTABLE[:Restore].call()
      end

      #
      # @brief 現在の描画設定を初期値にリセットする
      #
      def Reset()
        VTABLE[:Reset].call()
      end

      #
      # @brief Video.Stroke()で描画する線の色を設定する
      # @param color 線の色
      #
      def StrokeColor(color)
        VTABLE[:StrokeColor].call(VideoImpl::Color.from_a(color))
      end

      #
      # @brief Video.Stroke()で描画する線のパターンを設定する
      # @param paint 線のパターン
      # @note  パターンはVideo.LinearGradient()/BoxGradient()/RadialGradient()/ImagePattern()で作成する
      #
      def StrokePaint(paint)
        VTABLE[:StrokePaint].call(paint)
      end

      #
      # @brief Video.Fill()で描画する塗りつぶしの色を設定する
      # @param color 塗りつぶしの色
      #
      def FillColor(color)
        VTABLE[:FillColor].call(VideoImpl::Color.from_a(color))
      end

      #
      # @brief Video.Stroke()で描画する塗りつぶしのパターンを設定する
      # @param paint 塗りつぶしのパターン
      # @note  パターンはVideo.LinearGradient()/BoxGradient()/RadialGradient()/ImagePattern()で作成する
      #
      def FillPaint(paint)
        VTABLE[:FillPaint].call(paint)
      end

      #
      # @brief 折れ線の接合箇所のマイター限界（折れ線箇所を面取りしないで尖らせる）距離を指定する
      # @param limit マイター距離
      # @note  マイター距離についての解説は http://www.htmq.com/canvas/miterLimit.shtml を参照
      #
      def MiterLimit(limit)
        VTABLE[:MiterLimit].call(limit)
      end

      #
      # @brief Video.Stroke()で描画する線の幅を設定する
      # @param paint 線の幅
      #
      def StrokeWidth(size)
        VTABLE[:StrokeWidth].call(size)
      end

      #
      # @brief Video.Stroke()で描画する線の終端の形状を指定する
      # @param cap 線の終端の形状
      # @note  終端の形状については enum LineCap の解説を参照
      #
      def LineCap(cap)
        VTABLE[LineCap].call(cap)
      end

      #
      # @brief 折れ線の接続形状を指定する
      # @param join 線の終端の形状（bevel, round, miterのいずれか）
      # @note  終端の形状については enum LineCap の解説を参照
      #
      def LineJoin(join)
        VTABLE[:LineJoin].call(join)
      end

      #
      # @brief 描画全体の不透明度を0.0～1.0で指定
      # @param alpha 不透明度(1.0が完全不透明, 0.0が完全透明)
      #
      def GlobalAlpha(alpha)
        VTABLE[:GlobalAlpha].call(alpha)
      end

      #
      # @brief 現在の変形設定をリセットする（姿勢変換行列に単位行列を設定）
      # | 1, 0, 0 |
      # | 0, 1, 0 |
      # | 0, 0, 1 |
      #
      def ResetTransform()
        VTABLE[:ResetTransform].call()
      end

      #
      # @brief 現在の変形設定に任意の2x3姿勢変換行列を設定する
      # @param a,b,c,d,e,f 行列要素
      # @note  各引数と行列要素の対応は以下の通り
      # | a, c, e |
      # | b, d, f |
      # | 0, 0, 1 |
      #
      def Transform(a, b, c, d, e, f)
        VTABLE[:Transform].call(a, b, c, d, e, f)
      end

      #
      # @brief 現在の変形設定に移動(x, y)を右から適用
      # @param x 横方向移動量
      # @param y 縦方向移動量
      # @note  各引数と行列要素の対応は以下の通り
      # | 1, 0, x |
      # | 0, 1, y |
      # | 0, 0, 1 |
      #
      def Translate(x, y)
        VTABLE[:Translate].call(x, y)
      end

      #
      # @brief 現在の変形設定に 時計回りで angle ラジアンの回転を右から適用
      # @param angle 時計回りでの回転
      # @note  各引数と行列要素の対応は以下の通り
      # | c,-s, 0 |
      # | s, c, 0 |
      # | 0, 0, 1 |
      # c = cos(angle), s = sin(angle)
      #
      def Rotate(angle)
        VTABLE[:Rotate].call(angle)
      end

      #
      # @brief 現在の変形設定に angle ラジアンのX軸に沿った傾斜変形を右から適用
      # @param angle 傾斜角度
      # @note  各引数と行列要素の対応は以下の通り
      # | 1, t, 0 |
      # | 0, 1, 0 |
      # | 0, 0, 1 |
      # t = tan(angle)
      #
      def SkewX(angle)
        VTABLE[:SkewX].call(angle)
      end

      #
      # @brief 現在の変形設定に angle ラジアンのY軸に沿った傾斜変形を右から適用
      # @param angle 傾斜角度
      # @note  各引数と行列要素の対応は以下の通り
      # | 1, 0, 0 |
      # | t, 1, 0 |
      # | 0, 0, 1 |
      # t = tan(angle)
      #
      def SkewY(angle)
        VTABLE[:SkewY].call(angle)
      end

      #
      # @brief 現在の変形設定に 横 x 倍, 縦 y 倍のスケールを右から適用
      # @param x 横方向スケール
      # @param y 縦方向スケール
      # @note  各引数と行列要素の対応は以下の通り
      # | x, 0, 0 |
      # | 0, y, 0 |
      # | 0, 0, 1 |
      #
      def Scale(x, y)
        VTABLE[:Scale].call(x, y)
      end

      #if defined(UNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD)
      #
      # @brief 現在の変形設定に移動(x, y)を左から適用
      # @param x 横方向移動量
      # @param y 縦方向移動量
      # @note  各引数と行列要素の対応は以下の通り
      # | 1, 0, x |
      # | 0, 1, y |
      # | 0, 0, 1 |
      #
      def TranslateWorld(x, y)
        VTABLE[:TranslateWorld].call(x, y)
      end

      #
      # @brief 現在の変形設定に 時計回りで angle ラジアンの回転を左から適用
      # @param angle 時計回りでの回転
      # @note  各引数と行列要素の対応は以下の通り
      # | c,-s, 0 |
      # | s, c, 0 |
      # | 0, 0, 1 |
      # c = cos(angle), s = sin(angle)
      #
      def RotateWorld(angle)
        VTABLE[:RotateWorld].call(angle)
      end

      #
      # @brief 現在の変形設定に angle ラジアンのX軸に沿った傾斜変形を左から適用
      # @param angle 傾斜角度
      # @note  各引数と行列要素の対応は以下の通り
      # | 1, t, 0 |
      # | 0, 1, 0 |
      # | 0, 0, 1 |
      # t = tan(angle)
      #
      def SkewXWorld(angle)
        VTABLE[:SkewXWorld].call(angle)
      end

      #
      # @brief 現在の変形設定に angle ラジアンのY軸に沿った傾斜変形を左から適用
      # @param angle 傾斜角度
      # @note  各引数と行列要素の対応は以下の通り
      # | 1, 0, 0 |
      # | t, 1, 0 |
      # | 0, 0, 1 |
      # t = tan(angle)
      #
      def SkewYWorld(angle)
        VTABLE[:SkewYWorld].call(angle)
      end

      #
      # @brief 現在の変形設定に 横 x 倍, 縦 y 倍のスケールを左から適用
      # @param x 横方向スケール
      # @param y 縦方向スケール
      # @note  各引数と行列要素の対応は以下の通り
      # | x, 0, 0 |
      # | 0, y, 0 |
      # | 0, 0, 1 |
      #
      def ScaleWorld(x, y)
        VTABLE[:ScaleWorld].call(x, y)
      end

      #endif

      #
      # @brief 現在の変形設定を配列xformに取り出す
      # @param xform 6要素以上の配列。
      # @note  配列要素と行列要素の対応は以下の通り
      # | a, c, e |
      # | b, d, f |
      # | 0, 0, 1 |
      # xform = [a,b,c,d,e,f]
      #
      def CurrentTransform()
        xform = VideoImpl::Matrix.new
        VTABLE[:CurrentTransform].call(xform)
        return xform
      end

      #
      # @brief 配列 dst に単位行列を設定
      # @param dst 6要素以上の配列。
      # @note  配列要素と行列要素の対応は以下の通り
      # | 1, 0, 0 |
      # | 0, 1, 0 |
      # | 0, 0, 1 |
      # dst = [1,0,0,1,0,0]
      #
      def TransformIdentity()
        dst = VideoImpl::Matrix.new
        VTABLE[:TransformIdentity].call(dst)
        return dst
      end

      #
      # @brief 配列 dst に移動行列(x, y)を設定
      # @param dst 6要素以上の配列。
      # @param x 横方向移動量
      # @param y 縦方向移動量
      # @note  配列要素と行列要素の対応は以下の通り
      # | 1, 0, x |
      # | 0, 1, y |
      # | 0, 0, 1 |
      # dst = [1,0,0,1,x,y]
      #
      def TransformTranslate(sx, sy)
        dst = VideoImpl::Matrix.new
        VTABLE[:TransformTranslate].call(dst, sx, sy)
        return dst
      end

      #
      # @brief 配列 dst に時計回りで angle ラジアンの回転行列を設定
      # @param dst 6要素以上の配列。
      # @param angle 時計回りでの回転
      # @note  配列要素と行列要素の対応は以下の通り
      # | c,-s, 0 |
      # | s, c, 0 |
      # | 0, 0, 1 |
      # c = cos(angle), s = sin(angle)
      # dst = [c,s,-s,c,0,0]
      #
      def TransformRotate(a)
        dst = VideoImpl::Matrix.new
        VTABLE[:TransformRotate].call(dst, a)
        return dst
      end

      #
      # @brief 配列 dst にangle ラジアンのX軸に沿った傾斜変形行列を設定
      # @param dst 6要素以上の配列。
      # @param angle 傾斜角度
      # @note  配列要素と行列要素の対応は以下の通り
      # | 1, t, 0 |
      # | 0, 1, 0 |
      # | 0, 0, 1 |
      # t = tan(angle)
      # dst = [1,0,t,1,0,0]
      #
      def TransformSkewX(a)
        dst = VideoImpl::Matrix.new
        VTABLE[:TransformSkewX].call(dst, a)
        return dst
      end

      #
      # @brief 配列 dst にangle ラジアンのY軸に沿った傾斜変形行列を設定
      # @param dst 6要素以上の配列。
      # @param angle 傾斜角度
      # @note  配列要素と行列要素の対応は以下の通り
      # | 1, 0, 0 |
      # | t, 1, 0 |
      # | 0, 0, 1 |
      # t = tan(angle)
      # dst = [1,t,0,1,0,0]
      #
      def TransformSkewY(dst, a)
        dst = VideoImpl::Matrix.new
        VTABLE[:TransformSkewY].call(dst, a)
        return dst
      end

      #
      # @brief 配列 dst に横 x 倍, 縦 y 倍のスケール行列を設定
      # @param dst 6要素以上の配列。
      # @param x 横方向スケール
      # @param y 縦方向スケール
      # @note  各引数と行列要素の対応は以下の通り
      # | x, 0, 0 |
      # | 0, y, 0 |
      # | 0, 0, 1 |
      # dst = [x,0,0,y,0,0]
      #
      def TransformScale(dst, sx, sy)
        dst = VideoImpl::Matrix.new
        VTABLE[:TransformScale].call(dst, sx, sy)
        return dst
      end

      #
      # @brief 配列 dst と配列 src を行列と見なしてdst * src を行う。結果はdstに格納される。
      # @param dst 6要素以上の配列。
      # @param src 6要素以上の配列。
      # @note 配列要素と行列要素の対応は Transform() 関数のものと同じ
      #
      def TransformMultiply(dst, src)
        VTABLE[:TransformMultiply].call(dst, src)
      end

      #
      # @brief 配列 dst と配列 src を行列と見なしてsrc * dst を行う。結果はdstに格納される。
      # @param dst 6要素以上の配列。
      # @param src 6要素以上の配列。
      # @note 配列要素と行列要素の対応は Transform() 関数のものと同じ
      #
      def TransformPremultiply(dst, src)
        VTABLE[:TransformPremultiply].call(dst, src)
      end

      #
      # @brief 配列 dst を行列と見なして逆行列を求める。結果はdstに格納される。
      # @param dst 6要素以上の配列。
      # @param src 6要素以上の配列。
      # @retval 逆行列が存在する場合は～しない場合は～。
      #
      def TransformInverse(src)
        dst = VideoImpl::Matrix.new
        VTABLE[:TransformInverse].call(dst, src)
        return dst
      end

      #
      # @brief 配列 xform を行列と見なして点(srcx, srcy)に変形行列 xform を適用した結果得られる点の座標を(dstx, dsty)に格納する。
      # @param dstx 変形後の点のX座標を格納する変数のポインタ
      # @param dsty 変形後の点のY座標を格納する変数のポインタ
      # @param xform 変形に用いる行列としてみなす6要素以上の配列
      # @param srcx 変形前の点のX座標
      # @param srcy 変形前の点のY座標
      #
      def TransformPoint(xform, srcx, srcy)
        dstx = VideoImpl::Float.new
        dsty = VideoImpl::Float.new
        VTABLE[:TransformPoint].call(dstx, dsty, xform, srcx, srcy)
        return [dstx[:value], dsty[:value]]
      end

      #
      # @brief 角度をラジアンに変換
      # @param deg 角度
      # @return degに対応するラジアン
      #

      def DegToRad(deg)
        VTABLE[:DegToRad].call(deg)
      end

      #
      # @brief ラジアンを角度に変換
      # @param rad ラジアン
      # @return radに対応する角度
      #
      def RadToDeg(rad)
        VTABLE[:RadToDeg].call(rad)
      end

      #
      # @brief 画像データをファイルから読み込む（対応する形式はJPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC）
      # @param filename 画像ファイルのファイルパス
      # @param imageFlags 画像を描画で用いる際のオプションを enum ImageFlags のゼロ個以上の組み合わせで指定
      # @retval  == 0 読み込み失敗
      # @retval   > 0 画像ID
      #
      def CreateImage(filename, imageFlags)
        VTABLE[:CreateImageUTF8].call(filename, imageFlags)
      end

      #
      # @brief 画像データをメモリ上から読み込む（対応する形式はJPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC）
      # @param imageFlags 画像を描画で用いる際のオプションを enum ImageFlags のゼロ個以上の組み合わせで指定
      # @param data 画像データ(Array.pack('C*')等した文字列)
      # @retval  == 0 読み込み失敗
      # @retval   > 0 画像ID
      #
      def CreateImageMem(imageFlags, data)
        pData = FFI::MemoryPointer.from_string(data)
        VTABLE[:CreateImageMem].call(imageFlags, pData, data.length)
      end


      #
      # @brief 32bitRGBA画像データを読み込む
      # @param w 画像の幅
      # @param h 画像の高さ
      # @param imageFlags 画像を描画で用いる際のオプションを enum ImageFlags のゼロ個以上の組み合わせで指定
      # @param data 画像データ(Array.pack('C*')等した文字列)
      # @retval  == 0 読み込み失敗
      # @retval   > 0 画像ID
      #
      def CreateImageRGBA(w, h, imageFlags, data)
        pData = FFI::MemoryPointer.from_string(data)
        VTABLE[:CreateImageRGBA].call(w, h, imageFlags, pData)
      end

      #
      # @brief 32bitRGBA画像データ data で画像ID img の画像データを置き換える。画像サイズなどは変更できない
      # @param image 画像ID
      # @param data 画像データ(Array.pack('C*')等した文字列)
      # @note 動画やウェブカメラの画像などサイズは固定だが時間で変化する画像を描画する場合に用いる
      #
      def UpdateImage(image, data)
        pData = FFI::MemoryPointer.from_string(data)
        VTABLE[:UpdateImage].call(image, pData)
      end

      #
      # @brief 画像サイズを取得
      # @param image 画像ID
      # @param w 画像の幅の格納先ポインタ
      # @param h 画像の高さの格納先ポインタ
      #
      def ImageSize(image)
        w = VideoImpl::Int.new
        h = VideoImpl::Int.new
        VTABLE[:ImageSize].call(image, w, h)
        return [w[:value], h[:value]]
      end

      #
      # @brief 画像を破棄する
      # @param image 画像ID
      #
      def DeleteImage(image)
        VTABLE[:DeleteImage].call(image)
      end

      #
      # @brief 線形グラデーションの塗りつぶしパターンを作成
      # @param sx グラデーションの始点X座標
      # @param sy グラデーションの始点Y座標
      # @param ex グラデーションの終点X座標
      # @param ey グラデーションの終点Y座標
      # @param icol 始点側の色
      # @param ocol 終点側の色
      # @return 作成された塗りつぶしパターン
      #
      def LinearGradient(sx, sy, ex, ey, icol, ocol)
        VTABLE[:LinearGradient].call(sx, sy, ex, ey, icol, ocol)
      end

      #
      # @brief 矩形グラデーションの塗りつぶしパターンを作成
      # @param x 矩形の左上点のX座標
      # @param y 矩形の左上点のY座標
      # @param w 矩形の幅
      # @param h 矩形の高さ
      # @param r 矩形の角の丸め半径
      # @param f 矩形の境界のぼかし距離
      # @param icol 始点（中央）側の色
      # @param ocol 終点（境界）側の色
      # @return 作成された塗りつぶしパターン
      #
      def BoxGradient(x, y, w, h, r, f, icol, ocol)
        VTABLE[:BoxGradient].call(x, y, w, h, r, f, icol, ocol)
      end

      #
      # @brief 円形グラデーションの塗りつぶしパターンを作成
      # @param cx 円の中心点のX座標
      # @param cy 円の中心点のY座標
      # @param inr 内側の半径
      # @param outr 外側の半径
      # @param icol 内側の色
      # @param ocol 外側の色
      # @return 作成された塗りつぶしパターン
      #
      def RadialGradient(cx, cy, inr, outr, icol, ocol)
        VTABLE[:RadialGradient].call(cx, cy, inr, outr, icol, ocol)
      end

      #
      # @brief 画像パターンを作成
      # @param ox パターンに使う画像領域（矩形）の左上点のX座標
      # @param oy パターンに使う画像領域（矩形）の左上点のY座標
      # @param ex パターンに使う画像領域（矩形）の幅
      # @param ey パターンに使う画像領域（矩形）の高さ
      # @param angle 画像の回転角度（回転中心がパターンの左上であることに注意）
      # @param image 画像ID
      # @param alpha 画像の不透明度(0.0～1.0で指定)
      # @return 作成された塗りつぶしパターン
      #
      def ImagePattern(ox, oy, ex, ey, angle, image, alpha)
        VTABLE[:ImagePattern].call(ox, oy, ex, ey, angle, image, alpha)
      end

      #if defined(UNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN)
      #
      # @brief 画像パターンを作成(画像の色としてFillColorに設定されている色を用いる)
      # @param ox パターンに使う画像領域（矩形）の左上点のX座標
      # @param oy パターンに使う画像領域（矩形）の左上点のY座標
      # @param ex パターンに使う画像領域（矩形）の幅
      # @param ey パターンに使う画像領域（矩形）の高さ
      # @param angle 画像の回転角度（回転中心がパターンの左上であることに注意）
      # @param image 画像ID
      # @return 作成された塗りつぶしパターン
      # この命令は特殊な使い方を想定している（爆発演出のパーティクルにテクスチャを使って赤色を追加するなど）。
      # 命令の構成にはこの先も少し手を入れる予定なので必要でない限り使用を避けてください。
      #
      def ImagePatternWithFillColor(cx, cy, w, h, angle, image)
        VTABLE[:ImagePatternWithFillColor].call(cx, cy, w, h, angle, image)
      end

      #endif

      #
      # @brief シザリング（クリッピング）領域を新規設定する
      # @param x シザリング（クリッピング）領域（矩形）の左上点のX座標
      # @param y シザリング（クリッピング）領域（矩形）の左上点のY座標
      # @param w シザリング（クリッピング）領域（矩形）の幅
      # @param h シザリング（クリッピング）領域（矩形）の高さ
      # シザリング（クリッピング）領域を設定するとその領域の外側には描画が行われなくなる。
      # 画面分割でプレイヤーごとに画面を描画する際に用いると無駄な描画を減らすことができる。
      #
      def Scissor(x, y, w, h)
        VTABLE[:Scissor].call(x, y, w, h)
      end

      #
      # @brief シザリング（クリッピング）領域を設定されている領域と引数で与えた領域の共通領域に更新する
      # @param x 重ねるシザリング（クリッピング）領域（矩形）の左上点のX座標
      # @param y 重ねるシザリング（クリッピング）領域（矩形）の左上点のY座標
      # @param w 重ねるシザリング（クリッピング）領域（矩形）の幅
      # @param h 重ねるシザリング（クリッピング）領域（矩形）の高さ
      #
      def IntersectScissor(x, y, w, h)
        VTABLE[:IntersectScissor].call(x, y, w, h)
      end

      #
      # @brief シザリング（クリッピング）領域をリセットする。リセット後は画面全体が新しいシザリング領域になる
      #
      def ResetScissor()
        VTABLE[:ResetScissor].call()
      end

      #
      # @brief 図形の描画を開始する
      # 描画完了後は必ずVideo.EndFrame()を呼び出すこと。
      #
      def BeginPath()
        VTABLE[:BeginPath].call()
      end

      #
      # @brief 図形を描画するペンの座標を設定する
      # @param x ペンのX座標
      # @param y ペンのY座標
      #
      def MoveTo(x, y)
        VTABLE[:MoveTo].call(x, y)
      end

      #
      # @brief 現在のペン座標と指定した座標の間に辺を作成し、ペンを移動させる
      # @param x 移動先のペンのX座標
      # @param y 移動先のペンのY座標
      #
      def LineTo(x, y)
        VTABLE[:LineTo].call(x, y)
      end

      #
      # @brief 現在のペン座標、制御点1、制御点2、指定した座標を用いて3次ベジェ曲線 (Cubic Bezier curve) を作成し、ペンを移動させる
      # @param c1x 制御点1のX座標
      # @param c1y 制御点1のY座標
      # @param c2x 制御点2のX座標
      # @param c2y 制御点2のy座標
      # @param x 移動先のペンのX座標
      # @param y 移動先のペンのY座標
      # ベジェ曲線の開設は https://ja.wikipedia.org/wiki/%E3%83%99%E3%82%B8%E3%82%A7%E6%9B%B2%E7%B7%9A などを参照
      #
      def BezierTo(c1x, c1y, c2x, c2y, x, y)
        VTABLE[:BezierTo].call(c1x, c1y, c2x, c2y, x, y)
      end

      #
      # @brief 現在のペン座標、制御点、指定した座標を用いて2次ベジェ曲線 (Quadratic Bezier curve)を作成し、ペンを移動させる
      # @param cx 制御点のX座標
      # @param cy 制御点のY座標
      # @param x 移動先のペンのX座標
      # @param y 移動先のペンのY座標
      # ベジェ曲線の開設は https://ja.wikipedia.org/wiki/%E3%83%99%E3%82%B8%E3%82%A7%E6%9B%B2%E7%B7%9A などを参照
      #
      def QuadTo(cx, cy, x, y)
        VTABLE[:QuadTo].call(cx, cy, x, y)
      end

      #
      # @brief 現在のペン位置と制御点と移動先の点を元に半径radiusの円弧を描く
      # @param x1 制御点のX座標
      # @param y1 制御点のY座標
      # @param x2 移動先のペンのX座標
      # @param y2 移動先のペンのY座標
      # @param radius 円弧の半径
      # 円弧描画の解説は http://www.htmq.com/canvas/arcTo.shtml がまとまっている。
      #
      def ArcTo(x1, y1, x2, y2, radius)
        VTABLE[:ArcTo].call(x1, y1, x2, y2, radius)
      end

      #
      # @brief 図形の描画を完了させる
      #
      def ClosePath()
        VTABLE[:ClosePath].call()
      end

      #
      # @brief 直前に追加した図形の形状の特性を穴or面で指定する
      # @param dir 特性を示す enum Solidity 値
      #
      def PathWinding(dir)
        VTABLE[:PathWinding].call(dir)
      end

      #
      # @brief 中心点 (cx, cy) で半径 r の円の円周上で時計回りで角度 a0 から a1 までの円弧を描く
      # @param cx 円の中心点のX座標
      # @param cy 円の中心点のY座標
      # @param r  円の半径
      # @param a1 円弧の始点角度
      # @param a2 円弧の終点角度
      # @param dir 始点と終点と結ぶ際の回転方向を示す enum Winding 値
      #
      def Arc(cx, cy, r, a0, a1, dir)
        VTABLE[:Arc].call(cx, cy, r, a0, a1, dir)
      end

      #
      # @brief 左上座標(x,y) 幅 w 高さ h の矩形を描画する
      # @param x 矩形の左上点のX座標
      # @param y 矩形の左上点のY座標
      # @param w 矩形の幅
      # @param h 矩形の高さ
      #
      def Rect(x, y, w, h)
        VTABLE[:Rect].call(x, y, w, h)
      end

      #
      # @brief 左上座標(x,y) 幅 w 高さ h 角hを半径 r で丸めた矩形を描画する
      # @param x 矩形の左上点のX座標
      # @param y 矩形の左上点のY座標
      # @param w 矩形の幅
      # @param h 矩形の高さ
      # @param r 矩形の角の丸め半径
      #
      def RoundedRect(x, y, w, h, r)
        VTABLE[:RoundedRect].call(x, y, w, h, r)
      end

      #
      # @brief 中心点 (cx, cy) で横半径 rx 縦半径 ry の楕円を描く
      # @param cx 楕円の中心点のX座標
      # @param cy 楕円の中心点のY座標
      # @param rx 楕円の横半径
      # @param ry 楕円の縦半径
      #
      def Ellipse(cx, cy, rx, ry)
        VTABLE[:Ellipse].call(cx, cy, rx, ry)
      end

      #
      # @brief 中心点 (cx, cy) で半径 r の円を描く
      # @param cx 円の中心点のX座標
      # @param cy 円の中心点のY座標
      # @param r 円の半径
      #
      def Circle(cx, cy, r)
        VTABLE[:Circle].call(cx, cy, r)
      end

      #
      # @brief 描いた形状を塗りつぶす
      # 塗りつぶしは Video.FillColor() で設定した色、もしくは Video.FillPaint() で設定したパターンが使われる
      #
      def Fill()
        VTABLE[:Fill].call()
      end

      #
      # @brief 描いた形状の輪郭線を描画する
      # 線は Video.StrokeColor() で設定した色、もしくは Video.StrokePaint() で設定したパターンを用いて Video.StrokeWidth()で設定された太さで描画される
      #
      def Stroke()
        VTABLE[:Stroke].call()
      end

      #
      # @brief TrueType形式のフォントをTTFファイル filename から読み込み、フォント名に名前 name を割り当てる
      # @param name 読み込んだフォントに割り当てる名前
      # @param filename 読み込むTTF形式のフォントファイル
      # @retval == -1 読み込み失敗
      # @retval >=  0 読み込んだフォントに割り当てられるフォントID
      #
      def CreateFont(name, filename)
        VTABLE[:CreateFontUTF8].call(name, filename)
      end

      #
      # @brief TrueType形式のフォントデータ data をメモリから読み込み、フォント名に名前 name を割り当てる
      # @param name 読み込んだフォントに割り当てる名前
      # @param data 読み込むTTF形式のフォントデータ(Array.pack('C*')等した文字列)
      # @retval == -1 読み込み失敗
      # @retval >=  0 読み込んだフォントに割り当てられるフォントID
      #
      def CreateFontMem(name, data)
        pData = FFI::MemoryPointer.from_string(data)
        VTABLE[:CreateFontMemUTF8].call(name, pData, data.length, nil)
      end

      #
      # @brief 既に読み込んだフォントの中から名前が name のものを探す
      # @param name 探すフォントの名前
      # @retval == -1 見つからなかった
      # @retval >=  0 見つかったフォントに割り当てられているフォントID
      #
      def FindFont(name)
        VTABLE[:FindFontUTF8].call(name)
      end

      #
      # @brief 描画する文字のサイズを設定する
      # @param size フォントサイズ
      #
      def FontSize(size)
        VTABLE[:FontSize].call(size)
      end

      #
      # @brief 描画する文字のぼかし距離を設定する
      # @param size 境界をぼかす距離
      #
      def FontBlur(blur)
        VTABLE[:FontBlur].call(blur)
      end

      #
      # @brief 描画する文字の文字間隔を設定する
      # @param size 文字間隔
      #
      def TextLetterSpacing(spacing)
        VTABLE[:TextLetterSpacing].call(spacing)
      end

      #
      # @brief 描画する文字の行高を設定する
      # @param size 行高
      #
      def TextLineHeight(lineHeight)
        VTABLE[:TextLineHeight].call(lineHeight)
      end

      #
      # @brief 描画する文字の配置を指定する
      # @param align 縦横それぞれを enum Align の組み合わせで指定する
      #
      def TextAlign(align)
        VTABLE[:TextAlign].call(align)
      end

      #
      # @brief 文字の描画に使うフォントをフォントIDで指定する
      # @param font 描画に使うフォントのフォントID
      #
      def FontFaceId(font)
        VTABLE[:FontFaceId].call(font)
      end

      #
      # @brief 文字の描画に使うフォントをフォント名で指定する
      # @param font 描画に使うフォントのフォント名
      #
      def FontFace(font)
        VTABLE[:FontFaceUTF8].call(font)
      end

      #
      # @brief 座標 (x, y)を基準点として文字列 string を描画する
      # @param x 基準点のX座標
      # @param y 基準点のY座標
      # @param string 描画する文字列
      # @return 最後に描いた文字のX座標
      #
      def Text(x, y, string)
        VTABLE[:TextUTF8].call(x, y, string, nil)
      end

      #
      # @brief テキストを折り返す幅を breakRowWidth 、座標 (x, y)を基準点として文字列 string を描画する
      # @param x 基準点のX座標
      # @param y 基準点のY座標
      # @param breakRowWidth テキストを折り返す幅
      # @param string 描画する文字列
      #
      def TextBox(x, y, breakRowWidth, string)
        VTABLE[:TextBoxUTF8].call(x, y, breakRowWidth, string, nil)
      end

      #
      # @brief 座標 (x, y)を基準点として文字列 string を描画したときの描画領域を取得する（描画は行わない）
      # @param x 基準点のX座標
      # @param y 基準点のY座標
      # @param string 描画する文字列
      # @return [最後に描いた文字のX座標, 描画する文字列がぴったり入る領域（[左上のX座標, 左上のY座標, 右上のX座標, 右上のY座標]）]
      #
      def TextBounds(x, y, string)
        bounds = VideoImpl::Float4.new
        ret = VTABLE[:TextBoundsUTF8].call(x, y, string, nil, bounds)
        return [ret, bounds]
      end

      #
      # @brief 座標 (x, y)を基準点とし、テキストを折り返す幅を breakRowWidthとして文字列 string を描画したときの描画領域を取得する（描画は行わない）
      # @param x 基準点のX座標
      # @param y 基準点のY座標
      # @param breakRowWidth テキストを折り返す幅
      # @param string 描画する文字列
      # @return [最後に描いた文字のX座標, 描画する文字列がぴったり入る領域（[左上のX座標, 左上のY座標, 右上のX座標, 右上のY座標]）]
      #
      def TextBoxBounds(x, y, breakRowWidth, string)
        bounds = VideoImpl::Float4.new
        ret = VTABLE[:TextBoxBoundsUTF8].call(x, y, breakRowWidth, string, nil, bounds)
        return [ret, bounds]
      end

      #
      # @brief 座標 (x, y)を基準点として文字列 string を描画したときの各文字の位置情報を maxPositions 個取得して 配列 positions に格納する。
      # @param x 基準点のX座標
      # @param y 基準点のY座標
      # @param string 描画する文字列
      # @return 各文字の位置情報配列 {:index => 文字列先頭からのインデクス番号, :x => X基準座標, :minx => X最小位置 :maxx => X最大位置 }
      # ゲーム内にテキスト入力用のGUIを作る場合などで使う
      #
      def TextGlyphPositions(x, y, string)
        maxPositions = string.length
        pString = FFI::MemoryPointer.from_string(string)
        FFI::MemoryPointer.new(VideoImpl::GlyphPosition, maxPositions) do |pPositions|
          ret = VTABLE[:TextGlyphPositionsUTF8].call(x, y, pString, nil, pPositions, maxPositions)
          ret.times.map do |i|
            position = VideoImpl::GlyphPosition.new(pPositions + i * VideoImpl::GlyphPosition.size)
            {
                :index => string.byteslice(0..(position[:str].to_i-pString.to_i)).length,
                :x => position[:x],
                :minx => position[:minx],
                :maxx => position[:maxx]
            }
          end
        end
      end

      #
      # @brief 描画に用いるフォントのメトリクス情報(アセンダ、ディセンダ、行高さ)を取得する
      # @return {:ascender => フォントのアセンダ（ベースラインから文字上部までの高さ）,
      #          :descender => フォントのディセンダ（ベースラインから文字下部までの高さ）,
      #          :lineh => フォントの行高さ
      #         }
      #
      def TextMetrics()
        ascender = VideoImpl::Float.new
        descender = VideoImpl::Float.new
        lineh = VideoImpl::Float.new
        VTABLE[:TextMetrics].call(ascender, descender, lineh)
        return {:ascender => ascender.value, :descender => descender.value, :lineh => lineh.value}
      end

      #
      # @brief テキストを折り返す幅を breakRowWidthとして文字列 string を描画する際の画面上での各行の情報を得る
      # @param string 描画する文字列の始点を示すポインタ
      # @return 各行の情報を格納する TextRow 型変数配列
      # ゲーム内にテキスト入力用のGUIやクリック可能なテキストウィンドウを作る場合などで使う
      #
      def TextBreakLines(string, breakRowWidth)
        maxRows = string.length # 最悪一行に一文字づつ配置されるケースを想定
        pString = FFI::MemoryPointer.from_string(string)
        FFI::MemoryPointer.new(VideoImpl::TextRow, maxRows) do |pRows|
          ret = VTABLE[:TextBreakLinesUTF8].call(pString, nil, breakRowWidth, pRows, maxRows)
          ret.times.map do |i|
            row = VideoImpl::TextRow.new(pRows + i * VideoImpl::TextRow.size)
            {
                :start => string.byteslice(0..(row[:start].to_i-pString.to_i)).length,
                :end => string.byteslice(0..(row[:end].to_i-pString.to_i)).length,
                :next => string.byteslice(0..(row[:next].to_i-pString.to_i)).length,
                :width => row[:width],
                :minx => row[:minx],
                :maxx => row[:maxx]
            }
          end
        end
      end

      #
      # @brief 画像の左上座標(sx, sy), 幅sw, 高さsh で示される領域を画面の左上座標(dx,dy), 幅 dw, 高さ dh に角度 angle, 不透明度 alhpa で描画する
      # @param image 画像ID
      # @param sx 転送元領域の左上のX座標
      # @param sy 転送元領域の左上のY座標
      # @param sw 転送元領域の幅
      # @param sh 転送元領域の高さ
      # @param dx 転送先領域の左上のX座標
      # @param dy 転送先領域の左上のY座標
      # @param dw 転送先領域の幅
      # @param dh 転送先領域の高さ
      # @param angle 画像の回転角度（回転中心は画像の左上であることに注意）
      # @param alpha 不透明度(1.0が完全不透明, 0.0が完全透明)
      #
      def DrawImage(image, sx, sy, sw, sh, dx, dy, dw, dh, angle, alpha)
        VTABLE[:DrawImage].call(image, sx, sy, sw, sh, dx, dy, dw, dh, angle, alpha)
      end

      #
      # @brief 画像の左上座標(sx, sy), 幅sw, 高さsh で示される領域を画面の左上座標(dx,dy), 幅 dw, 高さ dh に角度 angleで描画する。描画色にはFillColorを用いる。
      # @param image 画像ID
      # @param sx 転送元領域の左上のX座標
      # @param sy 転送元領域の左上のY座標
      # @param sw 転送元領域の幅
      # @param sh 転送元領域の高さ
      # @param dx 転送先領域の左上のX座標
      # @param dy 転送先領域の左上のY座標
      # @param dw 転送先領域の幅
      # @param dh 転送先領域の高さ
      # @param angle 画像の回転角度（回転中心は画像の左上であることに注意）
      #
      def DrawImageWithFillColor(image, sx, sy, sw, sh, dx, dy, dw, dh, angle)
        VTABLE[:DrawImageWithFillColor].call(image, sx, sy, sw, sh, dx, dy, dw, dh, angle)
      end

      def FormatText(x, y, format, *args)
        Text(x, y, sprintf(format, *args))
      end

    end
  end
end
