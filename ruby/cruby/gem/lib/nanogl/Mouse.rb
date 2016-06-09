module NanoGL
  module Mouse
    module MouseImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private
      class Double < FFI::Struct
        layout :value, :double
      end

      class VTable < FFI::Struct
        layout :IsLeftButtonDown, callback([], :bool),
               :IsRightButtonDown, callback([], :bool),
               :IsMiddleButtonDown, callback([], :bool),
               :GetCursorPos, callback([MouseImpl::Double.by_ref, MouseImpl::Double.by_ref], :void)
      end

      attach_function :GetMouseVTable, [], MouseImpl::VTable.by_ref
    end
    private
    VTABLE = MouseImpl::GetMouseVTable()
    public
    class << self
      def IsLeftButtonDown()
        VTABLE[:IsLeftButtonDown].call()
      end

      def IsRightButtonDown()
        VTABLE[:IsRightButtonDown].call()
      end

      def IsMiddleButtonDown()
        VTABLE[:IsMiddleButtonDown].call()
      end

      def GetCursorPos()
        x = MouseImpl::Double.new()
        y = MouseImpl::Double.new()
        VTABLE[:GetCursorPos].call(x, y)
        return [x[:value], y[:value]]
      end
    end
  end
end
