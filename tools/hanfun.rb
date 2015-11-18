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

  CHeader = %q{

// =============================================================================
// <header>
// =============================================================================

}
  # =============================================================================
  # Helper classes
  # =============================================================================

  class Option
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

  class Interface < Option

    attr_reader :short

    def initialize(name, short)
      super(name)
      @short = short
      @short = @to_uid unless short
    end

  end

  class Attribute < Option

    attr_reader :uid
    attr_reader :type
    attr_reader :writable
    attr_reader :mandatory

    def initialize(name, interface, options)
      super(name)
      @to_uid = @name.upcase + "_ATTR"
      @interface = interface
      if options =~ /(\w+):(\d+):(rw?):(m|o)/i
        @type      = $1
        @uid       = $2.to_i
        @writable  = $3
        @mandatory = $4

        @writable  = @writable =~ /w/i ? true : false
        @mandatory  = @mandatory =~ /m/i ? true : false
      else
        raise ArgumentError, "Invalid option string '#{options}'"
      end
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
      argument :name,  required: true, type: :string
      argument :uid,   required: true, type: :string
      argument :short, optional: true, type: :string

      protected

      def __initialize__(namespace)
        @namespace = namespace
      end

      def top_path(name)
        File.join(CodeRoot, name)
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

      def cmake_path(name)
        File.join(CodeRoot, "cmake", name)
      end
    end

    class Interface < Abstract

      class_option "attributes", desc: "Attributes for the interface", aliases: "-a", type: :hash

      def __initialize__
        super("interfaces")
        @interface = Hanfun::Interface.new(name, short)
        @attributes = []
        options[:attributes].each do |key,value|
          @attributes << Attribute.new(key, @interface, value)
        end
        @attributes.sort! { |a,b| a.uid <=> b.uid }
      end

      desc "Add interface UID."
      def add_uid
        code = "#{@interface.to_uid} = #{uid},  //!< FIXME #{@interface.to_doc} interface UID.\n"
        inject_into_file include_path("interface.h", false), "         #{code}", :before => /^\s+\/\*\s+Reserved/
      end

      desc "Add configuration options"
      def config_files

        header = CHeader.gsub(/<header>/, "#{@interface.to_doc} Configuration")

        code_opts = []
        code_defs = []
        @attributes.select { |attr| !attr.mandatory }.each do |attr|
          name = "HF_ITF_#{@interface.short}_#{attr.to_uid}"
          code_opts << "option(#{name} \"Interface - #{@interface.to_doc} - #{attr.to_doc}\")"
          code_defs << "#define #{name}  @#{name}@"
        end

        # Add options for options attributes to cmake/options.cmake
        append_to_file cmake_path("options.cmake"), header.gsub(/^\/\//, "#") + code_opts.join("\n") + "\n"

        # Add optional attributes to config.h.in
        inject_into_file source_path("../config.h.in", false), header + code_defs.join('\n'),
                         :before => /\s+#endif/
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

        # Add optional attributes for test config.h.in
        header = CHeader.gsub(/<header>/, "#{@interface.to_doc} Configuration")

        code = []
        @attributes.select { |attr| !attr.mandatory }.each do |attr|
          code << "#define HF_ITF_#{@interface.short}_#{attr.to_uid}  1"
        end
        inject_into_file test_path("config.h.in", false), header + code.join('\n'),
                         :before => /\s+\/\/\s+=+\n\/\/\s+Other/

        # Add test code to file.
        code = "add_interface(\"#{name}\")\n"
        inject_into_file test_path("CMakeLists.txt", false), code,
                         :before => /^\s+##\s+Profiles/
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

