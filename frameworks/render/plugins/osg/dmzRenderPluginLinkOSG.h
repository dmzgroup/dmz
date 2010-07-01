#ifndef DMZ_RENDER_PLUGIN_LINK_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_LINK_OSG_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Node>
#include <osg/Vec4>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginLinkOSG :
         public Plugin,
         public TimeSlice,
         public MessageObserver,
         public ObjectObserverUtil {

      public:
         RenderPluginLinkOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginLinkOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void link_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         virtual void unlink_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         virtual void update_link_attribute_object (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle,
            const UUID &AttributeIdentity,
            const Handle AttributeObjectHandle,
            const UUID &PrevAttributeIdentity,
            const Handle PrevAttributeObjectHandle);

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

         virtual void update_object_vector (
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
         struct LinkDefStruct {

            const Handle AttrHandle;
            const Handle ScaleAttrHandle;
            const osg::Vec4 Color;
            const Float64 Radius;
            const Int32 Sides;
            const Boolean Glyph;
            const Boolean Capped;
            osg::ref_ptr<osg::Geode> model;

            LinkDefStruct (
                  const Handle TheAttrHandle,
                  const Handle TheScaleAttrHandle,
                  const osg::Vec4 &TheColor,
                  const Float64 TheRadius,
                  const Int32 TheSides,
                  const Boolean IsGlyph,
                  const Boolean IsCapped) :
                  AttrHandle (TheAttrHandle),
                  ScaleAttrHandle (TheScaleAttrHandle),
                  Color (TheColor),
                  Radius (TheRadius),
                  Sides (TheSides >= 3 ? TheSides : 3),
                  Glyph (IsGlyph),
                  Capped (IsCapped) {;}
         };

         struct ObjectStruct;

         struct LinkStruct {

            const Handle Link;
            const LinkDefStruct &Def;
            ObjectStruct &super;
            ObjectStruct &sub;
            Boolean hide;
            Float64 scale;
            osg::ref_ptr<osg::MatrixTransform> root;

            LinkStruct (
                  const Handle TheLink,
                  const LinkDefStruct &TheDef,
                  ObjectStruct &theSuper,
                  ObjectStruct &theSub) :
                  Link (TheLink),
                  Def (TheDef),
                  super (theSuper),
                  sub (theSub),
                  hide (False),
                  scale (1.0) {

               super.superTable.store (Link, this);
               sub.subTable.store (Link, this);
            }

            ~LinkStruct () {

               super.superTable.remove (Link);
               sub.subTable.remove (Link);
            }
         };

         struct ObjectStruct {

            const Handle Object;
            const Vector Center;
            Vector pos;
            Vector offset;
            Matrix ori;
            Boolean hide;
            HashTableHandleTemplate<LinkStruct> subTable;
            HashTableHandleTemplate<LinkStruct> superTable;

            ObjectStruct (
                  const Handle TheObject,
                  const Vector &TheCenter) :
                  Object (TheObject),
                  Center (TheCenter),
                  offset (TheCenter),
                  hide (False) {;}
         };

         ObjectStruct *_lookup_object (const Handle Object);
         void _create_link (LinkStruct &ls);
         void _update_link (LinkStruct &ls);
         void _update_links (ObjectStruct &os);
         void _create_geometry (LinkDefStruct &def);
         void _init (Config &local);

         Log _log;

         Handle _defaultAttrHandle;
         Handle _hideAttrHandle;

         RenderModuleCoreOSG *_render;

         UInt32 _masterMask;
         UInt32 _glyphMask;
         UInt32 _entityMask;
         UInt32 _cullMask;

         HashTableHandleTemplate<Vector> _centerTable;
         HashTableHandleTemplate<LinkDefStruct> _defTable;
         HashTableHandleTemplate<LinkStruct> _linkTable;
         HashTableHandleTemplate<LinkStruct> _attrTable;
         HashTableHandleTemplate<ObjectStruct> _objTable;

      private:
         RenderPluginLinkOSG ();
         RenderPluginLinkOSG (const RenderPluginLinkOSG &);
         RenderPluginLinkOSG &operator= (const RenderPluginLinkOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_LINK_OSG_DOT_H
