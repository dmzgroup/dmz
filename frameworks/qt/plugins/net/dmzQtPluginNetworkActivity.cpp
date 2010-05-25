#include "dmzQtPluginNetworkActivity.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::QtPluginNetworkActivity::QtPluginNetworkActivity (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeSystemTime, TimeSliceModeRepeating, 2.0),
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

   show ();
}


dmz::QtPluginNetworkActivity::~QtPluginNetworkActivity () {

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

_log.warn << _maxRate << endl;
      _cWriteSize = _cWriteCount = _cReadSize = _cReadCount = 0.0;

      _update_graph ();

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

}


void
dmz::QtPluginNetworkActivity::_update_graph () {

   QPainterPath inPath;
   QPainterPath outPath;

   Int32 place (_currentSample);

   const QSize Size = _ui.graphView->size ();
   const QPointF Min = _ui.graphView->mapToScene (0, 0);
   const QPointF Max = _ui.graphView->mapToScene (Size.width (), Size.height ());

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

   while (count < SampleCount) {

      if (!count) {

         inPath.moveTo (MaxX, MaxY - (_samples[place].inRate * Unit));
         outPath.moveTo (MaxX, MaxY - (_samples[place].outRate * Unit));
      }
      else {

         inPath.lineTo (MaxX - (count * Space), MaxY - (_samples[place].inRate * Unit));
         outPath.lineTo (MaxX - (count * Space), MaxY - (_samples[place].outRate * Unit));
      }

      count++;
      place--;
      if (place < 0) { place = SampleCount - 1; }
   }

   _inPath->setPath (inPath);
   _outPath->setPath (outPath);
}


void
dmz::QtPluginNetworkActivity::_init (Config &local) {

   _scene = new QGraphicsScene (this);
   _ui.graphView->setScene (_scene);
   _inPath = new QGraphicsPathItem;
   _outPath = new QGraphicsPathItem;
   _scene->addItem (_inPath);
   _scene->addItem (_outPath);
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
