#ifndef DMZ_RENDER_CAMERA_MANIPULATOR_OSG_DOT_H
#define DMZ_RENDER_CAMERA_MANIPULATOR_OSG_DOT_H

#include <dmzRenderUtilOSG.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>

#include <osgGA/MatrixManipulator>

namespace dmz {

   class RenderCameraManipulatorOSG :
         public osgGA::MatrixManipulator {

      public:
         RenderCameraManipulatorOSG ();
         ~RenderCameraManipulatorOSG ();

         // set the position of the matrix manipulator using a 4x4 Matrix
         virtual void setByMatrix(const osg::Matrixd& matrix) {;}

         // set the position of the matrix manipulator using a 4x4 Matrix
         virtual void setByInverseMatrix(const osg::Matrixd& matrix) {;}

         // get the position of the manipulator as 4x4 Matrix
         virtual osg::Matrixd getMatrix() const {

            const osg::Matrixd rotation (to_osg_matrix (_rotation));
            const osg::Matrixd result (osg::Matrixd::translate (_homeCenter) * rotation);
            return result;
         }

         // get the position of the manipulator as a inverse matrix of the manipulator,
         // typically used as a model view matrix
         virtual osg::Matrixd getInverseMatrix() const {

            const osg::Matrixd rotation (to_osg_inverse_matrix (_rotation));
            const osg::Matrixd result (osg::Matrixd::translate (-_homeCenter) * rotation);
            return result;
         }

         void setByVectorAndMatrix (const Vector &Pos, const Matrix &Ori) {

            _rotation = Ori;
            _homeCenter.set (to_osg_vector (Pos));
         }

         void getVectorAndMatrix (Vector &pos, Matrix &ori) {

            ori = _rotation;
            pos = to_dmz_vector (_homeCenter);
         }

      protected:
         Matrix _rotation;
   };
};

inline
dmz::RenderCameraManipulatorOSG::RenderCameraManipulatorOSG () :
      osgGA::MatrixManipulator (), _rotation () {;}


inline
dmz::RenderCameraManipulatorOSG::~RenderCameraManipulatorOSG () {;}


#endif //  DMZ_RENDER_CAMERA_MANIPULATOR_OSG_DOT_H
