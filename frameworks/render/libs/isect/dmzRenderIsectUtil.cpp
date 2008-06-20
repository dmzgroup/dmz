
#include <dmzRenderIsect.h>
#include <dmzRenderIsectUtil.h>
#include <dmzRenderModuleIsect.h>
#include <dmzTypesVector.h>

using namespace dmz;

namespace {

static const Vector Up (0.0, 1.0, 0.0);
static const Vector Down (0.0, -1.0, 0.0);

};


/*!

\brief Finds a valid intersection point from intersection results.
\ingroup Render

*/
dmz::Boolean
dmz::isect_validate_point (
      const Vector &Start,
      const Vector &Dir,
      const IsectResultContainer &IsectResults,
      Vector &point,
      Vector &normal) {

   Boolean result (False);

   const Vector Up (0.0, 1.0, 0.0);

   const Boolean TestingUp (Dir.get_angle (Up) < HalfPi64 ? True : False);

   IsectResult isResult;

   Boolean found (IsectResults.get_first (isResult));

   while (found) {

      Vector cpoint, cnormal;

      Handle handle (0);

      isResult.get_object_handle (handle);

      if (!handle && isResult.get_point (cpoint) && isResult.get_normal (cnormal)) {

         if ((!result) ||
               ((Start - point).magnitude_squared () >
                  (Start - cpoint).magnitude_squared ())) {

            Handle cullMode (0);

            isResult.get_cull_mode (cullMode);

            if (!(cullMode & IsectPolygonBackCulledMask) &&
                  !(cullMode & IsectPolygonFrontCulledMask)) {

               if (Dir.get_angle (cnormal) < HalfPi64) {

                  cnormal = -cnormal;
               }
            }
            else if (cullMode & IsectPolygonFrontCulledMask) { cnormal = -cnormal; }

            if (TestingUp) {

               if (Dir.get_angle (cnormal) < HalfPi64) {

                  result = True;
                  point = cpoint;
                  normal = cnormal;
               }
            }
            else {

               if (Dir.get_angle (cnormal) > HalfPi64) {

                  result = True;
                  point = cpoint;
                  normal = cnormal;
               }
            }
         }
      }

      found = IsectResults.get_next (isResult);
   }

   return result;
}


/*!

\brief Clamp point to scene.
\ingroup Render

*/
dmz::Boolean
dmz::isect_clamp_point (
      const Vector &Value,
      RenderModuleIsect &isect,
      Vector &point,
      Vector &normal) {

   Boolean result (False);
   Vector offset (0.0, 1.5, 0.0);
   Vector start (Value + offset);

   IsectParameters parameters;
   IsectTestContainer test;
   IsectResultContainer isectResults;

   test.set_test (1, IsectRayTest, start, Down);

   if (isect.do_isect (parameters, test, isectResults)) {

      result = isect_validate_point (Value, Down, isectResults, point, normal);
   }

   if (!result) {

      start = Value - offset;

      test.set_test (1, IsectRayTest, start, Up);

      if (isect.do_isect (parameters, test, isectResults)) {

         result = isect_validate_point (Value, Up, isectResults, point, normal);
      }
   }

   return result;
}
