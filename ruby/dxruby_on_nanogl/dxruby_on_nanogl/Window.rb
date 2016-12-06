FORMAT_JPG = :FORMAT_JPG
FORMAT_PNG = :FORMAT_PNG
FORMAT_BMP = :FORMAT_BMP
FORMAT_DDS = :FORMAT_DDS

class Window
  # dxrubyはARGB, NanoGL(NanoVG)はRGBA
  def self.color_dxruby2nanogl(color)
    if color == nil || !color.is_a?(Array)
      color = [0,0,0,0]
    else
      if color.length < 4
        color = (color[0..2] + [255]).map { |x| x / 255.0 }
      else
        color = color.rotate.map { |x| x / 255.0 }
      end
    end
    return color
  end

  def self.color_nanogl2dxruby(color)
    if color == nil || !color.is_a?(Array)
      color = [0,0,0,0]
    else
      color = color.rotate(-1).map { |x| (x * 255).to_i }
    end
    return color
  end

  def self.apply_blend(mode)
    case mode
      when :alpha then
        NanoGL::Video.GlobalCompositeBlendFuncSeparate(
            NanoGL::Video::BlendFactor::SRC_ALPHA,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_ALPHA,
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_ALPHA,
        )
      when :none then
        NanoGL::Video.GlobalCompositeBlendFuncSeparate(
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ZERO,
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ZERO,
        )
      when :add then
        NanoGL::Video.GlobalCompositeBlendFuncSeparate(
            NanoGL::Video::BlendFactor::SRC_ALPHA,
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_ALPHA,
        )
      when :add2 then
        NanoGL::Video.GlobalCompositeBlendFuncSeparate(
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_ALPHA,
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_ALPHA,
        )
      when :sub then
        NanoGL::Video.GlobalCompositeBlendFuncSeparate(
            NanoGL::Video::BlendFactor::ZERO,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_ALPHA,
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_ALPHA,
        )
      when :sub2 then
        NanoGL::Video.GlobalCompositeBlendFuncSeparate(
            NanoGL::Video::BlendFactor::SRC_ALPHA,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_COLOR,
            NanoGL::Video::BlendFactor::ONE,
            NanoGL::Video::BlendFactor::ONE_MINUS_SRC_ALPHA,
        )
    end
  end

  @@width = 800
  @@height = 600
  @@scale = 1
  @@caption = ''

  @@cache = {}
  @@screenshot = []

  def self.width
    return @@width
  end

  def self.height
    return @@height
  end

  def self.scale
    return @@scale
  end

  def self.caption
    return @@caption
  end

  def self.width=(value)
    @@width = value
    NanoGL::Video.SetSize(@@width * @@scale , @@height * @@scale )
  end

  def self.height=(value)
    @@height = value
    NanoGL::Video.SetSize(@@width * @@scale , @@height * @@scale )
  end

  def self.scale=(value)
    @@scale = value
    NanoGL::Video.SetSize(@@width * @@scale , @@height * @@scale )
  end

  def self.caption=(value)
    @@caption = value
    NanoGL::Video.SetWindowTitle(value)
  end

  def self.loop(close_cancel=false, &block)
    Kernel.loop do
      NanoGL::Video.Loop()
      Input.update()
      NanoGL::Video.ResetDrawEx()
      yield
      @@cache.keys.sort().each { |key| @@cache[key].each { |c| c.call() } }
      @@cache.clear
      NanoGL::Video.UpdateDrawEx()
      @@screenshot.each { |s| s.call() }
      @@screenshot.clear
    end
    return nil
  end

  def self.begin_draw(z, &block)

    @@cache[z] = @@cache[z] || []
    @@cache[z] << lambda {
      NanoGL::Video.BeginDrawEx()
      NanoGL::Video.Scale(@@scale,@@scale)
      NanoGL::Video.Save()
      block.call
      NanoGL::Video.Restore()
      NanoGL::Video.EndDrawEx()
    }
  end

  def self.draw(x, y, image, z = 0)
    image.sync!
    self.begin_draw(z) do
      NanoGL::Video.DrawImage(image.handle, 0, 0, image.width, image.height, x, y, image.width, image.height, 0, 1.0)
    end
  end

  def self.draw_ex(x, y, image, option={})
    option = {
        :center_x => nil,
        :center_y => nil,
        :alpha => 255,
        :blend => :alpha,
        :color => [255,255,255,255],
        :angle => 0,
        :z => 0
    }.merge(option)
    if option[:center_x] == nil
      option[:center_x] = image.width*0.5
    end
    if option[:center_y] == nil
      option[:center_y] = image.height*0.5
    end
    image.sync!
    self.begin_draw(option[:z]) do
      NanoGL::Video.Translate(x, y)
      NanoGL::Video.Translate(option[:center_x], option[:center_y])
      NanoGL::Video.Rotate(NanoGL::Video.DegToRad(option[:angle]))
      NanoGL::Video.Scale(option[:scale_x] || option[:scalex] || 1, option[:scale_y] || option[:scaley] || 1)
      NanoGL::Video.Translate(-option[:center_x], -option[:center_y])
      NanoGL::Video.FillColor(NanoGL::Video.TransRGBA(Window.color_dxruby2nanogl(option[:color]),option[:alpha]))
      self.apply_blend(option[:blend])
      NanoGL::Video.DrawImageWithFillColor(image.handle, 0, 0, image.width, image.height, 0, 0, image.width, image.height, 0)
    end
  end

  def self.draw_rot(x, y, image, angle, center_x=nil, center_y=nil, z=0)
    image.sync!
    center_x = center_x || image.width*0.5
    center_y = center_y || image.height*0.5

    self.begin_draw(z) do
      NanoGL::Video.Translate(x, y)
      NanoGL::Video.Translate(center_x, center_y)
      NanoGL::Video.Rotate(NanoGL::Video.DegToRad(angle))
      NanoGL::Video.Translate(-center_x, -center_y)
      NanoGL::Video.DrawImage(image.handle, 0, 0, image.width, image.height, 0, 0, image.width, image.height, 0, 1.0)
    end
  end

  def self.draw_scale(x, y, image, scale_x, scale_y, center_x=nil, center_y=nil, z=0)
    image.sync!
    center_x = center_x || image.width*0.5
    center_y = center_y || image.height*0.5

    self.begin_draw(z) do
      NanoGL::Video.Translate(x, y)
      NanoGL::Video.Translate(center_x, center_y)
      NanoGL::Video.Scale(scale_x, scale_y)
      NanoGL::Video.Translate(-center_x, -center_y)
      NanoGL::Video.DrawImage(image.handle, 0, 0, image.width, image.height, 0, 0, image.width, image.height, 0, 1.0)
    end
  end

  def self.draw_alpha(x, y, image, alpha, z = 0)
    image.sync!
    self.begin_draw(z) do
      NanoGL::Video.DrawImage(image.handle, 0, 0, image.width, image.height, x, y, image.width, image.height, 0, alpha / 255.0)
    end
  end

  def self.draw_box(l,t,r,b,color,z=0)
    c = Window.color_dxruby2nanogl(color)
    self.begin_draw(z) do
      NanoGL::Video.BeginPath
      NanoGL::Video.Rect(l,t,r-l,b-t)
      NanoGL::Video.ClosePath
      NanoGL::Video.StrokeColor(c)
      NanoGL::Video.Stroke
    end
  end

  def self.draw_box_fill(l,t,r,b,color,z=0)
    c = Window.color_dxruby2nanogl(color)
    self.begin_draw(z) do
      NanoGL::Video.BeginPath
      NanoGL::Video.Rect(l,t,r-l,b-t)
      NanoGL::Video.ClosePath
      NanoGL::Video.FillColor(c)
      NanoGL::Video.Fill
    end
  end

  def self.draw_font( x, y, str ,font, options = {})
    lines = str.split("\n")
    if lines.count == 0
      return
    end
    option = {
        :scale_x => 1,
        :scale_y => 1,
        :center_x => nil,
        :center_y => nil,
        :alpha => 255,
        :blend => :alpha,
        :color => [255,255,255],
        :angle => 0,
        :z => 0
    }.merge(options)
    self.begin_draw(option[:z]) do
      NanoGL::Video.TextAlign(NanoGL::Video::AlignFlags::ALIGN_LEFT | NanoGL::Video::AlignFlags::ALIGN_TOP)
      NanoGL::Video.FontSize(font.size)
      rowWidth = lines.map { |l| NanoGL::Video.TextBounds(0,0,l)[1] }.map { |b| b[2]-b[0] }.max
      bounds = NanoGL::Video.TextBoxBounds(0,0,rowWidth, str)
      if option[:center_x] == nil
        option[:center_x] = (bounds[2] - bounds[0])*0.5
      end
      if option[:center_y] == nil
        option[:center_y] = (bounds[3] - bounds[1])*0.5
      end
      NanoGL::Video.Translate(x, y)
      NanoGL::Video.Translate(option[:center_x], option[:center_y])
      NanoGL::Video.Rotate(NanoGL::Video.DegToRad(option[:angle]))
      NanoGL::Video.Scale(option[:scale_x] || option[:scalex] || 1, option[:scale_y] || option[:scaley] || 1)
      NanoGL::Video.Translate(-option[:center_x], -option[:center_y])
      c = NanoGL::Video.TransRGBA(Window.color_dxruby2nanogl(option[:color]),option[:alpha])
      NanoGL::Video.FillColor(c)
      self.apply_blend(option[:blend])
      NanoGL::Video.TextBox(0,0,rowWidth, str)
    end
  end

  # ファイル開くダイアログ
  def self.open_filename(filter, caption)
    if filter != nil
      filter = filter.map { |e|
        name = e[0]
        exts = e[1..-1].join(";")
        next "#{name}|#{exts}"
      }.join("\n")
    else
      filter = "all|*.*"
    end
    ret = NanoGL::Dialog.OpenFileDialog(caption,filter)
  end

  # ファイル保存ダイアログ
  def self.save_filename(filter, caption)
    if filter != nil
      filter = filter.map { |e|
        name = e[0]
        exts = e[1..-1].join(";")
        next "#{name}|#{exts}"
      }.join("\n")
    else
      filter = "all|*.*"
    end
    ret = NanoGL::Dialog.SaveFileDialog(caption, filter)
  end

  def self.fps
    NanoGL::FpsTimer.GetRealFPS()
  end

  def self.get_load
    NanoGL::FpsTimer.GetCPUPower()
  end

  def self.bgcolor=(color)
    NanoGL::Video.SetClearColor(Window.color_dxruby2nanogl(color))
  end

  def self.get_screen_shot(path, format=FORMAT_JPG)
    @@screenshot << lambda { NanoGL::Video.SaveScreenShot(0,0,self.width, self.height, false, path) }
  end

  class << self
    alias :openFilename :open_filename
    alias :drawFont :draw_font
    alias :getLoad :get_load
    alias :drawRot :draw_rot
    alias :drawScale :draw_scale
    alias :drawAlpha :draw_alpha
    alias :drawEx :draw_ex
    alias :getScreenShot :get_screen_shot
  end

end

