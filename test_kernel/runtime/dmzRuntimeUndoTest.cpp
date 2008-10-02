#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDataConverters.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeUndo.h>
#include <dmzSystem.h>
#include <dmzTest.h>
#include <dmzTypesBase.h>
#include <qdb.h>

using namespace dmz;

class UndoTest : public MessageObserver {

   public:
      UndoTest (
         const String ReceiverName,
         const Config &Data,
         RuntimeContext *context);

      ~UndoTest () {;}

      // MessageObserver interface
      void receive_message (
         const Message &Type,
         const UInt32 Count,
         const UInt32 Handle,
         const Data *InData,
         Data *outData);

      // Class specific
      Boolean valid ();
      void reset ();
      void set_undo ();

      const String Name;
      Boolean gotUndo;
      Boolean gotRedo;
      DataConverterString dcs;
      Undo undo;
      Message undoType;
      Message redoType;
};


UndoTest::UndoTest (
      const String ReceiverName,
      const Config &Data,
      RuntimeContext *context) :
      MessageObserver (0, ReceiverName, context),
      Name (ReceiverName),
      gotUndo (False),
      gotRedo (False),
      dcs (context),
      undo (context) {

   undoType = config_to_message_type (Name + ".undo", Data, context);
   redoType = config_to_message_type (Name + ".redo", Data, context);

   subscribe_to_message (undoType);
   subscribe_to_message (redoType);
}


void
UndoTest::receive_message (
      const Message &Type,
      const UInt32 Count,
      const UInt32 Handle,
      const Data *InData,
      Data *outData) {

   if ((Type == undoType) && (Name == dcs.to_string (InData))) {

      gotUndo = True;
      Data data (dcs.to_data (Name));
      undo.store_action (redoType, get_message_observer_handle (), &data);
   }
   else if ((Type == redoType) && (Name == dcs.to_string (InData))) {

      gotRedo = True;
      Data data (dcs.to_data (Name));
      undo.store_action (undoType, get_message_observer_handle (), &data);
   }
}


Boolean
UndoTest::valid () {

   Boolean result (False);
   if (undoType && redoType) { result = True; }
   return result;
}


void
UndoTest::reset () { gotUndo = gotRedo = False; }


void
UndoTest::set_undo () {

   Data data (dcs.to_data (Name));

   undo.store_action (undoType, get_message_observer_handle (), &data);
}


class TestObserver : public UndoObserver {

   public:
      TestObserver (RuntimeContext *context);
      ~TestObserver () {;}

      virtual void update_recording_state (
         const UndoRecordingStateEnum State,
         const UndoRecordingTypeEnum RecordingType,
         const UndoTypeEnum Type);

      virtual void update_current_undo_names (
         const String *NextUndoName,
         const String *NextRedoName);

      String undoName;
      String redoName;
};


TestObserver::TestObserver (RuntimeContext *context) :
      UndoObserver (0, context) {;}


void
TestObserver::update_recording_state (
      const UndoRecordingStateEnum State,
      const UndoRecordingTypeEnum RecordingType,
      const UndoTypeEnum Type) {

}


void
TestObserver::update_current_undo_names (
      const String *NextUndoName,
      const String *NextRedoName) {

   if (NextUndoName) { undoName = *NextUndoName; }
   else { undoName.flush (); }

   if (NextRedoName) { redoName = *NextRedoName; }
   else { redoName.flush (); }
}


int
main (int argc, char *argv[]) {

   Test test ("dmzRuntimeUndoTest", argc, argv);
   RuntimeContext *context (test.rt.get_context ());

   Undo undo (context);
   TestObserver obs (context);

   const String Level1 ("Level1");
   const String Level2 ("Level2");
   const String Level3 ("Level3");

   Config runtimeConfig;

   if (test.validate (
         "Initializing runtime",
         test.config.lookup_all_config_merged ("dmz.runtime", runtimeConfig))) {

      runtime_init (runtimeConfig, test.rt.get_context (), &(test.log));
   }

   UndoTest undo1 ("dmz.test1", test.config, context);
   UndoTest undo2 ("dmz.test2", test.config, context);
   UndoTest undo3 ("dmz.test3", test.config, context);

   test.validate (
      "Test object initialized",
       undo1.valid () && undo2.valid () && undo3.valid ());

   UInt32 record1 (undo.start_record (Level1));

   test.validate ("Received valid record handle", record1 != 0);

   test.validate ("Next undo and redo names not set", !obs.undoName && !obs.redoName);

   undo1.set_undo ();

   test.validate (
      "record_stop returns False with invalid handle",
      !undo.stop_record (record1 + 99));

   test.validate (
      "record_stop returns True with valid handle",
      undo.stop_record (record1));

   test.validate (
      "Next undo name set to Level1 and redo name is not set",
      (obs.undoName == Level1) && !obs.redoName);

   test.validate (
      "do_next (UndoTypeRedo) fails with empty redo stack",
      !undo.do_next (UndoTypeRedo));

   test.validate ("do_next (UndoTypeType) returns True", undo.do_next (UndoTypeUndo));

   test.validate (
      "Next redo name set to Level1 and undo name is not set",
      (obs.redoName == Level1) && !obs.undoName);

   test.validate (
      "Undo message sent to correct observer",
      undo1.gotUndo && !undo1.gotRedo);

   undo1.reset ();

   test.validate (
      "do_next (UndoTypeUndo) fails with empty undo stack",
      !undo.do_next (UndoTypeUndo));

   test.validate ("do_next (UndoTypeRedo) returns True", undo.do_next (UndoTypeRedo));

   test.validate (
      "Redo message sent to correct observer",
      !undo1.gotUndo && undo1.gotRedo);

   test.validate (
      "Next undo name == Level1 and redo name == NULL after do_next (UndoTypeRedo)",
      (obs.undoName == Level1) && !obs.redoName);

   undo.do_next (UndoTypeUndo);
   undo1.reset ();

   record1 = undo.start_record (Level1);

   undo1.set_undo ();
   undo2.set_undo ();

   undo.stop_record (record1);

   test.validate (
      "Next undo name == Level1 and redo name == NULL after record",
      (obs.undoName == Level1) && !obs.redoName);

   record1 = undo.start_record (Level1);

   undo1.set_undo ();
   undo2.set_undo ();

   UInt32 record2 (undo.start_record (Level2));

   test.validate ("Nested start_record returns valid handle", record2 != 0);

   undo3.set_undo ();

   test.validate ("stop_record of nested record return true", undo.stop_record (record2));
   test.validate ("stop_record of root record return true", undo.stop_record (record1));

   test.validate (
      "Next undo name == Level1 and redo name == NULL after record",
      (obs.undoName == Level1) && !obs.redoName);

   undo.do_next (UndoTypeUndo);

   test.validate (
      "Next undo name == Level1 and redo name == Level1 after do_next (UndoTypeUndo)",
      (obs.undoName == Level1) && (obs.redoName == Level1));

   test.validate (
      "Received all undo messages from first do_next (UndoTypeUndo)",
      undo1.gotUndo && !undo1.gotRedo &&
      undo2.gotUndo && !undo2.gotRedo &&
      undo3.gotUndo && !undo3.gotRedo);

   undo1.reset ();
   undo2.reset ();
   undo3.reset ();

   record1 = undo.start_record (Level1);
   undo1.set_undo ();
   record2 = undo.start_record (Level2);
   undo2.set_undo ();
   UInt32 record3 (undo.start_record (Level3));
   undo3.set_undo ();

   test.validate (
      "Level2 and Level3 are nested undo handles",
      undo.is_nested_handle (record2) && undo.is_nested_handle (record3));


   test.validate (
      "stop_record of root record Level1 return true",
      undo.stop_record (record1));

   undo.do_next (UndoTypeUndo);

   test.validate (
      "Received all undo messages from second do_next (UndoTypeUndo)",
      undo1.gotUndo && !undo1.gotRedo &&
      undo2.gotUndo && !undo2.gotRedo &&
      undo3.gotUndo && !undo3.gotRedo);

   undo1.reset ();
   undo2.reset ();
   undo3.reset ();

   undo.do_next (UndoTypeUndo);

   test.validate (
      "Received all undo messages from third do_next (UndoTypeUndo)",
      undo1.gotUndo && !undo1.gotRedo &&
      undo2.gotUndo && !undo2.gotRedo &&
      !undo3.gotUndo && !undo3.gotRedo);

   undo1.reset ();
   undo2.reset ();
   undo3.reset ();

   return test.result ();
}
