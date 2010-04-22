#include <dmzTypesMatrix.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTest.h>

#include <string.h>
#include <math.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzTypesMatrixTest", argc, argv);

   const Vector Up (0.0, 1.0, 0.0);
   const Vector Down (0.0, -1.0, 0.0);
   const Vector Right (1.0, 0.0, 0.0); 
   const Vector Left (-1.0, 0.0, 0.0); 
   const Vector Forward (0.0, 0.0, -1.0);
   const Vector Backward (0.0, 0.0, 1.0);

   const Matrix ForwardToUp (Right, HalfPi64);
   const Matrix ForwardToDown (Right, -HalfPi64);
   const Matrix ForwardToRight (Up, -HalfPi64);
   const Matrix ForwardToLeft (Up, HalfPi64);

   Vector v (Forward);

   ForwardToUp.transform_vector (v);

   test.validate (
      "Matrix created from Right vector and rotated half pi",
      (Up - v).is_zero ());

   v  = Forward;

   ForwardToDown.transform_vector (v);

   test.validate (
      "Matrix created from Right vector and rotated negative half pi",
      (Down - v).is_zero ());

   v = Forward;

   ForwardToRight.transform_vector (v);

   test.validate (
      "Matrix created from Up vector and rotated negative half pi",
      (Right - v).is_zero ());

   v = Forward;

   ForwardToLeft.transform_vector (v);

   test.validate (
      "Matrix created from Up vector and rotated half pi",
      (Left - v).is_zero ());

   const Matrix RotXHalfPi (HalfPi64, 0.0, 0.0);
   const Matrix RotYHalfPi (0.0, HalfPi64, 0.0);
   const Matrix RotZHalfPi (0.0, 0.0, HalfPi64);

   v = Forward;

   RotXHalfPi.transform_vector (v);

   test.validate (
      "Matrix created from pitch of half pi",
      (Up - v).is_zero ());

   v = Forward;

   RotYHalfPi.transform_vector (v);

   test.validate (
      "Matrix created from heading of half pi",
      (Left - v).is_zero ());

   v = Right;

   RotZHalfPi.transform_vector (v);

   test.validate (
      "Matrix created from roll of half pi",
      (Up - v).is_zero ());

   const Matrix RotXZHalfPi (HalfPi64, 0.0, HalfPi64);

   v = Right;
   Vector v2 = Right;

   RotXZHalfPi.transform_vector (v);
   (RotYHalfPi * (RotXHalfPi * RotZHalfPi)).transform_vector (v2);

   test.validate (
      "Matrix created from pitch of half pi and roll of half pi",
      (Backward - v).is_zero ());

   Matrix pitchInPlace, yawInPlace, rollInPlace;
   pitchInPlace.pitch_in_place (HalfPi64);
   yawInPlace.yaw_in_place (HalfPi64);
   rollInPlace.roll_in_place (HalfPi64);

   v = Forward;
   pitchInPlace.transform_vector (v);

   test.validate (
      "Pitch in place by half pi.",
      (Up - v).is_zero ());

   v = Forward;
   yawInPlace.transform_vector (v);

   test.validate (
      "Yaw in place by half pi.",
      (Left - v).is_zero ());

   v = Right;
   rollInPlace.transform_vector (v);

   test.validate (
      "Roll in place by half pi.",
      (Up - v).is_zero ());

   Matrix ToInvert (HalfPi64, Pi64, HalfPi64 / 2);

   Matrix theInvert (ToInvert);
   theInvert.invert_in_place ();

   test.validate (
      "Matrix times its inverse is the identity matrix.",
      (ToInvert * theInvert).is_identity ());

   const Matrix FromTwoVec (Forward, Up);
   v = Down;
   FromTwoVec.transform_vector (v);
   test.validate (
      "Matrix from two vectors Forward to Up",
       (v - Forward).is_zero ());

   return test.result ();
}
