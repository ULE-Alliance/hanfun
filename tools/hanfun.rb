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

  TemplateRoot = File.join(HAN_FUN_ROOT, 'hanfun', 'templates')
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

    def initialize(name, options)
      super(name)
      @to_uid = @name.upcase + "_ATTR"
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

  class Command < Option

    class Role
      attr_reader :mandatory

      def initialize(flag)
        @mandatory = flag =~ /m/ ? true : false
      end
    end

    attr_reader :uid
    attr_reader :event
    attr_reader :server
    attr_reader :client

    def initialize(name, options)
      super(name)
      @to_uid = @name.upcase + "_CMD"
      if options =~ /(\d+):(c|e):(m|o):(m|o)/i
        @uid       = $1.to_i
        @event     = $2
        @server    = Role.new($3)
        @client    = Role.new($4)

        @event     = @event =~ /e/i ? true : false
      else
        raise ArgumentError, "Invalid option string '#{options}'"
      end
    end

  end

  class Namespace < Option

    attr_reader :path
    attr_reader :short
    attr_reader :include
    attr_reader :parent

    def initialize(name, short, parent)
      @name    = name
      @parent  = parent
      @short   = short
      @short   = @to_uid unless short

      @path    = name.downcase
      @include = @path.gsub(/s$/, '')
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

      class_option "attributes", desc: "Attributes for the interface", aliases: "-a", type: :hash
      class_option "commands", desc: "Commands for the interface", aliases: "-c", type: :hash

      # Configure generator.
      #
      # This method expects that the following instance variables to be defined.
      #
      #  - @type      : "interface" or "service"
      #  - @namespace : Namespace.new(...)
      #  - @generator : Hash
      #     - uid: { reference: ..., insert_at: ... }
      #     - build: { insert_at: ...}
      #     - tests: { insert_config_at: ..., insert_cmake_at: ...}
      #
      #  The generator hash fields have the following usage.
      #
      #  - uid.reference   : Interface/Service UID regular expression to use as formatting base.
      #                      E.g: /\bALERT\b/
      #
      #  - uid.insert_at   : Regular expression used to locate the point to insert the new UID definition
      #                      on inc/hanfun/interface.h
      #
      #  - build.insert_at : Regular expression used to locate the point to insert the build configuration
      #                      for the new interface/service in src/CMakeLists.txt
      #
      #  - tests.insert_at : Regular expression used to locate the point to insert the build configuration
      #                      for the new interface/service tests in tests/CMakeLists.txt
      #
      #  - tests.insert_config_at : Regular expression used to locate the point to insert the build configuration
      #                             options for the new interface/service tests in tests/config.in.h
      #
      #  - tests.insert_cmake_at : Regular expression used to locate the point to insert the build configuration
      #                            for the new interface/service tests in tests/CMakeLists.txt
      #
      def configure
        raise RuntimeError, "Invalid generator configuration (@generator)" unless @namespace
        @generator[:build][:macro] = "add_#{@type}"

        raise RuntimeError, "Invalid namespace configuration (@namespace)" unless @namespace && @namespace.is_a?(Namespace)

        raise RuntimeError, "Invalid type configuration (@type)" unless @type && @type.is_a?(String)
        @interface = Hanfun::Interface.new(name, short)

        # Handle attributes.
        @attributes = []
        options[:attributes].each do |key,value|
          @attributes << Attribute.new(key, value)
        end
        @attributes.sort! { |a,b| a.uid <=> b.uid }

        # Handle commands
        @commands = []
        options[:commands].each do |key,value|
          @commands << Command.new(key, value)
        end
        @commands.sort! { |a,b| a.uid <=> b.uid }

        sizes = @commands.map { |c| c.to_uid.size }
        sizes += @attributes.map { |a| a.to_uid.size }
        @uid_align = sizes.max
      end

      # Add interface UID.
      def add_uid
        path = include_path("interface.h", false)
        code = create_uid_line(path, @generator[:uid][:reference], @interface.to_uid, uid,
                               "//!< FIXME #{@interface.to_doc} interface UID.\n")
        inject_into_file(path, code, :before => @generator[:uid][:insert_at])
      end

      # Add configuration options
      def config_files

        header = CHeader.gsub(/<header>/, "#{@interface.to_doc} Configuration")

        code_opts = []
        code_defs = []
        prefix = "HF_#{@namespace.short}_#{@interface.short}"
        @commands.select { |cmd| !cmd.server.mandatory || !cmd.client.mandatory}.each do |cmd|
          name = "#{prefix}_#{cmd.to_uid.ljust(@uid_align)}"
          code_opts << "option(#{name} \"#{@type.capitalize} - #{@interface.to_doc} - #{cmd.to_doc}\")"
          code_defs << "#define #{name}  @#{name.strip}@"
        end

        # Add empty line between commands and attributes
        code_opts << ""
        code_defs << ""

        @attributes.select { |attr| !attr.mandatory }.each do |attr|
          name = "#{prefix}_#{attr.to_uid.ljust(@uid_align)}"
          code_opts << "option(#{name} \"#{@type.capitalize} - #{@interface.to_doc} - #{attr.to_doc}\")"
          code_defs << "#define #{name}  @#{name.strip}@"
        end

        # Add options for options attributes to cmake/options.cmake
        append_to_file cmake_path("options.cmake") do
          header.gsub(/^\/\//, "#") + code_opts.join("\n") + "\n"
        end

        # Add optional attributes to config.h.in
        inject_into_file top_path("config.h.in"), :before => /\s+#endif/ do
          header + code_defs.join("\n")
        end
      end

      # Generate include file.
      def include_file
        template("header.h.erb", include_path("#{name}.h")) do |content|
          format_code(content)
        end
      end

      # Generate common code file.
      def common_file
        template("common.cpp.erb", source_path("#{name}.cpp")) do |content|
          format_code(content)
        end
      end

      # Generate server side code file.
      def server_file
        template("server.cpp.erb", source_path("#{name}_server.cpp")) do |content|
          format_code(content)
        end
      end

      # Generate client side code file.
      def client_file
        template("client.cpp.erb", source_path("#{name}_client.cpp")) do |content|
          format_code(content)
        end
      end

      # Add interface files to build.
      def add_files_to_build
        code = "#{@generator[:build][:macro]}(\"#{name}\")\n"
        inject_into_file(source_path("CMakeLists.txt", false), code,
                         :before => @generator[:build][:insert_at])
      end

      # Generate test code file and add it to the build.
      def test_file
        # Generate test code.
        template("test.cpp.erb", test_path("test_#{name}.cpp")) do |content|
          format_code(content)
        end

        # Add optional attributes for test config.h.in
        header = CHeader.gsub(/<header>/, "#{@interface.to_doc} Configuration")

        code = []
        prefix = "HF_#{@namespace.short}"
        @commands.select { |cmd| !cmd.server.mandatory || !cmd.client.mandatory }.each do |cmd|
          code << "#define #{prefix}_#{@interface.short}_#{cmd.to_uid.ljust(@uid_align)}  1"
        end

        # Put empty line between commands and attributes
        code << ""

        @attributes.select { |attr| !attr.mandatory }.each do |attr|
          code << "#define #{prefix}_#{@interface.short}_#{attr.to_uid.ljust(@uid_align)}  1"
        end

        inject_into_file test_path("config.h.in", false), :before => @generator[:tests][:insert_config_at] do
          header + code.join("\n")
        end

        # Add test code to file.
        code = "#{@generator[:build][:macro]}(\"#{name}\")\n"
        inject_into_file test_path("CMakeLists.txt", false), code, :before => @generator[:tests][:insert_cmake_at]
      end

      protected

      def top_path(name)
        File.join(CodeRoot, name)
      end

      def include_path(name, append = true)
        file = File.join(CodeRoot, "inc", "hanfun")
        file = File.join(file, @namespace.path) if append
        return File.join(file, name)
      end

      def source_path(name, append = true)
        file = File.join(CodeRoot, "src")
        file = File.join(file, @namespace.path) if append
        return File.join(file, name)
      end

      def test_path(name, append = true)
        file = File.join(CodeRoot, "tests")
        file = File.join(file, @namespace.path) if append
        return File.join(file, name)
      end

      def cmake_path(name)
        File.join(CodeRoot, "cmake", name)
      end

      def create_uid_line(path, match, uid, value, comment)
        baseline = nil
        IO.foreach(path) do |line|
          if (line[match])
            baseline = line
            break
          end
        end

        uid_col     = baseline.index(/\w/)
        value_col   = baseline.index('=')
        comment_col = baseline.index(/\/\//)

        code = ' ' * uid_col
        code += uid + ' '

        padding = value_col - code.size
        code += ' ' * padding if padding > 0

        code += "= " + value + ","

        padding = comment_col - (code.size + 1)
        code += ' ' * padding if padding > 0

        code + ' ' + comment
      end

      def format_code(content)
        file = Tempfile.new("hanfun_#{@interface.name}_")
        begin
          file.write(content)
          file.flush
          res = run("uncrustify -q -c scripts/uncrustify.cfg -l CPP -f #{file.path}",
                    capture: true, verbose: false)
        rescue Errno::ENOENT
          res = content
        ensure
          file.close
          file.unlink   # deletes the temp file
        end
        res
      end

    end

    class Interface < Abstract

      def configure
        @type = "interface"
        @namespace = Namespace.new("Interfaces", "ITF", "Interfaces::Base")

        @generator = {}
        @generator[:uid] = {
          reference: /\bALERT\b/,
          insert_at: /^\s+\/\*\s+Reserved/,
        }

        @generator[:build] = { insert_at: /^\s+##\s+Core/ }

        @generator[:tests] = {
          insert_config_at: /\s+\/\/\s+=+\n\/\/\s+Other/,
          insert_cmake_at: /^\s+##\s+Profiles/,
        }
        super
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

