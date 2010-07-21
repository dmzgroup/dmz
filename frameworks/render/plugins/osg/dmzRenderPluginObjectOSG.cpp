#include <dmzObjectAttributeMasks.h>
#include <dmzRenderConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginObjectOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToStringContainer.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

dmz::RenderPluginObjectOSG::RenderPluginObjectOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ResourcesObserver (Info),
      DefinitionsObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _rc (Info, &_log),
      _core (0),
      _cullMask (0),
      _masterIsectMask (0),
      _entityIsectMask (0),
      _glyphIsectMask (0) {

   _noModel.model = new osg::Group;
   _init (local);
}


dmz::RenderPluginObjectOSG::~RenderPluginObjectOSG () {

   _noModel.model = 0;
   _modelTable.empty ();
   _typeTable.clear ();
   _defTable.empty ();
   _objectTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginObjectOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginObjectOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = RenderModuleCoreOSG::cast (PluginPtr);

         if (_core) {

            _cullMask = _core->get_cull_mask ();
            _masterIsectMask = _core->get_master_isect_mask ();
            _entityIsectMask = _core->lookup_isect_mask (RenderIsectEntityName);
            _glyphIsectMask = _core->lookup_isect_mask (RenderIsectGlyphName);

            _add_models ();
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _remove_models ();
         _core = 0;
         _cullMask = 0;
         _masterIsectMask = 0;
         _entityIsectMask = 0;
         _glyphIsectMask = 0;
      }
   }
}


// Resources Observer Interface
void
dmz::RenderPluginObjectOSG::update_resource (
      const String &Name,
      const ResourcesUpdateTypeEnum Mode) {

}


// Definitions Observer Interface
void
dmz::RenderPluginObjectOSG::define_object_type (const ObjectType &Type) {

}


// Object Observer Interface
void
dmz::RenderPluginObjectOSG::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (!_ignoreType.contains_exact_type (Type)) {

      DefStruct *ds (_lookup_def_struct (Type));

      if (ds) {

         ObjectStruct *os (new ObjectStruct (*ds));

         if (os && !_objectTable.store (ObjectHandle, os)) { delete os; os = 0; }

         if (os && os->model.valid ()) {

            os->model->setSingleChildOn (0);

            if (_core) {

               osg::Group *group (_core->create_dynamic_object (ObjectHandle));

               if (group) {

                  os->model->setNodeMask (
                     (os->model->getNodeMask () & ~_masterIsectMask) |
                        (os->Def.Glyph ? _glyphIsectMask : _entityIsectMask));

                  group->addChild (os->model.get ());
               }
            }
         }
      }
      else { _ignoreType.add_object_type (Type); }
   }
}


void
dmz::RenderPluginObjectOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os (_objectTable.remove (ObjectHandle));

   if (os) {

      if (_core && os->model.valid ()) {

         osg::Group *group (_core->lookup_dynamic_object (ObjectHandle));

         if (group) { group->removeChild (os->model.get ()); }
      }

      delete os; os = 0;
   }
}


void
dmz::RenderPluginObjectOSG::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os && os->model.valid ()) {

      unsigned int place (0);

      StateStruct *ss (os->Def.stateMap);

      while (ss) {

         if (Value.contains (ss->State)) { place = ss->Place; ss = 0; }
         else { ss = ss->next; }
      }

      os->model->setSingleChildOn (place);
   }
}


void
dmz::RenderPluginObjectOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os && os->model.valid ()) {

      UInt32 mask = os->model->getNodeMask ();

      if (Value) { mask &= (~_cullMask); }
      else { mask |= _cullMask; }

      os->model->setNodeMask (mask);
   }
}


void
dmz::RenderPluginObjectOSG::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


dmz::RenderPluginObjectOSG::DefStruct *
dmz::RenderPluginObjectOSG::_lookup_def_struct (const ObjectType &Type) {

   ObjectType current (Type);

   DefStruct *result (0);

   while (!result && current) {

      result = _typeTable.lookup (current.get_handle ());

      if (!result) {

         result = _create_def_struct (current);

         if (result) { _typeTable.store (Type.get_handle (), result); }
         else { current.become_parent (); }
      }
   }

   return result;
}


dmz::RenderPluginObjectOSG::DefStruct *
dmz::RenderPluginObjectOSG::_create_def_struct (const ObjectType &Type) {

   DefStruct *result = _defTable.lookup (Type.get_handle ());

   Config modelList;

   if (!result && Type.get_config ().lookup_all_config ("render.model", modelList)) {

      result = new DefStruct (
         config_to_boolean ("value", Type.find_config ("glyph"), False));

      if (_defTable.store (Type.get_handle (), result)) {

         _typeTable.store (Type.get_handle (), result);

         ConfigIterator it;
         Config model;

         unsigned int place (1);

         StateStruct *currentState (0);

         while (modelList.get_next_config (it, model)) {

            const String ResourceName (config_to_string ("resource", model));
            const Boolean NoModel (config_to_boolean ("none", model));
            Mask state;
            String stateName;
            const Boolean StateNameFound (model.lookup_attribute ("state", stateName));

            if (StateNameFound) { _defs.lookup_state (stateName, state); }

            if (!StateNameFound || state) {

               ModelStruct *ms = (NoModel ? &_noModel : _load_model (ResourceName));

               if (ms) {

                  unsigned int switchPlace (StateNameFound ? place : 0);
                  if (StateNameFound) { place++; }

                  if (((switchPlace + 1) > result->model->getNumChildren ()) ||
                        !result->model->getChild (switchPlace)) {

                     result->model->insertChild (switchPlace, ms->model.get ());

                     if (switchPlace) {

                        StateStruct *ss (new StateStruct (switchPlace, state));

                        if (currentState) {

                           currentState->next = ss;
                           currentState = ss;
                        }
                        else { result->stateMap = currentState = ss; }
                     }
                  }
               }
            }
         }
      }
      else { delete result; result = 0; }
   }

   return result;
}


dmz::RenderPluginObjectOSG::ModelStruct *
dmz::RenderPluginObjectOSG::_load_model (const String &ResourceName) {

   ModelStruct *result (0);
   String foundFile (_rc.find_file (ResourceName));

   if (foundFile) {

      result = _modelTable.lookup (foundFile);

      if (!result) {

         result = new ModelStruct;
         result->model = osgDB::readNodeFile (foundFile.get_buffer ());

         if (result->model.valid ()) {

            osgUtil::Optimizer optimizer;
            optimizer.optimize(result->model.get());

            osg::Node::DescriptionList &list = result->model->getDescriptions ();

            String str ("<dmz><render><resource name=\"");
            str << ResourceName << "\"/></render></dmz>";
            list.push_back (str.get_buffer ());

            _log.info << "Loaded file: " << foundFile << " (" << ResourceName << ")"
               << endl;
            _modelTable.store (foundFile, result);
         }
         else {

            delete result; result = 0;
            _log.error << "Failed loading file: " << foundFile << " (" << ResourceName
               << ")" << endl;
         }
      }
   }

   return result;
}


void
dmz::RenderPluginObjectOSG::_add_models () {

   if (_core) {

      HashTableHandleIterator it;
      ObjectStruct *os (0);

      while (_objectTable.get_next (it, os)) {

         osg::Group *group (_core->create_dynamic_object (it.get_hash_key ()));

         if (os->model.valid () && group) {

            os->model->setNodeMask (
               (os->model->getNodeMask () & ~_masterIsectMask) |
                  (os->Def.Glyph ? _glyphIsectMask : _entityIsectMask));

            group->addChild (os->model.get ());
         }
      }
   }
}


void
dmz::RenderPluginObjectOSG::_remove_models () {

   if (_core) {

      HashTableHandleIterator it;
      ObjectStruct *os (0);

      while (_objectTable.get_next (it, os)) {

         osg::Group *group (_core->lookup_dynamic_object (it.get_hash_key ()));

         if (os->model.valid () && group) {

            os->model->setNodeMask (os->model->getNodeMask () | _masterIsectMask);

            group->removeChild (os->model.get ());
         }
      }
   }
}


void
dmz::RenderPluginObjectOSG::_init (Config &local) {

   activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask |
      ObjectStateMask);

   activate_object_attribute (
      config_to_string ("hide-object-flag.name", local, ObjectAttributeHideName),
      ObjectFlagMask);

   activate_resources_callback (ResourcesDumpNone);

   set_definitions_observer_callback_mask (
      DefinitionsDumpNone,
      DefinitionsObjectTypeMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginObjectOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginObjectOSG (Info, local);
}

};
