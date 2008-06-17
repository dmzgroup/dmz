#ifndef DMZ_QT_CANVAS_VIEW_UTIL_DOT_H
#define DMZ_QT_CANVAS_VIEW_UTIL_DOT_H

#include <dmzQtCanvasViewExport.h>


namespace dmz {

   class Config;
   class QtCanvasView;

   class DMZ_QT_CANVAS_VIEW_LINK_SYMBOL QtCanvasViewSession  {

      public:

         QtCanvasViewSession (QtCanvasView &canvas);

         Config get_config ();
         void load_config (const Config &Source);

      protected:
         QtCanvasView &_canvas;
   };
};


#endif // DMZ_QT_CANVAS_VIEW_UTIL_DOT_H
