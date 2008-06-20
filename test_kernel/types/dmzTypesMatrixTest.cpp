#include <dmzTypesMatrix.h>
#include <dmzTypesQuaternion.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTest.h>

#include <string.h>
#include <math.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzTypesMatrixTest", argc, argv);

   // ============================================================================ //
   // <validate constructors>

   // default constructor
   Matrix nullMatrix;
   Float64 nullMatrixArrayOut[9];
   nullMatrix.to_array (nullMatrixArrayOut);
   test.validate (
      "Matrix default constructor",
      fabs(nullMatrixArrayOut[0] - 1) < Epsilon64 &&
      fabs(nullMatrixArrayOut[1] - 0) < Epsilon64 &&
      fabs(nullMatrixArrayOut[2] - 0) < Epsilon64 &&
      fabs(nullMatrixArrayOut[3] - 0) < Epsilon64 &&
      fabs(nullMatrixArrayOut[4] - 1) < Epsilon64 &&
      fabs(nullMatrixArrayOut[5] - 0) < Epsilon64 &&
      fabs(nullMatrixArrayOut[6] - 0) < Epsilon64 &&
      fabs(nullMatrixArrayOut[7] - 0) < Epsilon64 &&
      fabs(nullMatrixArrayOut[8] - 1) < Epsilon64);

   // float array constructors
   const Float64 float64ArrayIn[9] = { 0.0,-1.0, 0.0,
                                       1.0, 0.0, 0.0,
                                       0.0, 0.0, 1.0 };
   Matrix matrixFromFloat64 (float64ArrayIn);
   Float64 float64ArrayOut[9];
   matrixFromFloat64.to_array (float64ArrayOut);

   test.validate (
      "Test Float64Array constructor",
      fabs(float64ArrayOut[0] - float64ArrayIn[0]) < Epsilon64 &&
      fabs(float64ArrayOut[1] - float64ArrayIn[1]) < Epsilon64 &&
      fabs(float64ArrayOut[2] - float64ArrayIn[2]) < Epsilon64 &&
      fabs(float64ArrayOut[3] - float64ArrayIn[3]) < Epsilon64 &&
      fabs(float64ArrayOut[4] - float64ArrayIn[4]) < Epsilon64 &&
      fabs(float64ArrayOut[5] - float64ArrayIn[5]) < Epsilon64 &&
      fabs(float64ArrayOut[6] - float64ArrayIn[6]) < Epsilon64 &&
      fabs(float64ArrayOut[7] - float64ArrayIn[7]) < Epsilon64 &&
      fabs(float64ArrayOut[8] - float64ArrayIn[8]) < Epsilon64);

   const Float32 float32ArrayIn[9] = { 0.5f, 0.0f,-0.7f,
                                       0.0f, 0.0f, 0.0f,
                                       0.7f, 0.0f, 0.5f };
   Matrix matrixFromFloat32 (float32ArrayIn);
   Float32 float32ArrayOut[9];
   matrixFromFloat32.to_array32 (float32ArrayOut);

   test.validate (
      "Test Float32Array constructor",
      fabs(float32ArrayOut[0] - float32ArrayIn[0]) < Epsilon32 &&
      fabs(float32ArrayOut[1] - float32ArrayIn[1]) < Epsilon32 &&
      fabs(float32ArrayOut[2] - float32ArrayIn[2]) < Epsilon32 &&
      fabs(float32ArrayOut[3] - float32ArrayIn[3]) < Epsilon32 &&
      fabs(float32ArrayOut[4] - float32ArrayIn[4]) < Epsilon32 &&
      fabs(float32ArrayOut[5] - float32ArrayIn[5]) < Epsilon32 &&
      fabs(float32ArrayOut[6] - float32ArrayIn[6]) < Epsilon32 &&
      fabs(float32ArrayOut[7] - float32ArrayIn[7]) < Epsilon32 &&
      fabs(float32ArrayOut[8] - float32ArrayIn[8]) < Epsilon32);

   // Construct from euler angles
   const Float64 float64ArrayPitch90[9] = { 1.0, 0.0, 0.0,
                                            0.0, 0.0,-1.0,
                                            0.0, 1.0, 0.0 };
   const Float64 float64ArrayYaw90[9] = { 0.0, 0.0, 1.0,
                                          0.0, 1.0, 0.0,
                                         -1.0, 0.0, 0.0 };
   const Float64 float64ArrayRoll90[9] = { 0.0,-1.0, 0.0,
                                           1.0, 0.0, 0.0,
                                           0.0, 0.0, 1.0 };
   Matrix bruteForcePitch90 (float64ArrayPitch90),
          bruteForceYaw90   (float64ArrayYaw90),
          bruteForceRoll90  (float64ArrayRoll90);

   Matrix pitch90 ( Pi64 / 2, 0, 0 ),
          yaw90   ( 0, Pi64 / 2, 0 ),
          roll90  ( 0, 0, Pi64 / 2 );
   test.validate (
      "Test Euler angle constructor : pitch - Pi/2 radians",
      (pitch90 == bruteForcePitch90));
   test.validate (
      "Test Euler angle constructor : yaw - Pi/2 radians",
      (yaw90 == bruteForceYaw90));
   test.validate (
      "Test Euler angle constructor : roll - Pi/2 radians",
      (roll90 == bruteForceRoll90));

   // construct from rotation Axis and angle
   Matrix anotherPitch90 (Vector (1, 0, 0), Pi64 / 2),
          anotherYaw90   (Vector (0, 1, 0), Pi64 / 2),
          anotherRoll90  (Vector (0, 0, 1), Pi64 / 2);
   test.validate (
      "Test axis/angle constructor : pitch - Pi/2 radians",
      (anotherPitch90 == bruteForcePitch90));
   test.validate (
      "Test axis/angle constructor : yaw - Pi/2 radians",
      (anotherYaw90 == bruteForceYaw90));
   test.validate (
      "Test axis/angle constructor : roll - Pi/2 radians",
      (anotherRoll90 == bruteForceRoll90));

   // test quaternion constructor
   Quaternion QuatPitch90 (Vector (1, 0, 0), Pi64 / 2);
   Matrix matrixFromQuaternionPitch( QuatPitch90 );
   test.validate (
      "Test quaternion constructor : pitch - Pi/2 radians",
      (matrixFromQuaternionPitch == bruteForcePitch90));

   Quaternion QuatYaw90 (Vector (0, 1, 0), Pi64 / 2);
   Matrix matrixFromQuaternionYaw( QuatYaw90 );
   test.validate (
      "Test quaternion constructor : yaw - Pi/2 radians",
      (matrixFromQuaternionYaw == bruteForceYaw90));

   Quaternion QuatRoll90 (Vector (0, 0, 1), Pi64 / 2);
   Matrix matrixFromQuaternionRoll( QuatRoll90 );
   test.validate (
      "Test quaternion constructor : roll - Pi/2 radians",
      (matrixFromQuaternionRoll == bruteForceRoll90));

   // test copy constructor
   Matrix copyOfYaw90 (yaw90);
   test.validate (
      "Test copy constructor",
      (copyOfYaw90 == bruteForceYaw90) &&
      (copyOfYaw90 == yaw90));

   // </validate constructors>
   // ============================================================================ //
   // <validate operators>

   // test comparison operators
   const Float64 somethingData[9] = { 0.0, 5.0, 0.0,
                                      1.0, 0.0, 7.0,
                                      0.0, 2.0, 0.0 };
   const Float64 alsoSomethingData[9] = { 0.0, 5.0, 0.0,
                                          1.0, 0.0, 7.0,
                                          0.0, 2.0, 0.0 };
   const Float64 notSomethingData[9] = { 6.0, 0.0, 6.0,
                                         0.0, 6.0, 0.0,
                                         6.0, 0.0, 6.0 };

   Matrix something     (somethingData),
          alsoSomething (alsoSomethingData),
          notSomething  (notSomethingData);

   test.validate (
      "Test == operator",
      (something == alsoSomething) &&
      !(something == notSomething) &&
      !(alsoSomething == notSomething));
   test.validate (
      "Test != operator",
      !(something != alsoSomething) &&
      (something != notSomething) &&
      (alsoSomething != notSomething));

   // test = operators
   Matrix goingToBeSomething = something,
          goingToBeNotSomething = notSomething;
   test.validate (
      "Test = (Martix) operator",
      (goingToBeSomething == something) &&
      (goingToBeNotSomething == notSomething));

   Matrix sameAsQuat90 = QuatRoll90;
   test.validate (
      "Test = (Quaternion) operator",
      (matrixFromQuaternionRoll == sameAsQuat90));

   // test +, +=, -, -= operators
   const Float64 mat1Data[9] = { 1.0, 1.0, 1.0,
                                 1.0, 0.0, 1.0,
                                 1.0, 1.0, 1.0 };
   const Float64 mat2Data[9] = { 2.0, 2.0, 2.0,
                                 2.0, 0.0, 2.0,
                                 2.0, 2.0, 2.0 };
   const Float64 mat3Data[9] = { 3.0, 3.0, 3.0,
                                 3.0, 0.0, 3.0,
                                 3.0, 3.0, 3.0 };
   const Float64 mat4Data[9] = { 4.0, 4.0, 4.0,
                                 4.0, 0.0, 4.0,
                                 4.0, 4.0, 4.0 };
   Matrix mat1 (mat1Data),
          mat2 (mat2Data),
          mat3 (mat3Data),
          mat4 (mat4Data);

   Matrix shouldBeMat3 = mat1 + mat2;
   test.validate (
      "Test + operator",
      (shouldBeMat3 == mat3));

   Matrix shouldBeMat4 (mat3); shouldBeMat4 += mat1;
   test.validate (
      "Test += operator",
      (shouldBeMat4 == mat4));

   Matrix shouldBeMat1 = mat3 - mat2;
   test.validate (
      "Test - operator",
      (shouldBeMat1 == mat1));

   Matrix shouldBeMat2 (mat4); shouldBeMat2 -= mat2;
   test.validate (
      "Test -= operator",
      (shouldBeMat2 == mat2));

   // test unary - operator
   const Float64 matMinus1Data[9] = {-1.0,-1.0,-1.0,
                                     -1.0, 0.0,-1.0,
                                     -1.0,-1.0,-1.0 };
   Matrix matMinus1 (matMinus1Data);
   test.validate (
      "Test unary - operator",
      (mat1 == -matMinus1) &&
      (matMinus1 == -mat1));

   // test scalar * operators
   Matrix willBeMat2 = mat1 * 2;
   test.validate (
      "Test * (scalar) operator",
      (willBeMat2 == mat2));

   Matrix willBeMat3 (mat1); willBeMat3 *= 3;
   test.validate (
      "Test *= (scalar) operator",
      (willBeMat3 == mat3));

   // test matrix * operators
   const Float64 mmIn1Data[9] = { 1.0, 0.0, 1.0,
                                   0.0, 1.0, 1.0,
                                   1.0, 1.0, 0.0 };
   const Float64 mmIn2Data[9] = { 0.0, 1.0, 1.0,
                                   1.0, 0.0, 1.0,
                                   1.0, 0.0, 1.0 };
   const Float64 mmOutData[9] = { 1.0, 1.0, 2.0,
                                   2.0, 0.0, 2.0,
                                   1.0, 1.0, 2.0 };
   Matrix mmIn1 (mmIn1Data),
          mmIn2 (mmIn2Data),
          mmOut (mmOutData);

   Matrix productA = mmIn1 * mmIn2;
   test.validate (
      "Test * (matrix) operator",
      (productA == mmOut));

   Matrix productB (mmIn1); productB *= mmIn2;
   test.validate (
      "Test *= (matrix) operator",
      (productB == mmOut));

   // </validate operators>
   // ============================================================================ //
   // <validate transformations>

   // test transpose function
   Matrix yaw45 (Vector (0, 1, 0), Pi64 / 4);
   Matrix yawMinus45 (Vector (0, 1, 0), -Pi64 / 4);

   Matrix yaw45Transpose; yaw45Transpose = yaw45.transpose ();
   test.validate (
      "Test transpose () function",
      (yaw45Transpose == yawMinus45));
   Matrix yaw45AlsoTranspose (yaw45); yaw45AlsoTranspose.transpose_in_place ();
   test.validate (
      "Test transpose_in_place () function",
      (yaw45AlsoTranspose == yawMinus45));

   // test invert functions
   Matrix yaw45Invert (yaw45);
   test.validate (
      "Test invert_in_place () function",
      yaw45Invert.invert_in_place ());
   test.validate (
      "Test invert_in_place () function worked",
      (yaw45Invert == yawMinus45));

   Matrix yaw45InvertAgain;
   test.validate (
      "Test invert () function",
      yaw45.invert (yaw45InvertAgain));
   test.validate (
      "Test invert () function worked",
      (yaw45InvertAgain == yawMinus45));

   const Float64 inversionKillerData[9] = { 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0 };
   Matrix inversionKiller (inversionKillerData);
   test.validate (
      "Test invert () function fails when it should",
      !inversionKiller.invert (yaw45InvertAgain));

   // test negate functions
   Matrix negateMatrix; negateMatrix = mat1.negate ();
   test.validate (
      "Test negate () function",
      (negateMatrix == matMinus1));

   Matrix negateMatrixPartDeux (mat1); negateMatrixPartDeux.negate_in_place ();
   test.validate (
      "Test negate_in_place () function",
      (negateMatrixPartDeux == matMinus1));

   // </validate transformations>
   // ============================================================================ //
   // <validate accessors>

   Matrix testMatrix;
   // test is_identity
   test.validate (
      "Test is_identity () function succeeds",
      testMatrix.is_identity ());
   test.validate (
      "Test is_identity () function fails",
      !mat1.is_identity () );

   // test set_identity
   Matrix becomeIdentity (mat1);
   test.validate (
      "Test set_identity ()",
      !becomeIdentity.is_identity () &&
      (becomeIdentity.set_identity (), becomeIdentity.is_identity ()) );

   // test get and set
   Float64 singleDataBefore, singleDataAfter;
   Float64 goalData = 0.441;
   testMatrix.get_element (2, 0, singleDataBefore);
   testMatrix.set_element (2, 0, goalData);
   testMatrix.get_element (2, 0, singleDataAfter);
   // test is_identity
   test.validate (
      "Test get_element () and set_element () functions work properly",
      (singleDataBefore != goalData) &&
      (singleDataAfter == goalData));

   Float64 tempFloat (0);
   test.validate (
      "Test get_element () fails when called with bad bounds",
      !testMatrix.get_element ( 0, -1, tempFloat) &&
      !testMatrix.get_element ( 0,  3, tempFloat) &&
      !testMatrix.get_element (-1,  0, tempFloat) &&
      !testMatrix.get_element ( 3,  0, tempFloat));
   test.validate (
      "Test set_element () fails when called with bad bounds",
      !testMatrix.set_element ( 0, -1, tempFloat) &&
      !testMatrix.set_element ( 0,  3, tempFloat) &&
      !testMatrix.set_element (-1,  0, tempFloat) &&
      !testMatrix.set_element ( 3,  0, tempFloat));

   // i've basically been assuming from_array and to_array work.
   // I'm a little unsure how to test them short of direct access the data,
   //   so I'll just make sure they work the same way.

   Float64 data64In[9] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
   Float32 data32In[9] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
   Matrix feedMe32s, feedMe64s;
   feedMe32s.from_array32 (data32In);
   feedMe64s.from_array (data64In);
   test.validate (
      "Test from_array32 and from_array both build the same matrix from the same values.",
      (feedMe32s == feedMe64s));

   Float32 getBack32s[9];
   feedMe32s.to_array32 (getBack32s);
   test.validate (
      "Test to_array32 returned the proper values.",
      (getBack32s[0] == data32In[0]) &&
      (getBack32s[1] == data32In[1]) &&
      (getBack32s[2] == data32In[2]) &&
      (getBack32s[3] == data32In[3]) &&
      (getBack32s[4] == data32In[4]) &&
      (getBack32s[5] == data32In[5]) &&
      (getBack32s[6] == data32In[6]) &&
      (getBack32s[7] == data32In[7]) &&
      (getBack32s[8] == data32In[8]));

   Float64 getBack64s[9];
   feedMe64s.to_array (getBack64s);
   test.validate (
      "Test to_array returned the proper values.",
      (getBack64s[0] == data64In[0]) &&
      (getBack64s[1] == data64In[1]) &&
      (getBack64s[2] == data64In[2]) &&
      (getBack64s[3] == data64In[3]) &&
      (getBack64s[4] == data64In[4]) &&
      (getBack64s[5] == data64In[5]) &&
      (getBack64s[6] == data64In[6]) &&
      (getBack64s[7] == data64In[7]) &&
      (getBack64s[8] == data64In[8]));

   test.validate (
      "Test to_array and to_array32 return the same data from reading the same data.",
      (getBack64s[0] == getBack32s[0]) &&
      (getBack64s[1] == getBack32s[1]) &&
      (getBack64s[2] == getBack32s[2]) &&
      (getBack64s[3] == getBack32s[3]) &&
      (getBack64s[4] == getBack32s[4]) &&
      (getBack64s[5] == getBack32s[5]) &&
      (getBack64s[6] == getBack32s[6]) &&
      (getBack64s[7] == getBack32s[7]) &&
      (getBack64s[8] == getBack32s[8]));

   // test the euler angle conversions
   Float64 rx (0.0),
           ry (0.0),
           rz (0.0);
   Float64 getRx, getRy, getRz;
   testMatrix.from_euler_angles_radians (rx, ry, rz);
   testMatrix.to_euler_angles_radians (getRx, getRy, getRz);
   test.validate (
      "Test to_euler_angles_radius spits out what from_euler_angles stuck in.",
      (fabs(rx - getRx) < Epsilon64) &&
      (fabs(ry - getRy) < Epsilon64) &&
      (fabs(rz - getRz) < Epsilon64));

   testMatrix.from_euler_angles_radians (2.0, 0, 0);
   testMatrix.to_euler_angles_radians (getRx, getRy, getRz);
   test.validate (
      "same with (2.0, 0, 0).",
      (fabs(getRx - 2.0) < Epsilon64) &&
      (fabs(getRy - 0) < Epsilon64) &&
      (fabs(getRz - 0) < Epsilon64));

   testMatrix.from_euler_angles_radians (0, 1.5, 0);
   testMatrix.to_euler_angles_radians (getRx, getRy, getRz);
   test.validate (
      "same with (0, 1.5, 0).",
      (fabs(getRx - 0) < Epsilon64) &&
      (fabs(getRy - 1.5) < Epsilon64) &&
      (fabs(getRz - 0) < Epsilon64));

   testMatrix.from_euler_angles_radians (0, 0, 0.8);
   testMatrix.to_euler_angles_radians (getRx, getRy, getRz);
   test.validate (
      "same with (0, 0, 0.8).",
      (fabs(getRx - 0) < Epsilon64) &&
      (fabs(getRy - 0) < Epsilon64) &&
      (fabs(getRz - 0.8) < Epsilon64));

   testMatrix.from_euler_angles_radians (-2.0, 0, 0);
   testMatrix.to_euler_angles_radians (getRx, getRy, getRz);
   test.validate (
      "same with (-2.0, 0, 0).",
      (fabs(getRx + 2.0 - Pi64*2) < Epsilon64) &&
      (fabs(getRy - 0) < Epsilon64) &&
      (fabs(getRz - 0) < Epsilon64));

   testMatrix.from_euler_angles_radians (0, -1.5, 0);
   testMatrix.to_euler_angles_radians (getRx, getRy, getRz);
   test.validate (
      "same with (0, -1.5, 0).",
      (fabs(getRx - 0) < Epsilon64) &&
      (fabs(getRy + 1.5 - Pi64*2) < Epsilon64) &&
      (fabs(getRz - 0) < Epsilon64));

   testMatrix.from_euler_angles_radians (0, 0, -0.8);
   testMatrix.to_euler_angles_radians (getRx, getRy, getRz);
   test.validate (
      "same with (0, 0, -0.8).",
      (fabs(getRx - 0) < Epsilon64) &&
      (fabs(getRy - 0) < Epsilon64) &&
      (fabs(getRz + 0.8 - Pi64*2) < Epsilon64));
//   test.log.out << "RESULT: " << getRx << " " << getRy << " " << getRz << endl;

   testMatrix.from_euler_angles_radians (2, 1, 0.0);
//   testMatrix.to_euler_angles_radians (getRx, getRy, getRz);

   Matrix similarMatrixIn1 (Vector (1, 0, 0), 2);
   Matrix similarMatrixIn2 (Vector (0, 1, 0), 1);
   // * similarMatrixIn2.transpose ();
   Matrix finalMatrix = similarMatrixIn1 * similarMatrixIn2;
   test.validate (
      "test that a combination of 2 euler vectors matches a transformation of rotation matrices built from it's components.",
      (testMatrix == finalMatrix));
//   test.log.out << "euler : " << testMatrix << endl << "multiply : " << finalMatrix << endl;

   // test the axis and angle conversions
   const Float64 Sqrt2 = sqrt( 2.0 );
   const Float64 yaw45ForcedData[9] = { Sqrt2/2, 0.0, Sqrt2/2,
                                            0.0, 1.0, 0.0,
                                       -Sqrt2/2, 0.0, Sqrt2/2 };
   const Matrix yaw45Forced (yaw45ForcedData);

   // axis angle
   Vector rotationAxis (0, 1, 0);
   Float64 rotationAngle (Pi64 / 4);

   testMatrix.from_axis_and_angle_radians (rotationAxis, rotationAngle);
   test.validate (
      "Test from_axis_and_angle_radians with (0, 1, 0) & Pi/2 makes the yaw45 matrix.",
      (testMatrix == yaw45ForcedData));

   // make sure it normalizes axis vectors
   Vector severiouslyNotNormalizedAxis (0, 52007.4431, 0);
   testMatrix.from_axis_and_angle_radians (severiouslyNotNormalizedAxis, rotationAngle);
   test.validate (
      "Test that the from_axis_and_angle_radians () call normalizes the axis.",
      (testMatrix == yaw45ForcedData));

   // test the quaternion conversions

   // create a Quat and Matrix, see if they match
   Vector arbitraryRotationAxis (14, 32, 24);
   Float64 arbitraryAngle (2.9953);

   Matrix matrixVersion (arbitraryRotationAxis, arbitraryAngle);
   Quaternion quaternionVersion (arbitraryRotationAxis, arbitraryAngle);

   testMatrix.from_quaternion (quaternionVersion);
   test.validate (
      "Test from_quaternion matrix matches a mat built from the same source.",
      (testMatrix == matrixVersion));
//   test.log.out << "quat : " << testMatrix << endl << "mat : " << matrixVersion << endl;

   Quaternion backToQuat;
   testMatrix.to_quaternion (backToQuat);
   test.validate (
      "Test to_quaternion recreates the original quaternion.",
      (quaternionVersion == backToQuat));

   // </validate accessors>
   // ============================================================================ //
   return test.result ();
}
