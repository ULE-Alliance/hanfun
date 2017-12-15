# =============================================================================
#
# @file       tools/hanfun/lib/hanfun.rb
#
# This file contains the implementation of helper classes used by the hanfun
# generators.
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

# =============================================================================
# Helper classes
# =============================================================================

module Hanfun

  Root = File.expand_path(File.join(File.dirname(__dir__), '..'))

  TemplateRoot = File.join(Root, 'templates')
  CodeRoot     = File.expand_path(File.join(Root, '../..'))

  CHeader = '
// =============================================================================
// <header>
// =============================================================================

'.freeze

  # Base class for all the generator options
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

  # Interface configuration
  class Interface < Option

    attr_reader :short
    attr_reader :path

    def initialize(name, short)
      super(name)
      @name = @to_class = @to_doc = name if name =~ /\A[A-Z]+\z/
      @short = short
      @short = @to_uid unless short
      @path  = @name.underscore
    end

  end

  # Interface/Service attribute configuration
  class Attribute < Option

    FIELDS = %w[acl type uid].freeze

    attr_reader :uid
    attr_reader :type
    attr_reader :writable
    attr_reader :mandatory

    def initialize(name, options)
      super(name)
      @to_uid = @name.upcase + '_ATTR'
      if options.is_a?(String) && options =~ /(\w+):(\d+):(rw?):(m|o)(:(\w+))?/i
        @type         = Regexp.last_match(1)
        @uid          = Regexp.last_match(2).to_i
        @writable     = Regexp.last_match(3)
        @mandatory    = Regexp.last_match(4)
        name_override = Regexp.last_match(6)
      elsif options.is_a?(Hash) && ((options.keys.sort & FIELDS) == FIELDS)
        @type = options['type']
        @uid  = options['uid'].to_i
        @writable     = options['acl']
        @mandatory    = options['acl']
        name_override = options['override']
      else
        raise ArgumentError, "Invalid options '#{options}'"
      end

      @writable  = @writable =~ /w/i ? true : false
      @mandatory = @mandatory =~ /m/i ? true : false

      @name     = name_override if name_override
      @to_class = @name.camelize if name_override

      @to_doc   = name if name =~ /\A[A-Z]+\z/
    end

  end

  # Interface/Service command configuration
  class Command < Option

    # Command role class
    class Role

      attr_reader :mandatory

      def initialize(flag)
        @mandatory = flag =~ /^m.*/i ? true : false
      end

    end

    FIELDS = %w[client server uid].freeze

    attr_reader :uid
    attr_reader :event
    attr_reader :server
    attr_reader :client

    def initialize(name, options)
      super(name)
      @to_uid = @name.upcase + '_CMD'
      if options.is_a?(String) && options =~ /(\d+):(c|e):(m|o):(m|o)/i
        @uid       = Regexp.last_match(1).to_i
        @event     = Regexp.last_match(2)
        @server    = Role.new(Regexp.last_match(3))
        @client    = Role.new(Regexp.last_match(4))
        @event     = @event =~ /e/i ? true : false
      elsif options.is_a?(Hash) && ((options.keys.sort & FIELDS) == FIELDS)
        @uid       = options['uid'].to_i
        @event     = options.fetch('event', false)
        @server    = Role.new(options['server'])
        @client    = Role.new(options['client'])
      else
        raise ArgumentError, "Invalid options '#{options}'"
      end
    end

  end

  # Interface/Service namespace configuration
  class Namespace < Option

    attr_reader :path
    attr_reader :short
    attr_reader :include

    def initialize(name, short)
      @name    = name
      @short   = short
      @short   = @to_uid unless short

      @path    = name.downcase
      @include = @path.gsub(/s$/, '')
    end

  end

end # Hanfun
