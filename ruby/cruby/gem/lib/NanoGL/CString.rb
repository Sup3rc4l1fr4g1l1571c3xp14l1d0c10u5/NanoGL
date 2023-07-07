module NanoGL
  module CString
    module CStringImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private

      class VTable < FFI::Struct
        layout :Create, callback([:string], :pointer),
               :Free, callback([:pointer], :void)
      end

      attach_function :GetStringVTable, [], CStringImpl::VTable.by_ref
    end
    private
    VTABLE = CStringImpl::GetStringVTable()
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
      def Create(str)
        return VTABLE[:Create].call(str)
      end

      def Free(ptr)
        VTABLE[:Free].call(ptr)
      end

    end
  end
end
