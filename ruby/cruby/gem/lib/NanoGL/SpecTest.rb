class TestObject
  def initialize(desc, &block)
    @desc = desc
    @result = yield
  end

  def should_be(&block)
    x = block.call(@result)
    if (x)
      puts "#{@desc}: OK."
    else
      puts "#{@desc}: NG!"
    end
  end
end

def test(desc, &block)
  TestObject.new(desc, &block)
end

color = NanoGL::Video::VideoImpl::Color.new
test('Color.to_a') { color.to_a }.should_be { |x| x == [0, 0, 0, 0] }
test('Color.from_a') { NanoGL::Video::VideoImpl::Color.from_a([1, 2, 3, 4]) }.should_be { |x| x.to_a == [1, 2, 3, 4] }
test('Color.to_s') { NanoGL::Video::VideoImpl::Color.from_a([1, 2, 3, 4]).to_s }.should_be { |x| 'Color(1, 2, 3, 4)' }

paint = NanoGL::Video::VideoImpl::Paint.new
test('Paint.to_h') { paint.to_h }.should_be { |x| x == NanoGL::Video::VideoImpl::Paint::DEFAULT }
data = NanoGL::Video::VideoImpl::Paint::DEFAULT.merge({
                                                          :xform => [1, 2, 3, 4, 5, 6],
                                                          :extent => [50, 60],
                                                          :radius => 1,
                                                          :feather => 80,
                                                          :innerColor => [5, 6, 7, 8],
                                                          :outerColor => [1, 2, 3, 4],
                                                          :image => 456
                                                      })
test('Paint.from_h') { NanoGL::Video::VideoImpl::Paint.from_h(data) }.should_be { |x| x.to_h == data }
test('Paint.to_s') { NanoGL::Video::VideoImpl::Paint.from_h(data).to_s }.should_be { |x| "Paint(#{data.to_s})" }
NanoGL::Video.Initialize();
NanoGL::Video.CreateFont("font", 'C:\msys\home\student\NanoGL-current\example\07-Text\font\IPAexfont00301\ipaexg.ttf');
NanoGL::Video.FontSize(12);
p NanoGL::Video.TextGlyphPositions(0,0,"hello, world")
NanoGL::Video.Finalize();
