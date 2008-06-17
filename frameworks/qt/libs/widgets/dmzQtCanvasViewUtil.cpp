#include <dmzQtCanvasView.h>
#include <dmzQtCanvasViewUtil.h>
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigWrite.h>
#include <QtGui/QScrollBar>


dmz::QtCanvasViewSession::QtCanvasViewSession (QtCanvasView &canvas) :
      _canvas (canvas) {;}


dmz::Config
dmz::QtCanvasViewSession::get_config () {
   
   String val;
   Config session ("canvasView");

   val.flush () << _canvas.get_scale ();
   session.store_attribute ("scale", val);
   
   val.flush () << _canvas.width ();
   session.store_attribute ("width", val);
   
   val.flush () << _canvas.height ();
   session.store_attribute ("height", val);
   
   Config scrollBars ("scrollBars");
   
   val.flush () << _canvas.verticalScrollBar ()->value ();
   scrollBars.store_attribute ("vertical", val);

   val.flush () << _canvas.horizontalScrollBar ()->value ();
   scrollBars.store_attribute ("horizontal", val);
   
   session.add_config (scrollBars);

   return session;
}


void
dmz::QtCanvasViewSession::load_config (const Config &Source) {

   Int32 width (_canvas.width ());
   width = config_to_float32 ("canvasView.width", Source, width);

   Int32 height (_canvas.height ());
   height = config_to_float32 ("canvasView.height", Source, height);

//   _canvas.resize (width, height);

   Float32 scale (_canvas.get_scale ());
   scale = config_to_float32 ("canvasView.scale", Source, scale);
//   _canvas.set_scale (scale);

   // _canvas.verticalScrollBar ()->setValue (
   //    config_to_int32 ("canvasView.scrollBars.vertical", Source, 0));
   // 
   // _canvas.horizontalScrollBar ()->setValue (
   //    config_to_int32 ("canvasView.scrollBars.horizontal", Source, 0));
}

