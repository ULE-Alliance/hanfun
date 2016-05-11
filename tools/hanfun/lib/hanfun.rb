# =============================================================================
#
# @file       tools/hanfun/lib/hanfun.rb
#
# This file implements a helper generator tool for implementing HAN-FUN interfaces,
# profiles and services.
#
# @version    x.x.x
#
# @copyright  Copyright &copy; &nbsp; 2016 ULE Alliance
#
# For licensing information, please see the file 'LICENSE' in the root folder.
#
# Initial development by Bithium S.A. [http://www.bithium.com]
#
# =============================================================================

require "thor"
require "yaml"

require "hanfun/version"
require "hanfun/extended"
require "hanfun/generators/interface"
require "hanfun/generators/core"

module Hanfun

  class Generator < Thor
    register(Hanfun::Generators::Interface, "interface", "interface [arguments]", "Generate a new interface scaffolding.")
    register(Hanfun::Generators::Core, "core", "core [arguments]", "Generate a new service scaffolding.")

    desc 'generate [filename]', 'Generate Interface/Services from a YAML file'

    def generate(filename)
      raise Thor::MalformattedArgumentError, "File #{filename} does not exist !" unless File.exist?(filename)

      config = YAML::load(File.open(filename))

      config.each do |interface|
        args = [interface['name'], interface['uid'].to_hex]
        interface['attributes'] = interface['attributes'].reduce({}, :merge) if interface['attributes'].is_a? Array
        interface['commands'] = interface['commands'].reduce({}, :merge) if interface['attributes'].is_a? Array

        if interface['core']
          args << interface.fetch('interface', false)
          generator = Generators::Core.new(args, interface)
        else
          generator = Generators::Interface.new(args,interface)
        end
        generator.invoke_all
      end
    end

  end

end # Hanfun
