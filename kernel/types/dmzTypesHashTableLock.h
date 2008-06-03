#ifndef DMZ_HASH_TABLE_LOCK_DOT_H
#define DMZ_HASH_TABLE_LOCK_DOT_H

namespace dmz {

   class HashTableLock {

      public:
         virtual ~HashTableLock () {;}

         virtual void lock () = 0;
         virtual void unlock () = 0;

      protected:
         HashTableLock () {;}
   };
};

#endif // DMZ_HASH_TABLE_LOCK_DOT_H
