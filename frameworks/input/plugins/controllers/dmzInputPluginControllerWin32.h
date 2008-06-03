#ifndef DMZ_INPUT_PLUGIN_CONTROLLER_WIN32_DOT_H
#define DMZ_INPUT_PLUGIN_CONTROLLER_WIN32_DOT_H

#include <dmzInputEventController.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>

#include <windows.h>
#include <mmsystem.h> //must link with winmm.lib


namespace dmz {

   class InputModule;

   class InputPluginControllerWin32 : public Plugin, public Sync {

      public:
         //! \cond
         InputPluginControllerWin32 (
            const PluginInfo &Info,
            Config &local);

         virtual ~InputPluginControllerWin32 ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 DeltaTime);

      protected:
         struct DeviceStruct {
            
            const UInt32 Handle;
            const String Name;
            
            JOYCAPS joyCaps;
            JOYINFOEX joyInfo;
            
            DeviceStruct (const UInt32 TheHandle, const String &TheName);
            ~DeviceStruct ();
         };
         
         struct AxisStruct;
         struct ButtonStruct;

         struct ControllerStruct {

            const Handle SourceHandle;
            DeviceStruct &device;

            HashTableUInt32Template<AxisStruct> axisTable;
            HashTableUInt32Template<ButtonStruct> buttonTable;
            
            ControllerStruct (const Handle &TheSource, DeviceStruct &theDevice);
            ~ControllerStruct ();
            
            Boolean poll ();
         };

         struct AxisStruct {

            const UInt32 Handle;
            const UInt32 ElementHandle;

            ControllerStruct &cs;

            InputEventAxis event;

            Boolean hatswitchAsX;
            Boolean hatswitchAsY;
            
            Boolean flip;
            Float32 minValue;
            Float32 maxValue;

            AxisStruct (
               const UInt32 TheHandle,
               const UInt32 TheElementHandle,
               ControllerStruct &theController);

            ~AxisStruct ();
            
            Float32 get_value () const;
         };

         struct ButtonStruct {

            const UInt32 Handle;
            const UInt32 ElementHandle;

            ControllerStruct &cs;

            InputEventButton event;
            
            ButtonStruct (
               const UInt32 TheHandle,
               const UInt32 TheElementHandle,
               ControllerStruct &theController);

            ~ButtonStruct ();
            
            Boolean get_value () const;
         };

         void _init (Config &local);
         void _init_devices ();
         void _init_controller (Config &controller);
         void _init_axis (ControllerStruct &cs, Config &cd);
         void _init_hatswitch (ControllerStruct &cs, Config &cd);
         void _init_button (ControllerStruct &cs, Config &cd);
         void _sync_controller (ControllerStruct &cs);

         Log _log;
         InputModule *_channels;

         HashTableUInt32Template<DeviceStruct> _deviceHandleTable;
         HashTableStringTemplate<DeviceStruct> _deviceNameTable;

         HashTableUInt32Template<ControllerStruct> _controllerTable;
         //! \endcond

      private:
         InputPluginControllerWin32 (const InputPluginControllerWin32 &);
         InputPluginControllerWin32 &operator= (const InputPluginControllerWin32 &);
   };
};


//! \cond
inline
dmz::InputPluginControllerWin32::DeviceStruct::DeviceStruct (
         const UInt32 TheHandle,
         const String &TheName) :
         Handle (TheHandle),
         Name (TheName),
         joyCaps (),
         joyInfo () {
            
   memset (&joyInfo, 0x00, sizeof (JOYINFOEX));
   joyInfo.dwSize = sizeof (JOYINFOEX);
   joyInfo.dwFlags = JOY_RETURNALL;
   
   if (joyGetPosEx (Handle, &joyInfo) == JOYERR_NOERROR) {

      joyGetDevCaps (Handle, &joyCaps, sizeof (joyCaps));
   }
}


inline
dmz::InputPluginControllerWin32::DeviceStruct::~DeviceStruct () {;}


inline
dmz::InputPluginControllerWin32::ControllerStruct::ControllerStruct (
         const Handle &TheSource,
         DeviceStruct &theDevice) :
         SourceHandle (TheSource),
         device (theDevice),
         axisTable (),
         buttonTable () {;}


inline
dmz::InputPluginControllerWin32::ControllerStruct::~ControllerStruct () {

   axisTable.empty ();
   buttonTable.empty ();
}


inline
dmz::InputPluginControllerWin32::AxisStruct::AxisStruct (
         const UInt32 TheHandle,
         const UInt32 TheElementHandle,
         ControllerStruct &theController) :
         Handle (TheHandle),
         ElementHandle (TheElementHandle),
         cs (theController),
         event (theController.SourceHandle, TheHandle),
         hatswitchAsX (False),
         hatswitchAsY (False),
         flip (False),
         minValue (-1.0f),
         maxValue (1.0f) {;}


inline
dmz::InputPluginControllerWin32::AxisStruct::~AxisStruct () {;}


inline
dmz::InputPluginControllerWin32::ButtonStruct::ButtonStruct (
         const UInt32 TheHandle,
         const UInt32 TheElementHandle,
         ControllerStruct &theController) :
         Handle (TheHandle),
         ElementHandle (TheElementHandle),
         cs (theController),
         event (theController.SourceHandle, TheHandle) {;}


inline
dmz::InputPluginControllerWin32::ButtonStruct::~ButtonStruct () {;}
//! \endcond


#endif // DMZ_INPUT_PLUGIN_CONTROLLER_WIN32_DOT_H
