
class RenderTarget
private
  def render_to(&block)
    NanoGL::Video.DrawStartFramebuffer(@pFrameBuffer)
    begin
      yield
    ensure
      NanoGL::Video.DrawEndFramebuffer()
    end
  end
public
  attr_reader :width, :height

  def handle
    NanoGL::Video.GetFrameBufferImage(@pFrameBuffer)
  end

  def initialize(width, height, bgcolor=[0,0,0,0])
    @width = width
    @height = height
    @bgcolor = Window.color_dxruby2nanogl(bgcolor)
    @pFrameBuffer = NanoGL::Video.CreateFramebuffer(width, height, NanoGL::Video::ImageFlags::IMAGE_REPEATX | NanoGL::Video::ImageFlags::IMAGE_REPEATY) # nvgluCreateFramebuffer
    render_to do
    end
  end

  def draw_tile(base_x, base_y, map, image_array, start_x, start_y, size_x, size_y, z=0)
    render_to do
      (sw,sh) = NanoGL::Video.GetSize()
      (cw,ch) = [image_array[0].width, image_array[0].height]
      base_x = base_x == nil ? 0 : base_x
      base_y = base_y == nil ? 0 : base_y
      start_x = start_x == nil ? 0 : start_x
      start_y = start_y == nil ? 0 : start_y
      size_x = size_x == nil ? ((sw+cw-1)/cw) : size_x
      size_y = size_y == nil ? ((sh+ch-1)/ch) : size_y
      size_x += 1
      size_y += 1

      start_chip_x = start_x / cw
      start_chip_y = start_y / ch
      offset_x = start_x % cw
      offset_y = start_y % ch
      draw_x = base_x - offset_x
      draw_y = base_y - offset_y

      size_y.times do |y|
        cy = start_chip_y + y
        if (cy < 0)
          while (cy < 0) do
            cy += map.length
          end
        elsif (cy >= map.length)
          while (cy >= map.length) do
            cy -= map.length
          end
        end
        mapline = map[cy]
        size_x.times do |x|
          cx = start_chip_x + x
          if (cx < 0)
            while (cx < 0) do
              cx += mapline.length
            end
          elsif (cx >= mapline.length)
            while (cx >= mapline.length) do
              cx -= mapline.length
            end
          end
          image_no = mapline[cx]
          if image_no == nil
            next
          end
          image = image_array[image_no]
          image.sync!
          NanoGL::Video.DrawImage(image.handle, 0, 0, cw, ch, draw_x + x * cw, draw_y + y * cw, cw, ch, 0, 1.0)
        end
      end
    end
  end

  def update
    # nothing
  end

alias :drawTile :draw_tile
alias :sync! :update

end
