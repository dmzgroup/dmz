#ifndef DMZ_RENDER_MODULE_ISECT_OGRE_DOT_H
#define DMZ_RENDER_MODULE_ISECT_OGRE_DOT_H

#include <dmzRenderModuleCoreOgre.h>
#include <dmzRuntimeLog.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimePlugin.h>
#include <dmzRenderIsect.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <Ogre/OgreRay.h>
#include <Ogre/OgreSceneQuery.h>


namespace dmz {

   class RenderModuleIsectOgre :
         public Plugin,
         private RenderModuleIsect {

      public:
         RenderModuleIsectOgre (const PluginInfo &Info, const Config &Local);
         ~RenderModuleIsectOgre ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderModuleIsect Interface
         virtual Boolean do_isect (
            const IsectParameters &Parameters,
            const IsectTestContainer &TestValues,
            IsectResultContainer &resultContainer);

         virtual UInt32 enable_isect (const Handle ObjectHandle);
         virtual UInt32 disable_isect (const Handle ObjectHandle);

      protected:
         void _init (const Config &Local);

         Handle _extract_object_handle (const Ogre::Entity &Entity);

         void _isect_entity (
            const Ogre::Entity &Entity,
            const UInt32 TestHandle,
            const IsectParameters &Parameters,
            IsectResultContainer &resultContainer);

         void _isect_sub_entity (
            const Ogre::Entity &Entity,
            const UInt32 SubEntityIndex,
            const UInt32 TestHandle,
            const IsectParameters &Parameters,
            IsectResultContainer &resultContainer);

         struct MeshStruct;

         MeshStruct *_get_current_mesh_data (
            const Ogre::Entity &Entity,
            const UInt32 SubEntityIndex);

         void _find_closest_result (
            IsectResultContainer &container,
            IsectResult &closest);

         RenderModuleCoreOgre *_core;
         Ogre::SceneManager *_sceneManager;
         Ogre::RaySceneQuery *_raySceneQuery;
         Ogre::Ray _isectRay;
         Float64 _maxSegmentDistance;
         Log _log;

         HashTableUInt32Template<UInt32> _disabledTable;

         struct EntityIsectStruct;
         EntityIsectStruct &_entityIsectData;

         Boolean _cacheSceneData;

         MeshStruct &_localMesh;

         struct EntityStruct {

            HashTableUInt32Template<MeshStruct> meshTable;
            ~EntityStruct () { meshTable.empty (); }
         };

         HashTableStringTemplate<EntityStruct> _entityTable;
         
      private:
         RenderModuleIsectOgre ();
         RenderModuleIsectOgre (const RenderModuleIsectOgre &);
         RenderModuleIsectOgre &operator= (const RenderModuleIsectOgre &);

         void __get_mesh_information (Ogre::SubMesh *subMesh, MeshStruct &meshStruct);
   };
};

#endif //  DMZ_RENDER_MODULE_ISECT_OGRE_DOT_H
