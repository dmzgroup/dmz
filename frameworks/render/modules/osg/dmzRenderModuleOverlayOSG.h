#ifndef DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H
#define DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H

#include <dmzRenderModuleOverlay.h>
#include <dmzRenderPortalSize.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesVector.h>

#include <osg/Camera>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Image>
#include <osg/Switch>
#include <osg/Matrix>
#include <osg/MatrixTransform>

#include <osgText/Text>

#include <qdb.h>
static dmz::qdb out;

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

         class ScaleAxis {

            public:
               virtual ~ScaleAxis () {;}
               virtual Float64 update (const Int32 Value) = 0;

            protected:
               ScaleAxis () {;}

            private:
               ScaleAxis (const ScaleAxis &);
               ScaleAxis &operator= (const ScaleAxis &);
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
         virtual Handle lookup_node_handle (const String &Name);

         virtual Handle lookup_node_clone_sub_handle (
            const Handle CloneHandle,
            const String &Name);

         virtual String lookup_node_name (const Handle Overlay);
         virtual RenderOverlayTypeEnum lookup_node_type (const Handle Overlay);

         virtual Boolean is_of_node_type (
            const Handle Overlay,
            const RenderOverlayTypeEnum Type);

         virtual Handle clone_template (const String &Name);

         virtual Boolean destroy_node (const Handle Overlay);

         // Overlay Text API
         virtual Boolean store_text (const Handle Overlay, const String &Value);
         virtual Boolean lookup_text (const Handle Overlay, String &value);

         // Overlay Group API
         virtual Boolean add_group_child (const Handle Parent, const Handle Child);
         virtual Boolean remove_group_child (const Handle Parent, const Handle Child);
         virtual Int32 lookup_group_child_count (const Handle Overlay);

         // Overlay Switch API
         virtual Boolean store_switch_state (
            const Handle Overlay,
            const Int32 Which,
            const Boolean SwitchState);

         virtual Boolean store_switch_state_all (
            const Handle Overlay,
            const Boolean SwitchState);

         virtual Boolean enable_switch_state_single (
            const Handle Overlay,
            const Int32 Which);

         virtual Boolean lookup_switch_state (
            const Handle Overlay,
            const Int32 Which);

         // Overlay Transform API
         virtual Boolean store_transform_position (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY);

         virtual Boolean lookup_transform_position (
            const Handle Overlay,
            Float64 &valueX,
            Float64 &valueY);

         virtual Boolean store_transform_rotation (
            const Handle Overlay,
            const Float64 Value);

         virtual Boolean lookup_transform_rotation (
            const Handle Overlay,
            Float64 &value);

         virtual Boolean store_transform_scale (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY);

         virtual Boolean lookup_transform_scale (
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

            virtual ~NodeStruct () {;}
         };

         struct TextStruct : public NodeStruct {

            osg::ref_ptr<osg::Geode> geode;
            osg::ref_ptr<osgText::Text> text;
            String value;

            TextStruct (
                  const String &Name,
                  const String &Text,
                  RuntimeContext *context,
                  osg::Geode *geodePtr,
                  osgText::Text *textPtr) :
                  NodeStruct (
                     Name,
                     RenderOverlayText,
                     "OSG Overlay Text",
                     context,
                     geodePtr),
                  value (Text) {

               geode = geodePtr;
               text = textPtr;
            }

            virtual ~TextStruct () {;}
         };

         struct GroupStruct : public NodeStruct {

            osg::ref_ptr<osg::Group> group;
            HashTableHandleTemplate<NodeStruct> childTable;

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
                  NodeStruct (Name, TheType, TypeName, context, ptr) { group = ptr; }

            virtual ~GroupStruct () {;}
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

            virtual ~SwitchStruct () {;}
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

            virtual ~TransformStruct () {;}
         };

         struct CloneStruct {

            CloneStruct *next;
            HashTableStringTemplate<NodeStruct> nameTable;

            CloneStruct () : next (0) {;}
            ~CloneStruct () { nameTable.clear (); }
         };

         struct GroupStackStruct {

            GroupStruct &gs;
            GroupStackStruct *next;

            GroupStackStruct (GroupStruct &TheGs) : gs (TheGs), next (0) {;}
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

         struct ScaleStruct {

            ScaleAxis *xaxis;
            ScaleAxis *yaxis;
            TransformStruct &ts;

            ScaleStruct (
                  ScaleAxis *theXAxis,
                  ScaleAxis *theYAxis,
                  TransformStruct &theTS) :
                  xaxis (theXAxis),
                  yaxis (theYAxis),
                  ts (theTS) {;}
         };

         void _update_layout (const Int32 TheX, const Int32 TheY);
         void _update_auto_scale (const Int32 TheX, const Int32 TheY);

         void _apply_transform (TransformStruct &ts);

         LayoutAxis *_create_layout_axis (const String &Prefix, Config &layout);
         ScaleAxis *_create_scale_axis (const String &Prefix, Config &data);
         TextureStruct *_create_texture (const String &Name);

         osg::Vec4 _config_to_color (Config &data);

         Boolean _register_node (NodeStruct *ptr);
         Boolean _register_text (TextStruct *ptr);
         Boolean _register_group (GroupStruct *ptr);
         Boolean _register_switch (SwitchStruct *ptr);
         Boolean _register_transform (TransformStruct *ptr);

         void _add_children (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_node (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_text (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_group (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_switch (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_transform (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_box (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_circle (osg::ref_ptr<osg::Group> &parent, Config &node);
         void _add_clone (osg::ref_ptr<osg::Group> &parent, Config &node);

         Boolean _remove_node (const Handle Overlay);
         Boolean _remove_text (const Handle Overlay);
         Boolean _remove_group (const Handle Overlay);
         Boolean _remove_switch (const Handle Overlay);
         Boolean _remove_transform (const Handle Overlay);

         void _init_colors (Config &local);
         void _init_templates (Config &local);
         void _init_layout (Config &local);
         void _init_auto_scale (Config &local);
         void _init (Config &local);

         Log _log;
         Resources _rc;
         RenderModuleCoreOSG *_core;

         osg::ref_ptr<osg::Camera> _camera;
         osg::ref_ptr<osg::Group> _rootNode;
         CloneStruct *_cloneStack;
         GroupStackStruct *_groupStack;
         HashTableStringTemplate<TextureStruct> _textureTable;

         HashTableStringTemplate<osg::Vec4> _colorTable;
         HashTableStringTemplate<Config> _templateTable;
         HashTableStringTemplate<NodeStruct> _nodeNameTable;
         HashTableHandleTemplate<NodeStruct> _nodeTable;
         HashTableHandleTemplate<TextStruct> _textTable;
         HashTableHandleTemplate<GroupStruct> _groupTable;
         HashTableHandleTemplate<SwitchStruct> _switchTable;
         HashTableHandleTemplate<TransformStruct> _transformTable;
         HashTableHandleTemplate<CloneStruct> _cloneTable;
         HashTableHandleTemplate<LayoutStruct> _layoutTable;
         HashTableHandleTemplate<ScaleStruct> _scaleTable;

      private:
         RenderModuleOverlayOSG ();
         RenderModuleOverlayOSG (const RenderModuleOverlayOSG &);
         RenderModuleOverlayOSG &operator= (const RenderModuleOverlayOSG &);

   };
};

#endif // DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H
