#ifndef DMZ_TYPES_HASH_TABLE_U_INT32_DOT_H
#define DMZ_TYPES_HASH_TABLE_U_INT32_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesHashTableLock.h>
#include <dmzTypesBase.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HashTableUInt32Iterator {

      public:
         HashTableUInt32Iterator ();
         ~HashTableUInt32Iterator ();
         UInt32 get_hash_key ();
         void reset ();
         struct idata;
         idata &data; //!< Internal state.

      private:
         HashTableUInt32Iterator (const HashTableUInt32Iterator &It);
   };

   class DMZ_KERNEL_LINK_SYMBOL HashTableUInt32 {

      public:
         HashTableUInt32 ();
         HashTableUInt32 (const Int32 Size, const UInt32 Attributes);
         ~HashTableUInt32 ();

         void clear ();

         Int32 get_size () const;
         Int32 get_count () const;

         Boolean move (
            const UInt32 &Key,
            const UInt32 *TargetKey,
            const Boolean SingleStep,
            const Boolean Before);

         void *get_next (HashTableUInt32Iterator &it, const Boolean Prev = False) const;

         void *lookup (const UInt32 &Key) const;
         Boolean store (const UInt32 &Key, void *data);
         void * remove (const UInt32 &Key);

         // For internal use
         void grow (const Int32 Size = 0);
         void set_table_size (const Int32 Size);
         void set_attributes (const UInt32 Attributes);
         UInt32 get_attributes () const;

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         HashTableUInt32 (const HashTableUInt32 &);
         HashTableUInt32 &operator= (const HashTableUInt32 &);
   };
};

#endif /* DMZ_TYPES_HASH_TABLE_U_INT32_DOT_H */
