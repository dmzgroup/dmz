#include <dmzRenderIsect.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <dmzTypesVector.h>

//! \cond
const dmz::UInt32 dmz::IsectPolygonBackCulledMask (0x01);
const dmz::UInt32 dmz::IsectPolygonFrontCulledMask (0x02);
const dmz::UInt32 dmz::IsectPolygonInvisibleMask (0x04);
//! \endcond

namespace {

   struct testStruct {

      dmz::UInt32 testID;
      dmz::IsectTestTypeEnum type;
      dmz::Vector pt1;
      dmz::Vector pt2;

      testStruct &operator= (const testStruct &Value) {

         testID = Value.testID;
         type = Value.type;
         pt1 = Value.pt1;
         pt2 = Value.pt2;

         return *this;
      }

      testStruct () : testID (0), type (dmz::IsectUnknownTest) {;}

      testStruct (
            const dmz::UInt32 TestID,
            const dmz::IsectTestTypeEnum TheType,
            const dmz::Vector &ThePt1,
            const dmz::Vector &ThePt2) :
            testID (TestID),
            type (TheType),
            pt1 (ThePt1),
            pt2 (ThePt2) {;}

      testStruct (const testStruct &Value) : testID (0), type (dmz::IsectUnknownTest) {

         *this = Value;
      }
   };
};


/*!

\class dmz::IsectTestContainer
\ingroup Render
\brief Intersection test container.
\details The intersection test container stores multiple intersection tests so that
the tests may be batch processed if the underlying implementation supports it.
Intersection test may use either rays or segments.

*/

struct dmz::IsectTestContainer::State {

   UInt32 count;
   HashTableUInt32Iterator it;
   HashTableUInt32Template<testStruct> table;

   State () : count (0) {;}
   ~State () { table.empty (); }
   void clear () { it.reset (); count = 0; table.empty (); }
};


//! Constructor.
dmz::IsectTestContainer::IsectTestContainer () : _state (*(new State)) {;}


//! Copy constructor.
dmz::IsectTestContainer::IsectTestContainer (const IsectTestContainer &Value) :
      _state (*(new State)) { *this = Value; }


//! Destructor.
dmz::IsectTestContainer::~IsectTestContainer () { delete &_state; }


//! Assignment operator.
dmz::IsectTestContainer &
dmz::IsectTestContainer::operator= (const IsectTestContainer &Value) {

   _state.count = Value._state.count;
   _state.table.empty ();
   _state.table.copy (Value._state.table);

   return *this;
}


//! Clear container.
void
dmz::IsectTestContainer::clear () { _state.clear (); }


/*!

\brief Adds or updates an intersection test in the container.
\param[in] TestID Intersection test identification value. ID values do not need to
be sequential.
\param[in] TestType Type intersection test to perform. Defines how \a Value1 and
\a Value2 are interpreted.
\param[in] Value1 The start point.
\param[in] Value2 Either the ray's direction or the end of the segment.
\return Returns dmz::True if the test was successfully stored in the container.
\sa dmz::IsectTestTypeEnum

*/
dmz::Boolean
dmz::IsectTestContainer::set_test (
      const UInt32 TestID,
      const IsectTestTypeEnum TestType,
      const Vector &Value1,
      const Vector &Value2) {

   Boolean result (False);

   testStruct *ts = _state.table.lookup (TestID);

   if (!ts) {

      ts = new testStruct (TestID, TestType, Value1, Value2);
      if (_state.table.store (TestID, ts)) { result = True; }
      else {  delete ts; ts = 0; }
   }
   else { ts->type = TestType; ts->pt1 = Value1; ts->pt2 = Value2; result = True; }

   return result;
}


/*!

\brief Adds or updates a ray intersection test in the container.
\param[in] TestID Intersection test identification value. ID values do not need to
be sequential.
\param[in] Position Starting point of the ray.
\param[in] Direction Unit vector containing the direction of the ray.
\return Returns dmz::True if the ray test was successfully stored in the container.

*/
dmz::Boolean
dmz::IsectTestContainer::set_ray_test (
      const UInt32 TestID,
      const Vector &Position,
      const Vector &Direction) {

   return set_test (TestID, IsectRayTest, Position, Direction);
}


/*!

\brief Adds or updates a segment intersection test in the container.
\param[in] TestID Intersection test identification value. ID values do not need to
be sequential.
\param[in] StartPoint Starting point of the segment.
\param[in] EndPoint End point of the segment.
\return Returns dmz::True if the segment test was successfully stored in the container.

*/
dmz::Boolean
dmz::IsectTestContainer::set_segment_test (
      const UInt32 TestID,
      const Vector &StartPoint,
      const Vector &EndPoint) {

   return set_test (TestID, IsectSegmentTest, StartPoint, EndPoint);
}


/*!

\brief Adds an intersection test in the container.
\param[in] TestType Type intersection test to perform. Defines how \a Value1 and
\a Value2 are interpreted.
\param[in] Value1 The start point.
\param[in] Value2 Either the ray's direction or the end of the segment.
\return Returns the test ID. Returns zero if the test was not added.
\sa dmz::IsectTestTypeEnum

*/
dmz::UInt32
dmz::IsectTestContainer::add_test (
      const IsectTestTypeEnum TestType,
      const Vector &Value1,
      const Vector &Value2) {

   UInt32 result (0);

   testStruct *ts (new testStruct (0, TestType, Value1, Value2));

   if (ts) {

      _state.count++;

      if (!_state.count) { _state.count++; }

      if (_state.table.store (_state.count, ts)) {

          result = ts->testID = _state.count;
      }
      else {

         const UInt32 Start (_state.count);

         _state.count++;
         if (!_state.count) { _state.count++; }

         while ((_state.count != Start) && _state.table.lookup (_state.count)) {

            _state.count++;
            if (!_state.count) { _state.count++; }
         }

         if (_state.count != Start) {

            if (_state.table.store (_state.count, ts)) {

               result = ts->testID = _state.count;
            }
            else { delete ts; ts = 0; }
         }
      }
   }

   return result;
}


/*!

\brief Adds a ray intersection test in the container.
\param[in] Position Starting point of the ray.
\param[in] Direction Unit vector containing the direction of the ray.
\return Returns the test ID. Returns zero if the test was not added.

*/
dmz::UInt32
dmz::IsectTestContainer::add_ray_test (const Vector &Position, const Vector &Direction) {

   return add_test (IsectRayTest, Position, Direction);
}


/*!

\brief Adds a segment intersection test in the container.
\param[in] StartPoint Starting point of the segment.
\param[in] EndPoint End point of the segment.
\return Returns the test ID. Returns zero if the test was not added.

*/
dmz::UInt32
dmz::IsectTestContainer::add_segment_test (
      const Vector &StartPoint,
      const Vector &EndPoint) {

   return add_test (IsectSegmentTest, StartPoint, EndPoint);
}


/*!

\brief Removes intersection test.
\param[in] TestID Intersection test ID value.
\return Returns dmz::True if the test was successfully removed from the container.

*/
dmz::Boolean
dmz::IsectTestContainer::remove_test (const UInt32 TestID) {

   Boolean result (False);

   testStruct *ts = _state.table.remove (TestID);

   if (ts) { result = True; delete ts; ts = 0; }

   return result;
}


/*!

\brief Looks up intersection test.
\param[in] TestID Intersection test ID value.
\param[out] testType The type of the test.
\param[out] value1 First Vector value of the intersection test.
\param[out] value2 Second Vector value of the intersection test.
\return Returns dmz::True if the intersection test specified by \a TestID is found.

*/
dmz::Boolean
dmz::IsectTestContainer::lookup_test (
      const UInt32 TestID,
      IsectTestTypeEnum &testType,
      Vector &value1,
      Vector &value2) const {

   Boolean result (False);

   testStruct *ts = _state.table.lookup (TestID);

   if (ts) {

      testType = ts->type;
      value1 = ts->pt1;
      value2 = ts->pt2;
      result = True;
   }

   return result;
}


/*!

\brief Gets first intersection test.
\param[out] testID Intersection test ID value.
\param[out] testType The type of the test.
\param[out] value1 First Vector value of the intersection test.
\param[out] value2 Second Vector value of the intersection test.
\return Returns dmz::True if the first intersection test is returned. Returns dmz::False
if the container is empty.

*/
dmz::Boolean
dmz::IsectTestContainer::get_first_test (
      UInt32 &testID,
      IsectTestTypeEnum &testType,
      Vector &value1,
      Vector &value2) const {

   Boolean result (False);

   testStruct *ts = _state.table.get_first (_state.it);

   if (ts) {

      testID = ts->testID;
      testType = ts->type;
      value1 = ts->pt1;
      value2 = ts->pt2;
      result = True;
   }

   return result;
}


/*!

\brief Gets next intersection test.
\param[out] testID Intersection test ID value.
\param[out] testType The type of the test.
\param[out] value1 First Vector value of the intersection test.
\param[out] value2 Second Vector value of the intersection test.
\return Returns dmz::True if the next intersection test is returned. Returns dmz::False
if the container has returned all intersection tests.

*/
dmz::Boolean
dmz::IsectTestContainer::get_next_test (
      UInt32 &testID,
      IsectTestTypeEnum &testType,
      Vector &value1,
      Vector &value2) const {

   Boolean result (False);

   testStruct *ts = _state.table.get_next (_state.it);

   if (ts) {

      testID = ts->testID;
      testType = ts->type;
      value1 = ts->pt1;
      value2 = ts->pt2;
      result = True;
   }

   return result;
}


/*!

\class dmz::IsectParameters
\ingroup Render
\brief Intersection test parameters.
\details The dmz::IsectParameters specifies what results should be collect from the
intersection test. Bu default, all parameters are set to dmz::True.

*/
struct dmz::IsectParameters::State {

   IsectTestResultTypeEnum type;
   Boolean findNormal;
   Boolean findHandle;
   Boolean findDistance;
   Boolean findCullMode;
   Boolean attrSet;
   HandleContainer attr;

   State () :
      type (IsectAllPoints),
      findNormal (True),
      findHandle (True),
      findDistance (True),
      findCullMode (True),
      attrSet (False) {;}
};


//! Constructor.
dmz::IsectParameters::IsectParameters () : _state (*(new State)) {;}


//! Copy constructor.
dmz::IsectParameters::IsectParameters (const IsectParameters &Value) :
      _state (*(new State)) { *this = Value; }


//! Destructor.
dmz::IsectParameters::~IsectParameters () { delete &_state; }


//! Assignment operator.
dmz::IsectParameters &
dmz::IsectParameters::operator= (const IsectParameters &Value) {

   _state.type = Value._state.type;
   _state.findNormal = Value._state.findNormal;
   _state.findHandle = Value._state.findHandle;
   _state.findDistance = Value._state.findDistance;
   _state.findCullMode = Value._state.findCullMode;

   return *this;
}


//! Specifies how test points should be processed and returned.
void
dmz::IsectParameters::set_test_result_type (const IsectTestResultTypeEnum Type) {

   _state.type = Type;
}


//! Gets how test points should be processed and returned.
dmz::IsectTestResultTypeEnum
dmz::IsectParameters::get_test_result_type () const { return _state.type; }


//! Specifies if intersection normal should be calculated for each result.
void
dmz::IsectParameters::set_calculate_normal (const Boolean AttrState) {

   _state.findNormal = AttrState;
}


//! Returns if intersection normal should be calculated for each result.
dmz::Boolean
dmz::IsectParameters::get_calculate_normal () const { return _state.findNormal; }


//! Specifies if the objects Handle should be calculated for each result.
void
dmz::IsectParameters::set_calculate_object_handle (const Boolean AttrState) {

   _state.findHandle = AttrState;
}


//! Returns if the objects Handle should be calculated for each result.
dmz::Boolean
dmz::IsectParameters::get_calculate_object_handle () const { return _state.findHandle; }


//! Specifies if the intersection distance should be calculated for each result.
void
dmz::IsectParameters::set_calculate_distance (const Boolean AttrState) {

   _state.findDistance = AttrState;
}


//! Returns if the intersection distance should be calculated for each result.
dmz::Boolean
dmz::IsectParameters::get_calculate_distance () const { return _state.findDistance; }


//! Specifies if the polygon cull mode should be calculated for each result.
void
dmz::IsectParameters::set_calculate_cull_mode (const Boolean Value) {

    _state.findCullMode = Value;
}


//! Returns if the polygon cull mode should be calculated for each result.
dmz::Boolean
dmz::IsectParameters::get_calculate_cull_mode () const { return _state.findCullMode; }


/*!

\brief Sets the isect attributes to use in the intersection tests.
\details By default, all intersections are tested against dmz::RenderIsectStaticName and
dmz::RenderIsectEntityName.
\param[in] Attr HandleContainer containing the isect attribute handles.

*/
void
dmz::IsectParameters::set_isect_attributes (const HandleContainer &Attr) {

   if (Attr.get_count () > 0) {

      _state.attrSet = True;
      _state.attr = Attr;
   }
}


/*!

\brief Gets the isect attributes to use in the intersection tests.
\param[out] attr HandleContainer containing the isect attribute handles.
\return Return dmz::True if any handles were returned in the HandleContainer.

*/
dmz::Boolean
dmz::IsectParameters::get_isect_attributes (HandleContainer &attr) const {

   Boolean result (_state.attrSet);

   if (result) { attr = _state.attr; }

   return result;
}

/*!

\class dmz::IsectResult
\ingroup Render
\brief Intersection test result.

*/
struct dmz::IsectResult::State {

   UInt32 testID;
   Boolean pointSet;
   Vector point;
   Boolean normalSet;
   Vector normal;
   Boolean objHandleSet;
   Handle objHandle;
   Boolean distanceSet;
   Float64 distance;
   Boolean cullModeSet;
   UInt32 cullMode;

   State () :
         testID (0),
         pointSet (False),
         normalSet (False),
         objHandleSet (False),
         objHandle (0),
         distanceSet (False),
         distance (0.0),
         cullModeSet (False),
         cullMode (0) {;}
};


//! Base constructor.
dmz::IsectResult::IsectResult () : _state (*(new State)) {;}


/*!

\brief Test ID Constructor.
\param[in] TestID Intersection test identification value.

*/
dmz::IsectResult::IsectResult (const UInt32 TestID) : _state (*(new State)) {

   _state.testID = TestID;
}


//! Copy constructor.
dmz::IsectResult::IsectResult (const IsectResult &Value) : _state (*(new State)) {

   *this = Value;
}


//! Destructor.
dmz::IsectResult::~IsectResult () { delete &_state; }


//! Assignment operator.
dmz::IsectResult &
dmz::IsectResult::operator= (const IsectResult &Value) {

   _state.testID = Value._state.testID;
   _state.pointSet = Value._state.pointSet;
   _state.point = Value._state.point;
   _state.normalSet = Value._state.normalSet;
   _state.normal = Value._state.normal;
   _state.objHandleSet = Value._state.objHandleSet;
   _state.objHandle = Value._state.objHandle;
   _state.distanceSet = Value._state.distanceSet;
   _state.distance = Value._state.distance;
   _state.cullModeSet = Value._state.cullModeSet;
   _state.cullMode = Value._state.cullMode;

   return *this;
}


//! Sets intersection test identification value.
void
dmz::IsectResult::set_isect_test_id (const UInt32 TestID) {

    _state.testID = TestID;
}


//! Gets intersection test identification value.
dmz::UInt32
dmz::IsectResult::get_isect_test_id () const { return _state.testID; }


//! Sets intersection point.
void
dmz::IsectResult::set_point (const Vector &Value) {

   _state.pointSet = True;
   _state.point = Value;
}


/*!

\brief Gets intersection point.
\param[out] value Vector containing intersection point.
\return Returns dmz::True if the intersection point was set.

*/
dmz::Boolean
dmz::IsectResult::get_point (Vector &value) const {

   value = _state.point;
   return _state.pointSet;
}


//! Sets intersection point normal.
void
dmz::IsectResult::set_normal (const Vector &Value) {

   _state.normalSet = True;
   _state.normal = Value;
}


/*!

\brief Gets intersection point normal.
\param[out] value Vector containing intersection point normal.
\return Returns dmz::True if the intersection point normal was set.

*/
dmz::Boolean
dmz::IsectResult::get_normal (Vector &value) const {

   value = _state.normal;
   return _state.normalSet;
}


//! Sets intersected object's Handle.
void
dmz::IsectResult::set_object_handle (const Handle Value) {

   _state.objHandleSet = True;
   _state.objHandle = Value;
}


/*!

\brief Gets intersection point object Handle.
\param[out] value Handle containing intersection point object Handle.
\return Returns dmz::True if the intersection point object Handle was set.

*/
dmz::Boolean
dmz::IsectResult::get_object_handle (Handle &value) const {

   value = _state.objHandle;
   return _state.objHandleSet;
}


//! Sets intersection distance from start point.
void
dmz::IsectResult::set_distance (const Float64 Value) {

   _state.distanceSet = True;
   _state.distance = Value;
}


/*!

\brief Gets intersection distance from the intersection start point.
\param[out] value Distance from the intersection start point.
\return Returns dmz::True if the intersection distance was set.

*/
dmz::Boolean
dmz::IsectResult::get_distance (Float64 &value) const {

   value = _state.distance;
   return _state.distanceSet;
}


//! Sets cull mode of intersected polygon.
void
dmz::IsectResult::set_cull_mode (const UInt32 Value) {

   _state.cullModeSet = True;
   _state.cullMode = Value;
}


/*!

\brief Gets intersection point cull mode.
\param[out] value Mask containing the polygons cull mode.
\return Returns dmz::True if the cull mode was set.

*/
dmz::Boolean
dmz::IsectResult::get_cull_mode (UInt32 &value) const {

   value = _state.cullMode;
   return _state.cullModeSet;
}


/*!

\class dmz::IsectResultContainer
\ingroup Render
\brief Intersection test result container.
\details The intersection result container will have an IsectResult for each
intersection point that is returned. Multiple IsectResult objects may result from
a single test.

*/
struct dmz::IsectResultContainer::State {

   UInt32 count;
   HashTableUInt32Iterator it;
   HashTableUInt32Template<IsectResult> table;

   State () : count (0) {;}
   ~State () { table.empty (); }
};


//! Base constructor.
dmz::IsectResultContainer::IsectResultContainer () : _state (*(new State)) {;}


//! Copy constructor.
dmz::IsectResultContainer::IsectResultContainer (const IsectResultContainer &Value) :
      _state (*(new State)) {

   *this = Value;
}


//! Destructor.
dmz::IsectResultContainer::~IsectResultContainer () { delete &_state; }


//! Assignment operator.
dmz::IsectResultContainer &
dmz::IsectResultContainer::operator= (const IsectResultContainer &Value) {

   _state.count = Value._state.count;
   _state.table.empty ();
   _state.table.copy (Value._state.table);

   return *this;
}


//! Returns number of intersection results are stored in the container.
dmz::Int32
dmz::IsectResultContainer::get_result_count () const { return _state.table.get_count (); }


//! Resets the iterator.
void
dmz::IsectResultContainer::reset () { _state.it.reset (); }


//! Clears the container.
void
dmz::IsectResultContainer::clear () {

   _state.it.reset ();
   _state.table.empty ();
   _state.count = 0;
}


//! Adds a result to the container.
void
dmz::IsectResultContainer::add_result (const IsectResult &Value) {

   IsectResult *ir (new IsectResult (Value));

   if (ir) {

      if (_state.table.store (_state.count, ir)) { _state.count++; }
      else { delete ir; ir = 0; }
   }
}


/*!

\brief Gets first result stored in the container.
\param[out] value First result stored in the container.
\return Returns dmz::True if the first result is returned in \a value. Returns
dmz::False if the container is empty.

*/
dmz::Boolean
dmz::IsectResultContainer::get_first (IsectResult &value) const {

   Boolean result (False);

   IsectResult *ir (_state.table.get_first (_state.it));

   if (ir) { value = *ir; result = True; }

   return result;
}


/*!

\brief Gets next result stored in the container.
\param[out] value next result stored in the container.
\return Returns dmz::True if the next result is returned in \a value. Returns
dmz::False if the container has returned all results.

*/
dmz::Boolean
dmz::IsectResultContainer::get_next (IsectResult &value) const {

   Boolean result (False);

   IsectResult *ir (_state.table.get_next (_state.it));

   if (ir) { value = *ir; result = True; }

   return result;
}

