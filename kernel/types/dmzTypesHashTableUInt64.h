#ifndef DMZ_TYPES_HASH_TABLE_U_INT64_DOT_H
#define DMZ_TYPES_HASH_TABLE_U_INT64_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesHashTableLock.h>
#include <dmzTypesBase.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HashTableUInt64Iterator {

      public:
         HashTableUInt64Iterator ();
         ~HashTableUInt64Iterator ();
         UInt64 get_hash_key ();
         void reset ();
         struct idata;
         idata &data; //!< Internal state.

      private:
         HashTableUInt64Iterator (const HashTableUInt64Iterator &It);
   };

   class DMZ_KERNEL_LINK_SYMBOL HashTableUInt64 {

      public:
         HashTableUInt64 ();
         HashTableUInt64 (const Int32 Size, const UInt32 Attributes);
         ~HashTableUInt64 ();

         void clear ();

         Int32 get_size () const;
         Int32 get_count () const;

         Boolean move (
            const UInt64 &Key,
            const UInt64 *TargetKey,
            const Boolean SingleStep,
            const Boolean Before);

         void *get_next (HashTableUInt64Iterator &it, const Boolean Prev = False) const;

         void *lookup (const UInt64 &Key) const;
         Boolean store (const UInt64 &Key, void *data);
         void * remove (const UInt64 &Key);

         // For internal use
         void grow (const Int32 Size = 0);
         void set_table_size (const Int32 Size);
         void set_attributes (const UInt32 Attributes);
         UInt32 get_attributes () const;

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         HashTableUInt64 (const HashTableUInt64 &);
         HashTableUInt64 &operator= (const HashTableUInt64 &);
   };
};

#endif /* DMZ_TYPES_HASH_TABLE_U_INT64_DOT_H */
