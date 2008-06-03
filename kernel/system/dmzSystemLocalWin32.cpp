#include "dmzSystemLocalWin32.h"
#include <windows.h>

//! For internal use only.
dmz::String
dmz::get_last_win32_error () {

   LPVOID lpMsgBuf = 0;

   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0,
      NULL);

   const dmz::String Error ((const char *)lpMsgBuf);

   LocalFree( lpMsgBuf );

   return Error;
}
