
#include <dmzKernelExport.h>
#include <dmzTypesHashTableLock.h>
#include <$(typeInclude)>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HashTable$(type)Iterator {

      public:
         HashTable$(type)Iterator ();
         ~HashTable$(type)Iterator ();
         $(type) get_hash_key ();
         void reset ();
         struct idata;
         idata &data; //!< Internal state.

      private:
         HashTable$(type)Iterator (const HashTable$(type)Iterator &It);
   };

   class DMZ_KERNEL_LINK_SYMBOL HashTable$(type) {

      public:
         HashTable$(type) ();
         HashTable$(type) (const Int32 Size, const UInt32 Attributes);
         ~HashTable$(type) ();

         void clear ();

         Int32 get_size () const;
         Int32 get_count () const;

         Boolean move (
            const $(type) &Key,
            const $(type) *TargetKey,
            const Boolean SignleStep,
            const Boolean Before);

         void *get_next (HashTable$(type)Iterator &it, const Boolean Prev = False) const;

         void *lookup (const $(type) &Key) const;
         Boolean store (const $(type) &Key, void *data);
         void * remove (const $(type) &Key);

         // For internal use
         void grow (const Int32 Size = 0);
         void set_table_size (const Int32 Size);
         void set_attributes (const UInt32 Attributes);
         UInt32 get_attributes () const;

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         HashTable$(type) (const HashTable$(type) &);
         HashTable$(type) &operator= (const HashTable$(type) &);
   };
};

