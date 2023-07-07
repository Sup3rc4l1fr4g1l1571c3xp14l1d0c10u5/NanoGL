# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'dxruby_on_nanogl/version'

Gem::Specification.new do |spec|
  spec.name          = "dxruby_on_nanogl"
  spec.version       = DxRubyOnNanoGL::VERSION
  spec.authors       = ["Whelp"]
  spec.email         = [""]

  spec.summary       = %q{A library that reproduces the DxRuby API using NanoGL.}
  spec.description   = %q{A library that reproduces the DxRuby API using NanoGL.}
  spec.homepage      = "https://github.com/Sup3rc4l1fr4g1l1571c3xp14l1d0c10u5/NanoGL"

  spec.files         = %w(Gemfile dxruby_on_nanogl.gemspec README.md Rakefile) + Dir.glob("{exe,lib,src}/*") + Dir.glob("{lib}/dxruby_on_nanogl/*")
  spec.require_paths = ["lib"]

  spec.required_ruby_version = '>= 2.0.0'
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_dependency 'nanogl', '>= 0.3.0'
  spec.add_development_dependency "bundler", "~> 1.10"
end
