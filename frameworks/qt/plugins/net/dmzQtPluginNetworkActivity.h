#ifndef DMZ_QT_PLUGIN_NETWORK_ACTIVITY_DOT_H
#define DMZ_QT_PLUGIN_NETWORK_ACTIVITY_DOT_H

#include <dmzNetPacketStatsObserver.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

#include <QtGui/QFrame>
#include <QtGui/QBrush>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QGraphicsView>
#include <QtGui/QPen>
#include <ui_NetworkActivity.h>

namespace dmz {

   const Int32 SampleCount = 100;

   class QtPluginNetworkActivity :
         public QFrame,
         public Plugin,
         public TimeSlice,
         public NetPacketStatsObserver,
         public MessageObserver {

      Q_OBJECT

      public:
         QtPluginNetworkActivity (const PluginInfo &Info, Config &local);
         ~QtPluginNetworkActivity ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 DeltaTime);

         // NetPacketStatsObserver Interface
         virtual void add_write_packet_stat (
            const Handle SourceHandle,
            const Handle TargetHandle,
            const Int32 PacketSize,
            const char *Buffer);

         virtual void add_read_packet_stat (
            const Handle SourceHandle,
            const Int32 PacketSize,
            const char *Buffer);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         struct SampleStruct {

            Float64 inRate;
            Float64 outRate;

            SampleStruct () : inRate (0.0), outRate (0.0) {;}
         };

         void _update_graph ();
         void _init (Config &local);

         Log _log;

         QGraphicsScene *_scene;
         QGraphicsPathItem *_inPath;
         QGraphicsPathItem *_outPath;

         Float64 _readCount;
         Float64 _readSize;
         Float64 _cReadCount;
         Float64 _cReadSize;
         Float64 _writeCount;
         Float64 _writeSize;
         Float64 _cWriteCount;
         Float64 _cWriteSize;

         SampleStruct _samples[SampleCount];
         Int32 _currentSample;
         Float64 _maxRate;

         Ui::NetworkActivity _ui;

      private:
         QtPluginNetworkActivity ();
         QtPluginNetworkActivity (const QtPluginNetworkActivity &);
         QtPluginNetworkActivity &operator= (const QtPluginNetworkActivity &);

   };
};

#endif // DMZ_QT_PLUGIN_NETWORK_ACTIVITY_DOT_H
