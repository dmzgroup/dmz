require "lmkbuild"

local ipairs = ipairs
local print = print
local sys = lmkbuild.system ()
local resolve = lmkbuild.resolve
local lmk = lmk
local tostring = tostring

module (...)

function set_name (name)
   lmk.set_name (name, { DMZ_USE_QT = true, })
end

function add_libs (list)
   if sys == "macos" then
      local paths = nil
      for ix, element in ipairs (list) do
         if paths then paths = paths .. " "
         else paths = ""
         end
         paths = paths .. "-framework " .. element
      end
      lmk.add_vars { localLibPaths = paths, }
   else
      local libNumber = ""
      local libDebug = ""
      if sys == "win32" then
         libNumber = 4
         local buildMode = resolve ("$(lmk.buildMode)")
         if buildMode == "debug" then libDebug = "d" end
      end
      local libs = nil
      for ix, element in ipairs (list) do
         if libs then libs = libs .. " "
         else libs = ""
         end
         libs = libs .. "$(lmk.libPrefix)" .. element .. libDebug .. libNumber .. "$(lmk.libSuffix)"
      end
      lmk.add_vars {
         localIncludes = "$(lmk.includePathFlag)$(DMZ_QT_INCLUDE_PATH)",
         localLibPaths = "$(lmk.libPathFlag)$(DMZ_QT_LIB_PATH)",
         localLibs = libs,
      }
   end
end
