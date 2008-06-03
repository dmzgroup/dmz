#include <dmzInputEventController.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include "dmzInputPluginControllerMacOS.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesBase.h>
#include <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>

/*!

\class dmz::InputPluginControllerMacOS
\ingroup Input
\brief Mac OS controller driver Plugin.

*/

namespace {

   const UInt32 AXIS_UNSUPPORTED = 0;
   const UInt32 AXIS_X = 1;
   const UInt32 AXIS_Y = 2;
   const UInt32 AXIS_Z = 3;
   const UInt32 AXIS_RX = 4;
   const UInt32 AXIS_RY = 5;
   const UInt32 AXIS_RZ = 6;
   const UInt32 AXIS_SLIDER = 7;
   const UInt32 AXIS_HATSWITCH = 8;
};

//! \cond
dmz::Float32
dmz::InputPluginControllerMacOS::AxisStruct::get_value () const {

   Float32 retVal (0.0f);

   pRecDevice device = cs.device.device;
   pRecElement element = cs.device.axisTable.lookup (ElementHandle);

   if (HIDIsValidElement (device, element)) {
      
      UInt32 value = HIDGetElementValue (device, element);
      
      Float32 scaledValue (0.0f);

      if (element->usage == kHIDUsage_GD_Hatswitch) {

         if (hatswitchAsX) {
            
            if ((value == cs.hatswitch.west) ||
                (value == cs.hatswitch.northwest) ||
                (value == cs.hatswitch.southwest)) {
            
               scaledValue = -1.0f;
            }

            if ((value == cs.hatswitch.east) ||
                (value == cs.hatswitch.northeast) ||
                (value == cs.hatswitch.southeast)) {
            
               scaledValue = 1.0f;
            }
         }
         else if (hatswitchAsY) {

            if ((value == cs.hatswitch.south) ||
                (value == cs.hatswitch.southwest) ||
                (value == cs.hatswitch.southeast)) {
            
               scaledValue = -1.0f;
            }

            if ((value == cs.hatswitch.north) ||
                (value == cs.hatswitch.northwest) ||
                (value == cs.hatswitch.northeast)) {
            
               scaledValue = 1.0f;
            }
         }
      }
      else {
         
         // Convert the raw value (0-255) to a value between -1 and 1
         scaledValue = 
            ((Float32 (value - element->min) /
              Float32 (element->max - element->min) * 2) - 1);
      }
      
      // Convert scaled value to a value between min and max
      retVal = (scaledValue * (max - min) + (max + min)) * 0.5f; 
   
      if (flip) { retVal *= -1; }
   }
   
   return retVal;
}


#if 0
dmz::String
dmz::InputPluginControllerMacOS::AxisStruct::get_name () const {

   String retVal;

   pRecDevice device = cs.device.device;
   pRecElement element = cs.device.axisTable.lookup (ElementHandle);
   
   if (HIDIsValidElement (device, element)) {
      
      char name[256];
      HIDGetUsageName (element->usagePage, element->usage, name);
      retVal = name;
   }
   
   return retVal;
}
#endif


dmz::Boolean
dmz::InputPluginControllerMacOS::ButtonStruct::get_value () const {

   Boolean retVal (False);

   pRecDevice device = cs.device.device;
   pRecElement element = cs.device.buttonTable.lookup (ElementHandle);

   if (HIDIsValidElement (device, element)) {

      UInt32 value = HIDGetElementValue (device, element);
       
      retVal = value ? True : False;
   }
   
   return retVal;
}


dmz::InputPluginControllerMacOS::InputPluginControllerMacOS (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      Sync (Info),
      _log (Info),
      _channels (0) {

   _init (local);
}


dmz::InputPluginControllerMacOS::~InputPluginControllerMacOS () {

   _controllerTable.clear ();
   
   _deviceHandleTable.empty ();
   _deviceNameTable.clear ();

   HIDReleaseDeviceList ();
}


void
dmz::InputPluginControllerMacOS::discover_plugin (const Plugin *PluginPtr) {

   if (!_channels) { _channels = InputModule::cast (PluginPtr); }
}


void
dmz::InputPluginControllerMacOS::start_plugin () {

}


void
dmz::InputPluginControllerMacOS::update_sync (const Float64 DeltaTime) {

   if (_channels) {

      HashTableHandleIterator it;

      ControllerStruct *controller = _controllerTable.get_first (it);

      while (controller) {

         _sync_controller (*controller);

         controller = _controllerTable.get_next (it);
      }
   }
}


void
dmz::InputPluginControllerMacOS::stop_plugin () {

}


void
dmz::InputPluginControllerMacOS::shutdown_plugin () {

}


void
dmz::InputPluginControllerMacOS::remove_plugin (const Plugin *PluginPtr) {

   if (_channels && (_channels == InputModule::cast (PluginPtr))) {
   
      _channels = 0;
   }
}


void
dmz::InputPluginControllerMacOS::_init (Config &local) {

   _init_devices ();
   
   Config controllerList;

   if (local.lookup_all_config ("controller", controllerList)) {

      ConfigIterator it;

      Config controller;

      Boolean found (controllerList.get_first_config (it, controller));

      while (found) {

         _init_controller (controller);

         found = controllerList.get_next_config (it, controller);
      }
   }
}


void
dmz::InputPluginControllerMacOS::_init_devices () {

   _log.info << "Discovering devices..." << endl;

   HIDBuildDeviceList (NULL, NULL);
   
   if (HIDHaveDeviceList ()) {

      pRecDevice deviceCandidate = HIDGetFirstDevice ();

      Int32 num_devices = HIDCountDevices ();

      Int32 ix (0);
      
      while (ix < num_devices) {
         
         if (deviceCandidate) {
            
            // filter out keyboards, mice and totally unrelated devices
            if ((deviceCandidate->usage == kHIDUsage_GD_Keyboard) ||
                (deviceCandidate->usagePage != kHIDPage_GenericDesktop) ||
                (deviceCandidate->usage == kHIDUsage_GD_Mouse)) {
                    
               if (HIDIsValidDevice (deviceCandidate)) {
                  //skip this one and just go on to the next one
                  deviceCandidate = HIDGetNextDevice (deviceCandidate);
               }
               else  {
                  
                  //handle error
               }
            }
            else {

               if (HIDIsValidDevice (deviceCandidate)) {
                  
                  _add_device (ix, deviceCandidate);
                  deviceCandidate = HIDGetNextDevice (deviceCandidate);
               }
               else {
                  
                  //handle error
               }
            }
         }
         
         ix++;
      }
   }
}


void
dmz::InputPluginControllerMacOS::_add_device (const UInt32 DeviceId, pRecDevice device) {

   if (HIDIsValidDevice (device)) {
      
      const String DeviceName (device->product);

      DeviceStruct *ds = new DeviceStruct (DeviceId, DeviceName);

      ds->device = device;
      
      if (!_deviceHandleTable.store (ds->Handle, ds) ||
          !_deviceNameTable.store (ds->Name, ds)) {
             
         _deviceHandleTable.remove (ds->Handle);
         _deviceNameTable.remove (ds->Name);
 
         delete ds; ds = 0;
      }
      else {
         
         _log.info <<  "Added device[" << DeviceId << "]: " << DeviceName << endl;

         UInt32 axisId = AXIS_UNSUPPORTED;
         UInt32 buttonId = 1;

         pRecElement elem =  HIDGetFirstDeviceElement (device, kHIDElementTypeInput);

         while (elem) {

            if (elem->usagePage == kHIDPage_GenericDesktop) {

               switch (elem->usage) {

                  case kHIDUsage_GD_X: axisId = AXIS_X; break;
                  case kHIDUsage_GD_Y: axisId = AXIS_Y; break;
                  case kHIDUsage_GD_Z: axisId = AXIS_Z; break;
                  case kHIDUsage_GD_Rx: axisId = AXIS_RX; break;
                  case kHIDUsage_GD_Ry: axisId = AXIS_RY; break;
                  case kHIDUsage_GD_Rz: axisId = AXIS_RZ; break;
                  case kHIDUsage_GD_Slider: axisId = AXIS_SLIDER; break;
                  case kHIDUsage_GD_Hatswitch: axisId = AXIS_HATSWITCH; break;
                  case kHIDUsage_GD_Dial: axisId = AXIS_UNSUPPORTED; break;
                  case kHIDUsage_GD_Wheel: axisId = AXIS_UNSUPPORTED; break;
                  default: break;
               }

               if (axisId != AXIS_UNSUPPORTED) {

                  ds->axisTable.store (axisId, elem);
               }
            }
            else if (elem->usagePage == kHIDPage_Button) {

               ds->buttonTable.store (buttonId, elem);
               buttonId++;
            }

            elem = HIDGetNextDeviceElement (elem, kHIDElementTypeInput);
         }
      }
   }
}


void
dmz::InputPluginControllerMacOS::_init_controller (Config &controller) {

   Definitions defs (get_plugin_runtime_context (), &_log);
   
   const String ControllerName (
      config_to_string ("name", controller, "controller_default"));

   const String DeviceName (config_to_string ("device", controller));
   const UInt32 DeviceHandle (config_to_uint32 ("deviceId", controller, 0));
   
   DeviceStruct *ds = _deviceNameTable.lookup (DeviceName);

   if (!ds) { ds = _deviceHandleTable.lookup (DeviceHandle); }

   if (ds) {

      _log.info  << "Mapping controller[" << ControllerName
                 << "] to device[" << ds->Handle << "] " << ds->Name << endl;

      const Handle ControllerHandle (defs.create_named_handle (ControllerName));
   
      ControllerStruct *cs (_controllerTable.lookup (ControllerHandle));
   
      if (!cs) {
   
         cs = new ControllerStruct (ControllerHandle, *ds);
   
         if (!_controllerTable.store (ControllerHandle, cs)) {
   
            delete cs; cs = 0;
         }
      }
   
      if (cs) {
         
         ConfigIterator elementIt;
   
         Config cd;
   
         Boolean elementFound (controller.get_first_config (elementIt, cd));
   
         while (elementFound) {
   
            const String Type (cd.get_name ().get_lower ());
   
            if (Type == "axis") {
   
               _init_axis (*cs, cd);
            }
            else if (Type == "hatswitch") {
   
               _init_hatswitch (*cs, cd);
            }
            else if (Type == "button") {
   
               _init_button (*cs, cd);
            }
            else {
   
            }
   
            elementFound = controller.get_next_config (elementIt, cd);
         }
      }
   }
   else {

      _log.warn  << "Failed to map controller[" << ControllerName
                    << "] to device[" << DeviceHandle << "] " << DeviceName << endl;
   }
}


void
dmz::InputPluginControllerMacOS::_init_axis (ControllerStruct &cs, Config &cd) {

   const UInt32 ElementHandle (config_to_uint32 ("elementId", cd, 1));
   const UInt32 AxisHandle (config_to_uint32 ("id", cd, 0));

   if (ElementHandle && AxisHandle) {

      AxisStruct *axis (cs.axisTable.lookup (AxisHandle));

      if (!axis) {

         axis = new AxisStruct (AxisHandle, ElementHandle, cs);

         if (!cs.axisTable.store (axis->Handle, axis)) {

            delete axis; axis = 0;
         }

         if (axis) {

            axis->flip = config_to_boolean ("flip", cd, False);
            axis->min = config_to_float32 ("min", cd, -1.0f);
            axis->max = config_to_float32 ("max", cd, 1.0f);

            if (axis->min < -1.0f) { axis->min = -1.0f; }
            else if (axis->min > 1.0f) { axis->min = 1.0f; }
            
            if (axis->max > 1.0f) { axis->max = 1.0f; }
            else if (axis->max < -1.0f) { axis->max = -1.0f; }
         }
      }
      else {

         _log.error << "Failed to map axis " << AxisHandle << endl;

         _log.error
            << "element: " << ElementHandle
            << " on device: " << cs.device.Handle
            << " is already mapped"
            << endl;
      }
   }
}


void
dmz::InputPluginControllerMacOS::_init_hatswitch (ControllerStruct &cs, Config &cd) {

   const UInt32 ElementHandle (config_to_uint32 ("elementId", cd, 1));
   const UInt32 AxisXHandle (config_to_uint32 ("xAxisId", cd, 0));
   const UInt32 AxisYHandle (config_to_uint32 ("yAxisId", cd, 0));

   if (ElementHandle && AxisXHandle && AxisYHandle) {

      AxisStruct *xAxis (cs.axisTable.lookup (AxisXHandle));
      AxisStruct *yAxis (cs.axisTable.lookup (AxisYHandle));

      if (!xAxis && !yAxis) {

         xAxis = new AxisStruct (AxisXHandle, ElementHandle, cs);
         xAxis->hatswitchAsX = True;

         if (!cs.axisTable.store (xAxis->Handle, xAxis)) {

            delete xAxis; xAxis = 0;
         }

         yAxis = new AxisStruct (AxisYHandle, ElementHandle, cs);
         yAxis->hatswitchAsY = True;

         if (!cs.axisTable.store (yAxis->Handle, yAxis)) {

            delete yAxis; yAxis = 0;
         }

         if (xAxis && yAxis) {

            xAxis->flip = config_to_boolean ("flip", cd, False);
            xAxis->min = config_to_float32 ("min", cd, -1.0f);
            xAxis->max = config_to_float32 ("max", cd, 1.0f);

            if (xAxis->min < -1.0f) { xAxis->min = -1.0f; }
            else if (xAxis->min > 1.0f) { xAxis->min = 1.0f; }
            
            if (xAxis->max > 1.0f) { xAxis->max = 1.0f; }
            else if (xAxis->max < -1.0f) { xAxis->max = -1.0f; }
            
            yAxis->flip = xAxis->flip;
            yAxis->min = xAxis->min;
            yAxis->max = xAxis->max;
         }
      }
      else {

       _log.warn << "Failed to map hatswitch " << AxisXHandle << " " << AxisYHandle<< endl;

       _log.error
          << "element: " << ElementHandle
          << " on device: " << cs.device.Handle
          << " is already mapped"
          << endl;
      }
   }
   else {

      cs.hatswitch.north     = config_to_uint32 ("north",     cd, 0);
      cs.hatswitch.northeast = config_to_uint32 ("northeast", cd, 1);
      cs.hatswitch.east      = config_to_uint32 ("east",      cd, 2);
      cs.hatswitch.southeast = config_to_uint32 ("southeast", cd, 3);
      cs.hatswitch.south     = config_to_uint32 ("south",     cd, 4);
      cs.hatswitch.southwest = config_to_uint32 ("southwest", cd, 5);
      cs.hatswitch.west      = config_to_uint32 ("west",      cd, 6);
      cs.hatswitch.northwest = config_to_uint32 ("northwest", cd, 7);
      cs.hatswitch.centered  = config_to_uint32 ("centered",  cd, 8);
   }
}


void
dmz::InputPluginControllerMacOS::_init_button (ControllerStruct &cs, Config &cd) {

   const UInt32 ElementHandle (config_to_uint32 ("elementId", cd, 1));
   const UInt32 ButtonHandle (config_to_uint32 ("id", cd, ElementHandle));

   if (ElementHandle) {

      ButtonStruct *button (cs.buttonTable.lookup (ButtonHandle));

      if (!button) {

         button = new ButtonStruct (ButtonHandle, ElementHandle, cs);

         if (!cs.buttonTable.store (button->Handle, button)) {

            delete button; button = 0;
         }
      }
      else {

         _log.warn << "Failed to map button " << ButtonHandle << endl;

         _log.warn
            << "element: " << ElementHandle
            << " on device: " << cs.device.Handle
            << " is already mapped"
            << endl;
      }
   }
}


void
dmz::InputPluginControllerMacOS::_sync_controller (ControllerStruct &cs) {

   if (_channels) {

      HashTableUInt32Iterator it;

      AxisStruct *axis = cs.axisTable.get_first (it);

      while (axis) {

         if (axis->event.update_axis_value (axis->get_value ())) {

            _channels->send_axis_event (axis->event);
         }

         axis = cs.axisTable.get_next (it);
      }

      ButtonStruct *button = cs.buttonTable.get_first (it);

      while (button) {

         if (button->event.update_button_value (button->get_value ())) {
               
            _channels->send_button_event (button->event);
         }

         button = cs.buttonTable.get_next (it);
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginControllerMacOS (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginControllerMacOS (Info, local);
}

};

