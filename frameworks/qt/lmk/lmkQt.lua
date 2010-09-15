require "lmkbuild"

local ipairs = ipairs
local sys = lmkbuild.system ()
local resolve = lmkbuild.resolve
local lmk = lmk

module (...)

function get_flags (flags) return lmk.merge_tables ({DMZ_USE_QT = true, }, flags) end

function set_name (name, flags)
   lmk.set_name (name, get_flags (flags))
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
      lmk.add_vars {
         localLibPaths = "-F$(DMZ_QT_LIB_PATH) " .. paths,
         localIncludes = "-F$(DMZ_QT_INCLUDE_PATH)",
      }
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
         libs = libs .. "$(lmk.libPrefix)" .. element .. libDebug .. libNumber ..
            "$(lmk.libSuffix)"
      end
      lmk.add_vars {
         localIncludes = "$(lmk.includePathFlag)$(DMZ_QT_INCLUDE_PATH)",
         localLibPaths = "$(lmk.libPathFlag)$(DMZ_QT_LIB_PATH)",
         localLibs = libs,
      }
   end
end

function add_uitools ()
   local libDebug = ""
   local buildMode = resolve ("$(lmk.buildMode)")
   if buildMode == "debug" then libDebug = "_debug" end
   lmk.add_vars {
      localLibPaths = "$(lmk.libPathFlag)$(lmk.projectRoot)depend/lib",
      localLibs = "$(lmk.libPrefix)QtUiTools" .. libDebug .. "$(lmk.libSuffix)",
      localIncludes = "$(lmk.includePathFlag)$(lmk.projectRoot)depend/include",
   }
end
