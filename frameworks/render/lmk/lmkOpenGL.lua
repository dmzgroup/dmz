require "lmkbuild"

local lmk = lmk
local sys = lmkbuild.system ()

module (...)

function setup ()
   local libs = nil
   local libPaths = nil
   local includePaths = nil
   if sys == "win32" then
      libs = "opengl32.lib glu32.lib"
   elseif sys == "macos" then
      libs = "-framework OpenGl -framework Carbon"
   elseif sys == "linux" then
      libs = "-lGL -lX11 -lGLU -lXmu"
      libPaths = "-L/usr/X11R6/lib"
      includePaths ="-I/usr/X11R6/include"
   else
   end
   if libs then lmk.add_vars { localLibs = libs, } end
   if libPaths then lmk.add_vars { localLibPaths = libPaths, } end
   if includePaths then lmk.add_vars { localIncludes = includePaths, } end
end
