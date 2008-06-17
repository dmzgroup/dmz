#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzRenderExtObjectOgre.h"
#include <dmzRenderUtilOgre.h>
#include <dmzRenderModuleCoreOgre.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeTime.h>
#include <Ogre/Ogre.h>


struct dmz::RenderExtObjectOgre::ModelStruct {

   const ObjectType ObjType;
   const Mask ObjState;
   RuntimeContext *context;

   Ogre::Entity *entity;

   Matrix orientation;
   Vector position;
   Vector scale;

   ModelStruct *next;

   ModelStruct (
      const ObjectType &TheType,
      const Mask &TheState,
      RuntimeContext *theContext);

   ~ModelStruct ();

   String get_name (Definitions &defs) const;
};


dmz::RenderExtObjectOgre::ModelStruct::ModelStruct (
      const ObjectType &TheType,
      const Mask &TheState,
      RuntimeContext *theContext) :
      ObjType (TheType),
      ObjState (TheState),
      context (theContext),
      entity (0),
      next (0) {;}


dmz::RenderExtObjectOgre::ModelStruct::~ModelStruct () {

   if (next) { delete next; next = 0; }
}


dmz::String
dmz::RenderExtObjectOgre::ModelStruct::get_name (Definitions &defs) const {

   String name (ObjType.get_name ());

   String stateName;
   defs.lookup_state_name (ObjState, stateName);

   if (!stateName) { stateName = "Default"; }

   name << "." << stateName;

   return name;
}


dmz::RenderExtObjectOgre::RenderExtObjectOgre (
         const PluginInfo &Info,
         Config &local) :
         Plugin (Info),
         ObjectObserverUtil (Info, local),
         _log (Info),
         _defs (Info, &_log),
         _defaultAttribute (0),
         _renderModuleCore (0),
         _entityTable (),
         _modelTable () {

   _init (local);

   activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask | ObjectTypeMask | ObjectStateMask);
}


dmz::RenderExtObjectOgre::~RenderExtObjectOgre () {

   _entityTable.clear ();
   _entityModelTable.clear ();
   _modelTable.empty ();
}


// Plugin Interface
void
dmz::RenderExtObjectOgre::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_renderModuleCore) {

         _renderModuleCore = RenderModuleCoreOgre::cast (PluginPtr);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_renderModuleCore &&
            (_renderModuleCore == RenderModuleCoreOgre::cast (PluginPtr))) {

         _renderModuleCore = 0;
      }
   }
}


// Object Observer Interface
void
dmz::RenderExtObjectOgre::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_renderModuleCore) {

      Ogre::SceneManager *sceneManager (_renderModuleCore->get_scene_manager ());

      Mask objState;
      _lookup_object_state (ObjectHandle, objState);

      ModelStruct *ms (_get_model_struct (Type, objState));

      if (ms && ms->entity && sceneManager) {

         _entityModelTable.store (ObjectHandle, ms);

         String name (ms->get_name (_defs));
         name << "." << ObjectHandle;

         Ogre::SceneNode *node (0);
         Ogre::Entity *clone (0);

         try {

            clone = ms->entity->clone (name.get_buffer ());

            if (clone) {

               node = sceneManager->createSceneNode ();

               if (node) {

                  Ogre::Any anyHandle (ObjectHandle);
                  clone->setUserAny (anyHandle);

                  node->attachObject (clone);
                  _entityTable.store (ObjectHandle, clone);

                  _log.info << "create object: " << name << endl;
               }
            }
         }
         catch (Ogre::Exception e) {

            if (clone) {

               sceneManager->destroyEntity (clone);
               clone = 0;
            }

            _log.error << "create_object failed: " << name << endl;
            _log.error << e.getFullDescription ().c_str () << endl;

            _entityModelTable.remove (ObjectHandle);
         }

         if (node) {

            Vector position;
            _lookup_object_position (ObjectHandle, position);

            node->setPosition (to_ogre_vector (position));

            Matrix orientation;
            _lookup_object_orientation (ObjectHandle, orientation);

            node->setOrientation (to_ogre_quaternion (orientation));

            if (!_renderModuleCore->add_dynamic_object (ObjectHandle, node)) {

               sceneManager->destroySceneNode (node->getName ());
               node = 0;

               _log.error << "Failed to add_dynamic_object: " << ObjectHandle;

               _entityModelTable.remove (ObjectHandle);
            }
         }
      }
   }
}


void
dmz::RenderExtObjectOgre::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (_renderModuleCore) {

      Ogre::SceneNode *node (_renderModuleCore->remove_dynamic_object (ObjectHandle));
      Ogre::SceneManager *sceneManager (_renderModuleCore->get_scene_manager ());

      if (node && sceneManager) {

         sceneManager->destroySceneNode (node->getName ());
         node = 0;
      }

      _entityModelTable.remove (ObjectHandle);
   }
}


void
dmz::RenderExtObjectOgre::update_object_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   Mask objState;
   _lookup_object_state (ObjectHandle, objState);

   _update_object (ObjectHandle, Value, objState);
}


void
dmz::RenderExtObjectOgre::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectType objType;
   _lookup_object_type (ObjectHandle, objType);

   _update_object (ObjectHandle, objType, Value);
}


dmz::RenderExtObjectOgre::ModelStruct *
dmz::RenderExtObjectOgre::_get_model_struct (
      const ObjectType &ObjType,
      const Mask &ObjState) {

   ModelStruct *retVal (0);

   Config modelList;
   ObjectType currentType (ObjType);

   if (_find_model_list_from_type (modelList, currentType)) {

      ModelStruct *headMs (_modelTable.lookup (currentType.get_handle ()));

      if (!headMs) {

         headMs = _config_to_model_struct (currentType, modelList);
      }

      retVal = _find_model_struct_containing_state (ObjState, headMs);
   }

   return retVal;
}


dmz::RenderExtObjectOgre::ModelStruct *
dmz::RenderExtObjectOgre::_find_model_struct_containing_state (
      const Mask &ObjState,
      ModelStruct *ms) {

   ModelStruct *retVal (0);

   if (ms) {

      Boolean found (False);

      ModelStruct *defaultMs (0);
      ModelStruct *currentMs (ms);

      while (currentMs && !found) {

         if (!defaultMs && !(currentMs->ObjState.is_set ())) {

            defaultMs = currentMs;
         }

         if (currentMs->ObjState.contains (ObjState)) {

            retVal = currentMs;
            found = True;
         }

         currentMs = currentMs->next;
      }

      if (!found) { retVal = defaultMs; }
   }

   return retVal;
}


dmz::Boolean
dmz::RenderExtObjectOgre::_find_model_list_from_type (
      Config &modelList,
      ObjectType &objType) {

   const String Name ("render.model");

   Boolean found (objType.get_config ().lookup_all_config (Name, modelList));

   if (!found) {

      ObjectType currentType (objType);
      currentType.become_parent ();

      while (currentType && !found) {

         if (currentType.get_config ().lookup_all_config (Name, modelList)) {

            found = True;
            objType = currentType;
         }

         currentType.become_parent ();
      }
   }

   return found;
}


void
dmz::RenderExtObjectOgre::_lookup_object_state (
      const Handle ObjectHandle,
      Mask &objState) {

   ObjectModule *objectModule (get_object_module ());

   if (objectModule) {

      objectModule->lookup_state (ObjectHandle, _defaultAttribute, objState);
   }
}


void
dmz::RenderExtObjectOgre::_lookup_object_type (
      const Handle ObjectHandle,
      ObjectType &objType) {

   ObjectModule *objectModule (get_object_module ());

   if (objectModule) {

      objectModule->lookup_object_type (ObjectHandle, _defaultAttribute, objType);
   }
}


void
dmz::RenderExtObjectOgre::_lookup_object_position (
      const Handle ObjectHandle,
      Vector &objPos) {

   ObjectModule *objectModule (get_object_module ());

   if (objectModule) {

      objectModule->lookup_position (ObjectHandle, _defaultAttribute, objPos);
   }
}


void
dmz::RenderExtObjectOgre::_lookup_object_orientation (
      const Handle ObjectHandle,
      Matrix &objOri) {

   ObjectModule *objectModule (get_object_module ());

   if (objectModule) {

      objectModule->lookup_orientation (ObjectHandle, _defaultAttribute, objOri);
   }
}


void
dmz::RenderExtObjectOgre::_update_object (
      const Handle ObjectHandle,
      const ObjectType &ObjType,
      const Mask &ObjState) {

   if (_renderModuleCore) {

      Ogre::SceneNode *node (_renderModuleCore->lookup_dynamic_object (ObjectHandle));
      Ogre::SceneManager *sceneManager (_renderModuleCore->get_scene_manager ());

      if (node && sceneManager) {

         ModelStruct *ms (_get_model_struct (ObjType, ObjState));

         if (ms && ms->entity && (ms != _entityModelTable.lookup (ObjectHandle))) {

            Ogre::Entity *entity (_entityTable.remove (ObjectHandle));

            if (entity) {

               node->detachObject (entity);
               sceneManager->destroyEntity (entity);
               entity = 0;
            }

            _entityModelTable.remove (ObjectHandle);
            _entityModelTable.store (ObjectHandle, ms);

            String name (ms->get_name (_defs));
            name << "." << ObjectHandle;

            try {

               Ogre::Entity *clone (ms->entity->clone (name.get_buffer ()));

               if (clone) {

                  Ogre::Any anyHandle (ObjectHandle);
                  clone->setUserAny (anyHandle);

                  node->attachObject (clone);
                  _entityTable.store (ObjectHandle, clone);
               }
            }
            catch (Ogre::Exception e) {

               _log.error << "update_object failed: " << name << endl;
               _log.error << e.getFullDescription ().c_str () << endl;

               _entityModelTable.remove (ObjectHandle);
            }
         }
      }
   }
}


dmz::RenderExtObjectOgre::ModelStruct *
dmz::RenderExtObjectOgre::_config_to_model_struct (
      const ObjectType &ObjType,
      Config &modelList) {

   ModelStruct *headMs (0);
   ModelStruct *currentMs (0);

   ConfigIterator it;
   Config modelData;

   Boolean found (modelList.get_first_config (it, modelData));

   while (found) {

      const String FileName (config_to_string ("file", modelData));
      if (FileName) {

         Mask objState;

         _defs.lookup_state (config_to_string ("state", modelData), objState);

         ModelStruct *newMs (
            new ModelStruct (ObjType, objState, get_plugin_runtime_context ()));

         if (newMs) {

            _modelTable.store (newMs->ObjType.get_handle (), newMs);

            newMs->entity = _load_mesh (newMs->get_name (_defs), FileName);

            _process_model_struct (modelData, *newMs);

            if (!headMs) { headMs = newMs; }

            if (currentMs) { currentMs->next = newMs; }

            currentMs = newMs;
         }
      }

      found = modelList.get_next_config (it, modelData);
   }

   return headMs;
}


void
dmz::RenderExtObjectOgre::_process_model_struct (
      Config &modelData,
      ModelStruct &ms) {

   ConfigIterator it;
   Config data;

   Boolean found (modelData.get_first_config (it, data));

   while (found) {

      const String DataName (data.get_name ().to_lower ());

      if (DataName == "position") {

         ms.position = config_to_vector (data);
      }
      else if (DataName == "orientation") {

         ms.orientation = config_to_matrix (data);
      }
      else if (DataName == "euler") {

         const Vector Euler (config_to_vector (data));
         const Matrix Value (Euler.get_x (), Euler.get_y (), Euler.get_z ());

         ms.orientation = Value;
      }
      else if (DataName == "scale") {

         ms.scale = config_to_vector (data);
      }

      found = modelData.get_next_config (it, data);
   }
}


Ogre::Entity *
dmz::RenderExtObjectOgre::_load_mesh (const String &Name, const String &Mesh) {

   Ogre::Entity *entity (0);

   if (_renderModuleCore) {

      Ogre::SceneManager *sceneManager (_renderModuleCore->get_scene_manager ());

      if (sceneManager) {

         try {

            entity = sceneManager->createEntity (Name.get_buffer (), Mesh.get_buffer ());

            if (entity) {

               _log.info << "loaded mesh: " << Mesh << " as " << Name << endl;
            }
         }
         catch (Ogre::Exception e) {

            _log.error << e.getFullDescription ().c_str () << endl;
         }
      }
   }

   return entity;
}


void
dmz::RenderExtObjectOgre::_init (Config &local) {

   _defaultAttribute = _defs.create_named_handle (ObjectAttributeDefaultName);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderExtObjectOgre (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderExtObjectOgre (Info, local);
}

};
