# =============================================================================
#
# @file       tools/hanfun/extended.rb
#
# This file injects helper methods to the standard Ruby classes.
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

class String

  def underscore
    downcase.gsub("\s+", "_")
  end

  def camelize
    human_name("")
  end

  def human_name(sep=' ')
    underscore.split("_").map { |w| w.capitalize }.join(sep)
  end

end

class Integer

  def to_hex(lenght = 4)
    "0x" + to_s(16).rjust(lenght,'0')
  end

end
