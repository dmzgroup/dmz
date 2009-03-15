#ifndef DMZ_RENDER_PLUGIN_ARTICULATE_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_ARTICULATE_OSG_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class RenderPluginArticulateOSG :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         struct ScalarStruct {

            ScalarStruct *next;
            ScalarStruct () : next (0) {;}
            virtual ~ScalarStruct () { if (next) { delete next; next = 0; } }
            virtual void update_scalar (
               const Float64 Value,
               const Float64 *PreviousValue) = 0;
         };

         RenderPluginArticulateOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginArticulateOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_locality (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality);

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

         virtual void update_object_velocity (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

      protected:
         struct AttrStruct {

            ScalarStruct *scalarList;

            AttrStruct () : scalarList (0) {;}
            ~AttrStruct () { if (scalarList) { delete scalarList; scalarList = 0; } }
         };

         struct ObjectStruct {

            HashTableHandleTemplate<AttrStruct> attr;
            ~ObjectStruct () { attr.empty (); }
         };

         void _init (Config &local);

         Log _log;

         HashTableHandleTemplate<ObjectStruct> _objTable;

      private:
         RenderPluginArticulateOSG ();
         RenderPluginArticulateOSG (const RenderPluginArticulateOSG &);
         RenderPluginArticulateOSG &operator= (const RenderPluginArticulateOSG &);
   };
};

#endif // DMZ_RENDER_PLUGIN_ARTICULATE_OSG_DOT_H
