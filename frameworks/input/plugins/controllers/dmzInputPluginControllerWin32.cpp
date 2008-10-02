#include <dmzInputEventController.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include "dmzInputPluginControllerWin32.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesBase.h>


namespace {

static const dmz::UInt32 AXIS_X = 1;
static const dmz::UInt32 AXIS_Y = 2;
static const dmz::UInt32 AXIS_Z = 3;
static const dmz::UInt32 AXIS_R = 4;
static const dmz::UInt32 AXIS_U = 5;
static const dmz::UInt32 AXIS_V = 6;
static const dmz::UInt32 AXIS_POV_X = 7;
static const dmz::UInt32 AXIS_POV_Y = 8;

enum POV_Direction {
   POV_NORTH     = 0,
   POV_NORTHEAST = 4500,
   POV_EAST      = 9000,
   POV_SOUTHEAST = 13500,
   POV_SOUTH     = 18000,
   POV_SOUTHWEST = 22500,
   POV_WEST      = 27000,
   POV_NORTHWEST = 31500,
   POV_CENTERED  = 65535
};

};

/*!

\class dmz::InputPluginControllerWin32
\ingroup Input
\brief Windows controller driver Plugin.

*/


//! \cond
dmz::Boolean
dmz::InputPluginControllerWin32::ControllerStruct::poll () {

   Boolean retVal (False);

   if (joyGetPosEx (device.Handle, &(device.joyInfo)) == JOYERR_NOERROR) {

      retVal = True;
   }

   return retVal;
}


dmz::Float32
dmz::InputPluginControllerWin32::AxisStruct::get_value () const {

   Float32 retVal (0.0f);

   UInt32 axisMin (0);
   UInt32 axisMax (65535);
   UInt32 axisValue (axisMax / 2);
   Float32 scaledAxisValue (0.0f);

   JOYINFOEX &joyInfo (cs.device.joyInfo);
   JOYCAPS &joyCaps (cs.device.joyCaps);

   switch (ElementHandle) {

      case AXIS_X:
         axisValue = joyInfo.dwXpos;
         axisMin = joyCaps.wXmin;
         axisMax = joyCaps.wXmax;
         break;

      case AXIS_Y:
         axisValue = joyInfo.dwYpos;
         axisMin = joyCaps.wYmin;
         axisMax = joyCaps.wYmax;
         break;

      case AXIS_Z:
         axisValue = joyInfo.dwZpos;
         axisMin = joyCaps.wZmin;
         axisMax = joyCaps.wZmax;
         break;

      case AXIS_R:
         axisValue = joyInfo.dwRpos;
         axisMin = joyCaps.wRmin;
         axisMax = joyCaps.wRmax;
         break;

      case AXIS_U:
         axisValue = joyInfo.dwUpos;
         axisMin = joyCaps.wUmin;
         axisMax = joyCaps.wUmax;
         break;

      case AXIS_V:
         axisValue = joyInfo.dwVpos;
         axisMin = joyCaps.wVmin;
         axisMax = joyCaps.wVmax;
         break;

      case AXIS_POV_X:
         if ((joyInfo.dwPOV == POV_WEST) ||
             (joyInfo.dwPOV == POV_NORTHWEST) ||
             (joyInfo.dwPOV == POV_SOUTHWEST)) {

            axisValue = axisMin;
         }
         else if ((joyInfo.dwPOV == POV_EAST) ||
                (joyInfo.dwPOV == POV_NORTHEAST) ||
                (joyInfo.dwPOV == POV_SOUTHEAST)) {

            axisValue = axisMax;
         }
         else {

            axisValue = axisMax / 2;
         }
         break;

      case AXIS_POV_Y:
         if ((joyInfo.dwPOV == POV_SOUTH) ||
             (joyInfo.dwPOV == POV_SOUTHWEST) ||
             (joyInfo.dwPOV == POV_SOUTHEAST)) {

            axisValue = axisMin;
         }
         else if ((joyInfo.dwPOV == POV_NORTH) ||
                  (joyInfo.dwPOV == POV_NORTHWEST) ||
                  (joyInfo.dwPOV == POV_NORTHEAST)) {

            axisValue = axisMax;
         }
         else {

            axisValue = axisMax / 2;
         }
         break;

      default:
         axisMin = 0;
         axisMax = 65535;
         axisValue = axisMax / 2;
         break;
   };

   // Convert the raw value (0-65535) to a value between -1 and 1
   scaledAxisValue =
      ((Float32 (axisValue - axisMin) / Float32 (axisMax - axisMin)) * 2) - 1;

   // Convert scaled value to a value between min and max
   retVal = (scaledAxisValue * (maxValue - minValue) + (maxValue + minValue)) * 0.5f;

   if (flip) { retVal *= -1; }

   return retVal;
}


dmz::Boolean
dmz::InputPluginControllerWin32::ButtonStruct::get_value () const {

   Boolean retVal (False);

   if (cs.device.joyInfo.dwButtons & (1<<(ElementHandle-1))) {

      retVal = True;
   }

   return retVal;
}


dmz::InputPluginControllerWin32::InputPluginControllerWin32 (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      _log (Info),
      _channels (0) {

   _init (local);
}


dmz::InputPluginControllerWin32::~InputPluginControllerWin32 () {

   _controllerTable.empty ();

   _deviceHandleTable.empty ();
   _deviceNameTable.clear ();
}


// Plugin Interface
void
dmz::InputPluginControllerWin32::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_channels) { _channels = InputModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_channels && (_channels == InputModule::cast (PluginPtr))) {

         _channels = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::InputPluginControllerWin32::update_time_slice (const Float64 DeltaTime) {

   if (_channels) {

      HashTableUInt32Iterator it;

      ControllerStruct *controller = _controllerTable.get_first (it);

      while (controller) {

         _time_slice_controller (*controller);

         controller = _controllerTable.get_next (it);
      }
   }
}


void
dmz::InputPluginControllerWin32::_init (Config &local) {

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
dmz::InputPluginControllerWin32::_init_devices () {

   _log.info << "Discovering devices..." << endl;

   JOYCAPS joyCaps;
   JOYINFOEX joyInfo;

   joyInfo.dwSize = sizeof (JOYINFOEX);
   joyInfo.dwFlags = JOY_RETURNALL;

   for (UInt32 ix = 0; ix < joyGetNumDevs (); ix++) {

      if (joyGetPosEx (ix, &joyInfo) == JOYERR_NOERROR) {

         joyGetDevCaps (ix, &joyCaps, sizeof (joyCaps));

         DeviceStruct *ds = new DeviceStruct (ix, joyCaps.szPname);

         if (!_deviceHandleTable.store (ds->Handle, ds) ||
             !_deviceNameTable.store (ds->Name, ds)) {

            _deviceHandleTable.remove (ds->Handle);
            _deviceNameTable.remove (ds->Name);

            delete ds; ds = 0;
         }
         else {

            _log.info <<  "Added device[" << ds->Handle << "]: " << ds->Name << endl;
         }
      }
   }
}


void
dmz::InputPluginControllerWin32::_init_controller (Config &controller) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   const String ControllerName (
      config_to_string ("name", controller, "controller_default"));

   const Handle SourceHandle (defs.create_named_handle (ControllerName));
   const String DeviceName (config_to_string ("device", controller));
   const UInt32 DeviceHandle (config_to_uint32 ("deviceId", controller, 0));

   DeviceStruct *ds = _deviceNameTable.lookup (DeviceName);

   if (!ds) { ds = _deviceHandleTable.lookup (DeviceHandle); }

   if (ds) {

      _log.info  << "Mapping controller[" << ControllerName
                 << "] to device[" << ds->Handle << "] " << ds->Name << endl;

      ControllerStruct *cs (_controllerTable.lookup (SourceHandle));

      if (!cs) {

         cs = new ControllerStruct (SourceHandle, *ds);

         if (!_controllerTable.store (SourceHandle, cs)) {

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
dmz::InputPluginControllerWin32::_init_axis (ControllerStruct &cs, Config &cd) {

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
            axis->minValue = config_to_float32 ("min", cd, -1.0f);
            axis->maxValue = config_to_float32 ("max", cd, 1.0f);

            if (axis->minValue < -1.0f) { axis->minValue = -1.0f; }
            else if (axis->minValue > 1.0f) { axis->minValue = 1.0f; }

            if (axis->maxValue > 1.0f) { axis->maxValue = 1.0f; }
            else if (axis->maxValue < -1.0f) { axis->maxValue = -1.0f; }
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
dmz::InputPluginControllerWin32::_init_hatswitch (ControllerStruct &cs, Config &cd) {

   const UInt32 AxisXHandle (config_to_uint32 ("xAxisId", cd, 0));
   const UInt32 AxisYHandle (config_to_uint32 ("yAxisId", cd, 0));

   if (AxisXHandle && AxisYHandle) {

      AxisStruct *xAxis (cs.axisTable.lookup (AxisXHandle));
      AxisStruct *yAxis (cs.axisTable.lookup (AxisYHandle));

      if (!xAxis && !yAxis) {

         xAxis = new AxisStruct (AxisXHandle, AXIS_POV_X, cs);

         if (!cs.axisTable.store (xAxis->Handle, xAxis)) {

            delete xAxis; xAxis = 0;
         }

         yAxis = new AxisStruct (AxisYHandle, AXIS_POV_Y, cs);

         if (!cs.axisTable.store (yAxis->Handle, yAxis)) {

            delete yAxis; yAxis = 0;
         }

         if (xAxis && yAxis) {

            xAxis->flip = config_to_boolean ("flip", cd, False);
            xAxis->minValue = config_to_float32 ("min", cd, -1.0f);
            xAxis->maxValue = config_to_float32 ("max", cd, 1.0f);

            if (xAxis->minValue < -1.0f) { xAxis->minValue = -1.0f; }
            else if (xAxis->minValue > 1.0f) { xAxis->minValue = 1.0f; }

            if (xAxis->maxValue > 1.0f) { xAxis->maxValue = 1.0f; }
            else if (xAxis->maxValue < -1.0f) { xAxis->maxValue = -1.0f; }

            yAxis->flip = xAxis->flip;
            yAxis->minValue = xAxis->minValue;
            yAxis->maxValue = xAxis->maxValue;
         }
      }
      else {

         _log.error << "Failed to map hatswitch " << AxisXHandle << " " << AxisYHandle
            << endl;
      }
   }
}


void
dmz::InputPluginControllerWin32::_init_button (ControllerStruct &cs, Config &cd) {

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

         _log.error << "Failed to map button " << ButtonHandle << endl;

         _log.error
            << "element: " << ElementHandle
            << " on device: " << cs.device.Handle
            << " is already mapped"
            << endl;
      }
   }
}


void
dmz::InputPluginControllerWin32::_time_slice_controller (ControllerStruct &cs) {

   if (_channels) {

      if (cs.poll ()) {

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
      else {

         _log.warn << "Error polling controller: " << cs.device.Name << endl;
         _log.warn << "Controller might be unplugged" << endl;
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginControllerWin32 (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginControllerWin32 (Info, local);
}

};

