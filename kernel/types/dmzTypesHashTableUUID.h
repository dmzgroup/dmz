#ifndef DMZ_TYPES_HASH_TABLE_UUID_DOT_H
#define DMZ_TYPES_HASH_TABLE_UUID_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesHashTableLock.h>
#include <dmzTypesUUID.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HashTableUUIDIterator {

      public:
         HashTableUUIDIterator ();
         ~HashTableUUIDIterator ();
         UUID get_hash_key ();
         void reset ();
         struct idata;
         idata &data; //!< Internal state.

      private:
         HashTableUUIDIterator (const HashTableUUIDIterator &It);
   };

   class DMZ_KERNEL_LINK_SYMBOL HashTableUUID {

      public:
         HashTableUUID ();
         HashTableUUID (const Int32 Size, const UInt32 Attributes);
         ~HashTableUUID ();

         void clear ();

         Int32 get_size () const;
         Int32 get_count () const;

         Boolean move (
            const UUID &Key,
            const UUID *TargetKey,
            const Boolean SignleStep,
            const Boolean Before);

         void *get_next (HashTableUUIDIterator &it, const Boolean Prev = False) const;

         void *lookup (const UUID &Key) const;
         Boolean store (const UUID &Key, void *data);
         void * remove (const UUID &Key);

         // For internal use
         void grow (const Int32 Size = 0);
         void set_table_size (const Int32 Size);
         void set_attributes (const UInt32 Attributes);
         UInt32 get_attributes () const;

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         HashTableUUID (const HashTableUUID &);
         HashTableUUID &operator= (const HashTableUUID &);
   };
};

#endif /* DMZ_TYPES_HASH_TABLE_UUID_DOT_H */
