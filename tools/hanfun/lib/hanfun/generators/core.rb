# =============================================================================
#
# @file       tools/hanfun/lib/generators/core.rb
#
# This file contains the implementation for the the core services & interfaces generator.
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

require_relative 'abstract'

module Hanfun
  module Generators
    #
    # This class is responsible for the generation of new HAN-FUN services/core interfaces.
    #
    class Core < Abstract

      class_option 'interface', desc: 'Generate a core interface, NOT a service', aliases: '-i',
      type: :boolean, default: false

      desc 'Generator for new HAN-FUN services/core interfaces.'

      # rubocop:disable Metrics/MethodLength
      def configure
        @type = options[:interface] ? 'interface' : 'service'
        @arguments = options[:interface] ? nil : 'Unit0 &unit'
        @namespace = Namespace.new('Core', 'CORE')

        @generator = {}
        @generator[:uid] = {
          reference: /\bDEVICE_MANAGEMENT\b/,
          insert_at: %r{^\s+/\*\s+Functional Interfaces}
        }

        @generator[:config] = {
          insert_config_at: %r{^\s+//\s+=+\n//\s+Interfaces},
          insert_cmake_at:  /^\s+#\s+=+\n#\s+Interfaces/
        }

        @generator[:debug] = {
          insert_at: %r{^\s+//\s+=+\n//\s+Protocol},
          protocol:  'Interfaces'
        }

        @generator[:include] = {
          insert_at: %r{^\s+//\s+=+\n//\s+Profiles}
        }

        @generator[:attributes] = {
          insert_at: {
            include: /^\susing/,
            factory: %r{^\s+/\*\s+Functional}
          }
        }
        @generator[:build] = {
          insert_at: /^\s+##\s+Units/,
          macro:     'add_core'
        }

        @generator[:tests] = {
          insert_config_at: %r{\s+//\s+=+\n//\s+Other},
          insert_cmake_at:  /^\s+##\s+Transport/
        }
        super
      end
      # rubocop:enable Metrics/MethodLength

      protected

      def service?
        @type == 'service'
      end

      def parent
        result = super
        result = "Interfaces::#{result}" unless service?
        result
      end

    end
  end # Hanfun::Generator
end # Hanfun
