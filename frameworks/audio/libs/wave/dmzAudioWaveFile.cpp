#include <dmzAudioWaveFile.h>

struct dmz::WaveFile::State {

};


dmz::WaveFile::WaveFile () : _state (*(new State)) {;}


dmz::WaveFile::WaveFile (const String &FileName) : _state (*(new State)) {

   load_file (FileName);
}


dmz::WaveFile::~WaveFile () { delete &_state; }


dmz::Boolean
dmz::WaveFile::load_file (const String &FileName) {

   Boolean result (False);

   return result;
}
