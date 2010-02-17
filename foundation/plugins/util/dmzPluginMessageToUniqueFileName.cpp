#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include "dmzPluginMessageToUniqueFileName.h"
#include <dmzSystemFile.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::PluginMessageToUniqueFileName
\ingroup Foundation
\brief Converts a message to a message with a unique file name as its payload.
\details
When the incoming message is received, the plugin creates a new file name of the form
$HOME/Desktop/\<root\>-\<number\>\<extension\> and sends that name as the payload in the
outgoing message. The plugin insures a file of the generated name does not exist before
sending the outgoing message. If the file name is not unique, the \<number\> is
incremented until a unique name is found.
\code
<local>
   <in-message name="Name of incoming message"/>
   <out-message name="Name of outgoing message"/>
   <target name="Name of outgoing message target"/>
   <file root="File root name [Defaults to 'DMZ File']" ext="File extensions"/>
</local>
\endcode

*/

//! \cond
dmz::PluginMessageToUniqueFileName::PluginMessageToUniqueFileName (
      const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _log (Info),
      _convert (Info),
      _target (0),
      _index (1) {

   _init (local);
}


dmz::PluginMessageToUniqueFileName::~PluginMessageToUniqueFileName () {

}


// Plugin Interface
void
dmz::PluginMessageToUniqueFileName::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::PluginMessageToUniqueFileName::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::PluginMessageToUniqueFileName::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _inMsg) {

      Boolean done = False;
      String file;

      while (!done) {

         String testFile (_filePath);
         testFile << _fileRoot << "-" << String::number (_index) << _fileExt;

         if (!is_valid_path (testFile)) { done = True; file = testFile; }

         _index++;
      }

      Data out = _convert.to_data (file);
      _outMsg.send (_target, &out);
   }
}


void
dmz::PluginMessageToUniqueFileName::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   _inMsg = config_create_message ("in-message.name", local, String (), context);
   subscribe_to_message (_inMsg);

   _outMsg = config_create_message ("out-message.name", local, String (), context);

   _target = config_to_named_handle ("target.name", local, String (), context);

   _fileRoot = config_to_string ("file.root", local, "DMZ File");
   _fileExt = config_to_string ("file.ext", local);

   _filePath = get_home_directory ();

   String desktop = _filePath + "Desktop/";
   if (is_valid_path (desktop)) { _filePath = desktop; }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginMessageToUniqueFileName (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginMessageToUniqueFileName (Info, local);
}

};
