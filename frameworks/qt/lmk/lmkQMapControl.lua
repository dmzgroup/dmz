require "lmkbuild"
require "lmkQt"

local ipairs = ipairs
local sys = lmkbuild.system ()
local resolve = lmkbuild.resolve
local lmk = lmk
local lmkQt = lmkQt

module (...)

function get_flags (flags) return lmk.merge_tables ({DMZ_USE_QMAPCONTROL = true}, lmkQt.get_flags (flags)) end

function set_name (name, flags)
   lmk.set_name (name, get_flags (flags))
end

function add_libs (list)
   lmk.add_libs {"QMapControl"}
   lmkQt.add_libs {"QtCore", "QtGui",}
end
