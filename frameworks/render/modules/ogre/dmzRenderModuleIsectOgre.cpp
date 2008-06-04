#include "dmzRenderModuleIsectOgre.h"
#include <dmzRenderUtilOgre.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesVector.h>
#include <dmzTypesString.h>
#include <Ogre/Ogre.h>


Ogre::Vector3
nearest_point (
      const Ogre::Vector3 &Point1,
      const Ogre::Vector3 &Point2,
      const Ogre::Vector3 &TestPoint) { 

   Ogre::Vector3 A = TestPoint - Point1;
   Ogre::Vector3 u = (Point2-Point1).normalisedCopy ();
   
   return Point1 + (A.dotProduct (u)) * u;
}


struct dmz::RenderModuleIsectOgre::EntityIsectStruct {

   Ogre::Matrix4 worldMat;
   Ogre::Matrix3 worldMat3;

   Ogre::Matrix4 invWorldMat;
   Ogre::Matrix3 invWorldMat3;

   Ogre::Ray localRay;

   EntityIsectStruct () {;}
   ~EntityIsectStruct () {;}

   void set_data (const Ogre::Entity &Entity, const Ogre::Ray &WorldRay) {

      worldMat = Entity._getParentNodeFullTransform ();
      worldMat.extract3x3Matrix (worldMat3);
   
      invWorldMat = worldMat.inverse ();
      invWorldMat3 = worldMat3.Inverse ();
   
      Ogre::Vector3 localPos (invWorldMat * WorldRay.getOrigin ());
      Ogre::Vector3 localDir (invWorldMat3 * WorldRay.getDirection ());
      localDir.normalise ();
   
      localRay.setOrigin (localPos);
      localRay.setDirection (localDir);
   }
};


struct dmz::RenderModuleIsectOgre::MeshStruct {

   UInt32 maxVertexCount;
   UInt32 maxIndexCount;
   UInt32 vertexCount;
   UInt32 indexCount;
   Ogre::Vector3 *vertices;
   UInt32 *indices;

   MeshStruct () :
      maxVertexCount (0),
      maxIndexCount (0),
      vertexCount (0),
      indexCount (0),
      vertices (0),
      indices (0) {;}

   ~MeshStruct () {

      if (vertices) { delete []vertices; vertices = 0; }
      if (indices) { delete []indices; indices = 0; }
   }

   void resize (const UInt32 TheVertexCount, const UInt32 TheIndexCount) {

      vertexCount = TheVertexCount;
      indexCount = TheIndexCount;

      if (TheVertexCount > maxVertexCount) {
         
         if (vertices) { delete []vertices; }
         maxVertexCount = TheVertexCount;
         vertices = new Ogre::Vector3[maxVertexCount];
      }

      if (TheIndexCount > maxIndexCount) {

         if (indices) { delete []indices; }
         maxIndexCount = TheIndexCount;
         indices = new UInt32[maxIndexCount];
      }
   }
};


dmz::RenderModuleIsectOgre::RenderModuleIsectOgre (
      const PluginInfo &Info, 
      const Config &Local) :
      Plugin (Info),
      RenderModuleIsect (Info),
      _core (0),
      _log (Info),
      _sceneManager (0),
      _raySceneQuery (0),
      _isectRay (),
      _maxSegmentDistance (-1.0),
      _disabledTable (),
      _entityIsectData (*(new EntityIsectStruct ())),
      _cacheSceneData (True),
      _localMesh (*(new MeshStruct ())),
      _entityTable () {

   _init (Local);
}


dmz::RenderModuleIsectOgre::~RenderModuleIsectOgre () {

   _disabledTable.empty ();
   _entityTable.empty ();

   delete &_entityIsectData;
   delete &_localMesh;
}


void
dmz::RenderModuleIsectOgre::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOgre::cast (PluginPtr);

         if (_core) {

            _sceneManager = _core->get_scene_manager ();

            if (_sceneManager) {

               try {

                  _raySceneQuery =
                     _sceneManager->createRayQuery (
                        Ogre::Ray (),
                        Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);
               }
               catch (Ogre::Exception e) {

                  _raySceneQuery = 0;
                  _log.error << e.getFullDescription ().c_str () << endl;
               }
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOgre::cast (PluginPtr))) {

         if (_sceneManager) {

            _sceneManager->destroyQuery (_raySceneQuery);
            _raySceneQuery = 0;
            _sceneManager = 0;
         }

         _core = 0;
      }
   }
}


dmz::Boolean
dmz::RenderModuleIsectOgre::do_isect (
      const IsectParameters &Parameters,
      const IsectTestContainer &TestValues,
      IsectResultContainer &resultContainer) {

   resultContainer.clear ();

   if (_core && _raySceneQuery) {
      
      if (Parameters.get_test_result_type () == IsectClosestPoint) {

         _raySceneQuery->setSortByDistance (True);
      }
      else {
         
         _raySceneQuery->setSortByDistance (False);
      }
      
      UInt32 testHandle (0);
      IsectTestTypeEnum testType;
      Vector vec1, vec2;
      
      TestValues.get_first_test (testHandle, testType, vec1, vec2);
      do {

         _isectRay.setOrigin (to_ogre_vector (vec1));
         
         if (testType == IsectRayTest) {

            _maxSegmentDistance = -1.0;
            _isectRay.setDirection (to_ogre_vector (vec2));
         }
         else if (testType == IsectSegmentTest) {

            Ogre::Vector3 dirVec (to_ogre_vector (vec2 - vec1));
            _maxSegmentDistance = dirVec.length ();
            dirVec.normalise ();
            _isectRay.setDirection (dirVec);
         }
         
         _raySceneQuery->setRay (_isectRay);
         
         Boolean done (False);
         Ogre::RaySceneQueryResult &queryResult = _raySceneQuery->execute ();
         Ogre::RaySceneQueryResult::iterator it = queryResult.begin ();

         while ((it != queryResult.end ()) && !done) {

            Ogre::Entity *entity = dynamic_cast<Ogre::Entity *> (it->movable);

            if (entity) {

               Handle objHandle (_extract_object_handle (*entity));
   
               // only do intersection if no object handle was found or
               // if an object handle was found make sure it is not in the 
               // disabled table
               if (!objHandle || !_disabledTable.lookup (objHandle)) {

                  _isect_entity (
                     *entity,
                     testHandle,
                     Parameters,
                     resultContainer);

                  // no need to continue checking if all we want is the first point
                  if ((Parameters.get_test_result_type () == IsectFirstPoint) &&
                      resultContainer.get_result_count ()) { 

                     done = True;
                  }
               }
            }

            it++;
         }
      } while (TestValues.get_next_test (testHandle, testType, vec1, vec2));
   }

   // to find the closest point loop through all returned points and only
   // keep the closest one
   if (Parameters.get_test_result_type () == IsectClosestPoint &&
       resultContainer.get_result_count () > 1) {

      IsectResult closestResult;
      _find_closest_result (resultContainer, closestResult);

      resultContainer.clear ();
      resultContainer.add_result (closestResult);
   }

   return (resultContainer.get_result_count () > 0);
}


dmz::UInt32
dmz::RenderModuleIsectOgre::enable_isect (const Handle ObjectHandle) {

   UInt32 result (0);

   UInt32 *count (_disabledTable.lookup (ObjectHandle));

   if (count) {

      if (*count > 1) {

         (*count)--;
         result = *count;
      }
      else { 

         _disabledTable.remove (ObjectHandle);
         delete count; count = 0;
      }
   }

   return result;
}


dmz::UInt32
dmz::RenderModuleIsectOgre::disable_isect (const Handle ObjectHandle) {

   UInt32 result (0);

   UInt32 *count (_disabledTable.lookup (ObjectHandle));

   if (count) {

      (*count)++;
      result = *count;
   }
   else {

      _disabledTable.store (ObjectHandle, new UInt32 (1));
      result = 1;
   }

   return result;
}


dmz::Handle
dmz::RenderModuleIsectOgre::_extract_object_handle (const Ogre::Entity &Entity) {

   Handle objHandle (0);

   // if the mesh belongs to a object in the object module then
   // its handle was stashed in the entity class so lets retrieve it 
   Ogre::Any userAny (Entity.getUserAny ());
   if (!userAny.isEmpty ()) {
      
      try { objHandle = Ogre::any_cast<Handle> (userAny); }
      catch (Ogre::Exception e) { objHandle = 0; }
   }

   return objHandle;
}


void
dmz::RenderModuleIsectOgre::_isect_entity (
      const Ogre::Entity &Entity,
      const UInt32 TestHandle,
      const IsectParameters &Parameters,
      IsectResultContainer &resultContainer) {

   Boolean doEntityIsect (True);

   if (_maxSegmentDistance > 0.0) {

      // Determain if the entity's bounding sphere is outside
      // of the itersection segment. If it is then lets skip it to save
      // on polygon level intersection checks

      Ogre::Sphere boundingSphere (Entity.getWorldBoundingSphere ());

      Ogre::Vector3 segmentStart (_isectRay.getOrigin ());
      Ogre::Vector3 segmentEnd (_isectRay.getPoint (_maxSegmentDistance));

      Ogre::Vector3 pointOnRay (
         nearest_point (
            segmentStart,
            segmentEnd,
            boundingSphere.getCenter ()));

      Ogre::Real radiusSquared (boundingSphere.getRadius () * boundingSphere.getRadius ());
      Ogre::Real distToPointOnRaySquared ((pointOnRay - segmentEnd).squaredLength ());

      if (distToPointOnRaySquared > radiusSquared) {

         doEntityIsect = False;
      }
   }

   if (doEntityIsect) {

      _entityIsectData.set_data (Entity, _isectRay);

      for (UInt32 sei = 0; sei < Entity.getNumSubEntities (); sei++) {

         _isect_sub_entity (
            Entity,
            sei,
            TestHandle,
            Parameters,
            resultContainer);

         if ((Parameters.get_test_result_type () == IsectFirstPoint) &&
             resultContainer.get_result_count ()) { break; }
      }
   }
}


void
dmz::RenderModuleIsectOgre::_isect_sub_entity (
      const Ogre::Entity &Entity,
      const UInt32 SubEntityIndex,
      const UInt32 TestHandle,
      const IsectParameters &Parameters,
      IsectResultContainer &resultContainer) {

   Ogre::SubEntity *subEntity = Entity.getSubEntity (SubEntityIndex);

   MeshStruct *currentMesh (_get_current_mesh_data (Entity, SubEntityIndex));

   if (subEntity && currentMesh) {

      Ogre::Vector3 v1, v2, normal, point;
   
      // test for hitting individual triangles on the mesh
      for (UInt32 ix = 0; ix < currentMesh->indexCount; ix += 3) {
   
         v1 = currentMesh->vertices[currentMesh->indices[ix]] -
              currentMesh->vertices[currentMesh->indices[ix+1]];
   
         v2 = currentMesh->vertices[currentMesh->indices[ix+2]] -
              currentMesh->vertices[currentMesh->indices[ix+1]];
   
         normal = v2.crossProduct (v1);
         normal.normalise ();
   
         // check for a hit against this triangle
         std::pair<bool, Ogre::Real> hit =
            Ogre::Math::intersects(
               _entityIsectData.localRay,
               currentMesh->vertices[currentMesh->indices[ix]],
               currentMesh->vertices[currentMesh->indices[ix+1]],
               currentMesh->vertices[currentMesh->indices[ix+2]],
               normal,
               true,
               true);
   
         if (hit.first) {
   
            if (_maxSegmentDistance < 0.0 || hit.second < _maxSegmentDistance) {
   
               IsectResult result;
   
               result.set_isect_test_id (TestHandle);
   
               if (Parameters.get_calculate_object_handle ()) {
   
                  result.set_object_handle (_extract_object_handle (Entity));
               }
   
               point =
                  _entityIsectData.worldMat * _entityIsectData.localRay.getPoint (hit.second);
   
               result.set_point (to_dmz_vector (point));
   
               // we need to save distance if test type is closest point
               // because we need to find the closest point once all points are found
               if (Parameters.get_calculate_distance () ||
                   Parameters.get_test_result_type () == IsectClosestPoint) {
   
                  result.set_distance (hit.second);
               }
   
               if (Parameters.get_calculate_normal ()) { 
   
                  normal = _entityIsectData.worldMat3 * normal;
                  normal.normalise ();
   
                  result.set_normal (to_dmz_vector (normal));
               }
   
               if (Parameters.get_calculate_cull_mode ()) { 
   
                  Ogre::Technique *technique (subEntity->getTechnique ());
                  if (technique) {
   
                     Ogre::Pass *pass0 (technique->getPass (0));
                     if (pass0) {
   
                        UInt32 cullMask (0);
   
                        if (pass0->getCullingMode () == Ogre::CULL_CLOCKWISE) {
   
                           cullMask |= IsectPolygonBackCulledMask;
                        }
                        else if (pass0->getCullingMode () == Ogre::CULL_ANTICLOCKWISE) {
   
                              cullMask |= IsectPolygonFrontCulledMask;
                        }
   
                        result.set_cull_mode (cullMask); 
                     }
                  }
               }
   
               resultContainer.add_result (result);
   
               if (Parameters.get_test_result_type () == IsectFirstPoint) { break; }
            }
         }
      }
   }
}


void
dmz::RenderModuleIsectOgre::_find_closest_result (
      IsectResultContainer &container,
      IsectResult &closest) {

   if (container.get_result_count () > 1) {

      IsectResult result;
      Boolean found (container.get_first (result));

      Float64 closestDist (-1.0);
      Float64 currentDist (0.0);
      result.get_distance (currentDist);

      while (found) {

         if (closestDist < 0.0f || currentDist < closestDist) {

            closestDist = currentDist;
            closest = result;
         }

         found = container.get_next (result);
      }
   }
}


dmz::RenderModuleIsectOgre::MeshStruct *
dmz::RenderModuleIsectOgre::_get_current_mesh_data (
      const Ogre::Entity &Entity,
      const UInt32 SubEntityIndex) {

   MeshStruct *meshData (0);

   Ogre::SubEntity *subEntity = Entity.getSubEntity (SubEntityIndex);

   if (subEntity) {

      if (_cacheSceneData && !_extract_object_handle (Entity)) {

         const String EntityName (Entity.getName ().c_str ());

         EntityStruct *entityData = _entityTable.lookup (EntityName);

         if (!entityData) {

            entityData = new EntityStruct ();
            _entityTable.store (EntityName, entityData);
         }

         if (entityData) {
            
            meshData = entityData->meshTable.lookup (SubEntityIndex);

            if (!meshData) {

               meshData = new MeshStruct ();

               entityData->meshTable.store (SubEntityIndex, meshData);

               __get_mesh_information (subEntity->getSubMesh (), *meshData);
            }
         }
      }
      else {

         __get_mesh_information (subEntity->getSubMesh (), _localMesh);

         meshData = &_localMesh;
      }
   }

   return meshData;
}


// Get the mesh information for the given mesh.
// Code found on this forum link: http://www.ogre3d.org/wiki/index.php/RetrieveVertexData
void
dmz::RenderModuleIsectOgre::__get_mesh_information (
      Ogre::SubMesh *subMesh,
      MeshStruct &meshStruct) {

   size_t current_offset = 0;
   size_t next_offset = 0;
   size_t index_offset = 0;

   UInt32 vertexCount = 0;
   UInt32 indexCount = 0;

   if (subMesh) {

      if (subMesh->useSharedVertices) {

         vertexCount = subMesh->parent->sharedVertexData->vertexCount;
      }
      else {

         vertexCount = subMesh->vertexData->vertexCount;
      }

      indexCount = subMesh->indexData->indexCount;

      if (vertexCount && indexCount) {

         meshStruct.resize (vertexCount, indexCount);
      
         Ogre::VertexData* vertex_data =
            subMesh->useSharedVertices ? subMesh->parent->sharedVertexData : 
                                         subMesh->vertexData;
      
         const Ogre::VertexElement *posElem =
            vertex_data->vertexDeclaration->findElementBySemantic (Ogre::VES_POSITION);
      
         Ogre::HardwareVertexBufferSharedPtr vbuf =
             vertex_data->vertexBufferBinding->getBuffer (posElem->getSource ());
      
         unsigned char* vertex =
            static_cast<unsigned char*> (vbuf->lock (Ogre::HardwareBuffer::HBL_READ_ONLY));
      
         // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a
         // double as second argument. So make it float, to avoid trouble when
         // Ogre::Real will be comiled/typedefed as double:
         //      Ogre::Real* pReal;
         float* pReal;
      
         for (size_t j = 0; j < vertex_data->vertexCount;
              ++j, vertex += vbuf->getVertexSize ()) {
   
            posElem->baseVertexPointerToElement (vertex, &pReal);
            Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
            meshStruct.vertices[current_offset + j] = pt;
         }
      
         vbuf->unlock();
      }
      
      Ogre::IndexData* index_data = subMesh->indexData;
      size_t numTris = index_data->indexCount / 3;
      Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
      
      bool use32bitindexes = (ibuf->getType () == Ogre::HardwareIndexBuffer::IT_32BIT);
      
      unsigned long*  pLong =
        static_cast<unsigned long*> (ibuf->lock (Ogre::HardwareBuffer::HBL_READ_ONLY));
                 
      unsigned short* pShort = reinterpret_cast<unsigned short*> (pLong);
      
      size_t offset = current_offset;
      
      if (use32bitindexes) {
   
         for (size_t k = 0; k < numTris*3; ++k) {
   
            meshStruct.indices[index_offset++] =
               pLong[k] + static_cast<unsigned long> (offset);
         }
      }
      else {
   
         for (size_t k = 0; k < numTris*3; ++k) {
   
            meshStruct.indices[index_offset++] = 
               static_cast<unsigned long> (pShort[k]) +
               static_cast<unsigned long> (offset);
         }
      }
      
      ibuf->unlock();
   }
}

void 
dmz::RenderModuleIsectOgre::_init (const Config &Local) {

   _cacheSceneData = config_to_boolean ("scene.useCache", Local, _cacheSceneData);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModuleIsectOgre (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModuleIsectOgre (Info, local);
}

};

