#ifndef DMZ_RENDER_EXT_WINDOW_OGRE_QT_DOT_H
#define DMZ_RENDER_EXT_WINDOW_OGRE_QT_DOT_H

#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzRenderExtWindowOgre.h>
#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <QtGui/QWidget>


namespace Ogre { class RenderWindow; class Camera; class Viewport; }


namespace dmz {
   
   class InputModule;
   class RenderModuleCoreOgre;

   class RenderExtWindowOgreQt :
         public QWidget,
         public Plugin,
         public RenderExtWindowOgre {
            
      Q_OBJECT

      public:
         RenderExtWindowOgreQt (const PluginInfo &Info, Config &local);
         ~RenderExtWindowOgreQt ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderExtWindowOgre Interface
         virtual Boolean init_render_window ();
         virtual String get_render_window_name () const;

      protected:
         virtual void closeEvent (QCloseEvent *event);
         virtual void focusOutEvent (QFocusEvent *event);
         virtual void resizeEvent (QResizeEvent* event);
         virtual void keyPressEvent (QKeyEvent *event);
         virtual void keyReleaseEvent (QKeyEvent* event);
         virtual void mousePressEvent (QMouseEvent *event);
         virtual void mouseReleaseEvent (QMouseEvent* event);
         virtual void mouseMoveEvent (QMouseEvent *event);
         virtual void wheelEvent (QWheelEvent* event);

         void _save_session ();
         void _load_session ();
         void _init (const Config &Local);
         
         virtual void _handle_key_event (
            const QKeyEvent &Event,
            const Boolean KeyState);
            
         void _handle_mouse_event (QMouseEvent *me, QWheelEvent *we);
         
         Exit _exit;
         Log _log;
         InputModule *_channels;
         RenderModuleCoreOgre *_core;
         String _portalName;
         UInt32 _fsaa;
         Ogre::RenderWindow *_window;
         Ogre::Camera *_camera;
         Ogre::Viewport *_viewport;
         InputEventKey _keyEvent;
         InputEventMouse _mouseEvent;
         HashTableUInt32Template<InputEventKey> _keyDownTable;
         
      private:
         RenderExtWindowOgreQt ();
         RenderExtWindowOgreQt (const RenderExtWindowOgreQt &);
         RenderExtWindowOgreQt &operator= (const RenderExtWindowOgreQt &);
   };
};


#endif // DMZ_RENDER_EXT_WINDOW_OGRE_QT_DOT_H
