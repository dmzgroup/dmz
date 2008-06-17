// <CODER --> JADE>
// ---------------{

#include <dmzTypesVector.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTest.h>

#include <string.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzTypesVectorTest", argc, argv);

   // ============================================================================ //
   // <validate constructors>

   // default constructor
   Vector nullVector;
   Float64 tempX, tempY, tempZ;
   nullVector.get_xyz (tempX, tempY, tempZ);
   test.validate (
      "Vector default constructor.",
      !tempX && !tempY && !tempZ );

   // standard constructor
   Vector testVector (12.0, 52.0, 9.0);
   testVector.get_xyz (tempX, tempY, tempZ);
   test.validate (
      "Vector standard constructor.",
      (tempX == 12.0) &&
      (tempY == 52.0) &&
      (tempZ == 9.0) );

   // copy constructor
   Vector copyOfTestVector (testVector);
   copyOfTestVector.get_xyz (tempX, tempY, tempZ);
   test.validate (
      "Vector copy constructor.",
      (tempX == 12.0) &&
      (tempY == 52.0) &&
      (tempZ == 9.0) );

   // </validate constructors>
   // ============================================================================ //
   // <validate get and set functions>

   // get / set X
   testVector.set_x (64.0);
   tempX = testVector.get_x ();
   test.validate (
      "Vector validate getX and setX.",
      (tempX == 64.0) );

   // get / set Y
   testVector.set_y (113.0);
   tempY = testVector.get_y ();
   test.validate (
      "Vector validate getY and setY.",
      (tempY == 113.0) );

   // get / set Z
   testVector.set_z (-5.0);
   tempZ = testVector.get_z ();
   test.validate (
      "Vector validate getZ and setZ.",
      (tempZ == -5.0) );

   // validate get_x, get_y, get_z and get_xyz behave the same
   testVector.set_xyz (-9.1, -9.2, -9.3);
   testVector.get_xyz (tempX, tempY, tempZ);
   test.validate (
      "Vector validate { get_x, get_y, get_z } behave the same as get_xyz.",
      (tempX == testVector.get_x ()) &&
      (tempY == testVector.get_y ()) &&
      (tempZ == testVector.get_z ()) );

   // validate set_x, set_y, set_z and set_xyz behave the same
   copyOfTestVector.set_xyz (40.0, 41.0, 42.0);
   testVector.set_x (40.0);
   testVector.set_y (41.0);
   testVector.set_z (42.0);
   test.validate (
      "Vector validate { set_x, set_y, set_z } behave the same as set_xyz.",
      (copyOfTestVector.get_x () == testVector.get_x ()) &&
      (copyOfTestVector.get_y () == testVector.get_y ()) &&
      (copyOfTestVector.get_z () == testVector.get_z ()) );

   // </validate get and set functions>
   // ============================================================================ //
   // <validate operators>

   // get / set X
   Vector vectorMinus1 (-1.0, -1.0, -1.0);
   Vector vector1 (1.0, 1.0, 1.0);
   Vector vector2 (2.0, 2.0, 2.0);
   Vector vector3 (3.0, 3.0, 3.0);
   Vector vector4 (4.0, 4.0, 4.0);
   Vector vector5 (5.0, 5.0, 5.0);

   testVector.set_xyz (1.0, 1.0, 1.0);

   // comparison operators
   test.validate (
      "Vector validate equality operator.",
      testVector == vector1 );
   test.validate (
      "Vector validate inequality operator.",
      testVector != vector2 );

   // Assignment operator
   testVector = vector1;
   test.validate (
      "Vector validate = Vector operator.",
      testVector == vector1 );

   // Unary - operator
   testVector = -testVector;
   test.validate (
      "Vector validate unary - Vector operator.",
      testVector == vectorMinus1 );
   -testVector;
   test.validate (
      "Vector validate unary - Vector operator doesn't change it's host.",
      testVector == vectorMinus1 );

   // Scalar Assignment math operators
   testVector = vector1;
   testVector += 2;
   test.validate (
      "Vector validate += Float64 operator.",
      testVector == vector3 );

   testVector -= 1.0;
   test.validate (
      "Vector validate -= Float64 operator.",
      testVector == vector2 );

   testVector *= 2.0;
   test.validate (
      "Vector validate *= Float64 operator.",
      testVector == vector4 );

   // Vector Assignment math operators
   testVector = vector1;
   testVector += vector2;
   test.validate (
      "Vector validate += Vector operator.",
      testVector == vector3 );

   testVector -= vector1;
   test.validate (
      "Vector validate -= Vector operator.",
      testVector == vector2 );

   testVector *= vector2;
   test.validate (
      "Vector validate *= Vector operator.",
      testVector == vector4 );

   // Scalar math operators
   Vector resultVector;
   resultVector = vector1 + 2;
   test.validate (
      "Vector validate + Float64 operator.",
      resultVector == vector3 );

   resultVector = vector3 - 1;
   test.validate (
      "Vector validate - Float64 operator.",
      resultVector == vector2 );

   resultVector = vector2 * 2;
   test.validate (
      "Vector validate * Float64 operator.",
      resultVector == vector4 );

   // Vector math operators
   resultVector = vector1 + vector2;
   test.validate (
      "Vector validate + Vector operator.",
      resultVector == vector3 );

   resultVector = vector3 - vector1;
   test.validate (
      "Vector validate - Vector operator.",
      resultVector == vector2 );

   resultVector = vector2 * vector2;
   test.validate (
      "Vector validate * Vector operator.",
      resultVector == vector4 );
   // </validate operators>
   // ============================================================================ //
   // <validate magnitude and normalize functions>

   // magnitude^2 and magnitude
   testVector.set_xyz (0.0, 3.0, 4.0);
   test.validate (
      "Vector validate magnitude_squared function.",
      (testVector.magnitude_squared () == 25.0) );
   test.validate (
      "Vector validate magnitude_squared function.",
      (testVector.magnitude () == 5.0) );

   // nirmalize
   const Float64 Epsilon (0.000001);
   testVector.set_xyz (80.0, 0.0, 80.0);
   resultVector = testVector.normalize ();
   test.validate (
      "Vector validate normalize function.",
      (fabs (resultVector.get_x () - sqrt (2.0) / 2) < Epsilon) &&
      (resultVector.get_y () == 0.0) &&
      (fabs (resultVector.get_z () - sqrt (2.0) / 2) < Epsilon) );
//   test.log.out << resultVector.get_x () <<
//                   sqrt (2.0) / 2 <<
//                   resultVector.get_z () << endl;

   // normalize_in_place
   testVector.normalize_in_place ();
   test.validate (
      "Vector validate normalize_in_place function.",
      (fabs (testVector.get_x () - sqrt (2.0) / 2) < Epsilon) &&
      (testVector.get_y () == 0.0) &&
      (fabs (testVector.get_z () - sqrt (2.0) / 2) < Epsilon) );

   // </validate magnitude and normalize functions>
   // ============================================================================ //
   // <validate vector products>

   // dot product
   test.validate (
      "Vector validate dot function.",
      (vector3.dot (vector2) == 18.0) &&
      (vector2.dot (vector3) == 18.0) );

   // cross product
   Vector vectorX (2.0, 0.0, 0.0);
   Vector vectorY (0.0, 2.0, 0.0);
   Vector vectorZ (0.0, 0.0, 4.0);
   test.validate (
      "Vector validate cross function.",
      vectorX.cross (vectorY) == vectorZ &&
      vectorY.cross (vectorX) == -vectorZ );

   // cross product
   testVector = vectorY;
   testVector.cross_in_place (vectorX);
   test.validate (
      "Vector validate cross_in_place function.",
      testVector == -vectorZ );

   // </validate vector products>
   // ============================================================================ //
   return test.result ();
}

//  }---------------
// </CODER <-- JADE>
