# keyboard
K_SPACE = :K_SPACE
K_F1 = :K_F1
K_F2 = :K_F2
K_F3 = :K_F3
K_F4 = :K_F4
K_F5 = :K_F5
K_F6 = :K_F6
K_F7 = :K_F7
K_F8 = :K_F8
K_F9 = :K_F9
K_F10 = :K_F10
K_F11 = :K_F11
K_F12 = :K_F12
K_ESCAPE = :K_ESCAPE
K_A = :K_A
K_B = :K_B
K_C = :K_C
K_D = :K_D
K_E = :K_E
K_F = :K_F
K_G = :K_G
K_H = :K_H
K_I = :K_I
K_J = :K_J
K_K = :K_K
K_L = :K_L
K_M = :K_M
K_N = :K_N
K_O = :K_O
K_P = :K_P
K_Q = :K_Q
K_R = :K_R
K_S = :K_S
K_T = :K_T
K_U = :K_U
K_V = :K_V
K_W = :K_W
K_X = :K_X
K_Y = :K_Y
K_Z = :K_Z
K_0 = :K_0
K_1 = :K_1
K_2 = :K_2
K_3 = :K_3
K_4 = :K_4
K_5 = :K_5
K_6 = :K_6
K_7 = :K_7
K_8 = :K_8
K_9 = :K_9
K_UP = :K_UP
K_DOWN = :K_DOWN
K_LEFT = :K_LEFT
K_RIGHT = :K_RIGHT
K_LSHIFT = :K_LSHIFT

#mouse
M_LBUTTON = :M_LBUTTON
M_RBUTTON = :M_RBUTTON
M_MBUTTON = :M_MBUTTON

#gamepad
P_UP = :P_UP
P_LEFT = :P_LEFT
P_RIGHT = :P_RIGHT
P_DOWN = :P_DOWN
P_BUTTON0 = :P_BUTTON0
P_BUTTON1 = :P_BUTTON1
P_BUTTON2 = :P_BUTTON2
P_BUTTON3 = :P_BUTTON3
P_BUTTON4 = :P_BUTTON4
P_BUTTON5 = :P_BUTTON5
P_BUTTON6 = :P_BUTTON6
P_BUTTON7 = :P_BUTTON7
P_BUTTON8 = :P_BUTTON8
P_BUTTON9 = :P_BUTTON9
P_BUTTON10 = :P_BUTTON10
P_BUTTON11 = :P_BUTTON11
P_BUTTON12 = :P_BUTTON12
P_BUTTON13 = :P_BUTTON13
P_BUTTON14 = :P_BUTTON14
P_BUTTON15 = :P_BUTTON15
P_L_UP = :P_L_UP
P_L_LEFT = :P_L_LEFT
P_L_RIGHT = :P_L_RIGHT
P_L_DOWN = :P_L_DOWN
P_R_UP = :P_R_UP
P_R_LEFT = :P_R_LEFT
P_R_RIGHT = :P_R_RIGHT
P_R_DOWN = :P_R_DOWN
P_D_UP = :P_D_UP
P_D_LEFT = :P_D_LEFT
P_D_RIGHT = :P_D_RIGHT
P_D_DOWN = :P_D_DOWN

class Input
  KEY_MAP = {
      :K_SPACE => NanoGL::Keyboard::KeyCode::KBKEY_SPACE,
      :K_F1 => NanoGL::Keyboard::KeyCode::KBKEY_F1,
      :K_F2 => NanoGL::Keyboard::KeyCode::KBKEY_F2,
      :K_F3 => NanoGL::Keyboard::KeyCode::KBKEY_F3,
      :K_F4 => NanoGL::Keyboard::KeyCode::KBKEY_F4,
      :K_F5 => NanoGL::Keyboard::KeyCode::KBKEY_F5,
      :K_F6 => NanoGL::Keyboard::KeyCode::KBKEY_F6,
      :K_F7 => NanoGL::Keyboard::KeyCode::KBKEY_F7,
      :K_F8 => NanoGL::Keyboard::KeyCode::KBKEY_F8,
      :K_F9 => NanoGL::Keyboard::KeyCode::KBKEY_F9,
      :K_F10 => NanoGL::Keyboard::KeyCode::KBKEY_F10,
      :K_F11 => NanoGL::Keyboard::KeyCode::KBKEY_F11,
      :K_F12 => NanoGL::Keyboard::KeyCode::KBKEY_F12,
      :K_ESCAPE => NanoGL::Keyboard::KeyCode::KBKEY_ESCAPE,
    :K_A => NanoGL::Keyboard::KeyCode::KBKEY_A,
    :K_B => NanoGL::Keyboard::KeyCode::KBKEY_B,
    :K_C => NanoGL::Keyboard::KeyCode::KBKEY_C,
    :K_D => NanoGL::Keyboard::KeyCode::KBKEY_D,
    :K_E => NanoGL::Keyboard::KeyCode::KBKEY_E,
    :K_F => NanoGL::Keyboard::KeyCode::KBKEY_F,
    :K_G => NanoGL::Keyboard::KeyCode::KBKEY_G,
    :K_H => NanoGL::Keyboard::KeyCode::KBKEY_H,
    :K_I => NanoGL::Keyboard::KeyCode::KBKEY_I,
    :K_J => NanoGL::Keyboard::KeyCode::KBKEY_J,
    :K_K => NanoGL::Keyboard::KeyCode::KBKEY_K,
    :K_L => NanoGL::Keyboard::KeyCode::KBKEY_L,
    :K_M => NanoGL::Keyboard::KeyCode::KBKEY_M,
    :K_N => NanoGL::Keyboard::KeyCode::KBKEY_N,
    :K_O => NanoGL::Keyboard::KeyCode::KBKEY_O,
    :K_P => NanoGL::Keyboard::KeyCode::KBKEY_P,
    :K_Q => NanoGL::Keyboard::KeyCode::KBKEY_Q,
    :K_R => NanoGL::Keyboard::KeyCode::KBKEY_R,
    :K_S => NanoGL::Keyboard::KeyCode::KBKEY_S,
    :K_T => NanoGL::Keyboard::KeyCode::KBKEY_T,
    :K_U => NanoGL::Keyboard::KeyCode::KBKEY_U,
    :K_V => NanoGL::Keyboard::KeyCode::KBKEY_V,
    :K_W => NanoGL::Keyboard::KeyCode::KBKEY_W,
    :K_X => NanoGL::Keyboard::KeyCode::KBKEY_X,
    :K_Y => NanoGL::Keyboard::KeyCode::KBKEY_Y,
    :K_Z => NanoGL::Keyboard::KeyCode::KBKEY_Z,
    :K_0 => NanoGL::Keyboard::KeyCode::KBKEY_0,
    :K_1 => NanoGL::Keyboard::KeyCode::KBKEY_1,
    :K_2 => NanoGL::Keyboard::KeyCode::KBKEY_2,
    :K_3 => NanoGL::Keyboard::KeyCode::KBKEY_3,
    :K_4 => NanoGL::Keyboard::KeyCode::KBKEY_4,
    :K_5 => NanoGL::Keyboard::KeyCode::KBKEY_5,
    :K_6 => NanoGL::Keyboard::KeyCode::KBKEY_6,
    :K_7 => NanoGL::Keyboard::KeyCode::KBKEY_7,
    :K_8 => NanoGL::Keyboard::KeyCode::KBKEY_8,
    :K_9 => NanoGL::Keyboard::KeyCode::KBKEY_9,
    :K_UP => NanoGL::Keyboard::KeyCode::KBKEY_UP,
    :K_LEFT => NanoGL::Keyboard::KeyCode::KBKEY_LEFT,
    :K_RIGHT => NanoGL::Keyboard::KeyCode::KBKEY_RIGHT,
    :K_DOWN => NanoGL::Keyboard::KeyCode::KBKEY_DOWN,
    :K_LSHIFT => NanoGL::Keyboard::KeyCode::KBKEY_LEFT_SHIFT
  }
  JOYSTICK_MAP = {
      :P_UP => [{ :Kind=>:Button, :No=>0 }, { :Kind=>:Keyboard, :Code=>:K_UP, :JoyId=>0 }],
      :P_LEFT => [{:Kind=>:Button, :No=>1 }, { :Kind=>:Keyboard, :Code=>:K_LEFT, :JoyId=>0 }],
      :P_RIGHT => [{:Kind=>:Button, :No=>2 }, { :Kind=>:Keyboard, :Code=>:K_RIGHT, :JoyId=>0 }],
      :P_DOWN => [{:Kind=>:Button, :No=>3 }, { :Kind=>:Keyboard, :Code=>:K_DOWN, :JoyId=>0 }],
      :P_BUTTON0 => [{:Kind=>:Button, :No=>0 }, { :Kind=>:Keyboard, :Code=>:K_Z, :JoyId=>0 }],
      :P_BUTTON1 => [{:Kind=>:Button, :No=>1 }, { :Kind=>:Keyboard, :Code=>:K_X, :JoyId=>0 }],
      :P_BUTTON2 => [{:Kind=>:Button, :No=>2 }, { :Kind=>:Keyboard, :Code=>:K_C, :JoyId=>0 }],
      :P_BUTTON3 => [{:Kind=>:Button, :No=>3 }],
      :P_BUTTON4 => [{:Kind=>:Button, :No=>4 }],
      :P_BUTTON5 => [{:Kind=>:Button, :No=>5 }],
      :P_BUTTON6 => [{:Kind=>:Button, :No=>6 }],
      :P_BUTTON7 => [{:Kind=>:Button, :No=>7 }],
      :P_BUTTON8 => [{:Kind=>:Button, :No=>8 }],
      :P_BUTTON9 => [{:Kind=>:Button, :No=>9 }],
      :P_BUTTON10 => [{:Kind=>:Button, :No=>10}],
      :P_BUTTON11 => [{:Kind=>:Button, :No=>11}],
      :P_BUTTON12 => [{:Kind=>:Button, :No=>12}],
      :P_BUTTON13 => [{:Kind=>:Button, :No=>13}],
      :P_BUTTON14 => [{:Kind=>:Button, :No=>14}],
      :P_BUTTON15 => [{:Kind=>:Button, :No=>15}],
      :P_L_UP    => [{:Kind=>:Axes, :No=>2, :Sign => false }],
      :P_L_LEFT  => [{:Kind=>:Axes, :No=>2, :Sign => true }],
      :P_L_RIGHT => [{:Kind=>:Axes, :No=>3, :Sign => false }],
      :P_L_DOWN  => [{:Kind=>:Axes, :No=>3, :Sign => true }],
      :P_R_UP    => [{:Kind=>:Axes, :No=>4, :Sign => false }],
      :P_R_LEFT  => [{:Kind=>:Axes, :No=>4, :Sign => true }],
      :P_R_RIGHT => [{:Kind=>:Axes, :No=>5, :Sign => false }],
      :P_R_DOWN  => [{:Kind=>:Axes, :No=>5, :Sign => true }],
      :P_D_UP    => [{:Kind=>:Axes, :No=>0, :Sign => false }],
      :P_D_LEFT  => [{:Kind=>:Axes, :No=>0, :Sign => true }],
      :P_D_RIGHT => [{:Kind=>:Axes, :No=>1, :Sign => false }],
      :P_D_DOWN  => [{:Kind=>:Axes, :No=>1, :Sign => true }],

  }

  @@DefaultRepeatEntry = { :wait => 1, :interval => 1, :tick => 0, :status => :None }
  @@PushKeyTable = {}
  @@DownKeyTable = {}
  @@KeyRepeatTable = {}
  @@PushJoystickTable = {}
  @@DownJoystickTable = {}
  @@JoystickRepeatTable = {}

  def self.requested_close?
    # 殺しておく
    return false
  end

  def self.update
    # keyboard
    KEY_MAP.each do |dx,ng|
      ret = NanoGL::Keyboard.IsKeyDown(ng)
      if (ret)
        if @@KeyRepeatTable[dx] == nil
          @@KeyRepeatTable[dx] = @@DefaultRepeatEntry.dup
          @@KeyRepeatTable[dx][:status] = :Trigger
        else
          @@KeyRepeatTable[dx][:tick] += 1
          case @@KeyRepeatTable[dx][:status]
            when :None then
              @@KeyRepeatTable[dx][:status] = :Trigger
            when :Trigger, :Wait then
              if @@KeyRepeatTable[dx][:tick] >= @@KeyRepeatTable[dx][:wait]
                @@KeyRepeatTable[dx][:tick] = 0
                @@KeyRepeatTable[dx][:status] = :Active
              else
                @@KeyRepeatTable[dx][:status] = :Wait
              end
            when :Active, :Inactive then
              if @@KeyRepeatTable[dx][:tick] >= @@KeyRepeatTable[dx][:interval]
                @@KeyRepeatTable[dx][:tick] = 0
                @@KeyRepeatTable[dx][:status] = :Active
              else
                @@KeyRepeatTable[dx][:status] = :Inactive
              end
          end
        end
        @@DownKeyTable[dx] = (@@DownKeyTable[dx] || 0) + 1
      else
        if @@KeyRepeatTable[dx] == nil
          @@KeyRepeatTable[dx] = @@DefaultRepeatEntry.dup
          @@KeyRepeatTable[dx][:status] = :None
        else
          @@KeyRepeatTable[dx][:tick] = 0
          @@KeyRepeatTable[dx][:status] = :None
        end
        @@DownKeyTable[dx] = 0
      end
      if @@KeyRepeatTable[dx][:status] == :Active || @@KeyRepeatTable[dx][:status] == :Trigger
        @@PushKeyTable[dx] = (@@PushKeyTable[dx] || 0) + 1
      else
        @@PushKeyTable[dx] = 0
      end
    end

    # Joystick
    NanoGL::Joystick::MaxJoystickPort().times.select { |id| NanoGL::Joystick::IsPresented(id) || id == 0 }.each do |id|
      JOYSTICK_MAP.each do |dx, cfgs|
        ret = false
        cfgs.each do |cfg|
          if (cfg[:JoyId] == nil) || (cfg[:JoyId] == id)
            case cfg[:Kind]
              when :Button then
                ret |= NanoGL::Joystick::IsButtonDown(id, cfg[:No])
              when :Keyboard then
                ret |= NanoGL::Keyboard::IsKeyDown(KEY_MAP[cfg[:Code]])
              when :Axes then
                if cfg[:Sign] == false
                  ret |= NanoGL::Joystick::GetAxesStatus(id, cfg[:No]) > 0.5
                else
                  ret |= NanoGL::Joystick::GetAxesStatus(id, cfg[:No]) < -0.5
                end
              else
                # nothing
            end
          end
        end
        if ret == true
          if @@JoystickRepeatTable[id] == nil
            @@JoystickRepeatTable[id] = {}
          end
          if @@JoystickRepeatTable[id][dx] == nil
            @@JoystickRepeatTable[id][dx] = @@DefaultRepeatEntry.dup
            @@JoystickRepeatTable[id][dx][:status] = :Trigger
            @@JoystickRepeatTable[id][dx][:tick] = 1
          else
            @@JoystickRepeatTable[id][dx][:tick] += 1
            case @@JoystickRepeatTable[id][dx][:status]
              when :None then
                @@JoystickRepeatTable[id][dx][:status] = :Trigger
              when :Trigger, :Wait then
                if @@JoystickRepeatTable[id][dx][:tick] >= @@JoystickRepeatTable[id][dx][:wait]
                  @@JoystickRepeatTable[id][dx][:tick] = 0
                  @@JoystickRepeatTable[id][dx][:status] = :Active
                else
                  @@JoystickRepeatTable[id][dx][:status] = :Wait
                end
              when :Active, :Inactive then
                if @@JoystickRepeatTable[id][dx][:tick] >= @@JoystickRepeatTable[id][dx][:interval]
                  @@JoystickRepeatTable[id][dx][:tick] = 0
                  @@JoystickRepeatTable[id][dx][:status] = :Active
                else
                  @@JoystickRepeatTable[id][dx][:status] = :Inactive
                end
            end
          end
          @@DownJoystickTable[id] = (@@DownJoystickTable[id] || {})
          @@DownJoystickTable[id][dx] = (@@DownJoystickTable[id][dx] || 0) + 1
        else
          if @@JoystickRepeatTable[id] == nil
            @@JoystickRepeatTable[id] = {}
          end
          if @@JoystickRepeatTable[id][dx] == nil
            @@JoystickRepeatTable[id][dx] = @@DefaultRepeatEntry.dup
            @@JoystickRepeatTable[id][dx][:status] = :None
          else
            @@JoystickRepeatTable[id][dx][:tick] = 0
            @@JoystickRepeatTable[id][dx][:status] = :None
          end
          @@DownJoystickTable[id] = (@@DownJoystickTable[id] || {})
          @@DownJoystickTable[id][dx] = 0
        end
        if @@PushJoystickTable[id] == nil
          @@PushJoystickTable[id] = {}
        end
        if @@JoystickRepeatTable[id][dx][:status] == :Active || @@JoystickRepeatTable[id][dx][:status] == :Trigger
          @@PushJoystickTable[id][dx] = (@@PushJoystickTable[id][dx] || 0) + 1
        else
          @@PushJoystickTable[id][dx] = 0
        end
      end
    end

  end

  def self.set_repeat(wait, interval)
    @@DefaultRepeatEntry[:wait] = wait
    @@DefaultRepeatEntry[:interval] = interval
    @@DefaultRepeatEntry[:tick] = 0
    @@DefaultRepeatEntry[:status] = :None

    @@KeyRepeatTable.each_key do |key|
      @@KeyRepeatTable[key][:wait] = wait
      @@KeyRepeatTable[key][:interval] = interval
      @@KeyRepeatTable[key][:tick] = 0
      @@KeyRepeatTable[key][:status] = :None
    end

    @@JoystickRepeatTable.each do |joystick|
      joystick.each_key do |key|
        joystick[key][:wait] = wait
        joystick[key][:interval] = interval
        joystick[key][:tick] = 0
        joystick[key][:status] = :None
      end
    end
  end

  def self.key_push?(key)
    if @@KeyRepeatTable[key][:wait] == 0 && @@KeyRepeatTable[key][:interval] == 0
      return @@DownKeyTable[key] == 1
    else
      return @@PushKeyTable[key] == 1
    end
  end

  def self.key_down?(key)
    return @@PushKeyTable[key] != 0
  end

  def self.mouse_down?(btn)
    case btn
      when :M_LBUTTON then
        return NanoGL::Mouse.IsLeftButtonDown()
      when :M_RBUTTON then
        return NanoGL::Mouse.IsRightButtonDown()
      when :M_MBUTTON then
        return NanoGL::Mouse.IsMiddleButtonDown()
    end
    return false
  end

  def self.mouse_push?(btn)
    case btn
      when :M_LBUTTON then
        return NanoGL::Mouse.IsLeftButtonPush()
      when :M_RBUTTON then
        return NanoGL::Mouse.IsRightButtonPush()
      when :M_MBUTTON then
        return NanoGL::Mouse.IsMiddleButtonPush()
    end
    return false
  end

  def self.mouse_pos_x
    return NanoGL::Mouse.GetCursorPos()[0].to_i
  end

  def self.mouse_pos_y
    return NanoGL::Mouse.GetCursorPos()[1].to_i
  end

  def self.pad_push?(pad_code, pad_number=0)
    if @@JoystickRepeatTable[pad_number][pad_code][:wait] == 0 && @@JoystickRepeatTable[pad_number][pad_code][:interval] == 0
      return (@@DownJoystickTable[pad_number] != nil) && @@DownJoystickTable[pad_number][pad_code] == 1
    else
      return (@@PushJoystickTable[pad_number] != nil) && (@@PushJoystickTable[pad_number][pad_code] == 1)
    end

  end

  # pad_numberが0の場合、デフォルトでは十字ボタンとカーソルキー、P_BUTTON0とZ、P_BUTTON1とX、P_BUTTON2とCを割り当てます。
  def self.pad_down?(pad_code, pad_number=0)
    return (@@DownJoystickTable[pad_number] != nil) && @@DownJoystickTable[pad_number][pad_code] != 0
  end

  def self.x
    (pad_down?(:P_LEFT) ? -1 : 0) + (pad_down?(:P_RIGHT) ? 1 : 0)
  end

  def self.y
    (pad_down?(:P_UP) ? -1 : 0) + (pad_down?(:P_DOWN) ? 1 : 0)
  end

  class << self
    alias :mousePush? :mouse_push?
    alias :keyPush? :key_push?
    alias :keyDown? :key_down?
    alias :setRepeat :set_repeat
    alias :mouseDown? :mouse_down?
    alias :mousePush? :mouse_push?
    alias :mousePosX :mouse_pos_x
    alias :mousePosY :mouse_pos_y
    alias :padPush? :pad_push?
    alias :padDown? :pad_down?
  end
end

