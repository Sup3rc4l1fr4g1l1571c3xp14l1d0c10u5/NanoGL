require 'ffi'

module NanoGL
  if FFI::Platform.mac?
    SHARED_LIB_PATH = File.expand_path('../libnanogl.bundle', __FILE__)
  elsif FFI::Platform.windows?
    SHARED_LIB_PATH = File.expand_path('../libnanogl.dll', __FILE__)
  elsif FFI::Platform.unix?
    SHARED_LIB_PATH = File.expand_path('../libnanogl.so', __FILE__)
  else
    raise "NanoGL is not supported in this platform."
  end
end

require_relative './NanoGL/Video.rb'
require_relative './NanoGL/Video.Helper.rb'
require_relative './NanoGL/Mouse.rb'
require_relative './NanoGL/Keyboard.rb'
require_relative './NanoGL/Sound.rb'
require_relative './NanoGL/FpsTimer.rb'
require_relative './NanoGL/System.rb'
require_relative './NanoGL/Dialog.rb'
require_relative './NanoGL/Joystick.rb'

if $0 == __FILE__
  require_relative './NanoGL/SpecTest.rb'
else
  Dir.chdir(File.expand_path('../', $0))

  NanoGL::Video.Initialize()
  NanoGL::Sound.Initialize()
  
  NanoGL::System.SetQuitHandler do 
    NanoGL::Sound.Finalize()
    NanoGL::Video.Finalize()
  end
  Kernel.at_exit do 
    NanoGL::Sound.Finalize()
    NanoGL::Video.Finalize()
  end
end
