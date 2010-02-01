require "lmkbuild"

local ipairs = ipairs
local pairs = pairs
local print = print
local sys = lmkbuild.system ()
local resolve = lmkbuild.resolve
local lmk = lmk
local type = type

module (...)

function get_flags (flags) return lmk.merge_tables ({DMZ_USE_OSG = true, }, flags) end

function set_name (name, flags)
   lmk.set_name (name, get_flags (flags))
end

function add_libs (list)
   if sys == "win32" then
      lmk.add_vars {localDefines = "$(lmk.defineFlag)WIN32"}
   end
--   if sys == "macos" then
   if sys == "" then
      local paths = "-Xlinker -executable_path -Xlinker " ..
         "$(DMZ_OSG_LIB_PATH) -F$(DMZ_OSG_LIB_PATH)"
      for ix, element in ipairs (list) do
         paths = paths .. " -framework " .. element
      end
      lmk.add_vars {
         localIncludes = "-F$(DMZ_OSG_INCLUDE_PATH)",
         localLibPaths = paths,
      }
   else
      local libDebug = ""
      if sys == "win32" then
         local buildMode = resolve ("$(lmk.buildMode)")
         if buildMode == "debug" then libDebug = resolve ("$(DMZ_OSG_DEBUG)") end
      end
      local libs = nil
      for ix, element in ipairs (list) do
         if libs then libs = libs .. " "
         else libs = ""
         end
         libs = libs .. "$(lmk.libPrefix)" .. element .. libDebug .. "$(lmk.libSuffix)"
      end
      lmk.add_vars { localLibs = libs, }
      local inc = resolve "$(DMZ_OSG_INCLUDE_PATH)"
      if inc and inc ~= "" then
         lmk.add_vars { localIncludes = "$(lmk.includePathFlag)" .. inc, }
      end
      local lpath = resolve "$(DMZ_OSG_LIB_PATH)"
      if lpath and lpath ~= "" then
         lmk.add_vars { localLibPaths = "$(lmk.libPathFlag)" .. lpath, }
      end
   end
end
