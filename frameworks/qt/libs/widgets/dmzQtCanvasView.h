#ifndef DMZ_QT_CANVAS_VIEW_DOT_H
#define DMZ_QT_CANVAS_VIEW_DOT_H

#include <dmzQtCanvasViewExport.h>
#include <QtGui/QGraphicsView>


namespace dmz {

   class DMZ_QT_CANVAS_VIEW_LINK_SYMBOL QtCanvasView : public QGraphicsView {

      Q_OBJECT

      public:
         QtCanvasView (QWidget *parent = 0);
         ~QtCanvasView ();

         qreal get_scale () const;
         void set_scale (const qreal Value);

         void pan_direction (const int Dx, const int Dy);

      signals:
         void pan_changed (const QPoint &);
         void scale_changed (qreal);

      protected:
         // virtual void drawBackground (QPainter *painter, const QRectF &rect);
         // virtual void drawForeground (QPainter *painter, const QRectF &rect);
         // virtual void paintEvent (QPaintEvent *event);
         virtual void keyPressEvent (QKeyEvent *event);
         virtual void keyReleaseEvent (QKeyEvent *event);
         virtual void wheelEvent (QWheelEvent *event);
         virtual void mouseDoubleClickEvent (QMouseEvent *event);
         virtual void mousePressEvent (QMouseEvent *event);
         virtual void mouseReleaseEvent (QMouseEvent *event);
         virtual void mouseMoveEvent (QMouseEvent *event);
         virtual void dragEnterEvent (QDragEnterEvent *event);

         void _updated ();

         QPointF _lastDragPos;

      private:
         QtCanvasView (const QtCanvasView &);
         QtCanvasView &operator= (const QtCanvasView &);
   };
};

#endif // DMZ_QT_CANVAS_VIEW_DOT_H
