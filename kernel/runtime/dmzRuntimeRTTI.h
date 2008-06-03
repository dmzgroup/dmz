#ifndef DMZ_RUNTIME_RTTI_DOT_H
#define DMZ_RUNTIME_RTTI_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Plugin;
   class PluginInfo;
   class RuntimeContext;

   DMZ_KERNEL_LINK_SYMBOL Boolean store_rtti_interface (
      const String &InterfaceName,
      const Handle InstanceHandle,
      RuntimeContext *context,
      void *ptr);

   DMZ_KERNEL_LINK_SYMBOL Boolean store_rtti_interface (
      const String &InterfaceName,
      const PluginInfo &Info,
      void *ptr);

   DMZ_KERNEL_LINK_SYMBOL void *lookup_rtti_interface (
      const String &InterfaceName,
      const Handle InstanceHandle,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL void *lookup_rtti_interface (
      const String &InterfaceName,
      const String &InstanceName,
      const Plugin *PluginPtr);

   DMZ_KERNEL_LINK_SYMBOL Boolean is_valid_rtti_handle (
      const Handle InterfaceHandle,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL Boolean has_rtti_interface (const Plugin *PluginPtr);

   DMZ_KERNEL_LINK_SYMBOL void *remove_rtti_interface (
      const String &InterfaceName,
      const Handle InstanceHandle,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL void *remove_rtti_interface (
      const String &InterfaceName,
      const PluginInfo &Info);
};

#endif // DMZ_RUNTIME_RTTI_DOT_H
