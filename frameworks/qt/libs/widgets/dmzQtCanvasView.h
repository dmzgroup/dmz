#ifndef DMZ_QT_CANVAS_VIEW_DOT_H
#define DMZ_QT_CANVAS_VIEW_DOT_H

#ifdef _WIN32

#ifdef DMZ_QT_CANVAS_VIEW_EXPORT
#define DMZ_QT_CANVAS_VIEW_LINK_SYMBOL __declspec (dllexport)
#else
#define DMZ_QT_CANVAS_VIEW_LINK_SYMBOL __declspec (dllimport)
#endif // DMZ_QT_CANVAS_VIEW_EXPORT

#else // !_WIN32

#define DMZ_QT_CANVAS_VIEW_LINK_SYMBOL

#endif // _WIN32

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
         void scale_changed (qreal);
         
      protected:
         virtual void drawBackground (QPainter *painter, const QRectF &rect);
         virtual void paintEvent (QPaintEvent *event);
//         virtual void resizeEvent (QResizeEvent *event);
         virtual void keyPressEvent (QKeyEvent *event);
         virtual void keyReleaseEvent (QKeyEvent *event);
         virtual void wheelEvent (QWheelEvent *event);
         virtual void mouseDoubleClickEvent (QMouseEvent *event);
         virtual void mousePressEvent (QMouseEvent *event);
         virtual void mouseReleaseEvent (QMouseEvent *event);
         virtual void mouseMoveEvent (QMouseEvent *event);
         
         QPointF _lastDragPos;

      private:
         QtCanvasView (const QtCanvasView &);
         QtCanvasView &operator= (const QtCanvasView &);
   };
};

#endif // DMZ_QT_CANVAS_VIEW_DOT_H
