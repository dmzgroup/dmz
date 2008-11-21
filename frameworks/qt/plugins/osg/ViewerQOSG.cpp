/*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include "ViewerQOSG.h"

ViewerQOSG::ViewerQOSG(QWidget *parent, const char *name, WindowFlags f):
      QOSGWidget (parent, name, f) {

   getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
   getCamera()->setProjectionMatrixAsPerspective(
      30.0f,
      static_cast<double>(width())/static_cast<double>(height()),
      1.0f,
      10000.0f);
#if 0
#endif

   getCamera()->setGraphicsContext(getGraphicsWindow());

#if 0
   setThreadingModel(osgViewer::Viewer::SingleThreaded);

   connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));

   _timer.start(10);
#endif
}

void
ViewerQOSG::paintEvent (QPaintEvent *event) {} //  frame(); }
