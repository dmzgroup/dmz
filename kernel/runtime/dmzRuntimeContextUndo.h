#ifndef DMZ_RUNTIME_CONTEXT_UNDO_DOT_H
#define DMZ_RUNTIME_CONTEXT_UNDO_DOT_H

#include <dmzRuntimeData.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeUndo.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   struct UndoActionStruct {

      const Message Type;
      const Handle ObserverHandle;
      Data *data;

      UndoActionStruct *next;

      UndoActionStruct (
            const Message &TheType,
            const Handle TheHandle,
            const Data *TheData) :
            Type (TheType),
            ObserverHandle (TheHandle),
            data (TheData ? new Data (*TheData) : 0),
            next (0) {;}

      ~UndoActionStruct () {

         if (data) { delete data; data = 0; }
         if (next) { delete next; next = 0; }
      }
   };

   struct UndoStackStruct {

      const String Name;
      const RuntimeHandle UndoHandle;
      const Boolean AutoCreated;
      UndoActionStruct *head;
      UndoStackStruct *next;

      UndoStackStruct (
            const String &TheName,
            RuntimeContext *context,
            const Boolean IsAutoCreated = True) :
            Name (TheName),
            UndoHandle (TheName + ".UndoHandle", context),
            AutoCreated (IsAutoCreated),
            head (0),
            next (0) {;}

      ~UndoStackStruct () {

         if (head) { delete head; head = 0; }
         if (next) { delete next; next = 0; }
      }
   };

/*!

\class dmz::RuntimeContextUndo
\brief \b For \b internal \b kernel \b use \b ONLY.

Internal runtime class that handles runtime undo. Should \b not be used outside of
the kernel.

*/
   class RuntimeContextUndo : public RefCountDeleteOnZero {

      public:
         RuntimeContextUndo (RuntimeContext &theContext);

         void update_record_state (
            const UndoRecordingStateEnum State,
            const UndoRecordingTypeEnum RecordingType,
            const UndoTypeEnum Type);

         void update_action_names ();

         const RuntimeHandle NestedHandle;

         RuntimeContext &context; //!< Runtime context reference.
         HashTableHandleTemplate<UndoObserver> obsTable; //!< Table.

         Boolean inUndo;

         UndoStackStruct *currentStack; //!< Current stack.
         UndoStackStruct *undoHead; //!< Undo list head.
         UndoStackStruct *redoHead; //!< Redo list head.


      private:
         virtual ~RuntimeContextUndo ();
   };
};


//! Constructor.
inline
dmz::RuntimeContextUndo::RuntimeContextUndo (
      RuntimeContext &theContext) :
      NestedHandle ("dmz.Undo.NestedHandle", &theContext),
      context (theContext),
      inUndo (False),
      currentStack (0),
      undoHead (0),
      redoHead (0) {;}


//! Destructor.
inline
dmz::RuntimeContextUndo::~RuntimeContextUndo () {

   obsTable.clear ();
   currentStack = 0;
   if (undoHead) { delete undoHead; undoHead = 0; }
   if (redoHead) { delete redoHead; redoHead = 0; }
}


//! Updates undo record state.
inline void
dmz::RuntimeContextUndo::update_record_state (
      const UndoRecordingStateEnum RecordingState,
      const UndoRecordingTypeEnum RecordingType,
      const UndoTypeEnum UndoType) {

   if (obsTable.get_count ()) {

      HashTableHandleIterator it;

      UndoObserver *obs (obsTable.get_first (it));

      while (obs) {

         obs->update_recording_state (RecordingState, RecordingType, UndoType);
         obs = obsTable.get_next (it);
      }
   }
}


//! Updates current action names.
inline void
dmz::RuntimeContextUndo::update_action_names () {

   if (obsTable.get_count ()) {

      const String *UndoName (0);
      const String *RedoName (0);

      if (undoHead) { UndoName = &(undoHead->Name); }
      if (redoHead) { RedoName = &(redoHead->Name); }

      HashTableHandleIterator it;

      UndoObserver *obs (obsTable.get_first (it));

      while (obs) {

         obs->update_current_undo_names (UndoName, RedoName);
         obs = obsTable.get_next (it);
      }
   }
}

#endif // DMZ_RUNTIME_CONTEXT_UNDO_DOT_H
