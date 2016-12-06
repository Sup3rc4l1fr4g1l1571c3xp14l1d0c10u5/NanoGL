require 'tempfile'
require_relative 'Sound'

WAVE_RECT = :WAVE_RECT  # 矩形波(デフォルト)
WAVE_SIN = :WAVE_SIN  # 正弦波
WAVE_SAW = :WAVE_SAW  # ノコギリ波
WAVE_TRI = :WAVE_TRI  # 三角波

class SoundEffect
private
  def sync
    if @need_sync
      filesize = (3*4) + (6*4) + (2*4) + @buffer.length * BlockSize

      fp = Tempfile.new(['SoundEffet_','.wav'])
      fp.binmode
      fp.write('RIFF' )
      fp.write([filesize - 8].pack('V') )
      fp.write('WAVE' )

      fp.write('fmt ' )
      fp.write([16].pack('V') )
      fp.write([1].pack('v') )
      fp.write([Channel].pack('v') )
      fp.write([SamplePerSec].pack('V') )
      fp.write([BuffetSizeParSec].pack('V') )
      fp.write([BlockSize].pack('v') )
      fp.write([BitParSec].pack('v') )

      fp.write('data' )
      fp.write([@buffer.length*BlockSize].pack('V') )
      fp.write(@buffer.pack('s*'))

      fp.close()

      if @sound != nil
        @sound.dispose()
        @sound = nil
        File.unlink(@fp.path)
      end
      @sound = Sound.new(fp.path)
      @fp = fp
      @need_sync = false
    end
  end

  # (おそらく) 44100hz 16bit モノラル の波形を作る
  SamplePerSec = 44100
  Channel = 1
  BitParSec = 16

  # １ブロックは Channel * BitParSec / 8(8bit=1byte) より 2バイト
  BlockSize = (Channel * BitParSec / 8).to_i

  # １秒に必要なバイト数は 44100(hz) * ブロックサイズ(2バイト) より 88200byte
  BuffetSizeParSec = SamplePerSec * BlockSize

public

  # time は 効果音のミリ秒単位の長さ
  # wave_typeは基本となる波形の形
  # resolution(hz)は解像度(1秒分のデータを作る際にblockを呼び出す回数。最大は44100hz)
  # blockは [周波数,ボリューム] を返す式
  # WAVE_RECTの場合、ブロックが返す配列に3つ目の要素を設定することでデューティ比を指定できます。この値は省略すると0.5になります。
  def initialize(time, wave_type=WAVE_RECT, resolution=1000, &block)
    @need_sync = true
    @sound = nil
    @time = time
    @resolution = resolution > 44100 ? 44100 : resolution
    @buffer = Array.new((SamplePerSec * @time / 1000).to_i, 0)

    add(wave_type, &block)
  end

  def add(wave_type, &block)
    # 波形生成用のパラメータ
    f = 0.0 # 周波数
    vol = 0.0 # ボリューム
    duty = 0.5 # デューティー比
    count = 0 # 生成したサンプル数
    step = (SamplePerSec / @resolution).to_i

    @need_sync = true

    @buffer.length.times do |i|
      # 指定時間間隔でブロックを呼び出して周波数、ボリューム、デューティー比のパラメータを取得する
      if (i % step) == 0
        ret = yield

        if !ret.is_a?(Array)
          raise 'block need return array.'
        end
        (f,vol,duty) = ret
        # 周波数のクリップ
        f = [20.0, f.to_f, SamplePerSec / 2.0].sort.at(1)

        # ボリュームのクリップ
        vol = [0.0, vol.to_f, 255.0].sort.at(1)

        # デューティー比のクリップ
        if duty != nil
          duty = [0.0, duty.to_f, 1.0].sort.at(1)
        else
          duty = 0.5
        end
      end

      # 波形上での位置を更新
      count += f
      if( count >= SamplePerSec)
        count -= SamplePerSec
      end

      # 1ブロック分のデータを生成して追加
      v = gen_waveform(wave_type, vol, count, SamplePerSec, duty)
      @buffer[i] += v
    end
  end

  # 一周期の長さがp、形状 wave_type の波形から 位置countに対応するデータを取得する
  # count 現在のサンプル数
  def gen_waveform(wave_type, vol, count, p, duty)
    case wave_type
      when :WAVE_SIN then
        # サイン波
        s = Math.sin( Math::PI * 2 * count.to_f / p.to_f )
        return (s * vol.to_f * 128).to_i
      when :WAVE_SAW then
        # ノコギリ波
        v = count.to_f / p.to_f - 0.5
        return (v * vol.to_f * 256).to_i
      when :WAVE_TRI then
        # 三角波
        # 区間で場合分け
        if    count < (p * 1 / 4).to_i
          # [0/4 ... 1/4] -> 0 to p/4
          v = count.to_f
        elsif count < (p * 3 / 4).to_i
          # [1/4 ... 3/4] -> p/4 downto -p/4
          v = (p.to_f / 2).to_i - count.to_f
        else
          # [3/4 ... 4/4] -> -p/4 to 0
          v = count.to_f - p.to_f
        end
        return (v * vol.to_f * 128 / (p.to_f / 4).to_i).to_i
      else
        # 矩形波, デフォルト
        if count < p * duty
          # [0 .. duty]
          return (vol * 128).to_i
        else
          # [duty .. p]
          return (-vol * 128).to_i
        end
    end
  end

  def play
    sync()
    if @sound != nil
      @sound.play
    end
  end

  def stop
    if @sound != nil
      @sound.stop
    end
  end

  def save(path)
    # not implemented
  end
end
