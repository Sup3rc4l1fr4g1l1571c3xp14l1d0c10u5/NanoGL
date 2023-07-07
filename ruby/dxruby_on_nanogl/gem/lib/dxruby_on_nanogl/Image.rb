require_relative './BresenhamLine.rb'
require_relative './PerlinNoise.rb'

class Image
  def self.color_dxruby2nanogl(color)
    if color == nil || !color.is_a?(Array)
      color = [0,0,0,0]
    else
      if color.length < 4
        color = (color[0..2] + [255])
      else
        color = color.rotate
      end
    end
    return color
  end
  attr_reader :handle, :width, :height, :data
  
  def initialize(*args)
    if args.length == 1 
      @handle = args[0]
      @data = nil
      (@width, @height) = NanoGL::Video.ImageSize(@handle)
      @need_sync = false
    elsif args.length == 2 || args.length == 3
      (@width, @height, color) = args[0..2]
      color = Image.color_dxruby2nanogl(color)
      @data = color * (@width * @height)
      @need_sync = true
    else
      raise "invalid argument number exception #{args}"
    end
  end
  
  def self.load(path)
    return Image.new(NanoGL::Video.CreateImage(path, NanoGL::Video::ImageFlags::IMAGE_RESIZE_NEAREST))
  end

  def self.load_tiles(path, xcount, ycount, share_switch=true)
    #share_switchは現在未対応
    (w, h, imagedata) = NanoGL::Video.LoadImageData(path)
    ww = (w / xcount).to_i
    hh = (h / ycount).to_i
    images = []
    ycount.times do |yc|
      y = yc * hh
      xcount.times do |xc|
        x = xc * ww
        pixels = []
        hh.times do |yy|
          start = (y + yy) * w * 4 + x * 4
          
          ddd = imagedata[start, ww*4]
          pixels += ddd
        end
        x = NanoGL::Video.CreateImageRGBA(ww,hh,NanoGL::Video::ImageFlags::IMAGE_RESIZE_NEAREST,pixels)
        images << Image.new(x)
      end
    end
    return images
  end

  @@perlin_noise = nil
  
  def self.perlin_noise(x,y,z,repeat_x=256,repeat_y=256,repeat_z=256)
    # repeat_x, repeat_y, repeat_z については現時点では対応せず
    if @@perlin_noise == nil
       @@perlin_noise = PerlinNoise.new
     end
     @@perlin_noise.noise0_1(x,y,z)
  end
  
  def self.octave_perlin_noise(x,y,z,octave,persistence,repeat_x=256,repeat_y=256,repeat_z=256)
    # repeat_x, repeat_y, repeat_z については現時点では対応せず
    if @@perlin_noise == nil
       @@perlin_noise = PerlinNoise.new
     end
     @@perlin_noise.octaveNoise0_1(octave.to_i, persistence, x, y, z)
  end

  def sync!
    if @need_sync
      if @data != nil
        if @handle == nil
          @handle = NanoGL::Video.CreateImageRGBA(@width, @height, NanoGL::Video::ImageFlags::IMAGE_RESIZE_NEAREST, @data)
        else
          NanoGL::Video.UpdateImage(@handle, @data.pack('C*'))
        end
      end
      @need_sync = false
    end
  end

  def []=( x, y ,v )
    if @data == nil
      return
    end
    x = x.to_i
    y = y.to_i
    if x < 0 || @width <= x || y < 0 || @height <= y
      return
    end
    v = Image.color_dxruby2nanogl(v.map(&:to_i))
    idx = y * (@width * 4) + x * 4
    @data[idx..idx+3] = v
    @need_sync = true
  end

  def []( x, y )
    if @data == nil
      return
    end
    x = x.to_i
    y = y.to_i
    if x < 0 || @width <= x || y < 0 || @height <= y
      return [0,0,0,0]
    end
    idx = y * (@width * 4) + x * 4
    return Image.color_nanogl2dxruby(@data[idx..idx+3])
  end

  def line(sx, sy, ex, ey, color)
    if @data == nil
      return
    end
    color = color.map(&:to_i)
    s = BresenhamLine::Point.new(sx,sy);
    e = BresenhamLine::Point.new(ex,ey);
    area = BresenhamLine::Rect.new(0,0,@width,@height)
    BresenhamLine.clipping(area,s,e)
    BresenhamLine.line(s,e) { |x,y|  self[x,y] = color  }
    return self
  end

  def box_fill(x1, y1, x2, y2, color)
    if @data == nil
      return
    end
    color = color.map(&:to_i)
    x = [[0, x1.to_i, @width-1].sort.at(1), [0, x2.to_i, @width-1].sort.at(1)].sort
    y = [[0, y1.to_i, @height-1].sort.at(1), [0, y2.to_i, @height-1].sort.at(1)].sort

    (y[0]..y[1]).each do |y|
      (x[0]..x[1]).each do |x|
        self[x,y] = color
      end
    end
    return self
  end

  def self.floor(sqr, limit)
    n = limit
    sum = limit * limit
    while sum > sqr
      n -= 1
      sum -= 2*n + 1
    end
    return n
  end

  def circle_draw(x0, y0, r, color)
    if @data == nil
      return
    end
    color = color.map(&:to_i)
    diameter = (r * 2).to_i
    cx = 0
    cy = (diameter / 2).to_i + 1
    d = (-diameter * diameter + 4 * cy * cy - 4 * cy + 2)
    dx = 4
    dy = -8 * cy + 8
    mirror_center_x = x0
    mirror_center_y = y0

    if (diameter & 1) ==0
      mirror_center_x += 1
      mirror_center_y += 1
    end

    cx = 0
    while (cx <= cy) do
      if (d > 0)
        d += dy
        dy += 8
        cy -= 1
      end

      self[-cy + mirror_center_x,  cx + y0] = color # 135-180  度の間
      self[ cy +              x0,  cx + y0] = color # 0-45     度の間

      self[-cx + mirror_center_x,  cy + y0] = color # 90-135   度の間
      self[ cx +              x0,  cy + y0] = color # 45-90    度の間

      self[-cy + mirror_center_x, -cx + mirror_center_y] = color # 180-225  度の間
      self[ cy +              x0, -cx + mirror_center_y] = color # 315-360  度の間

      self[-cx + mirror_center_x, -cy + mirror_center_y] = color # 225-270  度の間
      self[ cx +              x0, -cy + mirror_center_y] = color # 270-315  度の間

      d  += dx
      dx += 8
      cx += 1
    end
    return self
  end

  def circle_fill(x0, y0, r, color)
    if @data == nil
      return
    end
    color = color.map(&:to_i)
    diameter = (r * 2).to_i
    cx = 0
    cy = (diameter / 2).to_i + 1
    d = (-diameter * diameter + 4 * cy * cy - 4 * cy + 2)
    dx = 4
    dy = -8 * cy + 8
    mirror_center_x = x0
    mirror_center_y = y0
    if (diameter & 1) ==0
      mirror_center_x += 1
      mirror_center_y += 1
    end

    cx = 0
    while (cx <= cy) do
      if (d > 0)
        d += dy
        dy += 8
        cy -= 1
      end

      (-cy + mirror_center_x ..  cy + x0).each { |x| self[x, cx + y0] = color }
      #self[-cy + mirror_center_x,  cx + y0] = color # 135-180  度の間
      #self[ cy +              x0,  cx + y0] = color # 0-45     度の間

      (-cx + mirror_center_x ..  cx + x0).each { |x| self[x, cy + y0] = color }
      #self[-cx + mirror_center_x,  cy + y0] = color # 90-135   度の間
      #self[ cx +              x0,  cy + y0] = color # 45-90    度の間

      (-cy + mirror_center_x ..  cy + x0).each { |x| self[x, -cx + mirror_center_y] = color }
      #self[-cy + mirror_center_x, -cx + mirror_center_y] = color # 180-225  度の間
      #self[ cy +              x0, -cx + mirror_center_y] = color # 315-360  度の間

      (-cx + mirror_center_x ..  cx + x0).each { |x| self[x, -cy + mirror_center_y] = color }
      #self[-cx + mirror_center_x, -cy + mirror_center_y] = color # 225-270  度の間
      #self[ cx +              x0, -cy + mirror_center_y] = color # 270-315  度の間

      d  += dx
      dx += 8
      cx += 1
    end
    return self
  end

  def compare(x,y,color)
    if @data == nil
      return false
    end
    color = Image.color_dxruby2nanogl(color.map(&:to_i))
    x = x.to_i
    y = y.to_i
    if x < 0 || @width <= x || y < 0 || @height <= y
      return false
    end
    idx = y * (@width * 4) + x * 4
    return @data[idx..idx+3] == color
  end

  def copy_rect(x, y, image, x1=0, y1=0, width=image.width, height=image.height)
    src_rect = [x1,y1,x1+width,y1+width]
    dst_rect = [x,y,x+width,y+width]

    if (src_rect[0] < 0)
      dst_rect[0] -= src_rect[0]
      src_rect[0] -= src_rect[0]
    end
    if (src_rect[1] < 0)
      dst_rect[1] -= src_rect[1]
      src_rect[1] -= src_rect[1]
    end
    if (src_rect[2] >= image.width)
      dst_rect[2] -= (src_rect[2] - image.width)
      src_rect[2] -= (src_rect[2] - image.width)
    end
    if (src_rect[3] >= image.height)
      dst_rect[3] -= (src_rect[3] - image.height)
      src_rect[3] -= (src_rect[3] - image.height)
    end

    if (dst_rect[0] < 0)
      src_rect[0] -= dst_rect[0]
      dst_rect[0] -= dst_rect[0]
    end
    if (dst_rect[1] < 0)
      src_rect[1] -= dst_rect[1]
      dst_rect[1] -= dst_rect[1]
    end
    if (dst_rect[2] >= @width)
      src_rect[2] -= (dst_rect[2] - @width)
      dst_rect[2] -= (dst_rect[2] - @width)
    end
    if (dst_rect[3] >= @height)
      src_rect[3] -= (dst_rect[3] - @height)
      dst_rect[3] -= (dst_rect[3] - @height)
    end

    if (src_rect[0] >= src_rect[2]) || (src_rect[1] >= src_rect[3]) || (dst_rect[0] >= dst_rect[2]) || (dst_rect[1] >= dst_rect[3])
      return
    end

    w = (src_rect[2] - src_rect[0]).to_i
    h = (src_rect[3] - src_rect[1]).to_i

    sx = src_rect[0].to_i
    sy = src_rect[1].to_i
    dx = dst_rect[0].to_i
    dy = dst_rect[1].to_i

    h.times do |y|
      dl = (dy+y) * (    @width * 4) + dx * 4
      sl = (sy+y) * (image.width * 4) + sx * 4
      self.data[dl..dl+w*4] = image.data[sl..sl+w*4]
    end
    @need_sync = true
  end

  alias :boxFill :box_fill
  alias :circleFill :circle_fill
  alias :circle :circle_draw
  alias :copyRect :copy_rect
  class << self
    alias :loadToArray :load_tiles
  end

end

