
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

