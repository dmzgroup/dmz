#include "dmzRenderModuleIsectOSG.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <osg/Node>
#include <osg/Group>

#include <osg/LineSegment>
#include <osg/Drawable>
#include <osg/CullFace>
//#include <osgSim/HeightAboveTerrain>
//#include <osgSim/ElevationSlice>

#include <osgUtil/IntersectVisitor>

#include <dmzTypesVector.h>
#include <dmzTypesString.h>

#include <dmzRenderUtilOSG.h>

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
dmz::RenderModuleIsectOSG::discover_plugin (const Plugin *PluginPtr) {

   if (!_core) {

      _core = RenderModuleCoreOSG::cast (PluginPtr);
   }
}


void
dmz::RenderModuleIsectOSG::start_plugin () {

}


void
dmz::RenderModuleIsectOSG::stop_plugin () {

}


void
dmz::RenderModuleIsectOSG::shutdown_plugin () {

}


void
dmz::RenderModuleIsectOSG::remove_plugin (const Plugin *PluginPtr) {

}


dmz::Boolean
dmz::RenderModuleIsectOSG::do_isect (
      const IsectParameters &Parameters,
      const IsectTestContainer &TestValues,
      IsectResultContainer &resultContainer) {
   if (_core) {
      osg::ref_ptr<osg::Group> scene = _core->get_scene ();

      osg::BoundingSphere bs = scene->getBound();

//      osgSim::LineOfSight los;
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
//      los.computeIntersections (scene.get ());

      IsectTestResultTypeEnum param = Parameters.get_test_result_type ();
      bool closestPoint = (param == IsectClosestPoint);
      bool firstPoint = (param == IsectFirstPoint);

      if (closestPoint && !Parameters.get_calculate_distance ()) {
         firstPoint = true;
         closestPoint = false;
      }

      Boolean result (False);

      for (unsigned int i=0; i<lsList.size () && !result; i++) {

         osgUtil::IntersectVisitor::HitList resultHits;
         resultHits = visitor.getHitList (lsList[i]);

         if (!resultHits.empty ()) {

//            int j;// j=0;
//            const osgSim::LineOfSight::Intersections& intersections = los.getIntersections (i);

//            osgSim::LineOfSight::Intersections::const_iterator itr;
//            for (itr = intersections.begin (); 
//                 itr != intersections.end () && !result; 
//                 itr++) {
            for (unsigned int j=0; j<resultHits.size (); j++) {

               osgUtil::Hit currentHit = resultHits[j];

               Vector lsPoint = to_dmz_vector (currentHit.getWorldIntersectPoint ());
               IsectResult lsResult;

               lsResult.set_isect_test_id (handleArray[i]);

               lsResult.set_point (lsPoint);

               if (Parameters.get_calculate_normal ()) { 

                  lsResult.set_normal (
                     to_dmz_vector (currentHit.getWorldIntersectNormal ()));
               }

               if (Parameters.get_calculate_distance ()) { 

                  lsResult.set_distance ((lsPoint - sourceArray[i]).magnitude ());
               }

               if (Parameters.get_calculate_cull_mode ()) { 

                  osg::Drawable *drawObject = (currentHit.getDrawable ());

                  osg::StateSet *sSet = (drawObject ? drawObject->getStateSet () : 0);

                  osg::CullFace *cf = 
                     (osg::CullFace*)(sSet ? sSet->getAttribute (osg::StateAttribute::CULLFACE) : 0);

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
//                  _log.out << "need a way to grab object handle" << endl;
               }

               resultContainer.add_result (lsResult);
               
               if (firstPoint) {

                  result = true;
               }
//               ++j;
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

/*
   if (_core) {
      osg::ref_ptr<osg::Group> scene = _core->get_scene ();

      osg::BoundingSphere bs = scene->getBound();

      osgSim::LineOfSight los;

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
         los.addLOS (to_osg_vector (vec1), to_osg_vector (vec2));
         handleArray.push_back (testHandle);
         sourceArray.push_back (vec1);
      } while (TestValues.get_next_test (testHandle, testType, vec1, vec2));

      los.computeIntersections (scene.get ());

      IsectTestResultTypeEnum param = Parameters.get_test_result_type ();
      bool closestPoint = (param == IsectClosestPoint);
      bool firstPoint = (param == IsectFirstPoint);

      if (closestPoint && !Parameters.get_calculate_distance ()) {
         firstPoint = true;
         closestPoint = false;
      }

      Boolean result (False);

      for (unsigned int i=0; i<los.getNumLOS () && !result; i++) {

         int j; j=0;
         const osgSim::LineOfSight::Intersections& intersections = los.getIntersections (i);

         osgSim::LineOfSight::Intersections::const_iterator itr;
         for ( itr = intersections.begin (); 
               itr != intersections.end () && !result; 
               itr++) {

            Vector losPoint = to_dmz_vector (intersections[j]);
            IsectResult losResult;

            losResult.set_isect_test_id (handleArray[i]);

            losResult.set_point (losPoint);

            if (Parameters.get_calculate_normal ()) { 

               Vector null (0.0, 0.0, 0.0);
               losResult.set_normal (null);
               // problem
               _log.out << "need a way to compute normals" << endl;
            }

            if (Parameters.get_calculate_distance ()) { 

               losResult.set_distance ((losPoint - sourceArray[i]).magnitude ());
            }

            if (Parameters.get_calculate_cull_mode ()) { 

               losResult.set_cull_mode (IsectPolygonBackCulledMask); 
               // problem
               _log.out << "need a way to read cull modes" << endl;
            }

            if (Parameters.get_calculate_object_handle ()) { 

               _log.out << "need a way to grab object handle" << endl;
            }

            resultContainer.add_result (losResult);
            
            if (firstPoint) {

               result = true;
            }
            ++j;
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
*/
}


dmz::UInt32
dmz::RenderModuleIsectOSG::enable_isect (const Handle ObjectHandle) {

   UInt32 result (0);

   return result;
}


dmz::UInt32
dmz::RenderModuleIsectOSG::disable_isect (const Handle ObjectHandle) {

   UInt32 result (0);

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
