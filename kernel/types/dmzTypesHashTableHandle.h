#ifndef DMZ_TYPES_HASH_TABLE_HANDLE_DOT_H
#define DMZ_TYPES_HASH_TABLE_HANDLE_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesHashTableLock.h>
#include <dmzTypesBase.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HashTableHandleIterator {

      public:
         HashTableHandleIterator ();
         ~HashTableHandleIterator ();
         Handle get_hash_key ();
         void reset ();
         struct idata;
         idata &data; //!< Internal state.

      private:
         HashTableHandleIterator (const HashTableHandleIterator &It);
   };

   class DMZ_KERNEL_LINK_SYMBOL HashTableHandle {

      public:
         HashTableHandle ();
         HashTableHandle (const Int32 Size, const UInt32 Attributes);
         ~HashTableHandle ();

         void clear ();

         Int32 get_size () const;
         Int32 get_count () const;

         void *get_next (HashTableHandleIterator &it, const Boolean Prev = False) const;

         void *lookup (const Handle &Key) const;
         Boolean store (const Handle &Key, void *data);
         void * remove (const Handle &Key);

         // For internal use
         void grow (const Int32 Size = 0);
         void set_table_size (const Int32 Size);
         void set_attributes (const UInt32 Attributes);
         UInt32 get_attributes () const;

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         HashTableHandle (const HashTableHandle &);
         HashTableHandle &operator= (const HashTableHandle &);
   };
};

#endif /* DMZ_TYPES_HASH_TABLE_HANDLE_DOT_H */
