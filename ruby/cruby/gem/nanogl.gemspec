# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'nanogl/version'

Gem::Specification.new do |spec|
  spec.name          = "nanogl"
  spec.version       = NanoGL::VERSION
  spec.authors       = ["Whelp"]
  spec.email         = [""]

  spec.summary       = %q{NanoGL bindings}
  spec.description   = %q{A Ruby bindings of NanoGL.}
  spec.homepage      = "https://github.com/Sup3rc4l1fr4g1l1571c3xp14l1d0c10u5/NanoGL"

  spec.files         = %w(Gemfile nanogl.gemspec README.md Rakefile) + Dir.glob("{exe,lib,src}/*") + Dir.glob("{lib}/nanogl/*")
  spec.require_paths = ["lib"]

  spec.required_ruby_version = '>= 2.0.0'
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_dependency 'ffi', '>= 1.9.10'
  spec.add_development_dependency "bundler", "~> 1.10"
end
