#include <dmzFoundationXMLUtil.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderUtilOSG.h>
#include "dmzRenderPluginArticulateOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Transform>
#include <osgSim/DOFTransform>

namespace {

typedef dmz::RenderPluginArticulateOSG::ScalarStruct sstruct;

struct DofScalarStruct : public sstruct {

   const dmz::VectorComponentEnum Axis;
   osg::ref_ptr<osgSim::DOFTransform> transform;

   DofScalarStruct (
         const dmz::VectorComponentEnum TheAxis,
         osgSim::DOFTransform *node) :
         Axis (TheAxis),
         transform (node) {;}

   virtual void update_scalar (
         const dmz::Float64 Value,
         const dmz::Float64 *PreviousValue) {

      osg::Vec3 hpr;

      if (Axis == dmz::VectorComponentY) { hpr.x () = Value; }
      else if (Axis == dmz::VectorComponentX) { hpr.y () = Value; }
      else if (Axis == dmz::VectorComponentZ) { hpr.z () = Value; }

      if (transform.valid ()) { transform->setCurrentHPR (hpr); }
   }
};

struct MatrixScalarStruct : public sstruct {

   osg::ref_ptr<osg::MatrixTransform> transform;
   dmz::Vector axis;

   MatrixScalarStruct (
         const dmz::VectorComponentEnum TheAxis,
         osg::MatrixTransform *node) :
         transform (node) {

      if (TheAxis == dmz::VectorComponentY) { axis.set_xyz (0.0, 1.0, 0.0); }
      else if (TheAxis == dmz::VectorComponentX) { axis.set_xyz (1.0, 0.0, 0.0); }
      else if (TheAxis == dmz::VectorComponentZ) { axis.set_xyz (0.0, 0.0, -1.0); }
   }

   virtual void update_scalar (
         const dmz::Float64 Value,
         const dmz::Float64 *PreviousValue) {

      dmz::Matrix mat (axis, Value);
      static const dmz::Vector PosVec;
      static const dmz::Vector ScaleVec (1.0, 1.0, 1.0);

      if (transform.valid ()) {

         transform->setMatrix (dmz::to_osg_matrix (mat, PosVec, ScaleVec));
      }
   }
};

};


dmz::RenderPluginArticulateOSG::RenderPluginArticulateOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      osg::NodeVisitor (
         osg::NodeVisitor::NODE_VISITOR,
         osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
      _log (Info),
      _rc (Info),
      _core (0),
      _currentObj (0),
      _rcStack (0) {

   _init (local);
}


dmz::RenderPluginArticulateOSG::~RenderPluginArticulateOSG () {

   _objTable.empty ();
   _rcTable.clear ();
   _rcMasterTable.empty ();
   _regTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginArticulateOSG::update_plugin_state (
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
dmz::RenderPluginArticulateOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginArticulateOSG::update_time_slice (const Float64 TimeDelta) {


}


// Object Observer Interface
void
dmz::RenderPluginArticulateOSG::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_core) {

      osg::ref_ptr<osg::Group> root = _core->lookup_dynamic_object (ObjectHandle);

      if (root.valid ()) {

         _currentObj = new ObjectStruct;

         if (_currentObj) {

            root->accept (*this);

            if (_currentObj->attr.get_count () == 0) {

               delete _currentObj; _currentObj = 0;
            }
            else {

              if (_objTable.store (ObjectHandle, _currentObj)) { _currentObj = 0; }
              else { delete _currentObj; _currentObj = 0; }
            }

            _currentObj = 0;
         }
      }
   }
}


void
dmz::RenderPluginArticulateOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *obj = _objTable.remove (ObjectHandle);
   if (obj) { delete obj; obj = 0; }
}


void
dmz::RenderPluginArticulateOSG::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

}


void
dmz::RenderPluginArticulateOSG::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

}


void
dmz::RenderPluginArticulateOSG::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::RenderPluginArticulateOSG::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   ObjectStruct *obj = _objTable.lookup (ObjectHandle);

   if (obj) {

      AttrStruct *attr = obj->attr.lookup (AttributeHandle);

      ScalarStruct *current = (attr ? attr->scalarList : 0);

      while (current) {

         current->update_scalar (Value, PreviousValue);
         current = current->next;
      }
   }
}


// osg::NodeVisitor Interface
void
dmz::RenderPluginArticulateOSG::apply (osg::Node &node) {

   osg::Node::DescriptionList& list = node.getDescriptions ();

   Boolean popRc (False);

   if (list.size () > 0) {

      Boolean done = False;
      osg::Node::DescriptionList::iterator it = list.begin ();

      while (it != list.end ()) {

         if (it->compare (0, 5, "<dmz>") == 0) {

            Config data ("global");

            if (xml_string_to_config (it->c_str (), data, &_log)) {

               const String ResourceName =
                  config_to_string ("dmz.render.resource.name", data);

               popRc = _push_rc (ResourceName);
            }
         }

         it++;
      }
   }

   traverse (node);

   if (popRc) { _pop_rc (); }
}


void
dmz::RenderPluginArticulateOSG::apply (osg::Transform &transform) {

   if (_rcStack && _currentObj) {

      const String Name = transform.getName ().c_str ();

      if (Name) {

         osgSim::DOFTransform *dof = dynamic_cast<osgSim::DOFTransform *>(&transform);

         if (dof) {

            ScalarDefStruct *sds = _rcStack->rc.scalarMap.lookup (Name);

            if (sds) {

               ScalarStruct *ss = new DofScalarStruct (sds->Axis, dof);

               if (ss && !_currentObj->add_scalar (sds->AttrHandle, ss)) {

                  delete ss; ss = 0;
               }
            }
         }
         else {

            osg::MatrixTransform *mat = transform.asMatrixTransform ();

            if (mat) {

               ScalarDefStruct *sds = _rcStack->rc.scalarMap.lookup (Name);

               if (sds) {

                  ScalarStruct *ss = new MatrixScalarStruct (sds->Axis, mat);

                  if (ss && !_currentObj->add_scalar (sds->AttrHandle, ss)) {

                     delete ss; ss = 0;
                  }
               }
            }
         }
      }
   }

   osg::Node &node = transform;

   apply (node);
}


dmz::RenderPluginArticulateOSG::ResourceStruct *
dmz::RenderPluginArticulateOSG::_create_rc (const String &Name) {

   ResourceStruct *result (0);

   Config data;

   if (Name && _rc.lookup_resource_config (Name, data)) {

      const String TemplateName = config_to_string ("render.articulate.template", data);

      if (TemplateName) {

         result = _rcMasterTable.lookup (TemplateName);

         if (!result) { result = _create_rc (TemplateName); }
      }
      else {

         Config articList;

         if (data.lookup_all_config ("render.articulate.attribute", articList)) {

            result = new ResourceStruct;

            if (result) {

               ConfigIterator it;
               Config artic;

               while (articList.get_next_config (it, artic)) {

                  const String Type = config_to_string ("type", artic);
                  const Handle AttrHandle = config_to_named_handle (
                     "name",
                     artic,
                     get_plugin_runtime_context ());

                  const String NodeName = config_to_string ("node", artic);

                  Mask *sub = _regTable.lookup (AttrHandle);

                  if (!sub) {

                     sub = new Mask;

                     if (sub && !_regTable.store (AttrHandle, sub)) {

                        delete sub; sub = 0;
                     }
                  }

                  if (Type == "scalar") {

                     const VectorComponentEnum Axis =
                        config_to_vector_component ("axis", artic);

                     ScalarDefStruct *sds = new ScalarDefStruct (Axis, AttrHandle);

                     if (sds && !result->scalarMap.store (NodeName, sds)) {

                        delete sds; sds = 0;
                     }
                     else if (sds && sub) {

                        if (!sub->contains (ObjectScalarMask)) {

                           (*sub) |= ObjectScalarMask;
                           activate_object_attribute (AttrHandle, *sub);
                        }
                     }
                  }
               }

               if (!_rcMasterTable.store (Name, result)) {

                  delete result; result = 0;
               }
            }
         }
      }
   }

   if (result) { _rcTable.store (Name, result); }

   return result;
}


dmz::Boolean
dmz::RenderPluginArticulateOSG::_push_rc (const String Name) {

   Boolean result (False);

   ResourceStruct *rs = _rcTable.lookup (Name);

   if (!rs) { rs = _create_rc (Name); }

   if (rs) {

      ResourceStackStruct *rss = new ResourceStackStruct (*rs);

      if (rss) {

         rss->next = _rcStack;
         _rcStack = rss;
         result = True;
      }
   }

   return result;
}


void
dmz::RenderPluginArticulateOSG::_pop_rc () {

   ResourceStackStruct *tmp = _rcStack;

   if (tmp) {

      _rcStack = tmp->next;
      delete tmp; tmp = 0;
   }
}


void
dmz::RenderPluginArticulateOSG::_init (Config &local) {

   activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginArticulateOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginArticulateOSG (Info, local);
}

};
