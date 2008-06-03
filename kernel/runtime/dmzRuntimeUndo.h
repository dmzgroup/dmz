#ifndef DMZ_RUNTIME_UNDO_DOT_H
#define DMZ_RUNTIME_UNDO_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   enum UndoTypeEnum {
      UndoTypeUndo, //!< Undo
      UndoTypeRedo, //!< Redo
   };

   enum UndoRecordingStateEnum {
      UndoRecordingStateStart, //!< Start recording undo actions.
      UndoRecordingStateStop, //!< Stop recording undo actions.
   };

   enum UndoRecordingTypeEnum {
      UndoRecordingTypeExplicit, //!< Recording undo actions explicitly.
      UndoRecordingTypeAuto, //!< Recording undo actions automatically.
   };

   class Data;
   class MessageType;
   class PluginInfo;
   class RuntimeContext;
   class RuntimeContextUndo;

   class UndoDump {

      public:
         virtual void start_record (const Handle RecordHandle, const String &Name) = 0;

         virtual void store_action (
            const MessageType &Type,
            const Handle Target,
            const Data *Value) = 0;

      protected:
         UndoDump () {;}
         ~UndoDump () {;}

      private:
         UndoDump (const UndoDump &);
         UndoDump &operator= (const UndoDump &);
   };

   class DMZ_KERNEL_LINK_SYMBOL Undo {

      public:
         Undo (RuntimeContext *context);
         Undo (const PluginInfo &Info);
         ~Undo ();

         void reset ();

         Boolean is_nested_handle (const Handle UndoHandle) const;
         Boolean is_in_undo () const;
         Boolean is_recording () const;
         UndoTypeEnum get_type () const;
         void dump (const UndoTypeEnum Type, UndoDump &out);

         Boolean do_next (const UndoTypeEnum Type);

         Handle start_record (const String &Name);

         Boolean store_action (
            const MessageType &Type,
            const Handle ObserverHandle,
            const Data *UndoData);

         Boolean store_action (const MessageType &Type, const Data *UndoData) {

            return store_action (Type, 0, UndoData);
         }

         Boolean stop_record (const Handle Handle);
         Boolean abort_record (const Handle Handle);

      protected:
         RuntimeContextUndo *_context; //!< Internal state.

      private:
         Undo ();
         Undo (const Undo &);
         Undo &operator= (const Undo &);
   };

   class DMZ_KERNEL_LINK_SYMBOL UndoObserver {

      public:
         virtual void update_recording_state (
            const UndoRecordingStateEnum RecordingState,
            const UndoRecordingTypeEnum RecordingType,
            const UndoTypeEnum UndoType) = 0;

         virtual void update_current_undo_names (
            const String *NextUndoName,
            const String *NextRedoName) = 0;

      protected:
         UndoObserver (const Handle ObserverHandle, RuntimeContext *context);
         UndoObserver (const PluginInfo &Info);
         ~UndoObserver ();

      private:
         struct State;
         UndoObserver ();
         UndoObserver (const UndoObserver &);
         UndoObserver &operator= (const UndoObserver &);
         State &__state;
   };
};

#endif// DMZ_RUNTIME_UNDO_DOT_H
