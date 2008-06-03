#ifndef DMZ_RUNTIME_CONTEXT_THREAD_KEY_DOT_H
#define DMZ_RUNTIME_CONTEXT_THREAD_KEY_DOT_H

#include <dmzSystemRefCount.h>
#include <dmzSystemThread.h>

namespace dmz {

/*!

\class dmz::RuntimeContextThreadKey
\brief \b For \b internal \b kernel \b use \b ONLY.
\details Internal runtime class that indicates the main thread. Should \b not be
used outside of the kernel.

*/

   class RuntimeContextThreadKey : public RefCountDeleteOnZero {

      public:
         RuntimeContextThreadKey (void *contextPtr);

         Boolean is_main_thread ();

      protected:
         const void *_Ptr; //!< Void pointer that indicates main thread.
         ThreadStorage _key; //!< Thread key.

      private:
         RuntimeContextThreadKey ();
         RuntimeContextThreadKey (const RuntimeContextThreadKey &);
         ~RuntimeContextThreadKey ();
         const RuntimeContextThreadKey &operator= (const RuntimeContextThreadKey &);
   };
};


//! Constructor.
inline
dmz::RuntimeContextThreadKey::RuntimeContextThreadKey (void *contextPtr) :
      _Ptr (contextPtr) {

   _key.set_data (contextPtr);
}


//! Destructor.
inline
dmz::RuntimeContextThreadKey::~RuntimeContextThreadKey () { _key.set_data (0); }


//! Returns dmz::True if in main thread.
inline dmz::Boolean
dmz::RuntimeContextThreadKey::is_main_thread () { return _Ptr == _key.get_data (); }

#endif // DMZ_RUNTIME_CONTEXT_THREAD_KEY_DOT_H
