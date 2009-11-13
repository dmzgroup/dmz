#ifndef DMZ_RENDER_UTIL_OSG_DOT_H
#define DMZ_RENDER_UTIL_OSG_DOT_H

#include <dmzRenderUtilOSGExport.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>

#include <osg/Vec3d>
#include <osg/Matrixd>

namespace dmz {

typedef osg::Vec3d (*to_osg_vector_func) (const Vector &Source);
typedef Vector (*to_dmz_vector_func) (const osg::Vec3d &Source);
typedef osg::Matrixd (*to_osg_matrix_func) (const Matrix &Source, const Vector &Trans);
typedef osg::Matrixd (*to_osg_inverse_matrix_func) (const Matrix &Source);

DMZ_RENDER_UTIL_OSG_LINK_SYMBOL extern to_osg_vector_func to_osg_vector;
DMZ_RENDER_UTIL_OSG_LINK_SYMBOL extern to_dmz_vector_func to_dmz_vector;
DMZ_RENDER_UTIL_OSG_LINK_SYMBOL extern to_osg_matrix_func to_osg_matrix;
DMZ_RENDER_UTIL_OSG_LINK_SYMBOL extern to_osg_inverse_matrix_func to_osg_inverse_matrix;

DMZ_RENDER_UTIL_OSG_LINK_SYMBOL void set_osg_z_up ();
DMZ_RENDER_UTIL_OSG_LINK_SYMBOL void set_osg_y_up ();

DMZ_RENDER_UTIL_OSG_LINK_SYMBOL UInt32 convert_osg_key_to_dmz_key (const UInt32 Key);

}

#endif // DMZ_RENDER_UTIL_OSG_DOT_H
