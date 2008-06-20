#ifndef DMZ_QT_CANVAS_SCENE_DOT_H
#define DMZ_QT_CANVAS_SCENE_DOT_H

#include <QtGui/QGraphicsScene>


namespace dmz {

   class QtCanvasScene : public QGraphicsScene {

      Q_OBJECT

      public:
         QtCanvasScene (QObject *parent = 0);
         ~QtCanvasScene ();

      protected:
         virtual void drawBackground (QPainter *painter, const QRectF &rect);
         virtual void mouseMoveEvent (QGraphicsSceneMouseEvent *event);
         virtual void mousePressEvent (QGraphicsSceneMouseEvent *event);
         virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
         virtual void wheelEvent (QGraphicsSceneWheelEvent *event);

         bool _drawGrid;

      private:
         QtCanvasScene (const QtCanvasScene &);
         QtCanvasScene &operator= (const QtCanvasScene &);
   };
};

#endif // DMZ_QT_CANVAS_SCENE_DOT_H
