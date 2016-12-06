# dxruby on nanogl

## これは何？

- Windows環境以外で nanogl を使って dxruby ライブラリの一部命令を模する薄いラッパーです。
- 元々は IRPC2016 応募作品 n-fall の作者の開発環境向けに作成しました。
  - 上記理由より当初は n-fall の動作に必要な最低限の命令のみ実装していました。
  - 実験的な実装なので、完全な互換性はありません。 
    - 現時点では dxruby144-samples.zip の classic_sample が辛うじて動作します。
      - 行頭の #!ruby -Ks の削除が必要です。
- 動作確認環境
  - MacBook Pro 2011 Early (Mac OS X El Capitan) 
  - MacBook Air (Mac OS X El Capitan)
  - Lenovo Thinkpad X230 (Windows 10 Pro)
- 使い方
  - README.md に従って nanogl-0.2.2.gem をビルドしてください
  - gem install -l NanoGL-0.2.0.gem で nanogl をインストールしてください。
  - require 'dxruby' を実行しているrbファイルと同じディレクトリに dxruby_on_nanogl.rb と dxruby_on_nanoglフォルダ をコピーして配置してください。
  - require 'dxruby' を実行しているrbファイルを開き、 require 'dxruby' をコメントアウトし、直後に require_relative 'dxruby_on_nanogl.rb' を記述してください。
  - 対応している命令で動く範囲でそれっぽく動きます。
- 既知の問題
  - クラスの継承関係などは再現していません。
  - ラッパー部分はrubyで実装しているためImageクラスのピクセル操作は非常に低速です。
  - フォントレンダリングエンジンの関係でフォントの描画などの挙動が一致しません。
  - システムに導入済みのフォントを列挙していないため、システムフォントが使えません。
  - スクリーンショット命令は dds 形式をサポートしません。
  - Soundクラスは midi形式 をサポートしません。

