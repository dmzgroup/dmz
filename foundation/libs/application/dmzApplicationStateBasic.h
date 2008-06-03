#ifndef DMZ_APPLICATION_STATE_BASIC_DOT_H
#define DMZ_APPLICATION_STATE_BASIC_DOT_H

#include <dmzApplicationState.h>

namespace dmz {

   class ApplicationStateBasic : public ApplicationState {

      public:
         ApplicationStateBasic (RuntimeContext *context);
         virtual ~ApplicationStateBasic ();

         virtual void set_default_directory (const String &DirName);
         virtual String get_default_directory () const;

         virtual void set_autosave_file (const String &FileName);
         virtual String get_autosave_file () const;

         virtual void push_mode (const ApplicationModeEnum Mode);
         virtual ApplicationModeEnum get_mode () const;
         virtual ApplicationModeEnum pop_mode ();

      protected:
         struct ModeStruct {

            ApplicationModeEnum Mode;
            ModeStruct *next;

            ModeStruct (const ApplicationModeEnum TheMode) : Mode (TheMode), next (0) {;}
            ~ModeStruct () { if (next) { delete next; next = 0; } }
         };

         String _defaultDir;
         String _autosaveFile;
         ModeStruct *_modeStack;

      private:
         ApplicationStateBasic ();
         ApplicationStateBasic (const ApplicationStateBasic &);
         ApplicationStateBasic &operator= (const ApplicationStateBasic &);
   };
};

#endif // DMZ_APPLICATION_STATE_BASIC_DOT_H
