#ifndef DMZ_OBJECT_CALC_PRIVATE_H
#define DMZ_OBJECT_CALC_PRIVATE_H

#include <dmzObjectCalc.h>

namespace dmz {

   enum ObjectAttrOperatorEnum {
      ObjectAttrOperatorAdd,
      ObjectAttrOperatorSubtract,
      ObjectAttrOperatorMultiply,
      ObjectAttrOperatorDivide,
   };

   class ObjectAttrOperator : public ObjectAttributeCalculator {

      public:
         ObjectAttrOperator (const ObjectAttrOperatorEnum Operator);
         virtual ~ObjectAttrOperator ();
         virtual void store_object_module (ObjectModule *module);
         virtual Float64 calculate (const Handle ObjectHandle);
         void add_calculator (ObjectAttributeCalculator &calc);
      protected:
         struct ArgStruct {

            ArgStruct *next;
            ObjectAttributeCalculator &calc;
            ArgStruct (ObjectAttributeCalculator &theCalc) : next (0), calc (theCalc) {;}
            ~ArgStruct () { if (next) { delete next; next = 0; } delete &calc; }
         };

         const ObjectAttrOperatorEnum _Op;

         ArgStruct *_argList;
   };

   class ObjectAttrValue : public ObjectAttributeCalculator {

      public:
         ObjectAttrValue (const Handle TheAttrHandle);
         virtual ~ObjectAttrValue ();
         virtual void store_object_module (ObjectModule *module);
         virtual Float64 calculate (const Handle ObjectHandle);

      protected:
         const Handle _AttrHandle;
         ObjectModule *_objMod;
   };

   class ObjectAttrLinkCount : public ObjectAttributeCalculator {

      public:
         ObjectAttrLinkCount (const Handle TheAttrHandle, const UInt32 LinkMask);
         virtual ~ObjectAttrLinkCount ();
         virtual void store_object_module (ObjectModule *module);
         virtual Float64 calculate (const Handle ObjectHandle);

      protected:
         const Handle _AttrHandle;
         const UInt32 _LinkMask;
         ObjectModule *_objMod;
   };

   class ObjectAttrConst : public ObjectAttributeCalculator {

      public:
         ObjectAttrConst (const Float64 ConstValue);
         virtual ~ObjectAttrConst ();
         virtual void store_object_module (ObjectModule *module);
         virtual Float64 calculate (const Handle ObjectHandle);

      protected:
         const Float64 _ConstValue;
   };
};

#endif //  DMZ_OBJECT_CALC_PRIVATE_H
