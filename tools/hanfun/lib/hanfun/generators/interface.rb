# =============================================================================
#
# @file       tools/hanfun/lib/generators/interface.rb
#
# This file contains the implementation for the the interfaces generator.
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

#
module Hanfun
  #
  module Generators
    #
    # This class is responsible for the generation of new HAN-FUN interfaces.
    #
    class Interface < Abstract

      desc 'Generator for new HAN-FUN interfaces.'

      # rubocop:disable Metrics/MethodLength
      def configure
        @type = 'interface'
        @namespace = Hanfun::Namespace.new('Interfaces', 'ITF')

        @generator = {}
        @generator[:uid] = {
          reference: /\bALERT\b/,
          insert_at: %r{^\s+/\*\s+Reserved}
        }

        @generator[:config] = {
          insert_config_at: /\s#endif/,
          insert_cmake_at:  /^\s+#\s+=+\n#\s+Dependecies/
        }

        @generator[:debug] = {
          insert_at: %r{^\s+//\s+=+\n//\s+Core},
          protocol:  'Unknown'
        }

        @generator[:include] = {
          insert_at: %r{^\s+//\s+=+\n//\s+Core}
        }

        @generator[:attributes] = {
          insert_at: {
            include: /^\s.+core/,
            factory: %r{\};\s+//\s+=+\s+//\s+Attributes::get_factory}
          }
        }

        @generator[:build] = {
          insert_at: /^\s+##\s+Core/,
          macro:     'add_interface'
        }

        @generator[:tests] = {
          insert_config_at: %r{\s+//\s+=+\n//\s+Other},
          insert_cmake_at:  /^\s+##\s+Profiles/
        }
        super
      end
      # rubocop:enable Metrics/MethodLength

    end
  end # Hanfun::Generator
end # Hanfun
