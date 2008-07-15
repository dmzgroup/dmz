#include "dmzRenderModuleIsectOSG.h"
#include <dmzRenderUtilOSG.h>
#include <dmzRenderObjectDataOSG.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesVector.h>
#include <dmzTypesString.h>

#include <osg/CullFace>
#include <osg/Drawable>
#include <osg/Group>
#include <osg/LineSegment>
#include <osg/Node>
#include <osgUtil/IntersectVisitor>


dmz::RenderModuleIsectOSG::RenderModuleIsectOSG (
      const PluginInfo &Info,
      const Config &Local) :
      Plugin (Info),
      RenderModuleIsect (Info),
      _core (0),
      _log (Info.get_name (), Info.get_context ()) {

   _init (Local);
}


dmz::RenderModuleIsectOSG::~RenderModuleIsectOSG () {

}


void
dmz::RenderModuleIsectOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _core = 0;
      }
   }
}


dmz::Boolean
dmz::RenderModuleIsectOSG::do_isect (
      const IsectParameters &Parameters,
      const IsectTestContainer &TestValues,
      IsectResultContainer &resultContainer) {

   if (_core) {

      osg::ref_ptr<osg::Group> scene = _core->get_scene ();
//      osg::ref_ptr<osg::Group> scene = _core->get_static_objects ();

      osg::BoundingSphere bs = scene->getBound();

      std::vector<osg::LineSegment*> lsList;// = new osg::LineSegment[TestValues];

      osgUtil::IntersectVisitor visitor;

      UInt32 testHandle;
      IsectTestTypeEnum testType;
      Vector vec1, vec2;

      std::vector<UInt32> handleArray;
      std::vector<Vector> sourceArray;

      TestValues.get_first_test (testHandle, testType, vec1, vec2);

      do {

         if (testType == IsectRayTest) {

            vec2 = (vec1 + (vec2 * (bs.radius () * 2)));
         }

         osg::LineSegment *ls = new osg::LineSegment;
         ls->set (to_osg_vector (vec1), to_osg_vector (vec2));
         visitor.addLineSegment (ls);
         lsList.push_back (ls);//addLOS (to_osg_vector (vec1), to_osg_vector (vec2));
         handleArray.push_back (testHandle);
         sourceArray.push_back (vec1);

      } while (TestValues.get_next_test (testHandle, testType, vec1, vec2));

      scene->accept (visitor);

      IsectTestResultTypeEnum param = Parameters.get_test_result_type ();
      bool closestPoint = (param == IsectClosestPoint);
      bool firstPoint = (param == IsectFirstPoint);

      if (closestPoint && !Parameters.get_calculate_distance ()) {
         firstPoint = true;
         closestPoint = false;
      }

      Boolean result (False);

      for (unsigned int ix = 0; ix < lsList.size () && !result; ix++) {

         osgUtil::IntersectVisitor::HitList resultHits;
         resultHits = visitor.getHitList (lsList[ix]);

         if (!resultHits.empty ()) {

            for (unsigned int jy = 0; jy < resultHits.size (); jy++) {

               Handle objHandle (0);

               osgUtil::Hit currentHit = resultHits[jy];

               Boolean disabled (False);

               osg::NodePath path = currentHit.getNodePath ();

               for (
                     osg::NodePath::iterator it = path.begin ();
                     (it != path.end ()) && !disabled;
                     it++) {

                  osg::Node *node (*it);
                  osg::Referenced *r (node ? node->getUserData () : 0);

                  if (r) {

                     RenderObjectDataOSG *data (dynamic_cast<RenderObjectDataOSG *> (r));

                     if (data) {

                        if (!data->do_isect ()) { disabled = True; }
                        else { objHandle = data->get_handle (); }
                        //else if (!objHandle) { objHandle = data->get_handle (); }
                     }
                  }
               }

               if (!disabled) {

                  Vector lsPoint = to_dmz_vector (currentHit.getWorldIntersectPoint ());
                  IsectResult lsResult;

                  lsResult.set_isect_test_id (handleArray[ix]);

                  lsResult.set_point (lsPoint);
 
                  if (Parameters.get_calculate_object_handle ()) {

                    lsResult.set_object_handle (objHandle);
                  }

                  if (Parameters.get_calculate_normal ()) {

                     lsResult.set_normal (
                        to_dmz_vector (currentHit.getWorldIntersectNormal ()));
                  }

                  if (Parameters.get_calculate_distance ()) {

                     lsResult.set_distance ((lsPoint - sourceArray[ix]).magnitude ());
                  }

                  if (Parameters.get_calculate_cull_mode ()) {

                     osg::Drawable *drawObject = (currentHit.getDrawable ());

                     osg::StateSet *sSet = (drawObject ? drawObject->getStateSet () : 0);

                     osg::CullFace *cf = (osg::CullFace*)(
                        sSet ? sSet->getAttribute (osg::StateAttribute::CULLFACE) : 0);

                     UInt32 cullMask = 0;

                     if (cf) {

                        if (cf->getMode () == osg::CullFace::FRONT ||
                              cf->getMode () == osg::CullFace::FRONT_AND_BACK)  {

                           cullMask |= IsectPolygonFrontCulledMask;
                        }

                        if (cf->getMode () == osg::CullFace::BACK ||
                              cf->getMode () == osg::CullFace::FRONT_AND_BACK) {

                           cullMask |= IsectPolygonBackCulledMask;
                        }
                     }

                     lsResult.set_cull_mode (cullMask);
                  }

                  if (Parameters.get_calculate_object_handle ()) {

                     osg::Geode *hitObject = currentHit.getGeode ();
                  }

                  resultContainer.add_result (lsResult);

                  if (firstPoint) {

                     result = true;
                  }
               }
            }
         }
      }

      if (closestPoint && resultContainer.get_result_count () > 1) {

         IsectResult current;
         resultContainer.get_first (current);
         IsectResult closest (current);

         double closestDist;
         current.get_distance (closestDist);

         while (resultContainer.get_next (current)) {

            double testDist;
            if (current.get_distance (testDist)) {

               if (testDist < closestDist) {

                  closest = current;
               }
            }
         }

         IsectResultContainer closestContainer;
         closestContainer.add_result (closest);
         resultContainer = closestContainer;
      }
   }

   return resultContainer.get_result_count () > 0;
}


dmz::UInt32
dmz::RenderModuleIsectOSG::enable_isect (const Handle ObjectHandle) {

   UInt32 result (0);

   if (_core) {

      osg::Group *g (_core->lookup_dynamic_object (ObjectHandle));

      if (g) {

         RenderObjectDataOSG *data (
            dynamic_cast<RenderObjectDataOSG *> (g->getUserData ()));

         if (data) { result = UInt32 (data->enable_isect ()); }
      }
   }

   return result;
}


dmz::UInt32
dmz::RenderModuleIsectOSG::disable_isect (const Handle ObjectHandle) {

   UInt32 result (0);

   if (_core) {

      osg::Group *g (_core->lookup_dynamic_object (ObjectHandle));

      if (g) {

         RenderObjectDataOSG *data (
            dynamic_cast<RenderObjectDataOSG *> (g->getUserData ()));

         if (data) { result = UInt32 (data->disable_isect ()); }
      }
   }

   return result;
}


void
dmz::RenderModuleIsectOSG::_init (const Config &Local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModuleIsectOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModuleIsectOSG (Info, local);
}

};
