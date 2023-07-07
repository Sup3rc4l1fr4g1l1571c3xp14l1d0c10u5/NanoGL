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
        layout :Update, callback([], :void),
               :IsLeftButtonDown, callback([], :bool),
               :IsRightButtonDown, callback([], :bool),
               :IsMiddleButtonDown, callback([], :bool),
               :IsLeftButtonPush, callback([], :bool),
               :IsRightButtonPush, callback([], :bool),
               :IsMiddleButtonPush, callback([], :bool),
               :IsLeftButtonUp, callback([], :bool),
               :IsRightButtonUp, callback([], :bool),
               :IsMiddleButtonUp, callback([], :bool),
               :IsLeftButtonRelease, callback([], :bool),
               :IsRightButtonRelease, callback([], :bool),
               :IsMiddleButtonRelease, callback([], :bool),
               :GetCursorPos, callback([MouseImpl::Double.by_ref, MouseImpl::Double.by_ref], :void)
      end

      attach_function :GetMouseVTable, [], MouseImpl::VTable.by_ref
    end
    private
    VTABLE = MouseImpl::GetMouseVTable()
    public
    class << self
      def Update()
        VTABLE[:Update].call()
      end

      def IsLeftButtonDown()
        VTABLE[:IsLeftButtonDown].call()
      end

      def IsRightButtonDown()
        VTABLE[:IsRightButtonDown].call()
      end

      def IsMiddleButtonDown()
        VTABLE[:IsMiddleButtonDown].call()
      end

      def IsLeftButtonPush()
        VTABLE[:IsLeftButtonPush].call()
      end

      def IsRightButtonPush()
        VTABLE[:IsRightButtonPush].call()
      end

      def IsMiddleButtonPush()
        VTABLE[:IsMiddleButtonPush].call()
      end

      def IsLeftButtonUp()
        VTABLE[:IsLeftButtonUp].call()
      end

      def IsRightButtonUp()
        VTABLE[:IsRightButtonUp].call()
      end

      def IsMiddleButtonUp()
        VTABLE[:IsMiddleButtonUp].call()
      end

      def IsLeftButtonRelease()
        VTABLE[:IsLeftButtonRelease].call()
      end

      def IsRightButtonRelease()
        VTABLE[:IsRightButtonRelease].call()
      end

      def IsMiddleButtonRelease()
        VTABLE[:IsMiddleButtonRelease].call()
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
