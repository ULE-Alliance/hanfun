# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'hanfun/version'

Gem::Specification.new do |spec|
  spec.name          = "hanfun"
  spec.version       = Hanfun::VERSION
  spec.authors       = ["Filipe Alves"]
  spec.email         = ["filipe.alves@bithium.com"]

  spec.summary       = %q{HAN-FUN code generator.}
  spec.description   = %q{This gem provides code generators for HAN-FUN interfaces, services and profiles.}
  spec.homepage      = "https://github.com/ULE-Alliance/hanfun"
  spec.licenses      = ['GPLv2',  'Apache 2.0']

  spec.files         = `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  spec.bindir        = "exe"
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]

  spec.add_dependency 'thor', '~> 0.19.1'

  spec.add_development_dependency "bundler", "~> 1.8"
  spec.add_development_dependency "rake", "~> 10.0"
end
