#ifndef DMZ_RENDER_PLUGIN_EVENT_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_EVENT_OSG_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

#include <osg/AutoTransform>
#include <osg/Image>
#include <osg/MatrixTransform>
#include <osg/ref_ptr>

//#define DMZ_USE_BILLBOARD

namespace dmz {

   class EventType;
   class RenderModuleCoreOSG;

   class RenderPluginEventOSG :
         public Plugin,
         public TimeSlice,
         public EventObserverUtil {

      public:
         RenderPluginEventOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginEventOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Event Observer Interface
         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

      protected:
         struct TypeStruct {

            const Float64 Offset;
            const Float64 Scale;
            const Float64 Rate;
            osg::ref_ptr<osg::Image> image;

            TypeStruct (
                  const Float64 TheOffset,
                  const Float64 TheScale,
                  const Float64 TheRate) :
                  Offset (TheOffset),
                  Scale (TheScale),
                  Rate (TheRate) {;}
         };

         struct TypeTable {

            const Handle TypeAttr;
            HashTableHandleTemplate<TypeStruct> map;
            HashTableHandleTemplate<TypeStruct> table;

            TypeTable (const Handle TheTypeAttr) : TypeAttr (TheTypeAttr) {;}
            ~TypeTable () { map.clear (); table.empty (); }
         };

         struct EventStruct {

            const TypeStruct &Type;

            Float64 scalar;
#ifdef DMZ_USE_BILLBOARD
            osg::ref_ptr<osg::MatrixTransform> root;
#else
            osg::ref_ptr<osg::AutoTransform> root;
#endif
            osg::ref_ptr<osg::MatrixTransform> scale;

            EventStruct (const TypeStruct &TheType) : Type (TheType), scalar (1.0) {;}
         };

         TypeStruct *_get_type (const Handle EventHandle, const EventType &Type);
         TypeTable *_get_type_table (const EventType &Type);

         TypeStruct *_create_type (
            const Handle EventHandle,
            const EventType &Event,
            const ObjectType &Object,
            TypeTable &table);

         void _create_event (const Handle EventHandle, TypeStruct &ts);
         void _init (Config &local);

         Log _log;
         Definitions _defs;
         Resources _rc;

         UInt32 _isectMask;

         RenderModuleCoreOSG *_core;

         Handle _defaultHandle;

         HandleContainer _ignore;

         HashTableHandleTemplate<TypeTable> _typeTable;

         HashTableHandleTemplate<EventStruct> _eventTable;

      private:
         RenderPluginEventOSG ();
         RenderPluginEventOSG (const RenderPluginEventOSG &);
         RenderPluginEventOSG &operator= (const RenderPluginEventOSG &);
   };
};

#endif // DMZ_RENDER_PLUGIN_EVENT_OSG_DOT_H
