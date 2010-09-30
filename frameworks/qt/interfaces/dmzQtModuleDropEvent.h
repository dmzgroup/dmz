#ifndef DMZ_QT_MODULE_DROP_EVENT_DOT_H
#define DMZ_QT_MODULE_DROP_EVENT_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

class QDropEvent;

namespace dmz {

   class QtModuleDropEvent {

      public:
         static QtModuleDropEvent *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_qt_module_drop_event_name () const;
         Handle get_qt_module_drop_event_handle () const;

         // QtModuleDropEvent Interface
         virtual void receive_drop_event (
            const Handle Source,
            const QDropEvent &Event) = 0;

      protected:
         QtModuleDropEvent (const PluginInfo &Info);
         ~QtModuleDropEvent ();

      private:
         QtModuleDropEvent ();
         QtModuleDropEvent (const QtModuleDropEvent &);
         QtModuleDropEvent &operator= (const QtModuleDropEvent &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char QtModuleDropEventInterfaceName[] = "QtModuleDropEventInterface";
   //! \endcond
};


inline dmz::QtModuleDropEvent *
dmz::QtModuleDropEvent::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (QtModuleDropEvent *)lookup_rtti_interface (
      QtModuleDropEventInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::QtModuleDropEvent::QtModuleDropEvent (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (QtModuleDropEventInterfaceName, __Info, (void *)this);
}


inline
dmz::QtModuleDropEvent::~QtModuleDropEvent () {

   remove_rtti_interface (QtModuleDropEventInterfaceName, __Info);
}


inline dmz::String
dmz::QtModuleDropEvent::get_qt_module_drop_event_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::QtModuleDropEvent::get_qt_module_drop_event_handle () const { return __Info.get_handle (); }

#endif // DMZ_QT_MODULE_DROP_EVENT_DOT_H
