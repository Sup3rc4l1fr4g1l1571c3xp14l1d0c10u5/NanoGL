module NanoGL
  module Keyboard
    module KeyboardImpl
      extend FFI::Library
      ffi_lib SHARED_LIB_PATH
      private
      class VTable < FFI::Struct
        layout :IsKeyDown, callback([:int], :bool)
      end

      attach_function :GetKeyboardVTable, [], KeyboardImpl::VTable.by_ref
    end
    private
    VTABLE = KeyboardImpl::GetKeyboardVTable()
    public
    module KeyCode
      KBKEY_SPACE = 32
      KBKEY_APOSTROPHE = 39 # '
      KBKEY_COMMA = 44 # ,
      KBKEY_MINUS = 45 # -
      KBKEY_PERIOD = 46 # .
      KBKEY_SLASH = 47 # /
      KBKEY_0 = 48
      KBKEY_1 = 49
      KBKEY_2 = 50
      KBKEY_3 = 51
      KBKEY_4 = 52
      KBKEY_5 = 53
      KBKEY_6 = 54
      KBKEY_7 = 55
      KBKEY_8 = 56
      KBKEY_9 = 57
      KBKEY_SEMICOLON = 59 # ;
      KBKEY_EQUAL = 61 # =
      KBKEY_A = 65
      KBKEY_B = 66
      KBKEY_C = 67
      KBKEY_D = 68
      KBKEY_E = 69
      KBKEY_F = 70
      KBKEY_G = 71
      KBKEY_H = 72
      KBKEY_I = 73
      KBKEY_J = 74
      KBKEY_K = 75
      KBKEY_L = 76
      KBKEY_M = 77
      KBKEY_N = 78
      KBKEY_O = 79
      KBKEY_P = 80
      KBKEY_Q = 81
      KBKEY_R = 82
      KBKEY_S = 83
      KBKEY_T = 84
      KBKEY_U = 85
      KBKEY_V = 86
      KBKEY_W = 87
      KBKEY_X = 88
      KBKEY_Y = 89
      KBKEY_Z = 90
      KBKEY_LEFT_BRACKET = 91 # [
      KBKEY_BACKSLASH = 92 # \
      KBKEY_RIGHT_BRACKET = 93 # ]
      KBKEY_GRAVE_ACCENT = 96 # `
      KBKEY_WORLD_1 = 161 # non-US #1
      KBKEY_WORLD_2 = 162 # non-US #2

      # Function keys
      KBKEY_ESCAPE = 256
      KBKEY_ENTER = 257
      KBKEY_TAB = 258
      KBKEY_BACKSPACE = 259
      KBKEY_INSERT = 260
      KBKEY_DELETE = 261
      KBKEY_RIGHT = 262
      KBKEY_LEFT = 263
      KBKEY_DOWN = 264
      KBKEY_UP = 265
      KBKEY_PAGE_UP = 266
      KBKEY_PAGE_DOWN = 267
      KBKEY_HOME = 268
      KBKEY_END = 269
      KBKEY_CAPS_LOCK = 280
      KBKEY_SCROLL_LOCK = 281
      KBKEY_NUM_LOCK = 282
      KBKEY_PRINT_SCREEN = 283
      KBKEY_PAUSE = 284
      KBKEY_F1 = 290
      KBKEY_F2 = 291
      KBKEY_F3 = 292
      KBKEY_F4 = 293
      KBKEY_F5 = 294
      KBKEY_F6 = 295
      KBKEY_F7 = 296
      KBKEY_F8 = 297
      KBKEY_F9 = 298
      KBKEY_F10 = 299
      KBKEY_F11 = 300
      KBKEY_F12 = 301
      KBKEY_F13 = 302
      KBKEY_F14 = 303
      KBKEY_F15 = 304
      KBKEY_F16 = 305
      KBKEY_F17 = 306
      KBKEY_F18 = 307
      KBKEY_F19 = 308
      KBKEY_F20 = 309
      KBKEY_F21 = 310
      KBKEY_F22 = 311
      KBKEY_F23 = 312
      KBKEY_F24 = 313
      KBKEY_F25 = 314
      KBKEY_KP_0 = 320
      KBKEY_KP_1 = 321
      KBKEY_KP_2 = 322
      KBKEY_KP_3 = 323
      KBKEY_KP_4 = 324
      KBKEY_KP_5 = 325
      KBKEY_KP_6 = 326
      KBKEY_KP_7 = 327
      KBKEY_KP_8 = 328
      KBKEY_KP_9 = 329
      KBKEY_KP_DECIMAL = 330
      KBKEY_KP_DIVIDE = 331
      KBKEY_KP_MULTIPLY = 332
      KBKEY_KP_SUBTRACT = 333
      KBKEY_KP_ADD = 334
      KBKEY_KP_ENTER = 335
      KBKEY_KP_EQUAL = 336
      KBKEY_LEFT_SHIFT = 340
      KBKEY_LEFT_CONTROL = 341
      KBKEY_LEFT_ALT = 342
      KBKEY_LEFT_SUPER = 343
      KBKEY_RIGHT_SHIFT = 344
      KBKEY_RIGHT_CONTROL = 345
      KBKEY_RIGHT_ALT = 346
      KBKEY_RIGHT_SUPER = 347
      KBKEY_MENU = 348
    end
    class << self
      def IsKeyDown(keycode)
        if keycode.class == String
          VTABLE[:IsKeyDown].call(keycode.unpack('C')[0])
        else
          VTABLE[:IsKeyDown].call(keycode)
        end
      end
    end
  end
end
