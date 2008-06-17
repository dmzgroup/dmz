#ifndef DMZ_INPUT_PLUGIN_CONTROLLER_MACOS_DOT_H
#define DMZ_INPUT_PLUGIN_CONTROLLER_MACOS_DOT_H

#include <dmzInputEventController.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>
#include "macos/HID_Utilities_External.h"


namespace dmz {

   class InputModule;

   class InputPluginControllerMacOS : public Plugin, public TimeSlice {

      public:
         //! \cond
         InputPluginControllerMacOS (
            const PluginInfo &Info,
            Config &local);

         virtual ~InputPluginControllerMacOS ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 DeltaTime);

      protected:
         struct DeviceStruct {
            
            const UInt32 Handle;
            const String Name;
            
            pRecDevice device;
            HashTableUInt32Template<recElement> axisTable;
            HashTableUInt32Template<recElement> buttonTable;
            
            DeviceStruct (const UInt32 TheHandle, const String &TheName);
            ~DeviceStruct ();
         };
         
         struct DirectionStruct {

            UInt32 northwest, north,    northeast;
            UInt32 west,      centered, east;
            UInt32 southwest, south,    southeast;
         };

         struct AxisStruct;
         struct ButtonStruct;

         struct ControllerStruct {

            const Handle SourceHandle;
            DeviceStruct &device;

            HashTableUInt32Template<AxisStruct> axisTable;
            HashTableUInt32Template<ButtonStruct> buttonTable;
            
            DirectionStruct hatswitch;

            ControllerStruct (const Handle &TheSource, DeviceStruct &theDevice);
            ~ControllerStruct ();
         };

         struct AxisStruct {

            const UInt32 Handle;
            const UInt32 ElementHandle;

            ControllerStruct &cs;

            InputEventAxis event;

            Boolean hatswitchAsX;
            Boolean hatswitchAsY;
            
            Boolean flip;
            Float32 min;
            Float32 max;

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
         void _add_device (const UInt32 DeviceId, pRecDevice device);       
         void _init_controller (Config &controller);
         void _init_axis (ControllerStruct &cs, Config &cd);       
         void _init_hatswitch (ControllerStruct &cs, Config &cd);
         void _init_button (ControllerStruct &cs, Config &cd);
         void _time_slice_controller (ControllerStruct &cs);

         Log _log;
         InputModule *_channels;

         HashTableUInt32Template<DeviceStruct> _deviceHandleTable;
         HashTableStringTemplate<DeviceStruct> _deviceNameTable;

         HashTableHandleTemplate<ControllerStruct> _controllerTable;
         //! \endcond

      private:
         InputPluginControllerMacOS (const InputPluginControllerMacOS &);
         InputPluginControllerMacOS &operator= (const InputPluginControllerMacOS &);
   };
};


//! \cond
inline
dmz::InputPluginControllerMacOS::DeviceStruct::DeviceStruct (
         const UInt32 TheHandle,
         const String &TheName) :
         Handle (TheHandle),
         Name (TheName),
         device (0),
         axisTable (),
         buttonTable () {;}


inline
dmz::InputPluginControllerMacOS::DeviceStruct::~DeviceStruct () {
   
   device = 0;
   axisTable.clear ();
   buttonTable.clear ();
}


inline
dmz::InputPluginControllerMacOS::ControllerStruct::ControllerStruct (
         const Handle &TheSource,
         DeviceStruct &theDevice) :
         SourceHandle (TheSource),
         device (theDevice),
         axisTable (),
         buttonTable () {

   hatswitch.north = 0;
   hatswitch.northeast = 1;
   hatswitch.east = 2;
   hatswitch.southeast = 3;
   hatswitch.south = 4;
   hatswitch.southwest = 5;
   hatswitch.west = 6;
   hatswitch.northwest = 7;
   hatswitch.centered = 8;
}


inline
dmz::InputPluginControllerMacOS::ControllerStruct::~ControllerStruct () {

   axisTable.empty ();
   buttonTable.empty ();
}


inline
dmz::InputPluginControllerMacOS::AxisStruct::AxisStruct (
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
         min (-1.0f),
         max (1.0f) {;}


inline
dmz::InputPluginControllerMacOS::AxisStruct::~AxisStruct () {;}


inline
dmz::InputPluginControllerMacOS::ButtonStruct::ButtonStruct (
         const UInt32 TheHandle,
         const UInt32 TheElementHandle,
         ControllerStruct &theController) :
         Handle (TheHandle),
         ElementHandle (TheElementHandle),
         cs (theController),
         event (theController.SourceHandle, TheHandle) {;}


inline
dmz::InputPluginControllerMacOS::ButtonStruct::~ButtonStruct () {;}
//! \endcond


#endif // DMZ_INPUT_PLUGIN_CONTROLLER_MACOS_DOT_H
