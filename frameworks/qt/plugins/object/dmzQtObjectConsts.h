#ifndef DMZ_QT_OBJECT_CONSTS_DOT_H
#define DMZ_QT_OBJECT_CONSTS_DOT_H


namespace dmz {

   enum ObjectAttrEnum {

      ObjectAttrCreateObject,
      ObjectAttrDestroyObject,
      ObjectAttrHandle,
      ObjectAttrObjectType,
      ObjectAttrLocality,
      ObjectAttrUUID,
      ObjectAttrRemove,
      ObjectAttrLink,
      ObjectAttrUnlink,
      ObjectAttrLinkObject,
      ObjectAttrCounter,
      ObjectAttrCounterMin,
      ObjectAttrCounterMax,
      ObjectAttrAltObjectType,
      ObjectAttrState,
      ObjectAttrFlag,
      ObjectAttrTimeStamp,
      ObjectAttrPosition,
      ObjectAttrOrientation,
      ObjectAttrVelocity,
      ObjectAttrAcceleration,
      ObjectAttrScale,
      ObjectAttrVector,
      ObjectAttrScalar,
      ObjectAttrText,
      ObjectAttrData,
      Object,
      MaxObjectAttr
   };

   enum  ItemColEnum {

      AttributeCol = 0,
      ValueCol = 1,
   };


};


#endif // DMZ_QT_OBJECT_CONSTS_DOT_H
