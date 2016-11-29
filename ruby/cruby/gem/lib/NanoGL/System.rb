module NanoGL
  module System
    module SystemImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private

      callback :QuitHandler, [], :void
      
      class VTable < FFI::Struct
        layout :Run, callback([:int, :pointer], :void),
               :Quit, callback([], :void),
               :GetCommandLineArgCount, callback([], :int),
               :GetCommandLineArgValue, callback([:int], :string),
               :SetQuitHandler, callback([:QuitHandler], :void)
      end

      attach_function :GetSystemVTable, [], SystemImpl::VTable.by_ref
    end
    private
    VTABLE = SystemImpl::GetSystemVTable()
    public
    class << self
    private
      @@block = nil
      Callback = FFI::Function.new(:void, []) do
        if @@block != nil
          @@block.call
        end
      end
    public
      def Quit()
        VTABLE[:Quit].call()
      end

      def SetQuitHandler(&block)
        @@block = block
        VTABLE[:SetQuitHandler].call(Callback)
      end

    end
  end
end
