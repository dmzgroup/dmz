#include <dmzQtConfigRead.h>
#include <dmzQtConfigWrite.h>
#include "dmzQtPluginNetworkActivity.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>

dmz::QtPluginNetworkActivity::QtPluginNetworkActivity (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeSystemTime, TimeSliceModeRepeating, 1.0),
      NetPacketStatsObserver (Info),
      MessageObserver (Info),
      _log (Info),
      _scene (0),
      _inPath (0),
      _outPath (0),
      _readCount (0.0),
      _readSize (0.0),
      _cReadCount (0.0),
      _cReadSize (0.0),
      _writeCount (0.0),
      _writeSize (0.0),
      _cWriteCount (0.0),
      _cWriteSize (0.0),
      _currentSample (0),
      _maxRate (0.0) {

   _ui.setupUi (this);

   _init (local);
}


dmz::QtPluginNetworkActivity::~QtPluginNetworkActivity () {

   _rateTable.empty ();
}


// Plugin Interface
void
dmz::QtPluginNetworkActivity::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (context) {

         Config session (get_plugin_name ());

         session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));

         if (isVisible ()) {

            session.add_config (boolean_to_config ("window", "visible", True));
         }


         const int Index = _ui.rateComboBox->currentIndex ();

         session.add_config (int32_to_config ("index", "value", Index));

         set_session_config (context, session);
      }
   }
}


void
dmz::QtPluginNetworkActivity::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}

namespace {

static const dmz::Float64 KB = 1024.0;
static const dmz::Float64 ToKB = 1.0 / 1024.0;
static const dmz::Float64 MB = 1048576.0;
static const dmz::Float64 ToMB = 1.0 / 1048576.0;
static const dmz::Float64 GB = 1073741824.0;
static const dmz::Float64 ToGB = 1.0 / 1073741824.0;
static const dmz::Float64 TB = 1099511627776.0;
static const dmz::Float64 ToTB = 1.0 / 1099511627776.0;


QString
format_size (const dmz::Float64 Size) {

   QString result;

   if (Size > TB) { result = QString::number (Size * ToTB, 'f', 1) + " TB"; }
   else if (Size > GB) { result = QString::number (Size * ToGB, 'f', 1) + " GB"; }
   else if (Size > MB) { result = QString::number (Size * ToMB, 'f', 1) + " MB"; }
   else { result = QString::number (Size * ToKB, 'f', 1) + " KB"; }

   return result;
}

};


// TimeSlice Interface
void
dmz::QtPluginNetworkActivity::update_time_slice (const Float64 DeltaTime) {

   if (DeltaTime > 0.0) {

      _ui.packetsInLabel->setText (QString::number (_readCount, 'f', 0));
      _ui.packetsOutLabel->setText (QString::number (_writeCount, 'f', 0));
      _ui.dataReceivedLabel->setText (format_size (_readSize));
      _ui.dataSentLabel->setText (format_size (_writeSize));

      const Float64 DeltaInvert = 1 / DeltaTime;

      _ui.packetsInPerSecLabel->setText (
         QString::number (_cReadCount * DeltaInvert, 'f', 0));

      _ui.packetsOutPerSecLabel->setText (
         QString::number (_cWriteCount * DeltaInvert, 'f', 0));

      _ui.dataReceivedPerSecLabel->setText (format_size (_cReadSize * DeltaInvert));
      _ui.dataSentPerSecLabel->setText (format_size (_cWriteSize * DeltaInvert));

      _currentSample++;
      if (_currentSample >= SampleCount) { _currentSample = 0; }
      _samples[_currentSample].inRate = _cReadSize * DeltaInvert;
      _samples[_currentSample].outRate = _cWriteSize * DeltaInvert;

      if (_samples[_currentSample].inRate > _maxRate) {

         _maxRate = _samples[_currentSample].inRate;
      }

      if (_samples[_currentSample].outRate > _maxRate) {

         _maxRate = _samples[_currentSample].outRate;
      }

      _ui.peakLabel->setText (format_size (_maxRate) + "/sec");

      _cWriteSize = _cWriteCount = _cReadSize = _cReadCount = 0.0;

      if (isVisible ()) { _update_graph (); }
   }
}


// NetPacketStatsObserver Interface
void
dmz::QtPluginNetworkActivity::add_write_packet_stat (
      const Handle SourceHandle,
      const Handle TargetHandle,
      const Int32 PacketSize,
      const char *Buffer) {

   _writeSize += (Float64)PacketSize;
   _cWriteSize += (Float64)PacketSize;
   _writeCount += 1.0;
   _cWriteCount += 1.0;
}


void
dmz::QtPluginNetworkActivity::add_read_packet_stat (
      const Handle SourceHandle,
      const Int32 PacketSize,
      const char *Buffer) {

   _readSize += (Float64)PacketSize;
   _cReadSize += (Float64)PacketSize;
   _readCount += 1.0;
   _cReadCount += 1.0;
}


// Message Observer Interface
void
dmz::QtPluginNetworkActivity::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _showMsg) { show (); activateWindow (); }
}


void
dmz::QtPluginNetworkActivity::on_rateComboBox_currentIndexChanged (const QString &Text) {

   const Float64 *Sec = _rateTable.lookup (qPrintable (Text));

   if (Sec) {

      set_time_slice_interval (*Sec);

      for (Int32 ix = 0; ix < SampleCount; ix++) { _samples[ix].reset (); }

      _maxRate = 0.0;
   }
}


void
dmz::QtPluginNetworkActivity::_update_graph () {

   QPainterPath inPath;
   QPainterPath outPath;

   Int32 place (_currentSample);

   static const int Offset (8);

   const QSize Size = _ui.graphView->size ();
   const QPointF Min = _ui.graphView->mapToScene (0, Offset);
   const QPointF Max = _ui.graphView->mapToScene (Size.width (), Size.height () - Offset);

   const Float64 MinX = Min.x ();
   const Float64 MinY = Min.y ();
   const Float64 MaxX = Max.x ();
   const Float64 MaxY = Max.y ();
   const Float64 Width = Max.x () - Min.x ();
   const Float64 Height = Max.y () - Min.y ();

   const Float64 Space = Width / SampleCount > 0.0 ? Width / SampleCount : 1.0;
   const Float64 MaxRate = _maxRate > 0.0 ? _maxRate : 1.0;
   const Float64 Unit = Height / MaxRate;

   Float64 count (0.0);

   Float64 newMax (0.0);

   while (count < SampleCount) {

      const Float64 InRate (_samples[place].inRate), OutRate (_samples[place].outRate);

      if (!count) {

         inPath.moveTo (MaxX, MaxY - (InRate * Unit));
         outPath.moveTo (MaxX, MaxY - (OutRate * Unit));
      }
      else {

         inPath.lineTo (MaxX - (count * Space), MaxY - (InRate * Unit));
         outPath.lineTo (MaxX - (count * Space), MaxY - (OutRate * Unit));
      }
 
      if (newMax < InRate) { newMax = InRate; }
      if (newMax < OutRate) { newMax = OutRate; }

      count++;
      place--;
      if (place < 0) { place = SampleCount - 1; }
   }

   _inPath->setPath (inPath);
   _outPath->setPath (outPath);

   _maxRate = newMax;
}


void
dmz::QtPluginNetworkActivity::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _scene = new QGraphicsScene (this);
   _ui.graphView->setScene (_scene);
   _inPath = new QGraphicsPathItem;
   _outPath = new QGraphicsPathItem;
   const QColor Green (0, 255, 0);
   const QColor Red (255, 0, 0);
   QPen inStroke (Green);
   inStroke.setWidth (2);
   QPen outStroke (Red);
   outStroke.setWidth (2);
//   inStroke = config_to_qpen ("in.stroke", local, inStroke);
//   outStroke = config_to_qpen ("out.stroke", local, outStroke);
   _inPath->setPen (inStroke);
   _outPath->setPen (outStroke);
   _scene->addItem (_inPath);
   _scene->addItem (_outPath);

   _ui.graphView->setRenderHint (QPainter::Antialiasing, true);

   const QColor Black (0, 0, 0);
   QBrush bg (Black);
   _ui.graphView->setBackgroundBrush (bg);

   _rateTable.store ("Realtime", new Float64 (0.0));
   _rateTable.store ("1 sec", new Float64 (1.0));
   _rateTable.store ("2 sec", new Float64 (2.0));
   _rateTable.store ("3 sec", new Float64 (3.0));
   _rateTable.store ("4 sec", new Float64 (4.0));
   _rateTable.store ("5 sec", new Float64 (5.0));
   _rateTable.store ("10 sec", new Float64 (10.0));
   _rateTable.store ("30 sec", new Float64 (30.0));

   HashTableStringIterator it;
   Float64 *ptr (0);

   while (_rateTable.get_next (it, ptr)) {

      _ui.rateComboBox->addItem (it.get_hash_key ().get_buffer ());
   }

   if (context) {

      Config session (get_session_config (get_plugin_name (), context));

      Config gdata;

      if (session.lookup_config ("geometry", gdata)) {

         restoreGeometry (config_to_qbytearray (gdata));
      }

      if (config_to_boolean ("window.visible", session, False)) { show (); }

      const int Index = config_to_int32 ("index.value", session, 2);

      _ui.rateComboBox->setCurrentIndex (Index);
   }

   _showMsg = config_create_message (
      "show.name",
      local,
      "DMZ_Show_Network_Activity_Console",
      context);

   subscribe_to_message (_showMsg);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginNetworkActivity (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginNetworkActivity (Info, local);
}

};
