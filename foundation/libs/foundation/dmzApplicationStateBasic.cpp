#include "dmzApplicationStateBasic.h"

//! \cond
dmz::ApplicationStateBasic::ApplicationStateBasic (RuntimeContext *context) :
      ApplicationState (context),
      _modeStack (0) {;}


dmz::ApplicationStateBasic::~ApplicationStateBasic () {

   if (_modeStack) { delete _modeStack; _modeStack = 0; }
}


void
dmz::ApplicationStateBasic::set_default_directory (const String &DirName) {

   _defaultDir = DirName;
}


dmz::String
dmz::ApplicationStateBasic::get_default_directory () const { return _defaultDir; }


void
dmz::ApplicationStateBasic::set_autosave_file (const String &FileName) {

   _autosaveFile = FileName;
}


dmz::String
dmz::ApplicationStateBasic::get_autosave_file () const { return _autosaveFile; }


void
dmz::ApplicationStateBasic::push_mode (const ApplicationModeEnum Mode) {

   ModeStruct *ms (new ModeStruct (Mode));

   if (ms) { ms->next = _modeStack; _modeStack = ms; }
}


dmz::ApplicationModeEnum
dmz::ApplicationStateBasic::get_mode () const {

   return _modeStack ? _modeStack->Mode : ApplicationModeNormal;
}


dmz::ApplicationModeEnum
dmz::ApplicationStateBasic::pop_mode () {

   ModeStruct *ms (_modeStack);

   if (ms) { _modeStack = ms->next; ms->next = 0; delete ms; ms = 0; }

   return get_mode ();
}
//! \endcond
