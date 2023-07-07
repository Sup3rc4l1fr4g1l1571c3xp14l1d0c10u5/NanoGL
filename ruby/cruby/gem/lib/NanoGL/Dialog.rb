module NanoGL
  module Dialog
    require_relative './CString.rb'

    module DialogImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private

      class VTable < FFI::Struct
        layout :OpenFileDialog, callback([:string,:string], :pointer),
               :OpenFileDialogUTF8, callback([:string,:string], :pointer),
               :SaveFileDialog, callback([:string,:string], :pointer),
               :SaveFileDialogUTF8, callback([:string,:string], :pointer)
      end

      attach_function :GetDialogVTable, [], DialogImpl::VTable.by_ref
    end
    private
    VTABLE = DialogImpl::GetDialogVTable()
    public
    class << self
      def OpenFileDialog(title,filter)
        strPtr = VTABLE[:OpenFileDialogUTF8].call(title,filter)
        str = strPtr.null? ? nil : strPtr.read_string()
        CString.Free(strPtr)
        return str
      end

      def SaveFileDialog(title,filter)
        strPtr = VTABLE[:SaveFileDialogUTF8].call(title,filter)
        str = strPtr.null? ? nil : strPtr.read_string()
        CString.Free(strPtr)
        return str
      end
    end
  end
end
