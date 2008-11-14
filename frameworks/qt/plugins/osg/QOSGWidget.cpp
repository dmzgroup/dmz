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

#include "QOSGWidget.h"

QOSGWidget::QOSGWidget (QWidget * parent, const char * name, WindowFlags f):
       QWidget (parent, f) {

    createContext ();
    
    setAttribute (Qt::WA_PaintOnScreen);
    setAttribute (Qt::WA_NoSystemBackground);
}


void QOSGWidget::createContext() {

    osg::DisplaySettings* ds = osg::DisplaySettings::instance();

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    traits->readDISPLAY();
    if (traits->displayNum<0) traits->displayNum = 0;

    traits->windowName = "osgViewerQt";
    traits->screenNum = 0;
    traits->x = x();
    traits->y = y();
    traits->width = width();
    traits->height = height();
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    traits->inheritedWindowData = new WindowData((OpaqueWindowPtr*)winId());

    if (ds->getStereo ()) {

        switch(ds->getStereoMode()) {

            case(osg::DisplaySettings::QUAD_BUFFER):
               traits->quadBufferStereo = true; break;
            case(osg::DisplaySettings::VERTICAL_INTERLACE):
            case(osg::DisplaySettings::CHECKERBOARD):
            case(osg::DisplaySettings::HORIZONTAL_INTERLACE): traits->stencil = 8; break;
            default: break;
        }
    }

    osg::ref_ptr<osg::GraphicsContext> gc =
       osg::GraphicsContext::createGraphicsContext(traits.get());
    _gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
}


void QOSGWidget::destroyEvent(bool destroyWindow, bool destroySubWindows) {   

    _gw->getEventQueue()->closeWindow();
}


void QOSGWidget::closeEvent( QCloseEvent * event ) {

    _gw->getEventQueue()->closeWindow();
}


void QOSGWidget::resizeEvent( QResizeEvent * event ) {

    const QSize & size = event->size();
    _gw->getEventQueue()->windowResize(0, 0, size.width(), size.height() );
    _gw->resized(0, 0, size.width(), size.height());
}


void QOSGWidget::keyPressEvent( QKeyEvent* event ) {

    _gw->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
}

void QOSGWidget::keyReleaseEvent( QKeyEvent* event )
{
    int c = *event->text().toAscii().data();

    _gw->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) (c) );
}

void QOSGWidget::mousePressEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}

void QOSGWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseDoubleButtonPress(event->x(), event->y(), button);
}

void QOSGWidget::mouseReleaseEvent( QMouseEvent* event ) {
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}

void QOSGWidget::mouseMoveEvent( QMouseEvent* event ) {

    _gw->getEventQueue()->mouseMotion(event->x(), event->y());
}

