module NanoGL
  module Joystick
    module JoystickImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private

      class VTable < FFI::Struct
        layout :MaxJoystickPort, callback([], :int),
               :Initialize, callback([], :void),
               :Finalize, callback([], :void),
               :Connect, callback([], :void),
               :Disconnect, callback([], :void),
               :Update, callback([], :void),
               :IsPresented, callback([:int], :bool),
               :GetAxesCount, callback([:int], :int),
               :GetAxesStatus, callback([:int, :int], :float),
               :GetButtonCount, callback([:int], :int),
               :IsButtonDown, callback([:int, :int], :bool),
               :IsButtonPush, callback([:int, :int], :bool),
               :IsButtonUp, callback([:int, :int], :bool),
               :IsButtonRelease, callback([:int, :int], :bool)
      end

      attach_function :GetJoystickVTable, [], JoystickImpl::VTable.by_ref
    end
    private
    VTABLE = JoystickImpl::GetJoystickVTable()
    public
    class << self
      def MaxJoystickPort()
        VTABLE[:MaxJoystickPort].call()
      end

      def Connect()
        VTABLE[:Connect].call()
      end

      def Disconnect()
        VTABLE[:Disconnect].call()
      end

      def Update()
        VTABLE[:Update].call()
      end

      def IsPresented(id)
        VTABLE[:IsPresented].call(id)
      end

      def GetAxesCount(id)
        VTABLE[:GetAxesCount].call(id)
      end

      def GetAxesStatus(id, ax)
        VTABLE[:GetAxesStatus].call(id, ax)
      end

      def GetButtonCount(id)
        VTABLE[:GetButtonCount].call(id)
      end

      def IsButtonDown(id, btn)
        VTABLE[:IsButtonDown].call(id, btn)
      end

      def IsButtonPush(id, btn)
        VTABLE[:IsButtonPush].call(id, btn)
      end

      def IsButtonUp(id, btn)
        VTABLE[:IsButtonUp].call(id, btn)
      end

      def IsButtonRelease(id, btn)
        VTABLE[:IsButtonRelease].call(id, btn)
      end

    end
  end
end
