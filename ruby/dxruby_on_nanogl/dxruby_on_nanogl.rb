require 'NanoGL'

# とりあえずそれっぽく動く程度です

require_relative './dxruby_on_nanogl/Window.rb'
require_relative './dxruby_on_nanogl/Image.rb'
require_relative './dxruby_on_nanogl/RenderTarget.rb'
require_relative './dxruby_on_nanogl/Input.rb'
require_relative './dxruby_on_nanogl/Font.rb'
require_relative './dxruby_on_nanogl/Sound.rb'
require_relative './dxruby_on_nanogl/SoundEffect.rb'

if (1/2) != 0
  # mruby detect! :-)
  
  class << Dir
    @@curdir = Dir.pwd
    def tmpdir
      @@curdir+"/tmp"
    end
  end
  if Dir.exists?(Dir.tmpdir) == false
    Dir.mkdir(Dir.tmpdir)
  end

  class Tempfile 
    def initialize(basename, tempdir = Dir::tmpdir)
      @deleted = false
      @basename = basename
      @mode = "wb+"
      @perm = 0600
      @path = @path || make_tmpname(basename, tempdir)

      @entity = TempfilePath.new(@path)
      super(@path, @mode, @perm)
    end
    def binmode
    end
    def binmode?
      true
    end
    def unlink()
      File.delete(@path)
    end
  end
  
  class Array
    def transpose
      col_size = self.count
      row_size = self.map { |r| r.count }.max
      t = Array.new(row_size) { Array.new(col_size) } 
      col_size.times do |y|
        row_size.times do |x|
          t[y][x] = self[x][y]
        end
      end
      return t
    end
  end
end

