#include <dmzQtConfigRead.h>
#include <dmzQtUtil.h>
#include <QtCore/QtCore>
#include <QtGui/QtGui>

#include <QtCore/QDebug>


QPointF
dmz::config_to_qpointf (
      const String &Name,
      const Config &Source,
      const QPointF &DefaultValue) {

   Config cd;

   if (Name) { Source.lookup_config (Name, cd); }
   else { cd = Source; }

   const QPointF Result (
      config_to_float64 ("x", cd, DefaultValue.x ()),
      config_to_float64 ("y", cd, DefaultValue.y ()));

   return Result;
}


QSize
dmz::config_to_qsize (
      const String &Name,
      const Config &Source,
      const QSize &DefaultValue) {

   Config cd;

   if (Name) { Source.lookup_config (Name, cd); }
   else { cd = Source; }

   const QSize Result (
      config_to_int32 ("width", cd, DefaultValue.width ()),
      config_to_int32 ("height", cd, DefaultValue.height ()));

   return Result;
}


QSizeF
dmz::config_to_qsizef (
      const String &Name,
      const Config &Source,
      const QSizeF &DefaultValue) {

   Config cd;

   if (Name) { Source.lookup_config (Name, cd); }
   else { cd = Source; }

   const QSizeF Result (
      config_to_float64 ("width", cd, DefaultValue.width ()),
      config_to_float64 ("height", cd, DefaultValue.height ()));

   return Result;
}


QRectF
dmz::config_to_qrectf (
      const String &Name,
      const Config &Source,
      const QRectF &DefaultValue) {

   Config cd;

   if (Name) { Source.lookup_config (Name, cd); }
   else { cd = Source; }

   const QRectF Result (
      config_to_qpointf ("point", cd, DefaultValue.topLeft ()),
      config_to_qsizef ("size", cd, DefaultValue.size ()));

   return Result;
}


QMatrix
dmz::config_to_qmatrix (
      const String &Name,
      const Config &Source,
      const QMatrix &DefaultValue) {

   Config cd;

   if (Name) { Source.lookup_config (Name, cd); }
   else { cd = Source; }

   const QMatrix Result (
      config_to_float64 ("m11", cd, DefaultValue.m11()),
      config_to_float64 ("m12", cd, DefaultValue.m12 ()),
      config_to_float64 ("m21", cd, DefaultValue.m21 ()),
      config_to_float64 ("m22", cd, DefaultValue.m22 ()),
      config_to_float64 ("dx", cd, DefaultValue.dx ()),
      config_to_float64 ("dy", cd, DefaultValue.dy ()));

   return Result;
}


QByteArray
dmz::config_to_qbytearray (
      const String &Name,
      const Config &Source,
      const QByteArray &DefaultValue) {

   Config cd;

   if (Name) { Source.lookup_config (Name, cd); }
   else { cd = Source; }

   String base64 (config_to_string ("data", cd));

   QByteArray result (QByteArray::fromBase64 (base64.get_buffer ()));
   if (!base64) { result = DefaultValue; }

   return result;
}


QColor
dmz::config_to_qcolor (
      const String &Name,
      const Config &Source,
      const QColor &DefaultValue) {

   QColor result (DefaultValue);
   Config colorData;

   if (Name) { Source.lookup_config (Name, colorData); }
   else { colorData = Source; }

#if 0
   <textColor>
      <rgb x="0.0" y="0.0" z="0.0"/>
      <string value="#RRGGBB"/>
      <alpha value="0.5"/>
   <textColor/>
#endif

   Config cd;

   if (colorData.lookup_config ("rgb", cd)) {

      Vector rgb (config_to_vector (cd));
      result.setRgbF (rgb.get_x (), rgb.get_y (), rgb.get_z ());
   }
   else if (colorData.lookup_config ("string", cd)) {

      String name (config_to_string (cd));
      if (name) { result.setNamedColor (name.get_buffer ()); }
   }

   if (colorData.lookup_config ("alpha", cd)) {

      result.setAlphaF (config_to_float64 ("value", cd, 1.0));
   }

   return result;
}


QPen
dmz::config_to_qpen (
      const String &Name,
      const Config &Source,
      const QPen &DefaultValue) {

   QPen result (DefaultValue);

   Config penData;

   if (Name) { Source.lookup_config (Name, penData); }
   else { penData = Source; }

#if 0
   <pen>
      <color red="0.0" green="0.0" blue="0.0" alpha="0.5"/>
      <width value="1"/>
   </pen>
#endif

   Config cd;

   if (penData.lookup_config ("color", cd)) {

      QColor color;
      color.setRgbF (
         config_to_float32 ("red", cd, 0.0f),
         config_to_float32 ("green", cd, 0.0f),
         config_to_float32 ("blue", cd, 0.0f),
         config_to_float32 ("alpha", cd, 1.0f));

      result.setColor (color);
   }

   result.setWidth (config_to_int32 ("width.value", penData, 1));

   return result;
}


QBrush
dmz::config_to_qbrush (
      const String &Name,
      const Config &Source,
      const QBrush &DefaultValue) {

   QBrush result (DefaultValue);

   Config brushData;

   if (Name) { Source.lookup_config (Name, brushData); }
   else { brushData = Source; }

#if 0
   <brush>
      <color red="0.0" green="0.0" blue="0.0" alpha="0.5"/>
   </brush>
#endif

   Config cd;

   if (brushData.lookup_config ("color", cd)) {

      QColor color;
      color.setRgbF (
         config_to_float32 ("red", cd, 0.0f),
         config_to_float32 ("green", cd, 0.0f),
         config_to_float32 ("blue", cd, 0.0f),
         config_to_float32 ("alpha", cd, 1.0f));

      result.setColor (color);
      result.setStyle (Qt::SolidPattern);
   }

   return result;
}


Qt::DockWidgetAreas
dmz::config_to_dock_widget_areas (
      const String &Name,
      const Config &Source,
      const Qt::DockWidgetAreas DefaultValue) {

   Qt::DockWidgetAreas result (DefaultValue);
   Config areas;

   if (Name) { Source.lookup_config (Name, areas); }
   else { areas = Source; }

#if 0
   <allowed-areas all="true"/>
   <allowed-areas none="true"/>
   <allowed-areas left="true" right="true" top="true" bottom="true"/>
#endif

   ConfigIterator it;
   
   if (config_to_boolean ("none", areas)) { result = Qt::NoDockWidgetArea; }
   else if (config_to_boolean ("all", areas)) { result = Qt::AllDockWidgetAreas; }
   else {
      
      if (config_to_boolean ("left", areas)) { result |= Qt::LeftDockWidgetArea; }
      if (config_to_boolean ("right", areas)) { result |= Qt::RightDockWidgetArea; }
      if (config_to_boolean ("top", areas)) { result |= Qt::TopDockWidgetArea; }
      if (config_to_boolean ("bottom", areas)) { result |= Qt::BottomDockWidgetArea; }
   }

   return result;
}


void
dmz::qicon_config_read (const String &Name, const Config &Source, QIcon *icon) {

   if (icon) {

      QIcon result;
      Config iconData;

      if (Name) { Source.lookup_config (Name, iconData); }
      else { iconData = Source; }

#if 0
      <icon>
         <image file="Button_Up.png" mode="Normal" state="On"/>
         <image file="Button_Down.png" mode="Normal" state="Off"/>
      </icon>
#endif

      ConfigIterator it;
      Config cd;

      while (iconData.get_next_config (it, cd)) {

         const QString File (config_to_string ("file", cd).get_buffer ());
         QFileInfo fi (File);

         if (fi.exists ()) {

            const String ModeName (config_to_string ("mode", cd, "normal").to_lower ());
            const String StateName (config_to_string ("state", cd, "off").to_lower ());

            QIcon::Mode mode (QIcon::Normal);
            if (ModeName == "normal") { mode = QIcon::Normal; }
            else if (ModeName == "disabled") { mode = QIcon::Disabled; }
            else if (ModeName == "active") { mode = QIcon::Active; }
            else if (ModeName == "selected") { mode = QIcon::Selected; }

            QIcon::State state (QIcon::Off);
            if (StateName == "off") { state = QIcon::Off; }
            else if (StateName == "on") { state = QIcon::On; }

            icon->addPixmap (QPixmap (File), mode, state);
         }
      }
   }
}


void
dmz::qobject_config_read (
      const String &Name,
      const Config &Source,
      QObject *object) {

   if (object) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

#if 0
      <object name="MyObj">
         <properties>
            <name value="string"/>
            <foo value="bar"/>
         </properties>
      </object>
#endif

      String objName (config_to_string ("name", cd));
      if (objName) { object->setObjectName (objName.get_buffer ()); }

      set_qobject_properties ("properties", cd, object);
   }
}


void
dmz::qwidget_config_read (const String &Name, const Config &Source, QWidget *widget) {

   if (widget) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

#if 0
      <widget statusTip="String" toolTip="String" whatsThis="String" windowTitle="String">
         <stylesheet file="file.qss"/>
         <object>
            ...
         </object>
      </widget>
#endif

      widget->setMinimumSize (
         config_to_qsize ("minimum-size", cd, widget->minimumSize ()));

      widget->setMaximumSize (
         config_to_qsize ("maximum-size", cd, widget->maximumSize ()));

      qobject_config_read ("object", cd, widget);

      String value;

      value = config_to_string ("statusTip", cd);
      if (value) { widget->setStatusTip (value.get_buffer ()); }

      value = config_to_string ("toolTip", cd);
      if (value) { widget->setToolTip (value.get_buffer ()); }

      value = config_to_string ("whatsThis", cd);
      if (value) { widget->setWhatsThis (value.get_buffer ()); }

      value = config_to_string ("windowTitle", cd);
      if (value) { widget->setWindowTitle (value.get_buffer ()); }

      set_qwidget_stylesheet ("stylesheet", cd, widget);
   }
}


void
dmz::qframe_config_read (const String &Name, const Config &Source, QFrame *frame) {

   if (frame) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

#if 0
      <frame
         line-width="Int32"
         mid-line-width="Int32"
         shape="[noframe|box|panel|styledpanel|hline|vline]"
         shadow="[plain|raised|sunken]">
         <widget>
            ...
         </widget>
      </frame>
#endif

      qwidget_config_read ("widget", cd, frame);
      
      frame->setLineWidth (config_to_int32 ("line-width", cd, 1));
      frame->setMidLineWidth (config_to_int32 ("mmid-line-width", cd, 0));
      
      String value;
      
      value = config_to_string ("shape", cd).to_lower ();
      if (value == "noframe") { frame->setFrameShape (QFrame::NoFrame); }
      else if (value == "box") { frame->setFrameShape (QFrame::Box); }
      else if (value == "panel") { frame->setFrameShape (QFrame::Panel); }
      else if (value == "styledpanel") { frame->setFrameShape (QFrame::StyledPanel); }

      value = config_to_string ("shadow", cd).to_lower ();
      if (value == "plain") { frame->setFrameShadow (QFrame::Plain); }
      else if (value == "raised") { frame->setFrameShadow (QFrame::Raised); }
      else if (value == "sunken") { frame->setFrameShadow (QFrame::Sunken); }
   }
}


void
dmz::qaction_config_read (const String &Name, const Config &Source, QAction *action) {

   if (action) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

#if 0
      <action
         autoRepeat="Boolean"
         checkable="Boolean"
         iconText="String"
         separator="Boolean"
         statusTip="String"
         toolTip="String"
         whatsThis="String"
         shortcut="String"
         text="String"
         checkable="Boolean"
         enabled="Boolean"
         visible="Boolean">
         <icon>
            ...
         </icon>
         <object>
            ...
         </object>
      </widget>
#endif

      qobject_config_read ("object", cd, action);

      String value;

      action->setAutoRepeat (
         config_to_boolean ("autoRepeat", cd, action->autoRepeat ()));

      action->setCheckable (
         config_to_boolean ("checkable", cd, action->isCheckable ()));

      QIcon icon;
      qicon_config_read ("icon", cd, &icon);
      action->setIcon (icon);

      value = config_to_string ("iconText", cd);
      if (value) { action->setIconText (value.get_buffer ()); }

      if (config_to_boolean ("separator", cd, False)) {

         action->setSeparator (True);
      }

      value = config_to_string ("statusTip", cd);
      if (value) { action->setStatusTip (value.get_buffer ()); }

      value = config_to_string ("toolTip", cd);
      if (value) { action->setToolTip (value.get_buffer ()); }

      value = config_to_string ("whatsThis", cd);
      if (value) { action->setWhatsThis (value.get_buffer ()); }

      value = config_to_string ("shortcut", cd);
      if (value) {

         QKeySequence shortcut (value.get_buffer ());

         const String StandardKey (value.get_lower ());

         if (StandardKey == "undo") {

            shortcut = QKeySequence::Undo;
         }
         else if (StandardKey == "redo") {

            shortcut = QKeySequence::Redo;
         }
         else if (StandardKey == "open") {

            shortcut = QKeySequence::Open;
         }
         else if (StandardKey == "save") {
         
            shortcut = QKeySequence::Save;
         }
         else if (StandardKey == "saveas") {
            
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
            shortcut = QKeySequence::SaveAs;
#else
            shortcut = QKeySequence (Qt::Key_S + Qt::ControlModifier + Qt::ShiftModifier);
#endif
         }
         else if (StandardKey == "zoomin") {
            
            shortcut = QKeySequence::ZoomIn;
         }
         else if (StandardKey == "zoomout") {
            
            shortcut = QKeySequence::ZoomOut;
         }

         action->setShortcut (shortcut);
      }

      // use "&#38" or "&amp;" in the xml to represent '&'

      value = config_to_string ("text", cd);
      if (value) { action->setText (value.get_buffer ()); }

      action->setChecked (
         config_to_boolean ("checked", cd, action->isChecked ()));

      action->setEnabled (
         config_to_boolean ("enabled", cd, action->isEnabled ()));

      action->setVisible (
         config_to_boolean ("visible", cd, action->isVisible ()));
   }
}


void
dmz::qtoolbar_config_read (const String &Name, const Config &Source, QToolBar *toolBar) {

   if (toolBar) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

#if 0
      <toolBar floatable="Boolean" movable="Boolean">
         <iconSize widget="Int32" height="Int32"/>
         <widget>
            ...
         </widget>
      </toolBar>
#endif
      qwidget_config_read ("widget", cd, toolBar);

      String value;

      toolBar->setFloatable (
         config_to_boolean ("floatable", cd, toolBar->isFloatable ()));

      toolBar->setMovable (
         config_to_boolean ("movable", cd, toolBar->isMovable ()));

      QSize size (config_to_qsizef ("iconSize", cd, toolBar->iconSize ()).toSize ());
      toolBar->setIconSize (size);
   }
}


void
dmz::qgraphicsview_config_read (
      const String &Name,
      const Config &Source,
      QGraphicsView *view) {

   if (view) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

      Config matrixData;
      if (cd.lookup_config ("matrix", matrixData)) {

         QMatrix mat (config_to_qmatrix (matrixData));
         view->setMatrix (mat);
      }

      QPointF center (config_to_qpointf ("center", cd, QPointF (0, 0)));
      view->centerOn (center);
   }
}


void
dmz::qabstractbutton_config_read (
      const String &Name,
      const Config &Source,
      QAbstractButton *button) {

   if (button) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

      String value;

      qwidget_config_read ("widget", cd, button);

      button->setAutoExclusive (
         config_to_boolean ("autoExclusive", cd, button->autoExclusive ()));

      button->setAutoRepeat (
         config_to_boolean ("autoRepeat", cd, button->autoRepeat ()));

      button->setAutoRepeatDelay (
         config_to_int32 ("autoRepeatDelay", cd, button->autoRepeatDelay ()));

      button->setAutoRepeatInterval (
         config_to_int32 ("autoRepeatInterval", cd, button->autoRepeatInterval ()));

      button->setCheckable (
         config_to_boolean ("checkable", cd, button->isCheckable ()));

      button->setChecked (
         config_to_boolean ("checked", cd, button->isChecked ()));

      button->setDown (config_to_boolean ("down", cd, button->isDown ()));

      QIcon icon;
      qicon_config_read ("icon", cd, &icon);
      button->setIcon (icon);

      QSize size (config_to_qsizef ("iconSize", cd, button->iconSize ()).toSize ());
      button->setIconSize (size);

      value = config_to_string ("shortcut", cd);
      if (value) {

         QKeySequence shortcut (value.get_buffer ());
         button->setShortcut (shortcut);
      }

      value = config_to_string ("text", cd);
      if (value) { button->setText (value.get_buffer ()); }
   }
}


void
dmz::qpushbutton_config_read (
      const String &Name,
      const Config &Source,
      QPushButton *button) {

   if (button) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

      qabstractbutton_config_read ("button", cd, button);

      if (config_to_boolean ("autoDefault", cd, False)) {

         button->setAutoDefault (True);
      }

      button->setDefault (config_to_boolean ("default", cd, button->isDefault ()));
      button->setFlat (config_to_boolean ("flat", cd, button->isFlat ()));
   }
}


void
dmz::qtoolbutton_config_read (
      const String &Name,
      const Config &Source,
      QToolButton *button) {

   if (button) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

      qabstractbutton_config_read ("button", cd, button);

      button->setAutoRaise (config_to_boolean ("autoRaise", cd, button->autoRaise ()));

      const String StyleName (config_to_string ("style", cd).to_lower ());
      Qt::ToolButtonStyle style (Qt::ToolButtonIconOnly);
      if (StyleName == "textonly") { style = Qt::ToolButtonTextOnly; }
      if (StyleName == "textundericon") { style = Qt::ToolButtonTextUnderIcon; }
      button->setToolButtonStyle (style);

      Config actionData;
      if (cd.lookup_config ("action", actionData)) {

         QAction *action = new QAction (0);
         qaction_config_read ("", actionData, action);
         button->setDefaultAction (action);
      }
   }
}


void
dmz::qlineedit_config_read (
      const String &Name,
      const Config &Source,
      QLineEdit *lineEdit) {

   if (lineEdit) {

      lineEdit->setText (config_to_string (Name, Source).get_buffer ());
   }
}

