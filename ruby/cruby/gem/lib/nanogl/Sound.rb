module NanoGL
  module Sound
    module SoundImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private

      class VTable < FFI::Struct
        layout :Initialize, callback([], :bool),
               :Finalize, callback([], :void),
               :ChannelCreate, callback([:int, :string], :bool),
               :ChannelFree, callback([:int], :bool),
               :ChannelPlay, callback([:int, :bool], :bool),
               :ChannelStop, callback([:int], :bool)
      end

      attach_function :GetSoundVTable, [], SoundImpl::VTable.by_ref
    end
    private
    VTABLE = SoundImpl::GetSoundVTable()
    public
    class << self
      def Initialize()
        VTABLE[:Initialize].call()
      end

      def Finalize()
        VTABLE[:Finalize].call()
      end

      def ChannelCreate(channelId, path)
        VTABLE[:ChannelCreate].call(channelId, path)
      end

      def ChannelFree(channelId)
        VTABLE[:ChannelFree].call(channelId)
      end

      def ChannelPlay(channelId, isloop)
        VTABLE[:ChannelPlay].call(channelId, isloop)
      end

      def ChannelStop(channelId)
        VTABLE[:ChannelStop].call(channelId)
      end
    end
  end
end
