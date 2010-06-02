#ifndef DMZ_RENDER_ISECT_DOT_H
#define DMZ_RENDER_ISECT_DOT_H

#include <dmzRenderIsectExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class HandleContainer;

   //! \addtogroup Render
   //! @{

   //! \brief Intersection test type.
   enum IsectTestTypeEnum {

      IsectRayTest, //!< Ray intersection test.
      IsectSegmentTest, //!< Segment intersection test.
      IsectUnknownTest, //!< Unknown intersection test type.
   };

   //! \brief Intersection result type.
   enum IsectTestResultTypeEnum {

      IsectFirstPoint, //!< Returns first intersection point found.
      IsectClosestPoint, //!< Return closest intersection point.
      IsectAllPoints, //!< Return all intersection points.
      IsectUnknownType, //!< Unknown result type.
   };

   //! Polygon is back faced culled.
   DMZ_RENDER_ISECT_LINK_SYMBOL extern const UInt32 IsectPolygonBackCulledMask;
   //! Polygon is front faced culled.
   DMZ_RENDER_ISECT_LINK_SYMBOL extern const UInt32 IsectPolygonFrontCulledMask;
   //! Polygon is invisible.
   DMZ_RENDER_ISECT_LINK_SYMBOL extern const UInt32 IsectPolygonInvisibleMask;

   //! @}

   class Vector;

   class DMZ_RENDER_ISECT_LINK_SYMBOL IsectTestContainer {

      public:
         IsectTestContainer ();
         IsectTestContainer (const IsectTestContainer &Value);
         ~IsectTestContainer ();

         IsectTestContainer &operator= (const IsectTestContainer &Value);

         void clear ();

         Boolean set_test (
            const UInt32 TestID,
            const IsectTestTypeEnum TestType,
            const Vector &Value1,
            const Vector &Value2);

         Boolean set_ray_test (
            const UInt32 TestID,
            const Vector &Position,
            const Vector &Direction);

         Boolean set_segment_test (
            const UInt32 TestID,
            const Vector &StartPoint,
            const Vector &EndPoint);

         UInt32 add_test (
            const IsectTestTypeEnum TestType,
            const Vector &Value1,
            const Vector &Value2);

         UInt32 add_ray_test (const Vector &Position, const Vector &Direction);
         UInt32 add_segment_test (const Vector &StartPoint, const Vector &EndPoint);

         Boolean remove_test (const UInt32 TestID);

         Boolean lookup_test (
            const UInt32 TestID,
            IsectTestTypeEnum &testType,
            Vector &value1,
            Vector &value2) const;

         Boolean get_first_test (
            UInt32 &testID,
            IsectTestTypeEnum &testType,
            Vector &value1,
            Vector &value2) const;

         Boolean get_next_test (
            UInt32 &testID,
            IsectTestTypeEnum &testType,
            Vector &value1,
            Vector &value2) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   class DMZ_RENDER_ISECT_LINK_SYMBOL IsectParameters {

      public:
         IsectParameters ();
         IsectParameters (const IsectParameters &Value);
         ~IsectParameters ();

         IsectParameters &operator= (const IsectParameters &Value);

         void set_test_result_type (const IsectTestResultTypeEnum Value);
         IsectTestResultTypeEnum get_test_result_type () const;

         void set_calculate_normal (const Boolean Value);
         Boolean get_calculate_normal () const;

         void set_calculate_object_handle (const Boolean Value);
         Boolean get_calculate_object_handle () const;

         void set_calculate_distance (const Boolean Value);
         Boolean get_calculate_distance () const;

         void set_calculate_cull_mode (const Boolean Value);
         Boolean get_calculate_cull_mode () const;

         void set_isect_attributes (const HandleContainer &Attr);
         Boolean get_isect_attributes (HandleContainer &attr) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   class DMZ_RENDER_ISECT_LINK_SYMBOL IsectResult {

      public:
         IsectResult ();
         IsectResult (const UInt32 TestID);
         IsectResult (const IsectResult &Value);
         ~IsectResult ();

         IsectResult &operator= (const IsectResult &Value);

         void set_isect_test_id (const UInt32 TestID);
         UInt32 get_isect_test_id () const;

         void set_point (const Vector &Value);
         Boolean get_point (Vector &value) const;

         void set_normal (const Vector &Value);
         Boolean get_normal (Vector &value) const;

         void set_object_handle (const Handle Value);
         Boolean get_object_handle (Handle &value) const;

         void set_distance (const Float64 Value);
         Boolean get_distance (Float64 &value) const;

         void set_cull_mode (const UInt32 Value);
         Boolean get_cull_mode (UInt32 &value) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   class DMZ_RENDER_ISECT_LINK_SYMBOL IsectResultContainer {

      public:
         IsectResultContainer ();
         IsectResultContainer (const IsectResultContainer &Value);
         ~IsectResultContainer ();

         IsectResultContainer &operator= (const IsectResultContainer &Value);

         Int32 get_result_count () const;

         void clear ();
         void reset ();

         void add_result (const IsectResult &value);

         Boolean get_first (IsectResult &value) const;
         Boolean get_next (IsectResult &value) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

#endif // DMZ_RENDER_ISECT_DOT_H
