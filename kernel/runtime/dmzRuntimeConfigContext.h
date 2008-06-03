#ifndef DMZ_RUNTIME_CONFIG_CONTEXT_DOT_H
#define DMZ_RUNTIME_CONFIG_CONTEXT_DOT_H

#include <dmzSystemRefCount.h>
#include <dmzSystemSpinLock.h>
#include <dmzTypesHashTableLock.h>
#include <dmzTypesHashTableStringTemplate.h>

namespace dmz {

   class ConfigContextLock : public HashTableLock {

      protected:
         SpinLock _lock;

      public:
         ConfigContextLock () {;}
         ~ConfigContextLock () {;}

         virtual void lock () { _lock.lock (); }
         virtual void unlock () { _lock.unlock (); }

      private:
         ConfigContextLock (const ConfigContextLock &Lock);
         ConfigContextLock &operator= (const ConfigContextLock &Lock);
   };

   class ConfigContextCounter : protected RefCount {

      public:
         virtual ~ConfigContextCounter () {;}

         UInt32 get_next () { return UInt32 (ref ()); }

      protected:
         virtual void ref_count_is_zero () {;}
   };

   class ConfigAttributeContext {

      public:
         SpinLock lock;
         String value;

         explicit ConfigAttributeContext (const String &Value) : value (Value) {;}
         ~ConfigAttributeContext () {;}

      private:
         ConfigAttributeContext ();
         ConfigAttributeContext (const ConfigAttributeContext &);
         ConfigAttributeContext &operator= (const ConfigAttributeContext &);
   };

   class ConfigContext : public RefCountDeleteOnZero {

      public:
         struct DataStruct {

            UInt32 handle;
            ConfigContext *context;
            DataStruct *next;

            DataStruct (const UInt32 TheHandle, ConfigContext *theContext) :
                  handle (TheHandle),
                  context (theContext),
                  next (0) { if (context) { context->ref (); } }

            ~DataStruct () {

               if (context) { context->unref (); context = 0; }
               if (next) { delete next; next = 0; }
            }
         };

         struct DataList {

            SpinLock lock;
            DataStruct *head;
            DataStruct *tail;

            DataList () : head (0), tail (0) {;}
            ~DataList () { if (head) { delete head; head = tail = 0; } }
         };

         ConfigContext (const String &TheName);

         Boolean add_config (ConfigContext *context);
         Boolean remove_config (const String &Name);

         const String Name;

         Boolean isFormatted;

         ConfigContextLock attrLock;
         HashTableStringTemplate<ConfigAttributeContext> attrTable;

         ConfigContextCounter orderCount;

         ConfigContextLock dataLock;
         HashTableStringTemplate<DataList> configTable;

         ConfigContextLock dataOrderLock;
         HashTableStringTemplate<DataStruct> configOrderTable;

      protected:
         virtual ~ConfigContext ();

      private:
         ConfigContext ();
         ConfigContext (const ConfigContext &Context);
         ConfigContext &operator= (const ConfigContext &Context);
   };
};


inline
dmz::ConfigContext::ConfigContext (const String &TheName) :
      Name (TheName),
      isFormatted (False),
      attrTable (&attrLock),
      configTable (&dataLock),
      configOrderTable (&dataOrderLock) {;}


inline
dmz::ConfigContext::~ConfigContext () {

   attrTable.empty ();
   configOrderTable.clear ();
   configTable.empty ();
}
  

inline dmz::Boolean
dmz::ConfigContext::add_config (ConfigContext *context) {

   Boolean result (False);

   if (context) {

      DataList *dl = configTable.lookup (context->Name);

      if (!dl) {

         dl = new DataList;
         if (!configTable.store (context->Name, dl)) {

            delete dl; dl = 0;
            dl = configTable.lookup (context->Name);
         }
      }

      DataStruct *ds = new DataStruct (orderCount.get_next (), context);

      if (dl && ds) {

         String handleName;
         handleName << ds->handle;

         if (configOrderTable.store (handleName, ds)) {

            dl->lock.lock ();
               if (dl->tail) { dl->tail->next = ds; dl->tail = ds; }
               else { dl->head = dl->tail = ds; }
            dl->lock.unlock ();

            result = True;
         }
         else { delete ds; ds = 0; }
      }
      else if (!dl && ds) { delete ds; ds = 0; }
   }

   return result;
}


inline dmz::Boolean
dmz::ConfigContext::remove_config (const String &Name) {

   Boolean result (False);

   DataList *dl = configTable.lookup (Name);

   if (dl) {

      result = True;

      dl->lock.lock ();
      DataStruct *current (dl->head);
      dl->head = dl->tail = 0;
      dl->lock.unlock ();

      while (current) {

         current->handle = 0;
         current = current->next;
      }
   }

   return result;
}

#endif // DMZ_RUNTIME_CONFIG_CONTEXT_DOT_H

