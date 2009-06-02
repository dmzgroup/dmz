require "lmkbuild"

local ipairs = ipairs
local print = print
local sys = lmkbuild.system ()
local lmk = lmk

module (...)

function set_name (name)
   lmk.set_name (name, { DMZ_USE_OGRE = true, })
end

function add_libs ()
   if sys == "macos" then
      lmk.add_vars {
         localLibPaths = "-framework Ogre",
      }
   else
      lmk.add_vars {
         localIncludes = "$(lmk.includePathFlag)$(DMZ_OGRE_INCLUDE_PATH)",
         localLibPaths = "$(lmk.libPathFlag)$(DMZ_OGRE_LIB_PATH)",
         localLibs = "$(lmk.libPrefix)OgreMain$(lmk.libSuffix)",
      }
   end
end
