require "lmkQt"

local lmk = lmk
local lmkQt = lmkQt

module (...)

function set_up (appName)

   lmkQt.set_name (appName)
   lmk.set_type ("exe")
   lmk.add_files {"$(dmzQtAppShell.localPwd)/dmzQtAppShell.cpp",}
   lmk.add_libs {
      "dmzQtLogObserver",
      "dmzQtUtil",
      "dmzApplication",
      "dmzXML",
      "dmzKernel",
   }
   lmk.add_preqs {"dmzQtAppShell", "dmzFoundation",}
   lmkQt.add_libs {"QtCore", "QtGui"}
   lmk.add_vars ({localLibPaths = "-framework CoreFoundation"},{macos = true})
   lmk.add_vars (
      {localLibPaths="/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"},
      {win32=true, opt=true})
end

