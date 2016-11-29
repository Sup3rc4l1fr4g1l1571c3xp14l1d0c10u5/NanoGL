#
# DxRuby on NanoGL (alpha)
#

require 'nanogl'

class Window
  @@width = 800
  @@height = 600

  def self.width
    return @@width
  end

  def self.height
    return @@height
  end

  def self.width=(value)
    @@width = value
    NanoGL::Video.SetSize(@@width, @@height)
  end

  def self.height=(value)
    @@height = value
    NanoGL::Video.SetSize(@@width, @@height)
  end

  def self.loop(notclose, &block)
    Kernel.loop do
      NanoGL::Video.Drawing()
      Input.update()
      yield
    end
    return nil
  end

  def self.draw(x, y, image)
    NanoGL::Video.DrawImage(image.handle, 0, 0, image.width, image.height, x, y, image.width, image.height, 0, 1.0)
  end

  def self.draw_box(l,t,r,b,color)
    NanoGL::Video.BeginPath
    NanoGL::Video.Rect(l,t,r-l,b-t)
    NanoGL::Video.ClosePath
    NanoGL::Video.StrokeColor(color)
    NanoGL::Video.Stroke
  end

  def self.draw_box_fill(l,t,r,b,color)
    NanoGL::Video.BeginPath
    NanoGL::Video.Rect(l,t,r-l,b-t)
    NanoGL::Video.ClosePath
    NanoGL::Video.FillColor(color)
    NanoGL::Video.Fill
  end

  # 描画ライブラリとして用いているNanoVGの仕様上
  # イタリックや太字は独自実装が必要になるため現状は未対応としておく
  def self.draw_font( x, y, str ,font, options = {})
    options = {:color=> [255,255,255,255] }.merge(options)
    NanoGL::Video.TextAlign(NanoGL::Video::AlignFlags::ALIGN_LEFT | NanoGL::Video::AlignFlags::ALIGN_TOP)
    NanoGL::Video.FontSize(font.size)
    NanoGL::Video.FillColor(options[:color])
    str.split(/\n/).each_with_index do |line, i|
      NanoGL::Video.Text(x,y + font.size * i, line)
    end
  end

  # ファイル開くダイアログ
  def self.open_filename(filter, caption)
    filters = filter.map { |e|
      name = e[0]
      exts = e[1..-1].join(";")
      next "#{name}|#{exts}"
    }.join("\n") if filters != nil
    ret = NanoGL::Dialog.OpenFileDialog(caption,filters)
  end

  # ファイル保存ダイアログ
  def self.save_filename(filter, caption)
    filters = filter.map { |e|
      name = e[0]
      exts = e[1..-1].join(";")
      next "#{name}|#{exts}"
    }.join("\n") if filters != nil
    ret = NanoGL::Dialog.SaveFileDialog(caption, filter)
  end

end

class Image
  attr_reader :handle, :width, :height
  def initialize(handle)
    @handle = handle
    (@width, @height) = NanoGL::Video.ImageSize(@handle)
  end
  def self.load(path)
    return Image.new(NanoGL::Video.CreateImage(path, NanoGL::Video::ImageFlags::IMAGE_RESIZE_NEAREST))
  end
end

class Font
  attr_reader :size, :name, :options, :handle
  def initialize(size, name, options = {})
    @options = { :weight=>false, :italic => false }.merge(options)
    @size = size
    @name = name
    @handle = NanoGL::Video.CreateFont(name, name)
  end
  def self.install(path)
    NanoGL::Video.CreateFont('IPAexゴシック',path)
  end
end

K_SPACE = :K_SPACE
K_F1 = :K_F1
K_F2 = :K_F2
K_F3 = :K_F3
K_F4 = :K_F4
K_F5 = :K_F5
K_F6 = :K_F6
K_F7 = :K_F7
K_F8 = :K_F8
K_F9 = :K_F9
K_F10 = :K_F10
K_F11 = :K_F11
K_F12 = :K_F12
K_ESCAPE = :K_ESCAPE
M_LBUTTON = :M_LBUTTON
M_RBUTTON = :M_RBUTTON
M_MBUTTON = :M_MBUTTON

class Input
  KEY_MAP = {
  K_SPACE => NanoGL::Keyboard::KeyCode::KBKEY_SPACE,
  K_F1 => NanoGL::Keyboard::KeyCode::KBKEY_F1,
  K_F2 => NanoGL::Keyboard::KeyCode::KBKEY_F2,
  K_F3 => NanoGL::Keyboard::KeyCode::KBKEY_F3,
  K_F4 => NanoGL::Keyboard::KeyCode::KBKEY_F4,
  K_F5 => NanoGL::Keyboard::KeyCode::KBKEY_F5,
  K_F6 => NanoGL::Keyboard::KeyCode::KBKEY_F6,
  K_F7 => NanoGL::Keyboard::KeyCode::KBKEY_F7,
  K_F8 => NanoGL::Keyboard::KeyCode::KBKEY_F8,
  K_F9 => NanoGL::Keyboard::KeyCode::KBKEY_F9,
  K_F10 => NanoGL::Keyboard::KeyCode::KBKEY_F10,
  K_F11 => NanoGL::Keyboard::KeyCode::KBKEY_F11,
  K_F12 => NanoGL::Keyboard::KeyCode::KBKEY_F12,
  K_ESCAPE => NanoGL::Keyboard::KeyCode::KBKEY_ESCAPE
  }
  @@PushKeyTable = {}
  @@PushMouseTable = {}
  @@mouse_x = 0
  @@mouse_y = 0

  def self.requested_close?
    # 殺しておく
    return false
  end

  def self.update
    # mouse
    [M_LBUTTON, M_RBUTTON, M_MBUTTON].each do |btn|
      case btn
        when M_LBUTTON
          ret = NanoGL::Mouse.IsLeftButtonDown()
        when M_RBUTTON
          ret = NanoGL::Mouse.IsRightButtonDown()
        when M_MBUTTON
          ret = NanoGL::Mouse.IsMiddleButtonDown()
        else
          return false
      end
      if (ret)
        @@PushMouseTable[btn] = (@@PushMouseTable[btn] || 0) + 1
      else
        @@PushMouseTable[btn] = 0
      end
      (@@mouse_x, @@mouse_y) = NanoGL::Mouse.GetCursorPos().map(&:to_i)

    end

    # keyboard
    KEY_MAP.each do |dx,ng|
      ret = NanoGL::Keyboard.IsKeyDown(ng)
      if (ret)
        @@PushKeyTable[dx] = (@@PushKeyTable[dx] || 0) + 1
      else
        @@PushKeyTable[dx] = 0
      end
    end

  end

  def self.key_push?(key)
    return @@PushKeyTable[key] == 1
  end

  def self.mouse_push?(btn)
    return @@PushMouseTable[btn] == 1
  end

  def self.mouse_pos_x
    return @@mouse_x
  end

  def self.mouse_pos_y
    return @@mouse_y
  end
end


class Sound
  attr_reader :loop_count
  @@FreeChannels = 256.times.to_a
  def initialize(file)
    ch = @@FreeChannels.shift
    @loop_count=0
    if NanoGL::Sound.ChannelCreate(ch, file) 
      @ch = ch
    else
      @ch = -1
    end
  end
  def dispose()
    if @ch != -1
      @@FreeChannels << @ch
      NanoGL::Sound.ChannelFree(@ch)
      @ch = -1
    end
  end
  def play()
    if @ch != -1
      NanoGL::Sound.ChannelPlay(@ch, @loop_count != 0)
    end
  end
  def stop()
    if @ch != -1
      NanoGL::Sound.ChannelStop(@ch)
    end
  end
end
