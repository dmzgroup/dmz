#include "dmzQtLogObserverWidget.h"
#include <QtGui/QtGui>


namespace {
   
   static const char *localLevelStr[] = { "[D]:", "[I]:", "[W]:", "[E]:", "" };
   static const char *localLevelDivId[] = { "debug", "info", "warn", "error", "out" };
   static const char *localLevelColor[] = { "#888888", "blue", "orange", "red", "black" };
};


dmz::QtLogObserverWidget::QtLogObserverWidget (QWidget *parent) :
      QWidget (parent),
      _messageNumber (1),
      _lastSavedLog (QString::null) {

   _ui.setupUi (this);

   // QString css =
   //    "#debug { color:#888888; } " \
   //    "#info { color:blue; } " \
   //    "#warn { color:orange; } " \
   //    "#error { color:red; } ";
   // 
   // _ui.textEdit->document ()->setDefaultStyleSheet (css);
   // _ui.filteredTextEdit->document ()->setDefaultStyleSheet (css);

   connect (
      _ui.findPreviousButton, SIGNAL (clicked ()),
      this, SLOT (findPrevious ()));

   connect (
      _ui.findNextButton, SIGNAL (clicked ()),
      this, SLOT (findNext ()));

   connect (
      _ui.findLineEdit, SIGNAL (returnPressed ()),
      this, SLOT (findNext ()));

   connect (
      _ui.findLineEdit, SIGNAL (textEdited (const QString &)),
      this, SLOT (_find (const QString &)));

   connect (_ui.findButton, SIGNAL (clicked ()), this, SLOT (find ()));

   _ui.findFrame->hide ();
   _ui.wrappedLabel->hide ();
   _ui.wrappedIcon->hide ();
   _ui.textEdit->setFocus ();
   _logList.append (tr ("All"));

   _ui.logLabel->hide ();
   _ui.logComboBox->hide ();
}


dmz::QtLogObserverWidget::~QtLogObserverWidget () {

}


void
dmz::QtLogObserverWidget::store_log_message (
      const String &LogName,
      const LogLevelEnum Level,
      const String &Message) {

   if (Level >= 0) {
      
      if (!_logList.contains (LogName.get_buffer ())) {

         _logList.removeFirst ();
         _logList.append (LogName.get_buffer ());
         _logList.sort ();
         _logList.prepend (tr ("All"));
         _ui.logComboBox->clear ();
         _ui.logComboBox->addItems (_logList);
      }

      QString msg (Message.get_buffer ());
      msg.replace ("&", "&amp;");
      msg.replace ("<", "&lt;");
      msg.replace (">", "&gt;");
      msg.replace ("\n", "<br>");
      msg.replace (" ", "&nbsp;");

//       QString text =
//          QString ("<table><tr id=\"%1\">" \
//                   "  <td>%2:</td>" \
//                   "  <td>%3</td>"
//                   "  <td><b>%4</b>:</td>" \
//                   "  <td>%5</td>" \
//                   "</tr></table>").
//             arg (localLevelDivId[Level]).
//             arg (_messageNumber++).
//             arg (localLevelStr[Level]).
//             arg (LogName.get_buffer ()).
//             arg (msg);

      QString text = tr ("<font color=\"%1\">%2:%3<strong>%4</strong>:%5</font>").
         arg (localLevelColor[Level]).
         arg (_messageNumber++).
         arg (localLevelStr[Level]).
         arg (LogName.get_buffer ()).
         arg (msg);

      _ui.textEdit->append (text);
      
      if (_ui.stackedWidget->currentIndex () == 1) {

         if (text.contains (_ui.logComboBox->currentText ())) {

            _ui.filteredTextEdit->append (text);
         }
      }
   }
}


void
dmz::QtLogObserverWidget::find () {

   _ui.findFrame->show ();
   _ui.findLineEdit->setFocus (Qt::ShortcutFocusReason);
   _ui.findLineEdit->selectAll ();
}


void
dmz::QtLogObserverWidget::findNext () {

   _find (_ui.findLineEdit->text (), True, False);
}


void
dmz::QtLogObserverWidget::findPrevious () {

   _find (_ui.findLineEdit->text (), False, True);
}


void
dmz::QtLogObserverWidget::keyPressEvent (QKeyEvent *event) {

   int key = event->key();
   QString ttf = _ui.findLineEdit->text ();
   QString text = event->text ();

   if (_ui.findFrame->isVisible ()) {

      switch (key) {
         case Qt::Key_Escape:
            _ui.findFrame->hide ();
            _ui.wrappedLabel->hide ();
            _ui.wrappedIcon->hide ();
            break;
         case Qt::Key_Backspace:
            ttf.chop (1);
            break;
         case Qt::Key_Return:
         case Qt::Key_Enter:
            // Return/Enter key events are not accepted by QLineEdit
            return;
         default:
            if (text.isEmpty ()) { QWidget::keyPressEvent (event); }
            else { ttf += text; }
            break;
      }

      if (_ui.findFrame->isVisible ()) {

         _ui.findLineEdit->setText (ttf);
         _find (ttf, false, false);
      }
   }
   else {
      
      if (text.isEmpty () || text[0].isSpace () || !text[0].isPrint ()) {

         QWidget::keyPressEvent (event);
      }
      else if (text.startsWith (QLatin1Char ('/'))) {

         _ui.findLineEdit->clear ();
         find ();
      }
      else {

         ttf = text;
         _ui.findFrame->show ();
      }
   }
}


void
dmz::QtLogObserverWidget::on_loadButton_clicked () {

   QString log =
      QFileDialog::getOpenFileName (this, tr ("Open Log"), _lastSavedLog, "*.html");

   if (!log.isEmpty ()) {

      QFile data (log);
      if (data.open (QIODevice::ReadOnly | QIODevice::Text)) {

         QTextStream in (&data);

         _logList.clear ();
         _logList.append (tr ("All"));
         _ui.logComboBox->clear ();
         _ui.logComboBox->addItems (_logList);

         _ui.textEdit->setHtml (in.readAll ());
      }
   }
}


void
dmz::QtLogObserverWidget::on_saveButton_clicked () {

   const QString LogExt (".html");

   QString defaultLog (_lastSavedLog);

   if (defaultLog.isEmpty ()) {

      defaultLog = "DMZ_Log_";
      defaultLog += QDate::currentDate ().toString ();
      defaultLog += LogExt;
      defaultLog.replace (QRegExp (" "), "_");
   }

   QString log =
      QFileDialog::getSaveFileName (this, tr ("Save Log"), defaultLog, "*.html");

   if (!log.isEmpty ()) {

      if (!log.endsWith (LogExt)) { log += LogExt; }
      QFile data (log);

      if (data.open (QFile::WriteOnly | QFile::Truncate)) {

         QTextStream out (&data);
         out << _ui.textEdit->toHtml ();

         _lastSavedLog = log;
      }
   }
}


void
dmz::QtLogObserverWidget::on_logComboBox_activated (int index) {

   if (index) {

//      _ui.filteredTextEdit->document ()->clear ();

//      QString logText (_ui.textEdit->toHtml ());

//      QTextStream in (&logText);
//      QString line;
//      QString number, level, log, message;

      // do {
      //    // 4:
      //    // [E]:
      //    // dmzAppQt:
      //    // dmz.plugin-list.plugin not found. No plugins listed for loading
      //
      //
      //   line = in.readLine ();
      //
      //   if (!line.isEmpty ()) {
      //
      //      QString data (line);
      //
      //      data += in.readLine ();
      //      data += in.readLine ();
      //      data += in.readLine ();
      //
      //      number = data.section (":", 0, 0);
      //      level = data.section (":", 1, 1);
      //      log = data.section (":", 2, 2);
      //      message = data.section (":", 3, 3);
      //
      //      if (log == _ui.logComboBox->itemText (index)) {
      //
      //         QString text =
      //            QString ("<table><tr id=\"%1\">" \
      //                     "  <td>%2:</td>" \
      //                      "  <td>%3:</td>"
      //                      "  <td><b>%4</b>:</td>" \
      //                      "  <td>%5</td>" \
      //                      "</tr></table>").
      //                      arg ("warn").
      //                      arg (number).
      //                      arg (level).
      //                      arg (log).
      //                      arg (message);
      //
      //         _ui.filteredTextEdit->append (text);
      //       }
      //    }
      // } while (!line.isNull());

      _ui.stackedWidget->setCurrentIndex (1);
   }
   else {

      _ui.stackedWidget->setCurrentIndex (0);
   }
}


void
dmz::QtLogObserverWidget::_find (
      const QString &Ttf,
      const Boolean Forward,
      const Boolean Backward) {

   QTextEdit *editor = _ui.textEdit;
   QTextDocument *doc = editor->document ();
   QString oldText = _ui.findLineEdit->text ();
   QTextCursor c = editor->textCursor ();
   QTextDocument::FindFlags options;
   QPalette p = _ui.findLineEdit->palette ();
   p.setColor (QPalette::Active, QPalette::Base, Qt::white);

   if (c.hasSelection ()) {

      c.setPosition (Forward ? c.position () : c.anchor (), QTextCursor::MoveAnchor);
   }

   QTextCursor newCursor = c;

   if (!Ttf.isEmpty ()) {

      if (Backward) { options |= QTextDocument::FindBackward; }

      if (_ui.caseSensitiveCheckBox->isChecked ()) {

         options |= QTextDocument::FindCaseSensitively;
      }

      if (_ui.wholeWordsCheckBox->isChecked ()) {

         options |= QTextDocument::FindWholeWords;
      }

      newCursor = doc->find (Ttf, c, options);
      _ui.wrappedLabel->hide();
      _ui.wrappedIcon->hide ();

      if (newCursor.isNull ()) {

         QTextCursor ac (doc);

         ac.movePosition(
            options & QTextDocument::FindBackward ?
               QTextCursor::End :
               QTextCursor::Start);

         newCursor = doc->find (Ttf, ac, options);

         if (newCursor.isNull ()) {

            p.setColor (QPalette::Active, QPalette::Base, QColor(255, 102, 102));
            newCursor = c;
         }
         else {

            _ui.wrappedLabel->show ();
            _ui.wrappedIcon->show ();
         }
      }
   }

   if (!_ui.findFrame->isVisible ()) { _ui.findFrame->show (); }

   editor->setTextCursor (newCursor);
   _ui.findLineEdit->setPalette (p);
}

