#ifndef DMZ_RENDER_PLUGIN_ARTICULATE_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_ARTICULATE_OSG_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesMask.h>
#include <dmzTypesVector.h>

#include <osg/NodeVisitor>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginArticulateOSG :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public osg::NodeVisitor {

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

         // osg::NodeVisitor Interface
         virtual void apply (osg::Node &node);
         virtual void apply (osg::Transform &transform);

      protected:
         struct ScalarDefStruct {

            const VectorComponentEnum Axis;
            const Handle AttrHandle;

            ScalarDefStruct (
                  const VectorComponentEnum &TheAxis,
                  const Handle TheAttrHandle) :
                  Axis (TheAxis),
                  AttrHandle (TheAttrHandle) {;}
         };

         struct ResourceStruct {

            HashTableStringTemplate<ScalarDefStruct> scalarMap;

            ResourceStruct () {;}

            ~ResourceStruct () { scalarMap.empty (); }
         };

         struct ResourceStackStruct {

            ResourceStackStruct *next;
            ResourceStruct &rc;

            ResourceStackStruct (ResourceStruct &theRc) : next (0), rc (theRc) {;}
         };

         struct AttrStruct {

            ScalarStruct *scalarList;

            AttrStruct () : scalarList (0) {;}
            ~AttrStruct () { if (scalarList) { delete scalarList; scalarList = 0; } }
         };

         struct ObjectStruct {

            HashTableHandleTemplate<AttrStruct> attr;
            ~ObjectStruct () { attr.empty (); }

            Boolean add_scalar (const Handle Attr, ScalarStruct *ss) {

               Boolean result (False);

               AttrStruct *as = attr.lookup (Attr);

               if (!as) {

                  as = new AttrStruct;

                  if (as && !attr.store (Attr, as)) { delete as; as = 0; }
               }

               if (as) {

                  ss->next = as->scalarList;
                  as->scalarList = ss;
                  result = True;
               }

               return result;
            }
         };

         ResourceStruct *_create_rc (const String &Name);
         Boolean _push_rc (const String Name);
         void _pop_rc ();

         void _init (Config &local);

         Log _log;
         Resources _rc;

         RenderModuleCoreOSG *_core;

         ObjectStruct *_currentObj;
         ResourceStackStruct *_rcStack;

         HashTableHandleTemplate<Mask> _regTable;
         HashTableHandleTemplate<ObjectStruct> _objTable;
         HashTableStringTemplate<ResourceStruct> _rcTable;
         HashTableStringTemplate<ResourceStruct> _rcMasterTable;

      private:
         RenderPluginArticulateOSG ();
         RenderPluginArticulateOSG (const RenderPluginArticulateOSG &);
         RenderPluginArticulateOSG &operator= (const RenderPluginArticulateOSG &);
   };
};

#endif // DMZ_RENDER_PLUGIN_ARTICULATE_OSG_DOT_H
