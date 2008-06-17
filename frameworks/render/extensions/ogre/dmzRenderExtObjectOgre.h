#ifndef DMZ_RENDER_EXT_OBJECT_OGRE_DOT_H
#define DMZ_RENDER_EXT_OBJECT_OGRE_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMask.h>


namespace Ogre { class Entity; class SceneNode; }


namespace dmz {

   class RenderModuleCoreOgre;

   class RenderExtObjectOgre :
         public Plugin,
         public ObjectObserverUtil {

      public:
         RenderExtObjectOgre (const PluginInfo &Info, Config &local);
         ~RenderExtObjectOgre ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_type (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue);

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

      protected:
         struct ModelStruct;

         Boolean _find_model_list_from_type (Config &modelList, ObjectType &objType);

         ModelStruct *_config_to_model_struct (
            const ObjectType &ObjType,
            Config &modelList);

         void _process_model_struct (Config &modelData, ModelStruct &ms);

         ModelStruct *_find_model_struct_containing_state (
            const Mask &ObjState,
            ModelStruct *headMs);

         ModelStruct *_get_model_struct (const ObjectType &ObjType, const Mask &ObjState);

         void _lookup_object_state (const Handle ObjectHandle, Mask &objState);
         void _lookup_object_type (const Handle ObjectHandle, ObjectType &objType);
         void _lookup_object_position (const Handle ObjectHandle, Vector &objPos);
         void _lookup_object_orientation (const Handle ObjectHandle, Matrix &objOri);

         void _update_object (
            const Handle ObjectHandle,
            const ObjectType &ObjType,
            const Mask &ObjState);

         Ogre::Entity *_load_mesh (const String &Name, const String &Mesh);

         void _init (Config &local);

         Log _log;
         Definitions _defs;
         Handle _defaultAttribute;
         RenderModuleCoreOgre *_renderModuleCore;

         HashTableHandleTemplate<Ogre::Entity> _entityTable;
         HashTableHandleTemplate<ModelStruct> _modelTable;
         HashTableHandleTemplate<ModelStruct> _entityModelTable;

      private:
         RenderExtObjectOgre ();
         RenderExtObjectOgre (const RenderExtObjectOgre &);
         RenderExtObjectOgre &operator= (const RenderExtObjectOgre &);

   };
};

#endif // DMZ_RENDER_EXT_OBJECT_OGRE_DOT_H
