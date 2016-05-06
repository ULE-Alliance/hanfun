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

require "hanfun/version"
require "hanfun/extended"
require "hanfun/generators/interface"
require "hanfun/generators/core"

module Hanfun

  class Generator < Thor
    register(Hanfun::Generators::Interface, "interface", "interface [arguments]", "Generate a new interface scaffolding.")
    register(Hanfun::Generators::Core, "core", "core [arguments]", "Generate a new service scaffolding.")
  end

end # Hanfun
