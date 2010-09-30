#include <dmzQtCanvasView.h>
#include <math.h>
#include <QtGui/QtGui>


dmz::QtCanvasView::QtCanvasView (QWidget *parent) :
      QGraphicsView (parent),
      _lastDragPos () {

   setObjectName ("QtCanvasView");
}


dmz::QtCanvasView::~QtCanvasView () {

}


qreal
dmz::QtCanvasView::get_scale () const {

   QMatrix transform (matrix ());

   qreal scaleX = transform.m11 ();
   qreal scaleY = transform.m12 ();

   qreal scale = sqrt (scaleX * scaleX + scaleY + scaleY);
   return scale;
}


void
dmz::QtCanvasView::set_scale (const qreal Value) {

   if (Value) {
      
      QMatrix transform (matrix ());

      transform.reset ();
      transform.scale (Value, Value);

      setMatrix (transform);

      emit scale_changed (Value);
   }
}


void
dmz::QtCanvasView::pan_direction (const int Dx, const int Dy) {

   QScrollBar *hBar = horizontalScrollBar ();
   QScrollBar *vBar = verticalScrollBar ();

   if (hBar && vBar) {

      hBar->setValue (hBar->value () - Dx);
      vBar->setValue (vBar->value () - Dy);
      
      emit (pan_changed (QPoint (Dx, Dy)));
   }
}


// void
// dmz::QtCanvasView::drawBackground (QPainter *painter, const QRectF &rect) {
// 
//    QGraphicsView::drawBackground (painter, rect);
// 
// #if 0
//    painter->save();
//    painter->setPen(Qt::gray);
//    painter->setOpacity(0.2);
// 
//    const QRectF r = sceneRect();
// //   qreal spacing = gridSpacing().width();
//    qreal spacing = 50;
// 
//    //FIXME We should probably only draw those lines that intercept rect
// 
//    //vertical lines
//    qreal x = r.left() + spacing;
//    while (x < r.right()) {
//       QLineF line(QPointF(x, r.top()), QPointF(x, r.bottom()));
//       painter->drawLine(line);
//       x += spacing;
//    }
// 
// //   spacing = gridSpacing().height();
//    spacing = 50;
// 
//    //horizontal lines
//    qreal y = r.top() + spacing;
//    while (y < r.bottom()) {
//       QLineF line(QPointF(r.left(), y), QPointF(r.right(), y));
//       painter->drawLine(line);
//       y += spacing;
//    }
// 
//    painter->restore();
// #endif
// }


// void
// dmz::QtCanvasView::drawForeground (QPainter *painter, const QRectF &rect) {
// 
//    painter->save();
//    painter->setPen(Qt::red);
//    
//    painter->drawLine (0, -100000, 0, 100000);
//    painter->drawLine (-100000, 0, 100000, 0);
// 
//    painter->restore();
// 
// //   QGraphicsView::drawForeground (painter, rect);
// }


// void
// dmz::QtCanvasView::paintEvent (QPaintEvent *event) {
// 
//    if (event) {
// 
// //      QPaintEvent newEvent (event->region ().boundingRect ());
// //      QGraphicsView::paintEvent (&newEvent);
// 
//       QGraphicsView::paintEvent (event);
//    }
// }


void
dmz::QtCanvasView::keyPressEvent (QKeyEvent *event) {

   if (event) {

      event->ignore ();
   }
}


void
dmz::QtCanvasView::keyReleaseEvent (QKeyEvent *event) {

   if (event) {

      event->ignore ();
   }
}


void
dmz::QtCanvasView::wheelEvent (QWheelEvent *event) {

   if (event) {

      QGraphicsView::wheelEvent (event);
      event->ignore ();
   }
}


void
dmz::QtCanvasView::mouseDoubleClickEvent (QMouseEvent *event) {

   if (event) {

//      QGraphicsView::mouseDoubleClickEvent (event);
      event->ignore ();
   }
}


void
dmz::QtCanvasView::mousePressEvent (QMouseEvent *event) {

   if (event) {

      QGraphicsView::mousePressEvent (event);
      event->ignore ();
   }
}


void
dmz::QtCanvasView::mouseReleaseEvent (QMouseEvent *event) {

   if (event) {

      QGraphicsView::mouseReleaseEvent (event);
      event->ignore ();
   }
}


void
dmz::QtCanvasView::mouseMoveEvent (QMouseEvent *event) {

   if (event) {

      QGraphicsView::mouseMoveEvent (event);
      event->ignore ();
   }
}


void
dmz::QtCanvasView::dragEnterEvent (QDragEnterEvent *event) {

   if (event) { event->ignore (); }
}
