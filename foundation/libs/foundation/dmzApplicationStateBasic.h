#ifndef DMZ_APPLICATION_STATE_BASIC_DOT_H
#define DMZ_APPLICATION_STATE_BASIC_DOT_H

#include <dmzApplicationState.h>

namespace dmz {

   //! \cond
   class ApplicationStateBasic : public ApplicationStateInterface {

      public:
         ApplicationStateBasic (const String &AppName, RuntimeContext *context);
         virtual ~ApplicationStateBasic ();

         virtual String get_app_name () const;
         
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

         const String _AppName;
         String _defaultDir;
         String _autosaveFile;
         ModeStruct *_modeStack;

      private:
         ApplicationStateBasic ();
         ApplicationStateBasic (const ApplicationStateBasic &);
         ApplicationStateBasic &operator= (const ApplicationStateBasic &);
   };
   //! \endcond
};

#endif // DMZ_APPLICATION_STATE_BASIC_DOT_H
