class Font
  
  if ((1/2) != 0)
    @@dir = Dir.pwd
  else
    @@dir = File.expand_path('..', __FILE__ )
  end

  attr_reader :size, :name, :options, :handle
  @@load_default_font = false
  def initialize(size, name=nil, options = {})
    @options = { :weight=>false, :italic => false }.merge(options)
    @size = size
    if @@load_default_font == false
      NanoGL::Video.CreateFont('default', File.expand_path('./ipaexg.ttf', @@dir))
      @@load_default_font = true
    end
    if name == nil
      @name = 'default'
    else
      @name = name
    end
    @handle = NanoGL::Video.FindFont(@name)
  end
  def self.install(path)
    NanoGL::Video.CreateFont('default',path)
  end
end

