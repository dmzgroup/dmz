#include <dmzTypesQuaternion.h>
#include <dmzTypesString.h>
#include <dmzTypesBase.h>
#include <dmzTypesMatrix.h>
#include <dmzTest.h>

#include <math.h>

using namespace dmz;

Boolean local_equals (const Float64 Val1, const Float64 Val2) {

   Float64 diff = fabs (Val1 - Val2);
   return (diff < Epsilon64);
}


Boolean local_equals32 (const Float32 Val1, const Float32 Val2) {

   Float32 diff = fabs (Val1 - Val2);
   return (diff < Epsilon32);
}


int main (int argc, char *argv[]) {

   Vector pitchAxis (1.0, 0.0, 0.0);
   Vector yawAxis (0.0, 1.0, 0.0);
   Vector rollAxis (0.0, 0.0, 1.0);

   Matrix ninetyDegreeYaw (yawAxis, HalfPi64);
   Matrix ninetyDegreePitch (pitchAxis, HalfPi64);
   Matrix ninetyDegreeRoll (rollAxis, HalfPi64);

   Test test ("dmzTypesQuaternionTest", argc, argv);

   //Constructors (excluding matrix and axis/angle conversion constructors)
   Vector vecArbAxis (1.0, 2.0, 3.0);
   vecArbAxis.normalize_in_place ();
   Float64 arbAngle = (0.7) * Pi64;
   Float64 arbAngleAbove180 = (1.5) * Pi64;

   

   Quaternion consQuatEmpty;

   Float64 consQuatEmptyXValue = consQuatEmpty.get_x ();
   Float64 consQuatEmptyYValue = consQuatEmpty.get_y ();
   Float64 consQuatEmptyZValue = consQuatEmpty.get_z ();
   Float64 consQuatEmptyWValue = consQuatEmpty.get_w ();

   Float64 constQuatEmptyMagnitude = sqrt (
      (consQuatEmptyXValue * consQuatEmptyXValue) +
      (consQuatEmptyYValue * consQuatEmptyYValue) +
      (consQuatEmptyZValue * consQuatEmptyZValue) +
      (consQuatEmptyWValue * consQuatEmptyWValue));


   // Check for normalization
   test.validate (
      "Default empty constructor", 
      local_equals (1.0, constQuatEmptyMagnitude));

   Quaternion consQuatXYZWValues (1.0, 2.0, 3.0, 4.0);

   Float64 consQuatXYZWValuesXValue = consQuatXYZWValues.get_x ();
   Float64 consQuatXYZWValuesYValue = consQuatXYZWValues.get_y ();
   Float64 consQuatXYZWValuesZValue = consQuatXYZWValues.get_z ();
   Float64 consQuatXYZWValuesWValue = consQuatXYZWValues.get_w ();

   Float64 constQuatXYZWValuesMagnitude = sqrt (
      (consQuatXYZWValuesXValue * consQuatXYZWValuesXValue) +
      (consQuatXYZWValuesYValue * consQuatXYZWValuesYValue) +
      (consQuatXYZWValuesZValue * consQuatXYZWValuesZValue) +
      (consQuatXYZWValuesWValue * consQuatXYZWValuesWValue));

   // Check for normalization
   test.validate (
      "Constructor with arbitrary x, y, z, w values",
      local_equals (1.0, constQuatXYZWValuesMagnitude));


   Quaternion consQuatArb (vecArbAxis, arbAngle);
   Matrix matArbAxisArbAngle (vecArbAxis, arbAngle);

   Quaternion consQuatArbCopy (consQuatArb);
   test.validate (
      "Copy constructor",
      local_equals (consQuatArbCopy.get_x(), consQuatArb.get_x()) &&
      local_equals (consQuatArbCopy.get_y(), consQuatArb.get_y()) &&
      local_equals (consQuatArbCopy.get_z(), consQuatArb.get_z()) &&
      local_equals (consQuatArbCopy.get_w(), consQuatArb.get_w()));

  
   //Assignment operators
   Quaternion assignmentQuat = consQuatXYZWValues;
   test.validate (
      "= Operator (Quaternion) ",
      local_equals (assignmentQuat.get_x(), consQuatXYZWValues.get_x()) &&
      local_equals (assignmentQuat.get_y(), consQuatXYZWValues.get_y()) &&
      local_equals (assignmentQuat.get_z(), consQuatXYZWValues.get_z()) &&
      local_equals (assignmentQuat.get_w(), consQuatXYZWValues.get_w()));



   Quaternion assignmentQuat2;
   assignmentQuat2 = matArbAxisArbAngle;

   Matrix matArbAxisArbAngle2;
   matArbAxisArbAngle2.from_quaternion (assignmentQuat2);
   test.validate (
      "= Operator (Matrix) ",
      (matArbAxisArbAngle == matArbAxisArbAngle2));

  
   //Multiplication operators
   Matrix matYawAxis45Angle (yawAxis, HalfPi64/2.0);
   Matrix matPitchAxis45Angle (pitchAxis, HalfPi64/2.0);
   Matrix matRollAxis45Angle (rollAxis, HalfPi64/2.0);
   Matrix matArbAxisAndAngle (vecArbAxis, arbAngle);

   Matrix matCombined1 = 
      matYawAxis45Angle *
      matPitchAxis45Angle * 
      matRollAxis45Angle;

   Matrix matCombined2 = 
      matPitchAxis45Angle * 
      matRollAxis45Angle * 
      matArbAxisAndAngle *
      matYawAxis45Angle;

   Matrix matCombined3 = 
      matPitchAxis45Angle * 
      matYawAxis45Angle * 
      matRollAxis45Angle * 
      matArbAxisAndAngle;

   Quaternion quatYawAxis45Angle (yawAxis, HalfPi64/2.0);
   Quaternion quatPitchAxis45Angle (pitchAxis, HalfPi64/2.0);
   Quaternion quatRollAxis45Angle (rollAxis, HalfPi64/2.0);
   Quaternion quatArbAxisAndAngle (vecArbAxis, arbAngle);

   Quaternion quatCombined1 (quatYawAxis45Angle);
   quatCombined1 *= quatPitchAxis45Angle;
   quatCombined1 *= quatRollAxis45Angle;
   Matrix quatCombined1ToMatrix (quatCombined1);

   Quaternion quatCombined2 (quatPitchAxis45Angle);
   quatCombined2 *= quatRollAxis45Angle;
   quatCombined2 *= quatArbAxisAndAngle;
   quatCombined2 *= quatYawAxis45Angle;
   Matrix quatCombined2ToMatrix (quatCombined2);

   Quaternion quatCombined3 (quatPitchAxis45Angle);
   quatCombined3 *= quatYawAxis45Angle;
   quatCombined3 *= quatRollAxis45Angle;
   quatCombined3 *= quatArbAxisAndAngle;
   Matrix quatCombined3ToMatrix (quatCombined3);

   test.validate (
      "*= Operator (case 1)",
      (matCombined1 == quatCombined1ToMatrix));

   test.validate (
      "*= Operator (case 2)",
      (matCombined2 == quatCombined2ToMatrix));

   test.validate (
      "*= Operator (case 3)",
      (matCombined3 == quatCombined3ToMatrix));


   Quaternion quatCombined4 = 
      quatYawAxis45Angle * 
      quatPitchAxis45Angle * 
      quatRollAxis45Angle;

   Matrix quatCombined4ToMatrix (quatCombined4);

   Quaternion quatCombined5 = 
      quatPitchAxis45Angle * 
      quatRollAxis45Angle * 
      quatArbAxisAndAngle * 
      quatYawAxis45Angle;

   Matrix quatCombined5ToMatrix (quatCombined5);

   Quaternion quatCombined6 = 
      quatPitchAxis45Angle * 
      quatYawAxis45Angle * 
      quatRollAxis45Angle * 
      quatArbAxisAndAngle;

   Matrix quatCombined6ToMatrix (quatCombined6);


   test.validate (
      "* Operator (case 1)",
      (matCombined1 == quatCombined4ToMatrix));

   test.validate (
      "* Operator (case 2)",
      (matCombined2 == quatCombined5ToMatrix));

   test.validate (
      "* Operator (case 3)",
      (matCombined3 == quatCombined6ToMatrix));


   //Boolean operators
   Quaternion equalsQuat1 (vecArbAxis, arbAngle);
   Quaternion equalsQuat2 (vecArbAxis, arbAngle);
   test.validate (
      "== Operator (case 1)",
      (equalsQuat1 == equalsQuat2));


   Quaternion equalsQuat3 (1.0, 2.0, 3.0, 4.0);
   Quaternion equalsQuat4 (1.0, 2.0, 3.0, 4.0);
   test.validate (
      "== Operator (case 2)",
      (equalsQuat3 == equalsQuat4));

   Quaternion equalsQuat5 (matArbAxisArbAngle);
   Quaternion equalsQuat6 (matArbAxisArbAngle);
   test.validate (
      "== Operator (case 3)",
      (equalsQuat5 == equalsQuat6));

   Quaternion equalsQuat7 (2.0, 4.0, 8.0, 10.0);
   Quaternion equalsQuat8 (equalsQuat7);
   test.validate (
      "== Operator (case 4)",
      (equalsQuat7 == equalsQuat8));


   Vector vecArbAxisDiff (1.0, 2.0, 3.01);
   Float64 arbAngleDiff = (3.01 / 2.0) * Pi64;

   Quaternion notEqualsQuat1 (vecArbAxis, arbAngle);
   Quaternion notEqualsQuat2 (vecArbAxisDiff, arbAngleDiff);
   test.validate (
      "!= Operator (case 1)",
      (notEqualsQuat1 != notEqualsQuat2));


   Quaternion notEqualsQuat3 (1.0, 2.0, 3.0, 4.01);
   Quaternion notEqualsQuat4 (1.0, 2.0, 3.0, 4.0);
   test.validate (
      "!= Operator (case 2)",
      (notEqualsQuat3 != notEqualsQuat4));

   Matrix matArbAxisArbAngleDiff (vecArbAxisDiff, arbAngleDiff);
   Quaternion notEqualsQuat5 (matArbAxisArbAngle);
   Quaternion notEqualsQuat6 (matArbAxisArbAngleDiff);
   test.validate (
      "!= Operator (case 3)",
      (notEqualsQuat5 != notEqualsQuat6));

   Quaternion notEqualsQuat7 (2.0, 4.0, 8.0, 10.0);
   Quaternion notEqualsQuat8 (equalsQuat7);
   test.validate (
      "!= Operator (case 4)",
      !(notEqualsQuat7 != notEqualsQuat8));


   //Matrix conversions
   Matrix matArbAxisAndAngleAbove180 (vecArbAxis, arbAngleAbove180);

   Quaternion quatFromMat1;
   quatFromMat1.from_matrix (matCombined1);
   Quaternion quatFromMat2;
   quatFromMat2.from_matrix (matCombined2);
   Quaternion quatFromMat3;
   quatFromMat3.from_matrix (matCombined3);
   Quaternion quatFromMatArbBelow180deg1;
   quatFromMatArbBelow180deg1.from_matrix (matArbAxisAndAngle);
   Quaternion quatFromMatArbAbove180deg1;
   quatFromMatArbAbove180deg1.from_matrix (matArbAxisAndAngleAbove180);


   Quaternion quatFromMat4 (matCombined1);
   Quaternion quatFromMat5 (matCombined2);
   Quaternion quatFromMat6 (matCombined3);
   Quaternion quatFromMatArbBelow180deg2 (matArbAxisAndAngle);
   Quaternion quatFromMatArbAbove180deg2 (matArbAxisAndAngleAbove180);

   
   Matrix matFromMatToMat1 (quatFromMat1);
   Matrix matFromMatToMat2 (quatFromMat2);
   Matrix matFromMatToMat3 (quatFromMat3);
   Matrix matFromMatToMatArbBelow180deg1 (quatFromMatArbBelow180deg1);
   Matrix matFromMatToMatArbAbove180deg1 (quatFromMatArbAbove180deg1);

   Matrix matFromMatToMat4 (quatFromMat4);
   Matrix matFromMatToMat5 (quatFromMat5);
   Matrix matFromMatToMat6 (quatFromMat6);
   Matrix matFromMatToMatArbBelow180deg2 (quatFromMatArbBelow180deg2);
   Matrix matFromMatToMatArbAbove180deg2 (quatFromMatArbAbove180deg2);


   test.validate (
      "Conversion from matrix (case 1)",
      (matFromMatToMat1 == matCombined1));

   test.validate (
      "Conversion from matrix (case 2)",
      (matFromMatToMat2 == matCombined2));

   test.validate (
      "Conversion from matrix (case 3)",
      (matFromMatToMat3 == matCombined3));

   test.validate (
      "Constructor conversion from matrix (case 1)",
      (matFromMatToMat4 == matCombined1));

   test.validate (
      "Constructor conversion from matrix (case 2)",
      (matFromMatToMat5 == matCombined2));

   test.validate (
      "Constructor conversion from matrix (case 3)",
      (matFromMatToMat6 == matCombined3));

   test.validate (
      "Constructor conversion from matrix (common axis, angle below 180)",
      (matFromMatToMatArbBelow180deg1 == matArbAxisAndAngle));

   test.validate (
      "Constructor conversion from matrix (common axis, angle above 180)",
      (matFromMatToMatArbAbove180deg1 == matArbAxisAndAngleAbove180));


   Matrix matToMat1;
   quatCombined1.to_matrix (matToMat1);

   Matrix matToMat2;
   quatCombined2.to_matrix (matToMat2);

   Matrix matToMat3;
   quatCombined3.to_matrix (matToMat3);

   test.validate (
      "Conversion to matrix (case 1)",
      (matToMat1 == matCombined1));

   test.validate (
      "Conversion to matrix (case 2)",
      (matToMat2 == matCombined2));

   test.validate (
      "Conversion to matrix (case 3)",
      (matToMat3 == matCombined3));

   test.validate (
      "Conversion to matrix (common axis, angle below 180)",
      (matFromMatToMatArbBelow180deg2 == matArbAxisAndAngle));

   test.validate (
      "Conversion to matrix (common axis, angle above 180)",
      (matFromMatToMatArbAbove180deg2 == matArbAxisAndAngleAbove180));



   //Axis and angle conversions
   Quaternion quatYawAxis90Angle1 (yawAxis, HalfPi64);
   Quaternion quatPitchAxis90Angle1 (pitchAxis, HalfPi64);
   Quaternion quatRollAxis90Angle1 (rollAxis, HalfPi64);
   Quaternion quatArbAxisAndAngleAbove180deg1 (vecArbAxis, arbAngleAbove180);

   Quaternion quatYawAxis90Angle2;
   quatYawAxis90Angle2.from_axis_and_angle_radians (yawAxis, HalfPi64);
   Quaternion quatPitchAxis90Angle2;
   quatPitchAxis90Angle2.from_axis_and_angle_radians (pitchAxis, HalfPi64);
   Quaternion quatRollAxis90Angle2;
   quatRollAxis90Angle2.from_axis_and_angle_radians (rollAxis, HalfPi64);
   Quaternion quatArbAxisAndAngle2;
   quatArbAxisAndAngle2.from_axis_and_angle_radians (vecArbAxis, arbAngle);
   Quaternion quatArbAxisAndAngleAbove180deg2;
   quatArbAxisAndAngleAbove180deg2.from_axis_and_angle_radians (
      vecArbAxis, 
      arbAngleAbove180);

   Matrix matYawAxis90Angle (yawAxis, HalfPi64);
   Matrix matPitchAxis90Angle (pitchAxis, HalfPi64);
   Matrix matRollAxis90Angle (rollAxis, HalfPi64);
   Quaternion quatYawAxis90AngleFromMat (matYawAxis90Angle);
   Quaternion quatPitchAxis90AngleFromMat (matPitchAxis90Angle);
   Quaternion quatRollAxis90AngleFromMat (matRollAxis90Angle);
   Quaternion quatArbAxisAndAngleFromMat (matArbAxisAndAngle);
   Quaternion quatArbAxisAndAngleAbove180FromMat (matArbAxisAndAngleAbove180);


   
   Vector initialVecUntransformed (1.0, -0.2, 0.3);
   initialVecUntransformed.normalize_in_place ();

   Vector initialVecMatConversion (initialVecUntransformed);
   quatArbAxisAndAngleAbove180FromMat.transform_vector (initialVecMatConversion);
   
   //Transform copy of initial vector with constructor converter
   Vector initialVecConsConversion (initialVecUntransformed);
   quatArbAxisAndAngleAbove180deg1.transform_vector (initialVecConsConversion);

   //Transform copy of initial vector with from function 
   Vector initialVecFromFunction (initialVecUntransformed);
   quatArbAxisAndAngleAbove180deg2.transform_vector (initialVecFromFunction);

   test.validate (
      "Constructor conversion from axis and angle (case 1)",
      (quatYawAxis90AngleFromMat == quatYawAxis90Angle1));
   test.validate (
      "Constructor conversion from axis and angle (case 2)",
      (quatPitchAxis90AngleFromMat == quatPitchAxis90Angle1));
   test.validate (
      "Constructor conversion from axis and angle (case 3)",
      (quatRollAxis90AngleFromMat == quatRollAxis90Angle1));
   test.validate (
      "Constructor conversion from axis and angle (angle < 180)",
      (quatArbAxisAndAngleFromMat == quatArbAxisAndAngle));
   
   //Quaternion will not be same, but transformation will be identical to matrix
   test.validate (
      "Constructor conversion from axis and angle (angle > 180)",
      (initialVecMatConversion == initialVecConsConversion));


   test.validate (
      "From_axis_and_angle_radians function conversion from axis and angle (case 1)",
      (quatYawAxis90AngleFromMat == quatYawAxis90Angle2));
   test.validate (
      "From_axis_and_angle_radians function conversion from axis and angle (case 2)",
      (quatPitchAxis90AngleFromMat == quatPitchAxis90Angle2));
   test.validate (
      "From_axis_and_angle_radians function conversion from axis and angle (case 3)",
      (quatRollAxis90AngleFromMat == quatRollAxis90Angle2));
   test.validate (
      "From_axis_and_angle_radians function conversion from axis and angle (angle < 180)",
      (quatArbAxisAndAngleFromMat == quatArbAxisAndAngle2));

   //Quaternion will not be same, but transformation will be identical to matrix 
   test.validate (
      "From_axis_and_angle_radians function conversion from axis and angle (angle > 180)",
      (initialVecMatConversion == initialVecFromFunction));

   // Accessors
   Float64 initialX (0.0), initialY (0.0), initialZ (0.0), initialW (0.0);
   Quaternion quatSetGet;
   quatSetGet.set_x (1.1);
   quatSetGet.set_y (3.2);
   quatSetGet.set_z (4.5);
   quatSetGet.set_w (-3.4);

   initialX = quatSetGet.get_x ();
   initialY = quatSetGet.get_y ();
   initialZ = quatSetGet.get_z ();
   initialW = quatSetGet.get_w ();

   quatSetGet.set_x (-2.1);
   quatSetGet.set_y (-4.2);
   quatSetGet.set_z (-5.5);
   quatSetGet.set_w (2.4);

   test.validate (
      "Set/Get Accessors - x",
      (initialX == 1.1) && (quatSetGet.get_x () == -2.1));
   test.validate (
      "Set/Get Accessors - y",
      (initialY == 3.2) && (quatSetGet.get_y () == -4.2));
   test.validate (
      "Set/Get Accessors - z",
      (initialZ == 4.5) && (quatSetGet.get_z () == -5.5));
   test.validate (
      "Set/Get Accessors - w",
      (initialW == -3.4) && (quatSetGet.get_w () == 2.4));
      

   initialX = initialY = initialZ = initialW = 0.0;

   Quaternion quatSetGet2;
   quatSetGet2.set_xyzw (1.1, 3.2, 4.5, -3.4);
   quatSetGet2.get_xyzw (initialX, initialY, initialZ, initialW);
   quatSetGet2.set_xyzw (-2.1, -4.2, -5.5, 2.4);

   test.validate (
      "Set/Get Accessors - xyzw combined",
      (initialX == 1.1) && (quatSetGet2.get_x () == -2.1) &&
      (initialY == 3.2) && (quatSetGet2.get_y () == -4.2) &&
      (initialZ == 4.5) && (quatSetGet2.get_z () == -5.5) &&
      (initialW == -3.4) && (quatSetGet2.get_w () == 2.4));

   //////////////////////////////////////////////
   //Multiply angle
   Vector vecMultAngleAxis (2.0, 4.0, 8.0);
   vecMultAngleAxis.normalize_in_place ();
   Vector vecInitialMultAngle (3.0, -2.3, 5.6);
//   vecInitialMultAngle.normalize_in_place ();
   Vector vecFinalMultAngle (vecInitialMultAngle);

   //Transform, then multiply angle of quat by -1.0, then transform again back
   //to original location
   Quaternion quatMultAngle (vecMultAngleAxis, (1.2 * Pi64));
   quatMultAngle.transform_vector (vecFinalMultAngle);
   quatMultAngle.multiply_angle (-1.0);
   quatMultAngle.transform_vector (vecFinalMultAngle);

//   test.log.out << vecFinalMultAngle << vecInitialMultAngle << endl;
   test.validate (
      "Multiply angle - case 1",
      (vecFinalMultAngle == vecInitialMultAngle));

   //Multiply by zero to ensure no transformation
   vecFinalMultAngle = vecInitialMultAngle;
   quatMultAngle.multiply_angle (0.0);
   quatMultAngle.transform_vector (vecFinalMultAngle);
   quatMultAngle.multiply_angle (-1.0);
   quatMultAngle.transform_vector (vecFinalMultAngle);

   test.validate (
      "Multiply angle - case 2",
      (vecFinalMultAngle == vecInitialMultAngle));



   //Invert
   Quaternion quatInvertBelow180 (vecMultAngleAxis, (0.4 * Pi64));
   Quaternion quatInvertAbove180 (vecMultAngleAxis, (1.7 * Pi64));
   Vector vecInitialInvert (-3.0, 2.3, 5.6);
//   vecInitialInvert.normalize_in_place ();
   Vector vecFinalInvert (vecInitialInvert);

   quatInvertBelow180.transform_vector (vecFinalInvert);
   Quaternion quatInvertBelow180Inverted = quatInvertBelow180.invert ();
   quatInvertBelow180Inverted.transform_vector (vecFinalInvert);

   test.validate (
      "Invert - Arbitrary axis, angle < 180",
      (vecInitialInvert == vecFinalInvert));

   vecFinalInvert = vecInitialInvert;
   quatInvertAbove180.transform_vector (vecFinalInvert);
   Quaternion quatInvertAbove180Inverted = quatInvertAbove180.invert ();
   quatInvertAbove180Inverted.transform_vector (vecFinalInvert);

   test.validate (
      "Invert - Arbitrary axis, angle > 180",
      (vecInitialInvert == vecFinalInvert));

   //Invert in place
   Quaternion quatInvertBelow180Test2 (vecMultAngleAxis, (0.4 * Pi64));
   Quaternion quatInvertAbove180Test2  (vecMultAngleAxis, (1.7 * Pi64));
   Vector vecInitialInvertTest2  (-3.0, 2.3, 5.6);
   Vector vecFinalInvertTest2  (vecInitialInvertTest2);

   quatInvertBelow180Test2 .transform_vector (vecFinalInvertTest2);
   quatInvertBelow180Test2.invert_in_place ();
   quatInvertBelow180Test2.transform_vector (vecFinalInvertTest2);

   test.validate (
      "Invert_in_place - Arbitrary axis, angle < 180",
      (vecInitialInvertTest2  == vecFinalInvertTest2));

   vecFinalInvertTest2 = vecInitialInvertTest2 ;
   quatInvertAbove180Test2.transform_vector (vecFinalInvertTest2);
   quatInvertAbove180Test2.invert_in_place ();
   quatInvertAbove180Test2.transform_vector (vecFinalInvertTest2);

   test.validate (
      "Invert_in_place - Arbitrary axis, angle > 180",
      (vecInitialInvertTest2 == vecFinalInvertTest2));



   //Vector transformation
   Quaternion quatXAxis90 (pitchAxis, HalfPi64);
   Quaternion quatYAxis90 (yawAxis, HalfPi64);
   Quaternion quatZAxis90 (rollAxis, HalfPi64);
   Vector vecArbAxisTrans (-3.0, 2.0, -4.2);
   vecArbAxisTrans.normalize_in_place ();
   Quaternion quatArbAxisAngleAbove180Trans (vecArbAxisTrans, -1.4*Pi64);
   Quaternion quatArbAxisAngleBelow180Trans (vecArbAxisTrans, 0.2*Pi64);

   Matrix matXAxis90 (pitchAxis, HalfPi64);
   Matrix matYAxis90 (yawAxis, HalfPi64);
   Matrix matZAxis90 (rollAxis, HalfPi64);
   Matrix matArbAxisAngleAbove180Trans (vecArbAxisTrans, -1.4*Pi64);
   Matrix matArbAxisAngleBelow180Trans (vecArbAxisTrans, 0.2*Pi64);

   Vector transformedXAxisQuat (pitchAxis);
   Vector transformedXAxisMat (pitchAxis);//x
   Vector transformedYAxisQuat (yawAxis);
   Vector transformedYAxisMat (yawAxis);//y
   Vector transformedZAxisQuat (rollAxis);
   Vector transformedZAxisMat (rollAxis);//z

   Vector transformedArbAxisQuat1 (Vector (-3.0, 5.6, 9.2));
   Vector transformedArbAxisMat1 (Vector (-3.0, 5.6, 9.2));

   Vector transformedArbAxisQuat2 (Vector (-3.0, 5.6, 9.2));
   Vector transformedArbAxisMat2 (Vector (-3.0, 5.6, 9.2));


   //Rotate the Z Axis around the X Axis 90 Degrees -> Negative Y Axis
   quatXAxis90.transform_vector (transformedZAxisQuat);
   matXAxis90.transform_vector (transformedZAxisMat);

   //Rotate the X Axis around the Y Axis 90 Degrees -> Negative Z Axis 
   quatYAxis90.transform_vector (transformedXAxisQuat);
   matYAxis90.transform_vector (transformedXAxisMat);

   //Rotate the Y Axis around the Z Axis 90 Degrees -> Negative X Axis
   quatZAxis90.transform_vector (transformedYAxisQuat);
   matZAxis90.transform_vector (transformedYAxisMat);

   //Rotate arbitrary vector around arbitrary axis, greater than 180 degrees
   quatArbAxisAngleAbove180Trans.transform_vector (transformedArbAxisQuat1);
   matArbAxisAngleAbove180Trans.transform_vector (transformedArbAxisMat1);

   //Rotate arbitrary vector around arbitrary axis, less than 180 degrees
   quatArbAxisAngleBelow180Trans.transform_vector (transformedArbAxisQuat2);
   matArbAxisAngleBelow180Trans.transform_vector (transformedArbAxisMat2);

test.log.error << transformedZAxisQuat << " " << transformedZAxisMat << endl;
   test.validate (
      "Rotate the Z Axis around the X Axis 90 Degrees -> Positive Y Axis",
      (transformedZAxisQuat == transformedZAxisMat) &&
      (transformedZAxisQuat == Vector (0.0, -1.0, 0.0)));

   test.validate (
      "Rotate the X Axis around the Y Axis 90 Degrees -> Positive Z Axis",
      (transformedXAxisQuat == transformedXAxisMat) &&
      (transformedXAxisQuat == Vector (0.0, 0.0, -1.0)));

   test.validate (
      "Rotate the Y Axis around the Z Axis 90 Degrees -> Positive X Axis",
      (transformedYAxisQuat == transformedYAxisMat) &&
      (transformedYAxisQuat == Vector (-1.0, 0.0, 0.0)));

   test.validate (
      "Rotate arbitrary vector around arbitrary axis, angle > 180",
      (transformedArbAxisQuat1 == transformedArbAxisMat1));

   test.validate (
      "Rotate arbitrary vector around arbitrary axis, angle < 180",
      (transformedArbAxisQuat2 == transformedArbAxisMat2));

   ///////////////////////////////////////////////////////////////////////////
   //Matrix array conversions
   Matrix matArrayObject (Vector(4.0, -2.0, 2.1), (-0.67 * Pi64));
   Matrix matArrayObject32;
   Float64 matArray[9];
   Float32 matArray32[9];
   matArrayObject.to_array (matArray);
   matArrayObject.to_array32 (matArray32);
   matArrayObject32.from_array32 (matArray32);

   Quaternion quatFromArray, quatFromArray32,
              quatToArray (Vector(1.0, 2.0, 3.0), (1.4 * Pi64)),
              quatToArray32 (Vector(1.0, 2.0, 3.0), (1.4 * Pi64));
   Quaternion quatFromMatrixObject, quatFromMatrixObject32,
              quatToMatrixObject (Vector(1.0, 2.0, 3.0), (1.4 * Pi64));

   quatFromArray.from_matrix_array (matArray);
   quatFromArray32.from_matrix_array32 (matArray32);

   quatFromMatrixObject.from_matrix (matArrayObject);
   quatFromMatrixObject32.from_matrix (matArrayObject32);

   test.validate (
      "From matrix array",
      (quatFromArray == quatFromMatrixObject));

   test.validate (
      "From matrix array (32 bit)",
      (quatFromArray32 == quatFromMatrixObject32));

   Float64 quatArray[9];
   Float32 quatArray32[9];
   quatFromMatrixObject.to_matrix_array (quatArray);
   quatFromMatrixObject32.to_matrix_array32 (quatArray32);

   test.validate (
      "To matrix array",
      local_equals (quatArray[0], matArray[0]) &&
      local_equals (quatArray[1], matArray[1]) &&
      local_equals (quatArray[2], matArray[2]) &&
      local_equals (quatArray[3], matArray[3]) &&
      local_equals (quatArray[4], matArray[4]) &&
      local_equals (quatArray[5], matArray[5]) &&
      local_equals (quatArray[6], matArray[6]) &&
      local_equals (quatArray[7], matArray[7]) &&
      local_equals (quatArray[8], matArray[8]));

  test.validate (
      "To matrix array (32 bit)",
      local_equals32 (quatArray32[0], matArray32[0]) &&
      local_equals32 (quatArray32[1], matArray32[1]) &&
      local_equals32 (quatArray32[2], matArray32[2]) &&
      local_equals32 (quatArray32[3], matArray32[3]) &&
      local_equals32 (quatArray32[4], matArray32[4]) &&
      local_equals32 (quatArray32[5], matArray32[5]) &&
      local_equals32 (quatArray32[6], matArray32[6]) &&
      local_equals32 (quatArray32[7], matArray32[7]) &&
      local_equals32 (quatArray32[8], matArray32[8]));

   return test.result ();
}

