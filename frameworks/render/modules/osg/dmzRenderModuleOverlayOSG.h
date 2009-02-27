#ifndef DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H
#define DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H

#include <dmzRenderModuleOverlay.h>
#include <dmzRenderPortalSize.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesVector.h>

#include <osg/Camera>
#include <osg/Group>
#include <osg/Image>
#include <osg/Switch>
#include <osg/Matrix>
#include <osg/MatrixTransform>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderModuleOverlayOSG :
         public Plugin,
         public RenderModuleOverlay,
         public PortalSizeObserver {

      public:
         class LayoutAxis {

            public:
               virtual ~LayoutAxis () {;}
               virtual Float64 update (const Int32 Value) = 0;

            protected:
               LayoutAxis () {;}

            private:
               LayoutAxis (const LayoutAxis &);
               LayoutAxis &operator= (const LayoutAxis &);
         };

         RenderModuleOverlayOSG (const PluginInfo &Info, Config &local);
         ~RenderModuleOverlayOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderModuleOverlay Interface
         virtual Handle lookup_overlay_handle (const String &Name);
         virtual String lookup_overlay_name (const Handle Overlay);
         virtual RenderOverlayTypeEnum lookup_overlay_type (const Handle Overlay);

         virtual Boolean is_of_overlay_type (
            const Handle Overlay,
            const RenderOverlayTypeEnum Type);

         // Overlay Group API

         // Overlay Switch API
         virtual Boolean store_overlay_switch_state (
            const Handle Overlay,
            const Int32 Which,
            const Boolean SwitchState);

         virtual Boolean store_overlay_all_switch_state (
            const Handle Overlay,
            const Boolean SwitchState);

         virtual Boolean enable_overlay_single_switch_state (
            const Handle Overlay,
            const Int32 Which);

         virtual Boolean lookup_overlay_switch_state (
            const Handle Overlay,
            const Int32 Which);

         virtual Int32 lookup_overlay_switch_count (const Handle Overlay);

         // Overlay Transform API
         virtual Boolean store_overlay_position (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY);

         virtual Boolean lookup_overlay_position (
            const Handle Overlay,
            Float64 &valueX,
            Float64 &valueY);

         virtual Boolean store_overlay_rotation (
            const Handle Overlay,
            const Float64 Value);

         virtual Boolean lookup_overlay_rotation (
            const Handle Overlay,
            Float64 &value);

         virtual Boolean store_overlay_scale (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY);

         virtual Boolean lookup_overlay_scale (
            const Handle Overlay,
            Float64 &valueX,
            Float64 &valueY);

         // PortalSizeObserver Interface
         virtual void update_portal_size (
            const Handle PortalHandle,
            const Int32 TheX,
            const Int32 TheY);

      protected:
         struct TextureStruct {

            osg::ref_ptr<osg::Image> img;
         };

         struct NodeStruct {

            const String Name;
            const RenderOverlayTypeEnum Type;
            const RuntimeHandle RHandle;
            const Handle VHandle;
            osg::ref_ptr<osg::Node> node;

            NodeStruct (const String &TheName, RuntimeContext *context, osg::Node *ptr) :
                  Name (TheName),
                  Type (RenderOverlayNode),
                  RHandle ("OSG Overlay Node", context),
                  VHandle (RHandle.get_runtime_handle ()) { node = ptr; }

            NodeStruct (
                  const String &TheName,
                  const RenderOverlayTypeEnum TheType,
                  const String &TypeName,
                  RuntimeContext *context,
                  osg::Node *ptr) :
                  Name (TheName),
                  Type (TheType),
                  RHandle (TypeName, context),
                  VHandle (RHandle.get_runtime_handle ()) { node = ptr; }
         };

         struct GroupStruct : public NodeStruct {

            osg::ref_ptr<osg::Group> group;

            GroupStruct (const String &Name, RuntimeContext *context, osg::Group *ptr) :
                  NodeStruct (
                     Name,
                     RenderOverlayGroup,
                     "OSG Overlay Group",
                     context,
                     ptr) {

               group = ptr;
            }

            GroupStruct (
                  const String &Name,
                  const RenderOverlayTypeEnum TheType,
                  const String &TypeName,
                  RuntimeContext *context,
                  osg::Group *ptr) :
                  NodeStruct (Name, Type, TypeName, context, ptr) { group = ptr; }
         };

         struct SwitchStruct : public GroupStruct {

            osg::ref_ptr<osg::Switch> switchNode;

            SwitchStruct (const String &Name, RuntimeContext *context, osg::Switch *ptr) :
                  GroupStruct (
                     Name,
                     RenderOverlaySwitch,
                     "OSG Overlay Switch",
                     context,
                     ptr) {

               switchNode = ptr;
            }
         };

         struct TransformStruct : public GroupStruct {

            osg::ref_ptr<osg::MatrixTransform> transform;
            Vector pos;
            Vector scale;
            Float64 rot;

            TransformStruct (
                  const String &Name,
                  RuntimeContext *context,
                  osg::MatrixTransform *ptr) :
                  GroupStruct (
                     Name,
                     RenderOverlayTransform,
                     "OSG Overlay Transform",
                     context,
                     ptr),
                  rot (0.0) { transform = ptr; }
         };

         struct LayoutStruct {

            LayoutAxis &xaxis;
            LayoutAxis &yaxis;
            TransformStruct &ts;

            LayoutStruct (
                  LayoutAxis &theXAxis,
                  LayoutAxis &theYAxis,
                  TransformStruct &theTS) :
                  xaxis (theXAxis),
                  yaxis (theYAxis),
                  ts (theTS) {;}

            ~LayoutStruct () { delete &xaxis; delete &yaxis; }
         };

         void _update_layout (const Int32 TheX, const Int32 TheY);

         void _apply_transform (TransformStruct &ts);

         LayoutAxis *_create_axis (const String &Prefix, Config &layout);
         TextureStruct *_create_texture (const String &Name);

         Boolean _register_node (NodeStruct *ptr);
         Boolean _register_group (GroupStruct *ptr);
         Boolean _register_switch (SwitchStruct *ptr);
         Boolean _register_transform (TransformStruct *ptr);

         void _add_children (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_node (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_group (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_switch (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_transform (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_box (osg::ref_ptr<osg::Group> &parent, Config &node);

         void _init_layout (Config &local);
         void _init (Config &local);

         Log _log;
         Resources _rc;
         RenderModuleCoreOSG *_core;

         osg::ref_ptr<osg::Camera> _camera;
         osg::ref_ptr<osg::Group> _rootNode;
         HashTableStringTemplate<TextureStruct> _textureTable;

         HashTableStringTemplate<NodeStruct> _nodeNameTable;
         HashTableHandleTemplate<NodeStruct> _nodeTable;
         HashTableHandleTemplate<GroupStruct> _groupTable;
         HashTableHandleTemplate<SwitchStruct> _switchTable;
         HashTableHandleTemplate<TransformStruct> _transformTable;
         HashTableHandleTemplate<LayoutStruct> _layoutTable;

      private:
         RenderModuleOverlayOSG ();
         RenderModuleOverlayOSG (const RenderModuleOverlayOSG &);
         RenderModuleOverlayOSG &operator= (const RenderModuleOverlayOSG &);

   };
};

#endif // DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H
