class BresenhamLine

  class Point
    attr_accessor :x, :y

    def initialize(x,y)
      @x = x
      @y = y
    end

    def dup
      Point.new(x,y)
    end
  end

  class Rect
    attr_accessor :left, :top, :width, :height

    def initialize(left, top, width, height)
      @left = left
      @top = top
      @width = width
      @height = height
    end

    def right
      @left + @width
    end

    def bottom
      @top + @height
    end

    def dup
      Rect.new(left,top,width, height)
    end
  end

  LEFT = 1
  RIGHT = 2
  TOP = 4
  BOTTOM = 8

  def self.calc_seq_code( area, c )
    code = 0
    
    if c.x < area.left
      code |= LEFT
    end
    if c.x > area.right
      code |= RIGHT
    end
    if c.y < area.top
      code |= TOP
    end
    if c.y > area.bottom 
      code |= BOTTOM
    end

    return code
  end

  def self.calc_intsec_x( area, c0, c1, clip_x, c )
    cy = (( c1.y - c0.y ) * ( clip_x - c0.x ) / ( c1.x - c0.x )).to_i + c0.y

    # エリア外の場合はFalseを返す
    if ( cy < area.top ) || ( cy > area.bottom )
      return 0
    end

    c.x = clip_x
    c.y = cy

    return 1
  end

  def self.calc_intsec_y( area, c0, c1, clip_y, c )
    cx = (( c1.x - c0.x ) * ( clip_y - c0.y ) / ( c1.y - c0.y )).to_i + c0.x

    # エリア外の場合はFalseを返す
    if ( cx < area.left ) || ( cx > area.right ) 
      return 0
    end

    c.x = cx
    c.y = clip_y

    return 1
  end

  def self.calc_clipped_point( area, code, c0, c1, c)
    # ウィンドウの左端より外側にある
    if ( code & LEFT ) != 0
      if calc_intsec_x( area, c0, c1, area.left, c )
        return true
      end
    end

    # ウィンドウの右端より外側にある
    if ( code & RIGHT ) != 0
      if calc_intsec_x( area, c0, c1, area.right, c )
        return true
      end
    end

    # ウィンドウの上端より外側にある
    if ( code & TOP ) != 0
      if calc_intsec_y( area, c0, c1, area.top, c )
        return true
      end
    end

    # ウィンドウの下端より外側にある
    if ( code & BOTTOM ) != 0
      if calc_intsec_y( area, c0, c1, area.bottom, c )
        return true
      end
    end

    return false # クリッピングされなかった場合、線分は完全に不可視
  end

  def self.clipping( area, c0, c1 )

    code0 = calc_seq_code( area, c0 )  # 始点の端点分類コードを求める
    code1 = calc_seq_code( area, c1 )  # 終点の端点分類コードを求める

    # 端点分類コードがどちらも0000ならばクリッピングは必要なし
    if (code0 == 0) && (code1 == 0) 
      return 0
    end

    # 始点・終点の端点分類コードの論理積が非０ならば線分は完全に不可視
    if ( code0 & code1 ) != 0
      return -1
    end

    cc0 = c0.dup
    cc1 = c1.dup

    # 始点のクリッピング
    if code0 != 0
      if calc_clipped_point( area, code0, cc0, cc1, c0 ) == false
        return -1
      end
    end

    # 終点のクリッピング
    if code1 != 0
      if calc_clipped_point( area, code1, cc0, cc1, c1 ) == false
        return -1
      end
    end

    return 1
  end

  def self.line( s, e , &block)
    dx = ( e.x > s.x ) ? e.x - s.x : s.x - e.x
    dy = ( e.y > s.y ) ? e.y - s.y : s.y - e.y
    sx = ( e.x > s.x ) ? 1 : -1
    sy = ( e.y > s.y ) ? 1 : -1

    if dx >= dy
      # 傾きが1以下の場合
      _e = -dx
      (dx+1).times do
        block.call( s.x, s.y )
        s.x += sx
        _e += 2 * dy
        if _e >= 0
          s.y += sy
          _e -= 2 * dx
        end
      end
    else
      # 傾きが1より大きい場合
      _e = -dy
      (dy+1).times do
        block.call( s.x, s.y )
        s.y += sy
        _e += 2 * dx
        if _e >= 0
          s.x += sx
          _e -= 2 * dy
        end
      end
    end
  end

end

