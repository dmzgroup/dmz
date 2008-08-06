#ifndef DMZ_RUNTIME_CONTEXT_DOT_H
#define DMZ_RUNTIME_CONTEXT_DOT_H

#include <dmzRuntimeConfig.h>
#include <dmzSystemSpinLock.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesUUID.h>

namespace dmz {

   class HandleAllocator;
   class RuntimeContextDefinitions;
   class RuntimeContextLog;
   class RuntimeContextMessaging;
   class RuntimeContextMessageContainer;
   class RuntimeContextRTTI;
   class RuntimeContextThreadKey;
   class RuntimeContextTime;
   class RuntimeContextUndo;

   class RuntimeContext : public RefCountDeleteOnZero {

      public:
         RuntimeContext ();
         UUID uuid;

         void update_time_slice ();

         Config get_session_config () const;
         void set_session_config (const Config &Session);

         RuntimeContextThreadKey *get_thread_key ();

         HandleAllocator *get_handle_allocator ();

         RuntimeContextDefinitions *get_definitions_context ();
         RuntimeContextMessageContainer *get_message_container_context ();
         RuntimeContextMessaging *get_messaging_context ();
         RuntimeContextRTTI *get_rtti_context ();
         RuntimeContextTime *get_time_context ();
         RuntimeContextUndo *get_undo_context ();
         RuntimeContextLog *get_log_context ();

      protected:
         Config _session; //!< Session Config.
         SpinLock _keyLock;
         RuntimeContextThreadKey *_key; //!< Thread Key.
         SpinLock _allocLock;
         HandleAllocator *_handleAllocator; //!< Handle allocator.
         SpinLock _defLock;
         RuntimeContextDefinitions *_defContext; //!< Attribute definitions.
         SpinLock _msgContainerLock;
         RuntimeContextMessageContainer *_messageContainerContext;
         SpinLock _msgLock;
         RuntimeContextMessaging *_messagingContext; //!< Messaging context.
         SpinLock _rttiLock;
         RuntimeContextRTTI *_rttiContext; //!< RTTI context.
         SpinLock _timeLock;
         RuntimeContextTime *_timeContext; //!< Time context.
         SpinLock _undoLock;
         RuntimeContextUndo *_undoContext; //!< Undo context.
         SpinLock _logLock;
         RuntimeContextLog *_logContext; //!< Log context.

      private:
         ~RuntimeContext ();
   };
};

#endif // DMZ_RUNTIME_CONTEXT_DOT_H
