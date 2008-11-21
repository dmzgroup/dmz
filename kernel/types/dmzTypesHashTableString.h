#ifndef DMZ_TYPES_HASH_TABLE_STRING_DOT_H
#define DMZ_TYPES_HASH_TABLE_STRING_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesHashTableLock.h>
#include <dmzTypesString.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HashTableStringIterator {

      public:
         HashTableStringIterator ();
         ~HashTableStringIterator ();
         String get_hash_key ();
         void reset ();
         struct idata;
         idata &data; //!< Internal state.

      private:
         HashTableStringIterator (const HashTableStringIterator &It);
   };

   class DMZ_KERNEL_LINK_SYMBOL HashTableString {

      public:
         HashTableString ();
         HashTableString (const Int32 Size, const UInt32 Attributes);
         ~HashTableString ();

         void clear ();

         Int32 get_size () const;
         Int32 get_count () const;

         Boolean move (
            const String &Key,
            const String *TargetKey,
            const Boolean SignleStep,
            const Boolean Before);

         void *get_next (HashTableStringIterator &it, const Boolean Prev = False) const;

         void *lookup (const String &Key) const;
         Boolean store (const String &Key, void *data);
         void * remove (const String &Key);

         // For internal use
         void grow (const Int32 Size = 0);
         void set_table_size (const Int32 Size);
         void set_attributes (const UInt32 Attributes);
         UInt32 get_attributes () const;

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         HashTableString (const HashTableString &);
         HashTableString &operator= (const HashTableString &);
   };
};

#endif /* DMZ_TYPES_HASH_TABLE_STRING_DOT_H */
