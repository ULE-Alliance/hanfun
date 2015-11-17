#!/usr/bin/env ruby
# =============================================================================
#
# @file       tools/hanfun.rb
#
# This file implements a helper generator tool for implementing HAN-FUN interfaces,
# profiles and services.
#
# @version    x.x.x
#
# @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
#
# For licensing information, please see the file 'LICENSE' in the root folder.
#
# Initial development by Bithium S.A. [http://www.bithium.com]
#
# =============================================================================

HAN_FUN_ROOT = File.expand_path(File.dirname(__FILE__))

$:.unshift(HAN_FUN_ROOT) unless $:.include?(File.dirname(__FILE__)) || $:.include?(HAN_FUN_ROOT)

require 'thor'
require File.join('hanfun', 'extended.rb')

module Hanfun

  TemplateRoot = File.join(HAN_FUN_ROOT, 'hanfun')
  CodeRoot     = File.expand_path(File.join(HAN_FUN_ROOT, '..'))

  # =============================================================================
  # Helper classes
  # =============================================================================

  class Interface
    attr_reader :name
    attr_reader :to_doc
    attr_reader :to_class
    attr_reader :to_uid

    def initialize(name)
      @name     = name.underscore
      @to_doc   = @name.human_name
      @to_class = @name.camelize
      @to_uid   = @name.upcase
    end
  end

  module Generators

    # =============================================================================
    # Generator classes
    # =============================================================================

    class Abstract < Thor::Group
      include Thor::Actions

      def self.source_root
        File.join(TemplateRoot)
      end

      # Define arguments and options
      argument :name
      argument :uid

      protected

      def __initialize__(namespace)
        @namespace = namespace
      end

      def include_path(name, append = true)
        file = File.join(CodeRoot, "inc", "hanfun")
        file = File.join(file, @namespace) if append
        return File.join(file, name)
      end

      def source_path(name, append = true)
        file = File.join(CodeRoot, "src")
        file = File.join(file, @namespace) if append
        return File.join(file, name)
      end

      def test_path(name, append = true)
        file = File.join(CodeRoot, "tests")
        file = File.join(file, @namespace) if append
        return File.join(file, name)
      end
    end

    class Interface < Abstract

      def __initialize__
        super("interfaces")
        @interface = Hanfun::Interface.new(name)
      end

      desc "Add interface UID."
      def add_uid
        code = "#{@interface.to_uid} = #{uid},  //!< FIXME #{@interface.to_doc} interface UID.\n"
        inject_into_file include_path("interface.h", false), "         #{code}", :before => /^\s+\/\*\s+Reserved/
      end

      desc "Generate include file."
      def include_file
        template("interface/header.h.erb", include_path("#{name}.h"))
      end

      desc "Generate common code file."
      def common_file
        template("interface/common.cpp.erb", source_path("#{name}.cpp"))
      end

      desc "Generate server side code file."
      def server_file
        template("interface/server.cpp.erb", source_path("#{name}_server.cpp"))
      end

      desc "Generate client side code file."
      def client_file
        template("interface/client.cpp.erb", source_path("#{name}_client.cpp"))
      end

      desc "Add interface files to build."
      def add_files_to_build
        code = "add_interface(\"#{name}\")\n"
        inject_into_file source_path("CMakeLists.txt", false), code, :before => /^\s+##\s+Core/
      end

      desc "Generate test code file and add it to the build."
      def test_file
        # Generate test code.
        template("interface/test.cpp.erb", test_path("test_#{name}.cpp"))

        # Add test code to file.
        code = "add_interface(\"#{name}\")\n"
        inject_into_file test_path("CMakeLists.txt", false), code, :before => /^\s+##\s+Profiles/
      end

    end

  end

  class Generator < Thor
    register(Hanfun::Generators::Interface, "interface", "interface", "Generate a new interface scaffolding.")
  end

end

# =============================================================================
# MAIN
# =============================================================================

Hanfun::Generator.start(ARGV)

