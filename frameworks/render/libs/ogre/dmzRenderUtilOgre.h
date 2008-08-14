#ifndef DMZ_RENDER_UTIL_OGRE_DOT_H
#define DMZ_RENDER_UTIL_OGRE_DOT_H

#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>

#include <Ogre/OgreMath.h>
#include <Ogre/OgreVector3.h>
#include <Ogre/OgreMatrix3.h>
#include <Ogre/OgreQuaternion.h>


namespace dmz {

   // Vector
   inline Ogre::Vector3
   to_ogre_vector (const Vector &Source) {

      Ogre::Vector3 result (Source.get_x (), Source.get_y (), Source.get_z ());
      return result;
   }


   inline Vector
   to_dmz_vector (const Ogre::Vector3 &Source) {

      Vector result (Source.x, Source.y, Source.z);
      return result;
   }


   // Matrix
   inline Ogre::Matrix3
   to_ogre_matrix (const Matrix &Source) {

      Float64 data[9];
      Source.to_array (data);

      Ogre::Matrix3 result (
         data[0], data[1], data[2],
         data[3], data[4], data[5],
         data[6], data[7], data[8]);

      return result;
   }


   // Quaternion
   inline Ogre::Quaternion
   to_ogre_quaternion (const Matrix &Source) {

      Ogre::Quaternion result (to_ogre_matrix (Source));
      return result;
   }
}

#endif // DMZ_RENDER_UTIL_OGRE_DOT_H
