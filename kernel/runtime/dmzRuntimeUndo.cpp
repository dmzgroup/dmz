#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextUndo.h"
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeUndo.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <dmzTypesString.h>
#include <qdb.h>

// #define DMZ_RUNTIME_UNDO_DEBUG

#ifdef DMZ_RUNTIME_UNDO_DEBUG
static dmz::qdb out;
#endif

/*!

\file dmzRuntimeUndo.h
\brief Contains classes related to undo and redo functionality.

\enum dmz::UndoTypeEnum
\ingroup Runtime
\brief Type of undo enumeration.
\details Defined in dmzRuntimeUndo.h.

\enum dmz::UndoRecordingStateEnum
\ingroup Runtime
\brief Undo recording state enumeration.
\details Defined in dmzRuntimeUndo.h.

\enum dmz::UndoRecordingTypeEnum
\ingroup Runtime
\brief Undo recording type enumeration.
\details Defined in dmzRuntimeUndo.h.

\class dmz::UndoDump
\ingroup Runtime
\brief Allows the current undo and redo stack to be dumped.

\fn dmz::UndoDump::UndoDump ()
\brief Constructor.

\fn dmz::UndoDump::~UndoDump ()
\brief Constructor.

\fn void dmz::UndoDump::start_record (const Handle RecordHandle, const String &Name)
\brief Callback made when a new undo is started.
\param[in] RecordHandle Handle of the undo.
\param[in] Name String containing a description of the undo.

\fn void dmz::UndoDump::store_action (
const Message &Type,
const Handle Target,
const Data *Value)
\brief Callback made to store an undo action.
\param[in] Type Message sent for undo action.
\param[in] Target Handle of the undo action target.
\param[in] Value Pointer to the Data object send with the undo action Message.


\class dmz::Undo
\ingroup Runtime
\brief Interface for undo and redo.
\details The Undo class provides an interface for recording and triggering undo and redo
messages. Undo and redo actions may be nested. When an action is invoked all
nested actions are invoked as well.

*/


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::Undo::Undo (RuntimeContext *context) : _context (0) {

   if (context) { _context = context->get_undo_context(); }
   if (_context) { _context->ref (); }
}


/*!

\brief PluginInfo constructor.
\param[in] Info PluginInfo used to obtain the runtime context.

*/
dmz::Undo::Undo (const PluginInfo &Info) : _context (0) {

   RuntimeContext *context (Info.get_context ());
   if (context) { _context = context->get_undo_context(); }
   if (_context) { _context->ref (); }
}


//! Destructor.
dmz::Undo::~Undo () { if (_context) { _context->unref (); } }


//! \brief Clears recorded undo and redo actions.
void
dmz::Undo::reset () {

   if (_context && !_context->currentStack) {

      if (_context->undoHead) { delete _context->undoHead; _context->undoHead = 0; }
      if (_context->redoHead) { delete _context->redoHead; _context->redoHead = 0; }
      _context->update_action_names ();
   }
}


/*!

\brief Tests if handle is from a nested undo dmz::Undo::start_record().
\note A nested handle can not be used to abort and undo.
\param[in] UndoHandle Handle to test.
\return Returns dmz::True if the handle is a nested handle.

*/
dmz::Boolean
dmz::Undo::is_nested_handle (const Handle UndoHandle) const {

   return _context && (_context->NestedHandle.get_runtime_handle () == UndoHandle);
}


/*!

\brief Returns undo state.
\return Returns dmz::True if called while an undo or redo is in progress.

*/
dmz::Boolean
dmz::Undo::is_in_undo () const {

   Boolean result (False);

   if (_context) { result = _context->inUndo; }

   return result;
}


/*!

\brief Returns recording state.
\return Returns dmz::True if actions are currently being recorded.

*/
dmz::Boolean
dmz::Undo::is_recording () const { return _context && (_context->currentStack != 0); }


/*!

\brief Gets current type of action being recorded.
\return Returns dmz::UndoTypeUndo if undo actions are being recorded and
dmz::UndoTypedRedp if redo actions are being recorded.

*/
dmz::UndoTypeEnum
dmz::Undo::get_type () const {

   UndoTypeEnum type (UndoTypeUndo);

   if (_context) {

      if (_context->currentStack && (_context->currentStack == _context->redoHead)) {

         type = UndoTypeRedo;
      }
   }

   return type;
}


/*!

\brief Dumps the undo recordings.
\note The undo steps are dumped from newest to oldest while each step is dumped in the
order that it was recorded and \b not the order it will be played back.
\param[in] Type Specifies whether to dump the Undo or Redo recordings.
\param[in] out Object used to dump undo recordings.

*/
void
dmz::Undo::dump (const UndoTypeEnum Type, UndoDump &out) {

   if (_context && !_context->currentStack) {

      UndoStackStruct *current (
         (Type == UndoTypeUndo) ? _context->undoHead : _context->redoHead);

      while (current) {

         out.start_record (current->UndoHandle.get_runtime_handle (), current->Name);

         UInt32 count (0);
         HashTableUInt32Template<UndoActionStruct> table;

         UndoActionStruct *uas (current->head);

         while (uas) {

            table.store (count, uas);
            count++;
            uas = uas->next;
         }

         if (count) {

            HashTableUInt32Iterator it;

            uas = table.get_last (it);

            while (uas) {

               out.store_action (uas->Type, uas->ObserverHandle, uas->data);

               uas = table.get_prev (it);
            }
         }

         current = current->next;
      }
   }
}


/*!

\brief Does the next action.
\param[in] Type Specifies which action type to playback.
\return Returns dmz::True if there was an action to play.
\note When playing back the next undo action, redo actions are automatically recorded.
When playing back the next redo action, undo actions are automatically recorded.

*/
dmz::Boolean
dmz::Undo::do_next (const UndoTypeEnum Type) {

   Boolean result (False);

   if (_context && !_context->currentStack) {

      _context->inUndo = True;

      UndoStackStruct *current (0);

      if (Type == UndoTypeUndo) {

         current = _context->undoHead;

         if (current) {

            _context->undoHead = current->next;

            _context->currentStack =
               new UndoStackStruct (current->Name, &(_context->context));

            if (_context->currentStack) {

               _context->currentStack->next = _context->redoHead;
               _context->redoHead = _context->currentStack;
            }
         }
      }
      else if (Type == UndoTypeRedo) {

         current = _context->redoHead;

         if (current) {

            _context->redoHead = current->next;

            _context->currentStack =
               new UndoStackStruct (current->Name, &(_context->context));

            if (_context->currentStack) {

               _context->currentStack->next = _context->undoHead;
               _context->undoHead = _context->currentStack;
            }
         }
      }

      if (current && _context->currentStack) {

         current->next = 0;

         const UndoTypeEnum RecordType (
            Type == UndoTypeUndo ? UndoTypeRedo : UndoTypeUndo);

         _context->update_record_state (
            UndoRecordingStateStart,
            UndoRecordingTypeAuto,
            RecordType);

         UndoActionStruct *action (current->head);

         while (action) {

#ifdef DMZ_RUNTIME_UNDO_DEBUG
out << "------- Start Do Action -------" << endl;
out << action->Type.get_name () << endl;
if (action->data) { out << *(action->data) << endl; }
out << "-------------------------------" << endl;
#endif

            action->Type.send (
               action->ObserverHandle,
               action->data,
               0);

            action = action->next;
         }

         _context->currentStack = 0;

         delete current; current = 0;

         _context->update_action_names ();

         _context->update_record_state (
            UndoRecordingStateStop,
            UndoRecordingTypeAuto,
            RecordType);

         result = True;
      }

      _context->inUndo = False;
   }

   return result;
}


/*!

\brief Start action recording.
\param[in] Name String containing the name of the action being recorded.
\return Returns the unique runtime handle associated with the action.
\note Recording may be nested. If start_record has already been invoked when
the next start_record is called, the new actions are nested under the initial
start_record undo action.

*/
dmz::Handle
dmz::Undo::start_record (const String &Name) {

   Handle result (0);

   if (_context) {

      if (!_context->currentStack) {

         _context->currentStack = new UndoStackStruct (Name, &(_context->context), False);

         if (_context->currentStack) {

#ifdef DMZ_RUNTIME_UNDO_DEBUG
out << "+++ Start Record: " << Name << ":" << result << endl;
#endif
            _context->currentStack->next = _context->undoHead;
            _context->undoHead = _context->currentStack;

            _context->update_record_state (
               UndoRecordingStateStart,
               UndoRecordingTypeExplicit,
               UndoTypeUndo);

            result = _context->currentStack->UndoHandle.get_runtime_handle ();
         }
      }
      else { result = _context->NestedHandle.get_runtime_handle (); }
   }

   return result;
}


/*!

\brief Stores an action.
\param[in] Type Message that will be sent when the action is played back.
\param[in] ObserverHandle Handle to the observer the action message should be set to.
If set to zero, it is sent to all subscribers of the specified message type.
\param[in] UndoData Pointer to Data object to be sent with the action message. Pointer
may be NULL if no data needs to be sent with the message.
\return Returns dmz::True if the action was stored successfully. Will return
dmz::False if actions are not currently being recorded.

*/
dmz::Boolean
dmz::Undo::store_action (
      const Message &Type,
      const Handle ObserverHandle,
      const Data *UndoData) {

   Boolean result (False);

   if (_context && _context->currentStack) {

      UndoActionStruct *uas (new UndoActionStruct (Type, ObserverHandle, UndoData));

#ifdef DMZ_RUNTIME_UNDO_DEBUG
out << "###### Start Store Action ######" << endl;
out << Type.get_name () << endl;
if (UndoData) { out << *UndoData << endl; }
out << "################################" << endl;
#endif

      if (uas) {

         result = True;
         uas->next = _context->currentStack->head;
         _context->currentStack->head = uas;
      }
   }

   return result;
}

/*!

\fn dmz::Boolean dmz::Undo::store_action (const Message &Type, const Data *UndoData)
\brief Stores an action.
\param[in] Type Message that will be sent when the action is played back.
\param[in] UndoData Pointer to Data object to be sent with the action message. Pointer
may be NULL if no data needs to be send with the message.
\return Returns dmz::True if the action was stored successfully. Will return
dmz::False if actions are not currently being recorded.
\note When the stored action is played back, the message is sent to all subscribers to
the message type.

*/

/*!

\brief Stop action record.
\param[in] RecordHandle Unique runtime handle returned from start_record.
\return Returns dmz::True if the action record was stopped. Returns dmz::False
if the action record has already been stopped.
\note If an action record is stopped with nested action records still recording,
the nested action records are stopped as well.

*/
dmz::Boolean
dmz::Undo::stop_record (const Handle RecordHandle) {

   Boolean result (False);

   if (_context && _context->currentStack) {

      if (RecordHandle == _context->currentStack->UndoHandle.get_runtime_handle ()) {

#ifdef DMZ_RUNTIME_UNDO_DEBUG
out << "+++ Stop Record: " << _context->currentStack->Name << endl;
#endif
         result = True;

         // If the current root stack is for recording undo and it was not created by
         // do_next (i.e. AutoCreated == True) then the redo stack needs to be deleted.
         if ((_context->currentStack == _context->undoHead) &&
               _context->redoHead &&
               _context->currentStack &&
               !_context->currentStack->AutoCreated) {

            delete _context->redoHead; _context->redoHead = 0;
         }

         _context->currentStack = 0;
         _context->update_action_names ();
         _context->update_record_state (
            UndoRecordingStateStop,
            UndoRecordingTypeExplicit,
            UndoTypeUndo);
      }
      else if (RecordHandle == _context->NestedHandle.get_runtime_handle ()) {

         result = True;
      }
   }

   return result;
}


/*!

\brief Aborts an action record.
\param[in] RecordHandle Unique runtime handle returned from start_record.
\return Returns dmz::True if the action record was aborted. Returns dmz::False
if the action record has already been stopped or aborted.
\note If an action record is aborted with nested action records still recording,
the nested action records are aborted as well.

*/
dmz::Boolean
dmz::Undo::abort_record (const Handle RecordHandle) {

   Boolean result (False);

   if (_context &&
         _context->currentStack &&
         (RecordHandle == _context->currentStack->UndoHandle.get_runtime_handle ())) {

      UndoStackStruct *stack (_context->currentStack);
      _context->currentStack = 0;

      if (_context->undoHead == stack) { _context->undoHead = stack->next; }
      else if (_context->redoHead == stack) { _context->redoHead = stack->next; }
      stack->next = 0;

      _context->update_record_state (
         UndoRecordingStateStop,
         UndoRecordingTypeExplicit,
         UndoTypeUndo);

      delete stack; stack = 0;
   }

   return result;
}

/*!

\class dmz::UndoObserver
\ingroup Runtime
\brief Interface for observing state changes in the Undo class.

*/

struct dmz::UndoObserver::State {

   Handle handle;
   RuntimeHandle *handlePtr;
   RuntimeContextUndo *context;

   State (const Handle TheHandle, RuntimeContext *theContext) :
         handle (TheHandle),
         handlePtr (0),
         context (0) {

      if (theContext) { context = theContext->get_undo_context (); }
      if (context) { context->ref (); }

      if (!handle) {

         handlePtr = new RuntimeHandle ("UndoObserver", theContext);
         handle = handlePtr ? handlePtr->get_runtime_handle () : 0;
      }
   }

   ~State () {

      if (context) { context->unref (); }
      if (handlePtr) { delete handlePtr; handlePtr = 0; }
   }
};


/*!

\brief Constructor.
\param[in] ObserverHandle Unique runtime handle.
\param[in] context Pointer to runtime context.

*/
dmz::UndoObserver::UndoObserver (const Handle ObserverHandle, RuntimeContext *context) :
      __state (*(new State (ObserverHandle, context))) {

   if (__state.handle && __state.context) {

      if (!__state.context->obsTable.store (__state.handle, this)) {

         __state.handle = 0;
      }
   }
}


/*!

\brief PluginInfo construct.
\param[in] Info PluginInfo object containing Handle and RuntimeContext.

*/
dmz::UndoObserver::UndoObserver (const PluginInfo &Info) :
      __state (*(new State (Info.get_handle (), Info.get_context ()))) {

   if (__state.handle && __state.context) {

      if (!__state.context->obsTable.store (__state.handle, this)) {

         __state.handle = 0;
      }
   }
}


//! Destructor.
dmz::UndoObserver::~UndoObserver () {

   if (__state.handle && __state.context) {

      __state.context->obsTable.remove (__state.handle);
   }

   delete &__state;
}

/*!

\fn void dmz::UndoObserver::update_recording_state (
const UndoRecordingStateEnum RecordingState,
const UndoRecordingTypeEnum RecordingType,
const UndoTypeEnum UndoType)
\brief Pure virtual function that is invoked when the recording state changes.
\param[in] RecordingState UndoRecordingStateEnum Indicates whether actions are
currently being
recorded.
\param[in] RecordingType UndoRecordingTypeEnum Indicates whether the recording was
started automatically or explicitly. Auto records are started when dmz::Undo::do_next()
is called.
\param[in] UndoType UndoTypeEnum indicates which type of action is changing state. Either
undo or redo.

\fn void dmz::UndoObserver::update_current_undo_names (
const String *NextUndoName,
const String *NextRedoName)
\brief Pure virtual function that is invoked when the name of the next undo and redo
action change.
\param[in] NextUndoName Pointer to a String containing the name of the next undo action.
Pointer will be NULL if there is no next undo action.
\param[in] NextRedoName Pointer to a String containing the name of the next redo action.
Pointer will be NULL if there is no next redo action.

*/
