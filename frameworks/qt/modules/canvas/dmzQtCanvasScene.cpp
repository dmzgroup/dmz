#include "dmzQtCanvasScene.h"
//#include <math.h>
#include <QtGui/QtGui>
#include <QtCore/QMimeData>

#include <qdb.h>


dmz::QtCanvasScene::QtCanvasScene (QObject *parent) :
      QGraphicsScene (parent),
      _drawGrid (true) {

}


dmz::QtCanvasScene::~QtCanvasScene () {

}


void
dmz::QtCanvasScene::enableGrid (const bool Value) {
   
   _drawGrid = Value;
}


void
dmz::QtCanvasScene::drawBackground (QPainter *painter, const QRectF &rect) {

   painter->drawRect (sceneRect ());
   
   if (_drawGrid) {

      painter->fillRect (sceneRect (), QBrush (QColor (230, 240, 255)));
      painter->setPen (QPen (Qt::lightGray, 1));
      painter->setOpacity(0.75f);

      const int gridSize = 100;

      qreal left = int(rect.left ()) - (int(rect.left ()) % gridSize);
      qreal top = int(rect.top ()) - (int(rect.top ()) % gridSize);

      QVarLengthArray<QLineF, 100> lines;

      for (qreal x = left; x < rect.right (); x += gridSize) {

         lines.append (QLineF (x, rect.top (), x, rect.bottom ()));
      }

      for (qreal y = top; y < rect.bottom (); y += gridSize) {

         lines.append (QLineF (rect.left (), y, rect.right (), y));
      }

//   qDebug () << lines.size ();
      painter->drawLines (lines.data (), lines.size ());

      QGraphicsScene::drawBackground (painter, rect);
   }
}


void
dmz::QtCanvasScene::mouseMoveEvent (QGraphicsSceneMouseEvent *event) {

   if (event) {

//      QGraphicsScene::mouseMoveEvent (event);
      event->accept ();
   }
}


void
dmz::QtCanvasScene::mousePressEvent (QGraphicsSceneMouseEvent *event) {

   if (event) {

//      QGraphicsScene::mousePressEvent (event);
      event->accept ();
   }
}


void
dmz::QtCanvasScene::mouseReleaseEvent (QGraphicsSceneMouseEvent *event) {

   if (event) {

//      QGraphicsScene::mouseReleaseEvent (event);
      event->accept ();
   }
}


void
dmz::QtCanvasScene::wheelEvent (QGraphicsSceneWheelEvent *event) {

   if (event) {

//      QGraphicsScene::wheelEvent (event);
      event->accept ();
   }
}

