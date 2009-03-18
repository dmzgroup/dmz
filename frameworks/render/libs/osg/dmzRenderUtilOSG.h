#ifndef DMZ_RENDER_UTIL_OSG_DOT_H
#define DMZ_RENDER_UTIL_OSG_DOT_H

#include <dmzRenderUtilOSGExport.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>

#include <osg/Vec3d>
#include <osg/Matrixd>

namespace dmz {

   inline osg::Vec3d
   to_osg_vector (const Vector &Source) {

      osg::Vec3d result (Source.get_x (), Source.get_y (), Source.get_z ());
      return result;
   }


   inline Vector
   to_dmz_vector (const osg::Vec3d &Source) {

      Vector result (Source.x (), Source.y (), Source.z ());
      return result;
   }


   inline osg::Matrixd
   to_osg_matrix (const Matrix &Source) {

      Float64 data[9];
      Source.to_array (data);
      osg::Matrixd result (
         //data[0], data[1], data[2], 0.0,
         //data[3], data[4], data[5], 0.0,
         //data[6], data[7], data[8], 0.0,
         data[0], data[3], data[6], 0.0,
         data[1], data[4], data[7], 0.0,
         data[2], data[5], data[8], 0.0,
         0.0,     0.0,     0.0,     1.0);

      return result;
   }

   inline osg::Matrixd
   to_osg_matrix (const Matrix &Source, const Vector &Trans) {

      Float64 data[9];
      Source.to_array (data);
      osg::Matrixd result (
         //data[0], data[1], data[2], 0.0,
         //data[3], data[4], data[5], 0.0,
         //data[6], data[7], data[8], 0.0,
         data[0], data[3], data[6], 0.0,
         data[1], data[4], data[7], 0.0,
         data[2], data[5], data[8], 0.0,
         Trans.get_x (), Trans.get_y (), Trans.get_z (), 1.0);

      return result;
   }


   inline osg::Matrixd
   to_osg_inverse_matrix (const Matrix &Source) {

      Float64 data[9];
      Source.to_array (data);
      const osg::Matrixd result (
         //data[0], data[3], data[6], 0.0,
         //data[1], data[4], data[7], 0.0,
         //data[2], data[5], data[8], 0.0,
         data[0], data[1], data[2], 0.0,
         data[3], data[4], data[5], 0.0,
         data[6], data[7], data[8], 0.0,
         0.0,     0.0,     0.0,     1.0);

      return result;
   }

   DMZ_RENDER_UTIL_OSG_LINK_SYMBOL UInt32 convert_osg_key_to_dmz_key (const UInt32 Key);
}

#endif // DMZ_RENDER_UTIL_OSG_DOT_H
