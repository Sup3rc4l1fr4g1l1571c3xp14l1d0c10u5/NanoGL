require 'NanoGL'
include NanoGL

#
# 画面サイズの設定
#
SCREEN_WIDTH  = 320
SCREEN_HEIGHT = 480

SCREEN_LEFT   = 0
SCREEN_RIGHT  = (SCREEN_LEFT + SCREEN_WIDTH)
SCREEN_TOP    = 0
SCREEN_BOTTOM = (SCREEN_TOP + SCREEN_HEIGHT)

#
# 出現数上限の設定
#
MAX_ENEMY		= 256		# 敵・敵弾合わせての最大出現数
MAX_SHOT		= 10		# プレイヤーの弾の最大出現数
MAX_PARTICLE	= 256		# パーティクルの最大出現数

#
# 小型の敵からランダムに選んで球を打たせる「ランダム攻撃」の攻撃間隔などの設定で用いる変数
#
class RandomFire
  @shot_speed	= 0.0		# 弾速
  @time			= 0			# 最後の攻撃からの経過時間
  @time_next		= 0			# 次の攻撃を行う時間

  def self.reset()
    @shot_speed = 2
    @time       = 0
    @time_next  = 60
  end

  class << self
    attr_accessor :shot_speed
    attr_accessor :time
    attr_accessor :time_next
  end
end


#
# 読み込んだ画像のIDを入れる変数
#
class Image
  @img_player = 0
  @img_shot_l = 0
  @img_shot_r = 0
  @img_enemy_shot = 0
  @img_enemy_s0 = 0
  @img_enemy_s1 = 0
  @img_enemy_m0 = 0
  @img_enemy_boss1 = 0

  class << self
    attr_accessor :img_player
    attr_accessor :img_shot_l
    attr_accessor :img_shot_r
    attr_accessor :img_enemy_shot
    attr_accessor :img_enemy_s0
    attr_accessor :img_enemy_s1
    attr_accessor :img_enemy_m0
    attr_accessor :img_enemy_boss1
  end
end

#
# 座標(x,y)が画面境界からpピクセル以上外側にあるか判定。
#
def IsOutOfScreen?(x, y, p)
  return (x < SCREEN_LEFT - p || SCREEN_RIGHT + p <= x || y < SCREEN_TOP - p || SCREEN_BOTTOM + p <= y)
end

#
# 当り判定
#

class HitArea
  attr_accessor :r	# 当たり判定の半径
  attr_accessor :x	# 当たり判定の中心座標
  attr_accessor :y	# 当たり判定の中心座標
  def initialize(r,x,y)
    @r = r	# 当たり判定の半径
    @x = x	# 当たり判定の中心座標
    @y = y	# 当たり判定の中心座標
  end
end

#
# 敵と敵弾を表現するデータ
# 最大 MAX_ENEMY 個
#

class Enemy
  attr_accessor :enemy		# 敵として使われている場合はtrue
  attr_accessor :x			# X座標
  attr_accessor :y			# Y座標
  attr_accessor :s			# 移動速度
  attr_accessor :d			# 移動方向
  attr_accessor :life			# ライフ値
  attr_accessor :action		# 動作
  attr_accessor :time			# 実行時間
  attr_accessor :image		# 画像番号
  attr_accessor :score		# 得点
  attr_accessor :randomfire	# ランダム砲火が可能か
  attr_accessor :cr			# 当たり判定

  def initialize()
    @enemy = false
    @x = 0.0
    @y = 0.0
    @s = 0.0
    @d = 0.0
    @life = 0
    @action = nil
    @time = 0
    @image = 0
    @score = 0
    @randomfire = false
    @cr = []
  end

  # 描画する
  def draw
    Video.Save()

    # 敵画像のサイズを得る
    (w, h) = Video.ImageSize(@image)

    # 変形をリセット
    Video.ResetTransform()

    # 変形処理を設定
    # （プログラム上では移動→回転の順序になっているが行列の掛け算なので演算結果は逆、つまり回転→移動となることに注意）

    # 表示したい座標への移動
    Video.Translate(@x, @y)

    # 回転を適用
    Video.Rotate(@d - Video.DegToRad(90))

    # 画像を描画
    Video.DrawImage(@image, 0, 0, w, h, -w / 2.0, -h / 2.0, w, h, 0, 1.0)

    # 変形が終わったので変形をリセット
    Video.Restore()
  end

  # 稼働中の敵のリスト
  @enemies = []

  class << self
    attr_accessor :enemies
  end
  def self.create(&block)
    e = Enemy.new
    @enemies << e
    yield e
  end

  # 全ての敵・敵弾を消去
  def self.reset_enemies
    @enemies.clear
  end

  # 敵をすべて描画する
  def self.draw
    @enemies.each(&:draw)
  end

  def self.alive_enemy
    @enemies.count { |e| e.enemy }
  end
end



#
# プレイヤーの弾を管理する
#
class Shot
  attr_accessor :x	  # X座標
  attr_accessor :y	  # Y座標
  attr_accessor :s	  # 移動速度
  attr_accessor :d	  # 移動方向
  attr_accessor :r	  # 当り判定の半径
  attr_accessor :image  # 画像番号

  def initialize()
    @x = 0.0
    @y = 0.0
    @s = 0.0
    @d = 0.0
    @r = 0.0
    @image = 0
  end

  def draw()
    Video.Save()
    # 変形をリセット
    Video.ResetTransform()
    Video.Translate(@x, @y)
    Video.Rotate(@d + Video.DegToRad(90))
    (w, h) = Video.ImageSize(@image)
    Video.DrawImage(@image, 0, 0, w, h, -w / 2.0, -h / 2.0, w, h, 0, 1.0)
    Video.Restore()
  end

  def move()
    if IsOutOfScreen?(@x, @y, 16)
      return false
    else
      @x += Math.cos(@d) * @s
      @y += Math.sin(@d) * @s
    end
  end

  @shots = []

  class << self
    attr_accessor :shots
  end

  def self.create(&block)
    s = Shot.new
    @shots << s
    yield s
  end

  def self.length()
    @shots.length
  end

  # 全ての敵・敵弾を消去
  def self.reset
    @shots.clear
  end

  # 敵をすべて描画する
  def self.draw
    @shots.each(&:draw)
  end

  # 敵をすべて描画する
  def self.move
    @shots.select!(&:move)
  end
end

#
# プレイヤーを管理する
#
class Player
  @alive = false	# 生存フラグ
  @x = 0.0		# X座標
  @y = 0.0		# Y座標
  @r = 8.0		# 当り判定の半径
  @reload = 0	# 攻撃のリロード時間
  class << self
    attr_accessor :alive
    attr_accessor :x
    attr_accessor :y
    attr_accessor :r
    attr_accessor :reload
  end
  def self.reset()
    @alive = true
    @x = SCREEN_LEFT + SCREEN_WIDTH  / 2
    @y = SCREEN_TOP  + (SCREEN_HEIGHT * 9) / 10
    @reload = 0
    @r = 0
  end
  def self.move
    # プレイヤーが生きているならプレイヤーの操作を反映させる
    if @alive == false
      return
    end

    # 十字キーの入力に応じて移動させる
    if (Keyboard.IsKeyDown(Keyboard::KeyCode::KBKEY_DOWN))
      @y += 3
    end
    if (Keyboard.IsKeyDown(Keyboard::KeyCode::KBKEY_UP))
      @y -= 3
    end
    if (Keyboard.IsKeyDown(Keyboard::KeyCode::KBKEY_LEFT))
      @x -= 3
    end
    if (Keyboard.IsKeyDown(Keyboard::KeyCode::KBKEY_RIGHT))
      @x += 3
    end

    # 移動した結果画面外に出た場合は画面内に戻す
    if (@x < SCREEN_LEFT  )
      @x = SCREEN_LEFT
    end
    if (@x > SCREEN_RIGHT )
      @x = SCREEN_RIGHT
    end
    if (@y < SCREEN_TOP   )
      @y = SCREEN_TOP
    end
    if (@y > SCREEN_BOTTOM)
      @y = SCREEN_BOTTOM
    end

    # 攻撃のリロードを管理する
    if (@reload == 0)
      # リロードが終わっている場合、'z' キーを押すと弾を撃つ
      if (Keyboard.IsKeyDown('Z') && Shot.length < MAX_SHOT-2)

        # 左側の弾
        Shot.create do |s|
          s.x = @x - 8
          s.y = @y
          s.d = Video.DegToRad(-90)
          s.s = 12
          s.r = 12
          s.image = Image.img_shot_l
        end

        # 右側の弾
        Shot.create do |s|
          s.x = @x + 8
          s.y = @y
          s.d = Video.DegToRad(-90)
          s.s = 12
          s.r = 12
          s.image = Image.img_shot_r
        end
        @reload = 4
      end
    else
      # リロードが終わっていないのでリロード時間を１減らす
      @reload -= 1
    end
  end
  def self.draw()
    # プレイヤーが生きていれば描画する
    if (@alive)
      (w, h) = Video.ImageSize(Image.img_player)
      Video.DrawImage(Image.img_player, 0, 0, w, h, @x - w / 2.0, @y - h / 2.0, w, h, 0, 1.0)
    end
  end
end


#
# 演出用のパーティクル
#
class Particle
  attr_accessor :x	  # X座標
  attr_accessor :y	  # Y座標
  attr_accessor :s	  # 移動速度
  attr_accessor :d	  # 移動方向
  attr_accessor :r	  # 半径
  attr_accessor :c	 # 色
  attr_accessor :ttl	# 消滅までの時間
  attr_accessor :time	# 生存時間


  def initialize()
    @x = 0.0		# X座標
    @y = 0.0		# Y座標
    @s = 0.0		# 移動速度
    @d = 0.0		# 移動方向
    @r = 0.0		# 半径
    @c = [0,0,0,0]	# 色
    @ttl = 0		# 消滅までの時間
    @time = 0		# 生存時間
  end

  def move()
    @time += 1
    if (@time == @ttl)
      return false
    else
      @x += Math.cos(@d) * @s
      @y += Math.sin(@d) * @s
      return true
    end
  end

  def draw()
    Video.Save()
    Video.BeginPath()
    Video.Circle(@x, @y, @r * (1.0 - @time*1.0 / @ttl))
    Video.FillColor(@c)
    Video.Fill()
    Video.ClosePath()
    Video.Restore()
  end

  @particles = []

  # 全パーティクルを消去
  def self.reset()
    @particles.clear()
  end

  # パーティクルをひとつ作る
  def self.create(&block)
    p = Particle.new
    @particles << p
    yield p
  end

  # パーティクルを描画する
  def self.draw()
    @particles.each(&:draw)
  end

  # パーティクルを動かす
  def self.move()
    @particles.select!(&:move)
  end

end

# パーティクルの色を決定するために、HSVカラー指定方法からRGBカラーを作る
# h : 色相 一般的には0～360度で示されるがこの関数では0.0～1.0の範囲（正確には小数部のみ）で扱う
# s : 彩度 0.0～1.0の範囲で扱い、範囲外はクリップされる
# h : 明度 0.0～1.0の範囲で扱い、範囲外はクリップされる
def FromHsv(h, s, v)

  h = h - h.to_i
  s = [1.0, [0.0, s].max].min
  v = [1.0, [0.0, v].max].min
  r = v
  g = v
  b = v
  if (s > 0.0)
    h *= 6.0
    i  = h.to_i % 6
    f  = h - i
    case i
      when 1 then
        r *= 1 - s * f
        b *= 1 - s
      when 2 then
        r *= 1 - s
        b *= 1 - s * (1 - f)
      when 3 then
        r *= 1 - s
        g *= 1 - s * f
      when 4 then
        r *= 1 - s * (1 - f)
        g *= 1 - s
      when 5 then
        g *= 1 - s
        b *= 1 - s * f
      else
        g *= 1 - s * (1 - f)
        b *= 1 - s
    end
  end
  return [(r * 255).to_i, (g * 255).to_i, (b * 255).to_i]
end

class EnemyAction
  def self.action(e)
  end
end

#
# 敵の弾の動作（１）：直進する敵弾の動作
#
class Action_b1 < EnemyAction
  def self.action(e)
    e.x += Math.cos(e.d) * e.s
    e.y += Math.sin(e.d) * e.s
  end
end

#
# 敵弾を作る
#
def create_enemy_burret(x, y, s, d)
  Enemy.create do |e|
    e.enemy = false
    e.x = x
    e.y = y
    e.s = s
    e.d = d
    e.life = 1
    e.action = Action_b1
    e.time = 0
    e.image = Image.img_enemy_shot
    e.cr << HitArea.new(10, 0, 0)
    e.randomfire = false
  end
end

#
# 小型の敵１を作る
#
def create_enemy_s0(x, y, s, d, action)
  Enemy.create do |e|
    e.enemy = true
    e.x = x
    e.y = y
    e.s = s
    e.d = d
    e.life = 1
    e.score = 9
    e.action = action
    e.time = 0
    e.image = Image.img_enemy_s0
    e.cr << HitArea.new(14, 0, 0)
    e.randomfire = true
  end
end

#
# 小型の敵２を作る
#
def create_enemy_s1(x, y, s, d, action)
  Enemy.create do |e|
    e.enemy = true
    e.x = x
    e.y = y
    e.s = s
    e.d = d
    e.life = 7
    e.score = 63
    e.action = action
    e.time = 0
    e.image = Image.img_enemy_s1
    e.cr << HitArea.new(10, 0, 0)
    e.randomfire = true
  end
end

#
# 敵の動作：設定された速度と方向で直進
#
class Action_s0 < EnemyAction
  def self.action(e)
    e.x += Math.cos(e.d) * e.s
    e.y += Math.sin(e.d) * e.s
  end
end

#
# 敵の動作：画面下に向かって左右にふらふら揺れながら直進
#
class Action_s1 < EnemyAction
  def self.action(e)
    e.x += Math.cos(Video.DegToRad(e.time % 120 * 360 / 120)) * 2
    e.y += e.s
  end
end

#
# 敵の動作：設定された速度と方向で直進するが徐々に加速する
#
class Action_s2 < EnemyAction
  def self.action(e)
    e.x += Math.cos(e.d) * e.s
    e.y += Math.sin(e.d) * e.s
    e.s *= 1.03
  end
end

#
# 敵の動作：画面下に向かって直進してくるが、プレイヤーと横軸が合うと停止し、プレイヤーの向きに曲がってから加速する
#
class Action_s3 < EnemyAction
  def self.action(e)
    e.y += e.s
    if (e.y - 5 <= Player.y && Player.y <= e.y + 5)
      e.s = 0.5
      if (Player.x > e.x)
        e.d = Video.DegToRad(0)
      else
        e.d = Video.DegToRad(180)
      end
      e.action = Action_s2	#直進動作に切り替え
    end
  end
end

#
# 敵の動作：プレイヤーの方向に旋回しながら追尾する。距離が近くなると追尾を終了する
#
class Action_s4 < EnemyAction
  def self.action(e)
    e.x += Math.cos(e.d) * e.s
    e.y += Math.sin(e.d) * e.s
    if (Player.alive)
      d = (Player.y - e.y) * (Player.y - e.y) + (Player.x - e.x) * (Player.x - e.x)
      if (d < 60 * 60)
        # 追尾終了
        #直進動作に切り替え
        e.action = Action_s0
      else
        r = Math.atan2(Player.y - e.y, Player.x - e.x)
        vx = Math.cos(e.d) * e.s
        vy = Math.sin(e.d) * e.s
        tx = (Player.x - e.x)
        ty = (Player.y - e.y)
        s = vx * ty - vy * tx
        c = (vx * tx + vy * ty) / (Math.sqrt(vx*vx + vy*vy) * Math.sqrt(tx*tx + ty*ty))
        if (c > Math.cos(Video.DegToRad(1.5)))
          e.d = r
        else
          e.d += (s > 0 ? 1 : -1)*Video.DegToRad(1.5)
        end
      end
    end
  end
end

#
# 中型の敵を作る
#
def create_enemy_m0(x, y, s, d, action)
  Enemy.create do |e|
    e.enemy = true
    e.x = x
    e.y = y
    e.s = s
    e.d = d
    e.life = 100
    e.score = 900
    e.action = action
    e.time = 0
    e.image = Image.img_enemy_m0
    e.cr << HitArea.new(32, 0, 0)
    e.cr << HitArea.new(24, -48, 0)
    e.cr << HitArea.new(24,  48, 0)
    e.randomfire = false
  end
end

#
# 中型の敵の動作：
# 翼から真下に向かって角度固定の３方向に弾を撃つ
# 胴体からプレイヤーを狙う全方向弾を打つ
#
class Action_m1 < EnemyAction
  def self.action(e)

    case e.time
      when 0...40 then
        e.x += Math.cos(e.d) * (40 - e.time) / 40 * e.s
        e.y += Math.sin(e.d) * (40 - e.time) / 40 * e.s
      when 40...340 then
        e.d = Video.DegToRad(90)
        e.x += Math.cos(e.d) * 0.2
        e.y += Math.sin(e.d) * 0.2

        st = e.time - 40
        if ((st % 80) == 30 || (st % 80) == 32 || (st % 80) == 34 || (st % 80) == 36 || (st % 80) == 38 || (st % 80) == 40)
          s = ((st % 80) - 30) * 0.1
          (-1..1).each do |i|
            create_enemy_burret(e.x - 51, e.y + 4, 2 + s + RandomFire.shot_speed / 8.0, Video.DegToRad(90 + 20 * i))
            create_enemy_burret(e.x + 51, e.y + 4, 2 + s + RandomFire.shot_speed / 8.0, Video.DegToRad(90 + 20 * i))
          end
        end
        if ((st % 60) == 30)
          d = Math.atan2(Player.y - e.y, Player.x - e.x)
          9.times do |i|
            3.times do |j|
              create_enemy_burret(e.x, e.y + 4, 3 + j * 0.2 + RandomFire.shot_speed / 4, d + Video.DegToRad(360 * i / 8))
            end
          end
        end
      else
        e.d = Video.DegToRad(90)
        e.x += Math.cos(e.d) * [e.time - 340, 60].min / 60 * -e.s
        e.y += Math.sin(e.d) * [e.time - 340, 60].min / 60 * -e.s
    end
  end
end

#
# ボス敵を作る
#
def create_enemy_b0(x, y, s, d, action)
  Enemy.create do |e|
    e.enemy = true
    e.x = x
    e.y = y
    e.s = s
    e.d = d
    e.life = 1000
    e.score = 9000
    e.action = action
    e.time = 0
    e.image = Image.img_enemy_boss1
    e.cr << HitArea.new(48, 0, 0)
    e.cr << HitArea.new(32, -56, 0)
    e.cr << HitArea.new(32,  56, 0)
    e.cr << HitArea.new(16, -104, 16)
    e.cr << HitArea.new(16,  104, 16)
    e.randomfire = false
  end
end

#
# ボス敵の動作：
#
class Action_boss1 < EnemyAction
  @@x = 0
  @@y = 0
  def self.action(e)
    if (e.time > 40) && (e.time < 2000)
      e.x = @@x + Math.cos(Video.DegToRad(e.time/5))*[160, e.time-40].min/4
      e.y = @@y + Math.sin(Video.DegToRad(e.time/2))*[40, e.time-40].min/4
    end
    case e.time
      when 0...40 then
        e.x += Math.cos(e.d) * (40 - e.time) / 40 * e.s
        e.y += Math.sin(e.d) * (40 - e.time) / 40 * e.s
        @@x = e.x
        @@y = e.y
      when 40...220 then
        # 竜巻全方向弾
        # 射出方向に動きながら遅い弾の隙間を抜けると避けやすい
        st = e.time - 40
        if (st%2 == 0)
          3.times do |i|
            create_enemy_burret(e.x, e.y, (1 + RandomFire.shot_speed / 8.0)+st/30.0, Video.DegToRad(90 + 120 * i+st*17))
          end
        end
      when 220...250 then
        # 休憩
      when 250...370 then
        # 自機狙い
        st = e.time - 250

        r = Math.atan2(Player.y - e.y, Player.x - e.x)
        if (st%15 == 0)
          3.times do |i|
            rr = Video.DegToRad((Video.RadToDeg(r)/10).to_i*10)
            create_enemy_burret(e.x, e.y, (4 + RandomFire.shot_speed / 8.0)+i*0.2, rr)
          end
        end
      when 370...550 then
        # 自機狙い＋左右の扇弾
        st = e.time - 370

        r = Math.atan2(Player.y - e.y, Player.x - e.x)
        if (st%15 == 0)
          3.times do |i|
            rr = Video.DegToRad((Video.RadToDeg(r)/10).to_i*10)
            create_enemy_burret(e.x, e.y, (4 + RandomFire.shot_speed / 8.0)+i*0.2, rr)
          end
        end

        if (st%60 < 30)
          n = st%60
          create_enemy_burret(e.x + 56, e.y - 24, (3 + RandomFire.shot_speed / 8.0), Video.DegToRad(0+n*3.3))
        else
          n = st%60-30
          create_enemy_burret(e.x - 56, e.y - 24, (3 + RandomFire.shot_speed / 8.0), Video.DegToRad(180-n*3.3))
        end

      when 550...580 then
        # 休憩
      when 580...700 then
        # 全方向ばらまき
        st = e.time - 580

        create_enemy_burret(e.x + 80, e.y + 8, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90+st*23))
        create_enemy_burret(e.x - 80, e.y + 8, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90-st*23))
      when 700...730 then
        # 休憩
      when 730...970 then
        st = e.time - 730


        # 真下に連射

        if (st % 40) < 20 && (st % 40) % 5 == 0
          create_enemy_burret(e.x + 113, e.y + 25, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90))
          create_enemy_burret(e.x - 113, e.y + 25, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90))
        elsif (st % 40) >= 20 && (st % 40) % 5 == 0
          create_enemy_burret(e.x + 32, e.y + 40, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90))
          create_enemy_burret(e.x - 32, e.y + 40, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90))
        end
      when 970...1150 then
        st = e.time - 730

        # 真下に連射＋自機狙い３ＷＡＹ

        if (st % 40) < 20 && (st % 40) % 5 == 0
          create_enemy_burret(e.x + 113, e.y + 25, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90))
          create_enemy_burret(e.x - 113, e.y + 25, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90))
        elsif (st % 40) >= 20 && (st % 40) % 5 == 0
          create_enemy_burret(e.x + 32, e.y + 40, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90))
          create_enemy_burret(e.x - 32, e.y + 40, (4 + RandomFire.shot_speed / 8.0), Video.DegToRad(90))
        end
        if (st % 20) == 10
          r1 = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x + 56))
          3.times do |i|
            create_enemy_burret(e.x + 56, e.y - 24, (5 + RandomFire.shot_speed / 8.0), r1+Video.DegToRad(i-1)*2)
          end
          r2 = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x - 56))
          3.times do |i|
            create_enemy_burret(e.x - 56, e.y - 24, (5 + RandomFire.shot_speed / 8.0), r2+Video.DegToRad(i-1)*2)
          end
        end

      when 1150...1330 then
        st = e.time - 1150

        r = Math.atan2(Player.y - e.y, Player.x - e.x)

        # 全砲台から射撃
        if (st % 100) ==  0
          5.times do |i|
            create_enemy_burret(e.x + 80, e.y + 8, (3 + RandomFire.shot_speed / 8.0), r+ Video.DegToRad(-2+i)*20)
            create_enemy_burret(e.x - 80, e.y + 8, (3 + RandomFire.shot_speed / 8.0), r+ Video.DegToRad(-2+i)*20)
          end
        end
        if (st % 100) == 25
          5.times do |i|
            create_enemy_burret(e.x + 32, e.y + 40, (3 + RandomFire.shot_speed / 8.0), r+ Video.DegToRad(-2+i)*20)
            create_enemy_burret(e.x - 32, e.y + 40, (3 + RandomFire.shot_speed / 8.0), r+ Video.DegToRad(-2+i)*20)
          end
        end

        if (st % 100) == 50
          5.times do |i|
            create_enemy_burret(e.x + 56, e.y - 24, (3 + RandomFire.shot_speed / 8.0), r+ Video.DegToRad(-2+i)*20)
            create_enemy_burret(e.x - 56, e.y - 24, (3 + RandomFire.shot_speed / 8.0), r+ Video.DegToRad(-2+i)*20)
          end
        end

        if (st % 100) == 75
          5.times do |i|
            create_enemy_burret(e.x + 113, e.y + 25, (3 + RandomFire.shot_speed / 8.0), r+ Video.DegToRad(-2+i)*20)
            create_enemy_burret(e.x - 113, e.y + 25, (3 + RandomFire.shot_speed / 8.0), r+ Video.DegToRad(-2+i)*20)
          end
        end

        if (st % 30) == 0
          3.times do |i|
            create_enemy_burret(e.x, e.y, (4 + RandomFire.shot_speed / 8.0), r + Video.DegToRad(-1+i)*3)
          end
        end

      when 1330 ... 1360 then
        # 休憩

      when 1360 ... 1390 then
        st = e.time - 1360

        diff = Video.DegToRad(120+15)

        # ワインダー
        if (st % 9) == 0
          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x + 80))
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x - 80))
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 3
          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x + 32))
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x - 32))
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 6
          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x + 56))
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x - 56))
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end

      when 1390 ... 1510 then
        st = e.time - 1390

        diff = Video.DegToRad((1510-e.time)*2+15)

        # ワインダー
        if (st % 9) == 0
          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x + 80))
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x - 80))
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 3
          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x + 32))
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x - 32))
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 6
          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x + 56))
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x - 56))
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end

      when 1510 ... 1570 then
        st = e.time - 1360

        diff = Video.DegToRad(15)

        # ワインダー
        if (st % 9) == 0
          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x + 80))
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x - 80))
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 3
          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x + 32))
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x - 32))
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 6
          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x + 56))
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x - 56))
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end

      when 1570 ... 1870 then
        st  = e.time - 1360
        st2 = e.time - 1570

        diff = Video.DegToRad(Math.cos(st2*Math::PI*6.0/300)*5+15)

        # ワインダー
        if (st % 9) == 0
          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x + 80))
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x - 80))
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 3
          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x + 32))
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x - 32))
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 6
          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x + 56))
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x - 56))
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end

        if (st % 60) == 10
          r = Math.atan2(Player.y - e.y, Player.x - e.x)
          3.times do |i|
            create_enemy_burret(e.x, e.y, (4 + RandomFire.shot_speed / 8.0), r + Video.DegToRad(-1+i)*2)
          end
        end
        if (st % 60) == 40
          r = Math.atan2(Player.y - e.y, Player.x - e.x)
          3.times do |i|
            create_enemy_burret(e.x, e.y, (4 + RandomFire.shot_speed / 8.0), r + Video.DegToRad(-1+i)*8)
          end
        end

      when 1870 ... 1990	 then
        st = e.time - 1360

        diff = Video.DegToRad(e.time-1870+15)

        # ワインダー
        if (st % 9) == 0
          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x + 80))
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 8), Player.x - (e.x - 80))
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 80, e.y + 8, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 3
          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x + 32))
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y + 40), Player.x - (e.x - 32))
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 32, e.y + 40, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end
        if (st % 9) == 6
          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x + 56))
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x + 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)

          r = Math.atan2(Player.y - (e.y - 24), Player.x - (e.x - 56))
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r+ diff)
          create_enemy_burret(e.x - 56, e.y - 24, (6 + RandomFire.shot_speed / 8.0), r- diff)
        end


      else
        e.d = Video.DegToRad(90)
        e.x += Math.cos(e.d) * [e.time - 340, 60].min / 60 * -e.s
        e.y += Math.sin(e.d) * [e.time - 340, 60].min / 60 * -e.s
    end
  end
end


#
# 敵グループの出現を管理する処理で使う変数
#
class Spawn
  @time = 0
  @work = 0
  @enable = true
  @action = 0

  # 出現処理をリセットする
  def self.reset()
    @time = 0
    @work = 0
    @enable = true
    @action = 0
  end
  class << self
    attr_accessor :time
    attr_accessor :work
    attr_accessor :enable
    attr_accessor :action
  end
end

#
# 敵の動作：登場後は静止してプレイヤーを狙う全方向弾を打つ。一定時間後に加速しながら突進する。
#
class Action_s5 < EnemyAction
  def self.action(e)
    case e.time
      when 0...40 then
        e.randomfire = false
        e.x += Math.cos(e.d) * (40 - e.time) / 40 * e.s
        e.y += Math.sin(e.d) * (40 - e.time) / 40 * e.s
      when 40...280 then
        e.d = Math.atan2(Player.y - e.y, Player.x - e.x)
        if ((e.time - 40) % 60 == 0)
          8.times do |i|
            create_enemy_burret(e.x, e.y + 4, 2 + RandomFire.shot_speed / 4, e.d + Video.DegToRad(360 * i / 8))
          end
        end
      else
        e.s = 3
        e.action = Action_s4
        e.randomfire = true
    end
  end
end

#
# 敵グループの出現動作
#
class SpawnEnemy
  def self.action()
  end
end

#
# 敵グループの出現動作：0.5秒なにもしない
#
class SpawnEnemy_s0 < SpawnEnemy
  def self.action()
    # 0.5秒待つ
    if (Spawn.time == 30)
      Spawn.enable = true
    end
  end
end


#
# 敵グループの出現動作：画面上から画面下に向かって直進してくる敵を0.33秒間隔で１０体出現させる
#
class SpawnEnemy_s1 < SpawnEnemy
  def self.action()
    if ((Spawn.time % 20) == 0)
      Spawn.work += ((rand() * 13).to_i + 5) * 11
      create_enemy_s0((Spawn.work % 240) + 40, -10, 4, Video.DegToRad(90), Action_s0)
    end
    if (Spawn.time == 20 * 10 - 1)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：画面上から画面下に向かって左右に揺れながら直進してくる敵を0.33秒間隔で１０体出現させる
#
class SpawnEnemy_s2 < SpawnEnemy
  def self.action()
    if ((Spawn.time % 20) == 0)
      Spawn.work += ((rand() * 19).to_i + 5) * 11
      create_enemy_s0((Spawn.work % 220) + 50, -10, 2.5, Video.DegToRad(90), Action_s1)
    end
    if (Spawn.time == 20 * 20 - 1)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：画面左右からプレイヤーを追尾する敵を出現させる
#
class SpawnEnemy_s3 < SpawnEnemy
  def self.action()
    if ((Spawn.time % 10 == 0))
      i = 4 - Spawn.time / 10
      create_enemy_s0(SCREEN_LEFT  - 10, i * 60, 3, Video.DegToRad(  0), Action_s4)
      create_enemy_s0(SCREEN_RIGHT + 10, i * 60, 3, Video.DegToRad(180), Action_s4)
    end
    if (Spawn.time == 40-1)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：画面上部を左右に横断する敵を1/6秒間隔で作る
#
class SpawnEnemy_s4 < SpawnEnemy
  def self.action()
    if (Spawn.time % 10 == 0)
      3.times do |i|
        create_enemy_s0(SCREEN_LEFT  - 10, 100 * i + 50, 8, Video.DegToRad(  0), Action_s0)
        create_enemy_s0(SCREEN_RIGHT + 10, 100 * i     , 8, Video.DegToRad(180), Action_s0)
      end
    end
    if (Spawn.time == 100)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：画面両脇を上下に横断する敵を1/6秒間隔で作る
#
class SpawnEnemy_s5 < SpawnEnemy
  def self.action()
    # 上下に横切る敵を出す
    if (Spawn.time % 10 == 0)
      create_enemy_s0(SCREEN_LEFT  + 25 * 3, SCREEN_TOP    - 10, 8, Video.DegToRad( 90), Action_s0)
      create_enemy_s0(SCREEN_LEFT  + 25 * 1, SCREEN_BOTTOM + 10, 8, Video.DegToRad(-90), Action_s0)
      create_enemy_s0(SCREEN_RIGHT - 25 * 1, SCREEN_BOTTOM + 10, 8, Video.DegToRad(-90), Action_s0)
      create_enemy_s0(SCREEN_RIGHT - 25 * 3, SCREEN_TOP    - 10, 8, Video.DegToRad( 90), Action_s0)
    end
    if (Spawn.time == 100)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：
# 画面下に向かって直進し、プレイヤーと横軸が合うと停止し、曲がって加速する敵を
# 上から連続で降らせる
#
class SpawnEnemy_s6 < SpawnEnemy
  def self.action()
    if ((Spawn.time % 20) == 0)
      Spawn.work += ((rand() * 13).to_i + 5) * 11
      x = SCREEN_LEFT + Spawn.work % (SCREEN_WIDTH - 80) + 40
      create_enemy_s0(  x, -10, 6, Video.DegToRad(90), Action_s3)
    end
    if (Spawn.time == 20 * 10 - 1)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：
# 中型の敵を上から出現させる
#
class SpawnEnemy_m0 < SpawnEnemy
  def self.action()
    if (Spawn.time == 0)
      Spawn.work += ((rand() * 13).to_i + 5) * 11
      x = SCREEN_LEFT + Spawn.work % (SCREEN_WIDTH - 120) + 60
      create_enemy_m0(x, -10, 4, Video.DegToRad(90), Action_m1)
    end
    if (Spawn.time == 20 * 10 - 1)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：
# 画面上から直進する敵とプレイヤーを追尾する敵を交互に出現させる
#
class SpawnEnemy_s7 < SpawnEnemy
  def self.action()
    if ((Spawn.time % 20) == 0)
      Spawn.work += ((rand() * 13).to_i + 5) * 11
      x = SCREEN_LEFT + Spawn.work % (SCREEN_WIDTH - 80) + 40
      create_enemy_s0(x, -10, 4, Video.DegToRad(90), Action_s0)
    end
    if ((Spawn.time % 20) == 10)
      Spawn.work += ((rand() * 13).to_i + 5) * 11
      x = SCREEN_LEFT + Spawn.work % (SCREEN_WIDTH - 80) + 40
      create_enemy_s0(x, -10, 5, Video.DegToRad(90), Action_s4)
    end
    if (Spawn.time == 30 * 10 - 1)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：
# 静止して弾を打つ敵を出現させてから左右に揺れ画なら直進してくる敵を連続で降らせる
#
class SpawnEnemy_s8 < SpawnEnemy
  def self.action()
    if (Spawn.time % 10 == 0 && Spawn.time / 10 <  3)
      i = Spawn.time / 10
      create_enemy_s1( SCREEN_LEFT-10, 0 + i * 60 + 30, 5 - i * 0.6, Video.DegToRad(  0+20), Action_s5)
      create_enemy_s1( SCREEN_RIGHT+10, 0 + i * 60 + 30, 5 - i * 0.6, Video.DegToRad(180-20), Action_s5)
    end
    if ((Spawn.time % 20) == 0)
      Spawn.work += ((rand() * 13).to_i + 5) * 11
      x = SCREEN_LEFT + Spawn.work % (SCREEN_WIDTH - 80) + 40
      create_enemy_s0(x, -10, 2.5, Video.DegToRad(90), Action_s1)
    end
    if (Spawn.time == 20 * 10 - 1)
      Spawn.enable = true
    end
  end
end

#
# 敵グループの出現動作：
# ボス敵を上から出現させる
#
class SpawnEnemy_b0 < SpawnEnemy
  def self.action()
    if (Spawn.time == 0)
      x = SCREEN_LEFT + SCREEN_WIDTH/2
      create_enemy_b0(x, -10, 4, Video.DegToRad(90), Action_boss1)
    end
    if (Spawn.time == 20 * 10 - 1)
      Spawn.enable = true
    end
  end
end


#
# 敵グループの出現動作を並べた表
#
SPAWN_TABLE = [
    SpawnEnemy_s0,	# 0番目は使われない
    SpawnEnemy_s1,
    SpawnEnemy_s2,
    SpawnEnemy_s0,
    SpawnEnemy_s3,
    SpawnEnemy_s0,
    SpawnEnemy_s4,
    SpawnEnemy_s0,
    SpawnEnemy_s5,
    SpawnEnemy_s6,
    SpawnEnemy_s0,
    SpawnEnemy_m0,	# 中型機出現
    SpawnEnemy_s7,
    SpawnEnemy_s8,
    SpawnEnemy_b0,	# ボス出現
    nil			# 終端を意味する nil を入れておく
]

# 敵グループの出現を処理する
def do_spawn_enemy()
  # 敵がいなくて、敵グループを作ってもいい場合は敵の出現テーブルを読み進めて実行するように設定する
  if (Enemy.alive_enemy == 0 && Spawn.enable == true)
    Spawn.action += 1;	# 出現テーブルで稼働させる要素の番号を+1する
    Spawn.time = 0;		# 出現テーブルの動作時間を0にリセット
    Spawn.enable = false;	# 敵の出現処理側から許可が出るまで敵の出現処理を更新しないようにする
    Spawn.work = 0;			# 敵の出現処理内で使う用途を定めない変数を一応リセットしておく
  end

  if (Spawn.enable == false)
    # 敵の出現テーブルが終端まで読み進められていたら先頭に戻す。
    if (SPAWN_TABLE[Spawn.action] == nil)
      Spawn.action = 0
    end
    # 出現動作を実行して経過時間を進める
    SPAWN_TABLE[Spawn.action].action()
    Spawn.time += 1
  end
end

# 敵の動作を行う
def move_enemies()
  random_fire_enemies = []
  es = Enemy.enemies
  Enemy.enemies = []
  es.select! do |e|
    # 画面外に出てしまった敵を消去
    if (IsOutOfScreen?(e.x, e.y, 32))
      next false
    end

    # ランダム攻撃可能敵を列挙
    if (e.randomfire)
      random_fire_enemies << e
    end

    # 敵ごとに設定されている動作を実行
    e.action.action(e)

    # 敵の動作時間を更新
    e.time += 1

    next true
  end
  Enemy.enemies = es + Enemy.enemies

  # ランダム攻撃可能な状況かを判定して可能なら変数onfireをtrueに設定する
  onfire = false
  if (RandomFire.time >= RandomFire.time_next)
    # 最後の攻撃からの経過時間 RandomFire.time が次の攻撃を行う時間 RandomFire.time_next 以上の場合、
    # 弾を撃ってほしい状況なので変数onfireをtrueに設定し、RandomFire.timeを０にリセットする
    # また、次の攻撃までの時間を１引くことで速める（ただし20以下にはならないようにする）
    onfire = true
    RandomFire.time -= RandomFire.time_next
    RandomFire.time_next = [RandomFire.time_next - 1, 20].max
  else
    # 次の攻撃まで時間経過していないので経過時間を
    RandomFire.time += 1
  end

  # ランダム攻撃可能で生存している敵がいるならその中から敵を選んで弾を打たせる
  if onfire
    e = random_fire_enemies.sample(1).first
    if (e != nil)
      r = Math.atan2(Player.y - e.y, Player.x - e.x)
      create_enemy_burret(e.x, e.y, RandomFire.shot_speed, r)
    end
  end
end

# パーティクルを使って敵の爆発演出を作る
def explod_enemy(e)
  # ここでは敵の持つ当たり判定（円）の中心点をパーティクル生成位置の中心座標に、
  # 当たり判定の大きさをパーティクルの初期サイズに設定している
  e.cr.each do |c|
    # 中心座標に１６個のパーティクルを作成して四方八方に飛ばす
    16.times do |i|
      # パーティクルの色はHSV色空間と呼ばれるもの（ペイントソフトにある丸い色選択のアレ）を用いて鮮やかな色を選んでいる
      color = FromHsv(i / 16.0, 0.5, 1.0)
      d = (rand() * Math::PI * 2)
      s = (rand() * 1.5 + 0.5)
      Particle.create do |p|
        p.x = e.x + c.x;
        p.y = e.y + c.y;
        p.s = s
        p.d = d
        p.r = c.r
        p.c = color
        p.ttl= (rand() * 20).to_i + 10
      end
    end
  end
end

#
# 得点
#
class Score
  @score = 0

  def self.draw()
    Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)

    Video.FontSize(24)
    Video.FillColor(FromHsv((Video.GetTime()*0.05), 0.5, 1.0))
    Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + 24, "SCORE: #{sprintf("%5.5d",@score)}")
  end
  def self.reset()
    @score = 0
  end
  def self.add(v)
    @score += v
  end
  class << self
    attr_accessor :score
  end

end

#
# 当たり判定：円が重なっているか判定
#
def hitcheck?(x1, y1, r1, x2, y2, r2)
  len2 = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)
  d2 = (r1 + r2) * (r1 + r2)
  return len2 < d2
end

#
# 全体の当たり判定を実行
#
def hit_check()
  # プレイヤーの弾と敵の当たり判定を行う
  # プレイヤーの弾一つ一つに対して敵全てとの判定を行っていく
  Shot.shots.select! do |s|
    # プレイヤーの弾が命中したらtrueになる変数
    shot_hit = false
    Enemy.enemies.select! do |e|
      if (e.enemy == false)
        next true
      end
      # 当たり判定領域と判定
      next e.cr.each do |c|
        if (hitcheck?(s.x, s.y, s.r, e.x + c.x, e.y + c.y, c.r)==false)
          next
        end

        # 衝突したので弾が当たったことを記録する変数をtrueにする
        shot_hit = true

        #当たっているのでダメージを与える処理を行う
        e.life -= 1

        # スコアを足す
        Score.add(1)

        # ダメージを与えた結果敵の耐久値が０になったら敵を壊す
        if (e.life != 0)
          break true
        end

        # 爆発音を鳴らす
        Sound.ChannelPlay(0,false)

        # 敵の弾の発射速度を少し速くする
        RandomFire.shot_speed += 0.01

        # スコアを加算する
        Score.add(e.score)

        # 敵の爆発演出を作る
        explod_enemy(e)

        break false
      end
    end

    # プレイヤーの弾が当たっていた場合、着弾点に小さなパーティクルを作る
    if (shot_hit == true)
      r = rand() / 8.0
      8.times do |i|
        color = FromHsv(r + i / 8.0, 0.5, 1.0)
        d = s.d + Video.DegToRad(rand() * 6 - 3)
        spd = s.s * (rand() * 0.3 + 0.2)
        Particle.create do |p|
          p.x = s.x;
          p.y = s.y;
          p.s = spd
          p.d = d
          p.r = 10
          p.c = color
          p.ttl= (rand() * 20).to_i + 20
        end
      end
    end

    next !shot_hit
  end

  # プレイヤーが生きていれば当たり判定
  if (Player.alive)
    Enemy.enemies.select! do |e|
      if (e.enemy == true)
        next true
      end
      # 当たり判定領域と判定
      next e.cr.each do |c|
        if (hitcheck?(Player.x, Player.y, Player.r, e.x + c.x, e.y + c.y, c.r)==false)
          next true
        end
        Player.alive = false
        next false
      end
    end
    if (Player.alive == false)
      # 爆発音を鳴らす
      Sound.ChannelPlay(0,false)
      r = rand() / 8.0
      8.times do |i|
        color = FromHsv(r + i / 8.0, 0.5, 1.0)
        d = Video.DegToRad(i * 360 / 8)
        Particle.create do |p|
          p.x = Player.x
          p.y = Player.y
          p.s = rand()*3.0
          p.d = d
          p.r = 10
          p.c = color
          p.ttl= (rand() * 20).to_i + 20
        end
      end
    end
  end
end

#
# ゲームのデータをリセットする
#
def reset_game_datas()
  # 敵・敵弾を初期化
  Enemy.reset_enemies()

  # パーティクルの初期化
  Particle.reset()

  # プレイヤーの初期化
  Player.reset()

  # プレイヤーの弾の初期化
  Shot.reset()

  # ランダム攻撃の設定をリセット
  RandomFire.reset()

  # 敵の出現をリセットする
  Spawn.reset()
end

#
# ゲームのメイン処理
#
def start()

  #
  # 初期化処理
  #

  # 画面サイズを横 SCREEN_WIDTH 縦 SCREEN_HEIGHT ピクセル に 設定
  Video.SetSize(SCREEN_WIDTH, SCREEN_HEIGHT)

  # タイトルを設定
  Video.SetWindowTitle("[Example] 51-STG")

  # ゲームデータをリセットする
  reset_game_datas()

  # 画像の読み込み
  Image.img_player = Video.CreateImage("./Image/player.png", 0)
  Image.img_shot_l = Video.CreateImage("./Image/shot_l.png", 0)
  Image.img_shot_r = Video.CreateImage("./Image/shot_r.png", 0)
  Image.img_enemy_s0 = Video.CreateImage("./Image/enemy_s0.png", 0)
  Image.img_enemy_s1 = Video.CreateImage("./Image/enemy_s1.png", 0)
  Image.img_enemy_m0 = Video.CreateImage("./Image/enemy_m.png", 0)
  Image.img_enemy_boss1 = Video.CreateImage("./Image/enemy_boss1.png", 0)
  Image.img_enemy_shot = Video.CreateImage("./Image/enemy_shot.png", 0)

  # 音源の読み込み
  Sound.ChannelCreate(0, "./Sound/explod.wav")
  Sound.ChannelCreate(1, "./Sound/bgm.ogg")

  # フォント読み込み
  Video.CreateFont("font", "./Font/IPAexfont00301/ipaexg.ttf")
  Video.FontFace("font")

  # 直前のZキーの状態を保持する変数
  prevZ = false


  #
  # 準備完了なのでゲーム本体処理を開始する
  #
  loop do
    # タイトル画面ではプレイヤーを消しておく
    Player.alive = false

    # タイトル画面
    prevZ = Keyboard.IsKeyDown('Z')
    Video.Draw do

      # 当たり判定を行う
      hit_check()

      # 敵全体の動作を行う
      move_enemies()

      # パーティクルを動かす
      Particle.move()

      # 敵の出現を行う
      do_spawn_enemy()

      # 敵を描画する
      Enemy.draw()

      # パーティクルを描画する
      Particle.draw()

      # ゲーム画面自体を全体的に暗くしておく
      Video.FillColor([0, 0, 0, 128])
      Video.BeginPath()
      Video.Rect(SCREEN_LEFT, SCREEN_TOP, SCREEN_WIDTH, SCREEN_HEIGHT)
      Video.ClosePath()
      Video.Fill()

      # タイトル文字などを表示
      Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)

      Video.FontSize(64)
      Video.FillColor(FromHsv((Video.GetTime()*0.05), 0.5, 1.0))
      Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 2 / 5, "Nano STG")

      Video.FontSize(16)
      Video.FillColor([255, 255, 255])
      Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 4 / 5, "Push Z to Start.")

      # Zキーが押されたらタイトル画面を抜ける
      if (Keyboard.IsKeyDown('Z') && !prevZ)
        break
      end
      prevZ = Keyboard.IsKeyDown('Z')
    end

    # ゲームデータをリセットする
    reset_game_datas()
    Score.reset

    # BGM再生開始
    Sound.ChannelPlay(1,true)

    # ゲーム本体開始
    Video.Draw do
      # 当たり判定を行う
      hit_check()

      # 敵全体の動作を行う
      move_enemies()

      # プレイヤーの弾の移動を行う
      Shot.move()

      # プレイヤーを動かす
      Player.move()

      # パーティクルを動かす
      Particle.move()

      # 敵の出現を行う
      do_spawn_enemy()

      # 得点を描く
      Score.draw()

      # 敵を描画する
      Enemy.draw()

      # プレイヤーの弾を描画する
      Shot.draw()

      # プレイヤーを描画する
      Player.draw()

      # パーティクルを描画する
      Particle.draw()

      # プレイヤーが死んでいたらゲームオーバー画面へ
      if (Player.alive == false)
        break
      end
    end

    # ゲームオーバー画面へ行く前に1秒待機
    t = 0
    Video.Draw do
      t += 1

      # 当たり判定を行う
      hit_check()

      # 敵全体の動作を行う
      move_enemies()

      # プレイヤーの弾の移動を行う
      Shot.move()

      # パーティクルを動かす
      Particle.move()

      # 敵の出現を行う
      do_spawn_enemy()

      # 得点を描く
      Score.draw()

      # 敵を描画する
      Enemy.draw()

      # プレイヤーの弾を描画する
      Shot.draw()

      # パーティクルを描画する
      Particle.draw()

      if (t >= 60)
        t = 0
        break
      end
    end

    # ゲームオーバー画面
    prevZ = Keyboard.IsKeyDown('Z')
    Video.Draw do

      # 当たり判定を行う
      hit_check()

      # 敵全体の動作を行う
      move_enemies()

      # パーティクルを動かす
      Particle.move()

      # 敵の出現を行う
      do_spawn_enemy()

      # 敵を描画する
      Enemy.draw()

      # パーティクルを描画する
      Particle.draw()

      # ゲーム画面自体を全体的に暗くしておく
      Video.FillColor([0, 0, 0, 128])
      Video.BeginPath()
      Video.Rect(SCREEN_LEFT, SCREEN_TOP, SCREEN_WIDTH, SCREEN_HEIGHT)
      Video.ClosePath()
      Video.Fill()

      # タイトル文字などを表示
      Video.TextAlign(Video::AlignFlags::ALIGN_CENTER | Video::AlignFlags::ALIGN_MIDDLE)

      Video.FontSize(64)
      Video.FillColor([255, 0, 0])
      Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 2 / 10, "GAME")
      Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 4 / 10, "OVER")

      Video.FontSize(32)
      Video.FillColor([255, 255, 255])
      Video.Text(SCREEN_LEFT + SCREEN_WIDTH / 2, SCREEN_TOP + SCREEN_HEIGHT * 6 / 10, "SCORE : #{sprintf("%5.5d", Score.score)}")

      # Zキーが押されたらタイトル画面を抜ける
      if (Keyboard.IsKeyDown('Z') && !prevZ)
        break
      end
      prevZ = Keyboard.IsKeyDown('Z')
    end
  end
end

start()

