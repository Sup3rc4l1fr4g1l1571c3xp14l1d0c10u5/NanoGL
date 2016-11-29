module NanoGL
  module Dialog
    module DialogImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private

      class VTable < FFI::Struct
        layout :OpenFileDialog, callback([:string,:string], :string),
               :OpenFileDialogUTF8, callback([:string,:string], :string),
               :SaveFileDialog, callback([:string,:string], :string),
               :SaveFileDialogUTF8, callback([:string,:string], :string)
      end

      attach_function :GetDialogVTable, [], DialogImpl::VTable.by_ref
    end
    private
    VTABLE = DialogImpl::GetDialogVTable()
    public
    class << self
      def OpenFileDialog(title,filter)
        VTABLE[:OpenFileDialogUTF8].call(title,filter)
      end

      def SaveFileDialog(title,filter)
        VTABLE[:SaveFileDialogUTF8].call(title,filter)
      end
    end
  end
end
