#ifndef DMZ_RUNTIME_PLUGIN_FACTORY_LINK_SYMBOL_DOT_H
#define DMZ_RUNTIME_PLUGIN_FACTORY_LINK_SYMBOL_DOT_H

/*!

\file
\brief Defines link symbol for Plugin factory functions.
\details The Win32 platform requires that public DLL symbols be exported
explicitly with the __declspec (dllexport) compiler directive. In order for
a factory function to be accessible, it must be exported. The
DMZ_PLUGIN_FACTORY_LINK_SYMBOL C macro will compile the correct export
directive for Win32 compiles.

*/

#ifdef _WIN32
#   define DMZ_PLUGIN_FACTORY_LINK_SYMBOL __declspec (dllexport)
#else
#   define DMZ_PLUGIN_FACTORY_LINK_SYMBOL //!< Export macro.
#endif

#endif // DMZ_RUNTIME_PLUGIN_FACTORY_LINK_SYMBOL_DOT_H
