module NanoGL
  module FpsTimer
    module FpsTimerImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private

      class VTable < FFI::Struct
        layout :Initialize, callback([], :bool),
               :Finalize, callback([], :void),
               :SetFPS, callback([:uint], :void),
               :GetFPS, callback([], :uint),
               :GetRealFPS, callback([], :float),
               :GetCPUPower, callback([], :int),
               :ResetTimer, callback([], :void),
               :WaitFrame, callback([], :void),
               :ResetElapseTimeCounter, callback([], :void),
               :GetElapseTimeCounter, callback([], :uint)
      end

      attach_function :GetFpsTimerVTable, [], FpsTimerImpl::VTable.by_ref
    end
    private
    VTABLE = FpsTimerImpl::GetFpsTimerVTable()
    public
    class << self
      def Initialize()
        VTABLE[:Initialize].call()
      end

      def Finalize()
        VTABLE[:Finalize].call()
      end

      def SetFPS(fps)
        VTABLE[:SetFPS].call(fps)
      end

      def GetFPS()
        VTABLE[:GetFPS].call()
      end

      def GetRealFPS()
        VTABLE[:GetRealFPS].call()
      end

      def GetCPUPower()
        VTABLE[:GetCPUPower].call()
      end

      def ResetTimer()
        VTABLE[:ResetTimer].call()
      end

      def WaitFrame()
        VTABLE[:WaitFrame].call()
      end

      def ResetElapseTimeCounter()
        VTABLE[:ResetElapseTimeCounter].call()
      end

      def GetElapseTimeCounter()
        VTABLE[:GetElapseTimeCounter].call()
      end

    end
  end
end
