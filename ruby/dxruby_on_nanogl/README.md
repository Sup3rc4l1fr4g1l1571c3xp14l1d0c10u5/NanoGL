# dxruby on nanogl

## これは何？

- nanogl を使って dxruby ライブラリの一部命令を再現する薄いラッパーです。
- IRPC2016 応募作品 n-fall の作者の環境向けに作成しました。
  - 上記理由より当初はn-fallの動作に必要な最低限の命令のみ実装していました。
  - 実験的な実装なので、完全な互換性はありません。 
    - 現時点ではdxrubyのclassic_sampleが動作しますがシステムフォントが使えません。
- 動作確認環境
  - MacBook Pro 2011 Early (Mac OS X El Capitan) 
  - MacBook Air (Mac OS X El Capitan)
  - Lenovo Thinkpad X230 (Windows 10 Pro)
- 使い方
  - NanoGLのgemを作成する 
  - gem install -l NanoGL-<version>.gem でインストール
  - dxruby_on_nanogl.rb と dxruby_on_nanoglフォルダ をコピーして配置
  - require 'dxruby' の代わりに require_relative 'dxruby_on_nanogl.rb' する

