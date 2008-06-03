#ifndef DMZ_TYPES_BOOLEAN_OPERATOR_DOT_H
#define DMZ_TYPES_BOOLEAN_OPERATOR_DOT_H

#define DMZ_BOOLEAN_OPERATOR \
   struct UniqueStruct { Int32 valid; UniqueStruct () : valid (0) {;} }; \
   typedef Int32 UniqueStruct::* DataMemberPtr; \
   operator DataMemberPtr () const { return !*this ? 0 : &UniqueStruct::valid; }

#endif // DMZ_TYPES_BOOLEAN_OPERATOR_DOT_H
