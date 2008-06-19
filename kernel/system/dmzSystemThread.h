#ifndef DMZ_SYSTEM_THREAD_DOT_H
#define DMZ_SYSTEM_THREAD_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL ThreadStorage {

      public:
         ThreadStorage ();
         ~ThreadStorage ();

         void set_data (void *ptr);
         void *get_data ();

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   //! \brief Template that wraps ThreadStorage.
   //! \ingroup System
   template <class T> class ThreadStorageTemplate {

      public:
         ThreadStorageTemplate () {;}
         ~ThreadStorageTemplate () {;}

         void set_data (T *data);
         T *get_data ();

      protected:
         ThreadStorage _ts; //!< ThreadStorage object wrapped by template.

      private:
         ThreadStorageTemplate (const ThreadStorageTemplate<T> &Ts);
         ThreadStorageTemplate<T> &operator= (const ThreadStorageTemplate<T> &Ts);
   };

   class DMZ_KERNEL_LINK_SYMBOL ThreadCleanup {

      public:
         virtual ~ThreadCleanup ();

      protected:
         ThreadCleanup ();
         struct ThreadCleanupState;
         ThreadCleanupState &_threadCleanupState; //!< Internal state.
   };

   class DMZ_KERNEL_LINK_SYMBOL ThreadFunction {

      public:
         virtual ~ThreadFunction () {;} //!< Destructor
         virtual void run_thread_function () = 0;

      protected:
         ThreadFunction () {;} //!< Constructor.
   };

   DMZ_KERNEL_LINK_SYMBOL Boolean create_thread (ThreadFunction &tf);

   DMZ_KERNEL_LINK_SYMBOL void thread_exit ();

   DMZ_KERNEL_LINK_SYMBOL void cleanup_thread ();
};


/*!

\brief Ensures type safety when setting thread specific data.
\param[in] data Pointer value being associated with the thread.

*/
template <class T> inline void
dmz::ThreadStorageTemplate<T>::set_data (T *data) { _ts.set_data ((void *)data); }


/*!

\brief Ensures type safety when getting thread specific data.
\return Returns pointer to value associated with the thread. Returns NULL if no data
has been associated with the thread.

*/
template <class T> inline T *
dmz::ThreadStorageTemplate<T>::get_data () { return (T *)_ts.get_data (); }


#endif // DMZ_SYSTEM_THREAD_DOT_H
