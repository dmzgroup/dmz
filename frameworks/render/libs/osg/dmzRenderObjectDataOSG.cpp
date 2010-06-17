#include <dmzRenderObjectDataOSG.h>

dmz::RenderObjectDataOSG::RenderObjectDataOSG (const Handle ObjectHandle) :
      _ObjectHandle (ObjectHandle),
      _mask (0),
      _isectCount (0) {;}


dmz::Handle
dmz::RenderObjectDataOSG::get_handle () const { return _ObjectHandle; }


void
dmz::RenderObjectDataOSG::set_mask (const UInt32 IsectMask) { _mask = IsectMask; }


dmz::UInt32
dmz::RenderObjectDataOSG::get_mask () const { return _mask; }


dmz::Boolean
dmz::RenderObjectDataOSG::do_isect () const { return _isectCount <= 0; }


dmz::Int32
dmz::RenderObjectDataOSG::disable_isect () {

   _isectCount++;
   return _isectCount;
}


dmz::Int32
dmz::RenderObjectDataOSG::enable_isect () {

   _isectCount--;
   if (_isectCount < 0) { _isectCount = 0; }
   return _isectCount;
}
