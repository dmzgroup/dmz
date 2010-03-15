require "lmkbuild"

local ipairs = ipairs
local sys = lmkbuild.system ()
local resolve = lmkbuild.resolve
local lmk = lmk
local print = print

module (...)

function add_lib ()
   if sys == "macos" then
      if resolve "$(DMZ_USE_LUA_JIT)" == "true" then
         lmk.add_vars {
            localDefines = "-DDMZ_USE_LUA_JIT",
            localLibPaths = "-L$(DMZ_LUA_JIT_LIB_PATH) -lluajit",
            localIncludes = "-I$(DMZ_LUA_JIT_INCLUDE_PATH)",
         }
      else
         lmk.add_libs {"lua"}
      end
   else
      lmk.add_libs {"lua"}
   end
end
