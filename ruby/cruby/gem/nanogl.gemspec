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
  spec.homepage      = "http://whelp.0t0.jp/"

  spec.files         = %w(Gemfile nanogl.gemspec README.md Rakefile) + Dir.glob("{exe,lib,src}/*") + Dir.glob("{lib}/nanogl/*")
  spec.require_paths = ["lib"]

#  spec.required_ruby_version = '>= 2.2.3'
  spec.required_ruby_version = '>= 2.0.0'
  spec.add_development_dependency "bundler", "~> 1.10"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency 'ffi', '>= 1.9.10'
end
