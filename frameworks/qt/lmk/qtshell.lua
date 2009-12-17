require "lmkQt"

local lmk = lmk
local lmkQt = lmkQt
local AppShellName = "dmzQtAppShell"

module (...)

function set_up (appName)

   lmkQt.set_name (appName)
   lmk.set_type ("exe")
   lmk.add_files {
      "$(dmzQtAppShell.localPwd)/dmzQtAppShell.cpp",
      "$(dmzQtAppShell.localPwd)/dmzQtSplashScreen.cpp",
   }
   lmk.add_libs {
      "dmzQtLogObserver",
      "dmzQtUtil",
      "dmzFoundation",
      "dmzKernel",
   }

   if appName ~= AppShellName then
      lmk.add_preqs {AppShellName,}
   end
   lmk.add_preqs {"dmzFoundation",}
   lmkQt.add_libs {"QtCore", "QtGui"}
   lmk.add_vars ({localLibPaths = "-framework CoreFoundation"},{macos = true})
   lmk.add_vars (
      {localLibPaths="/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"},
      {win32=true, opt=true})
end

