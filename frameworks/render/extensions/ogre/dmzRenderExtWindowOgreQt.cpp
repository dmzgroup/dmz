#include "dmzRenderExtWindowOgreQt.h"
#include <dmzInputModule.h>
#include <dmzQtUtil.h>
#include <dmzRenderModuleCoreOgre.h>
#include <dmzRenderUtilOgre.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <Ogre/Ogre.h>
#include <QtGui/QtGui>

#if defined(Q_WS_X11)
#include <QtGui/QX11Info>
#endif


dmz::RenderExtWindowOgreQt::RenderExtWindowOgreQt (
      const PluginInfo &Info,
      Config &local) :
      QWidget (0),
      Plugin (Info),
      RenderExtWindowOgre (Info),
      _exit (get_plugin_runtime_context ()),
      _log (Info), 
      _channels (0),
      _core (0),
      _portalName (DefaultPortalNameOgre),
      _fsaa (0),
      _window (0),
      _camera (0),
      _viewport (0),
      _keyEvent (),
      _mouseEvent (),
      _keyDownTable () {

   _init (local);

   setAttribute(Qt::WA_NoSystemBackground);
   setFocusPolicy (Qt::WheelFocus);
}


dmz::RenderExtWindowOgreQt::~RenderExtWindowOgreQt () {

   _keyDownTable.empty ();
}


// Plugin Interface


void
dmz::RenderExtWindowOgreQt::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {
      
      _load_session ();
      show ();
      setFocus (Qt::ActiveWindowFocusReason);
   }
   else if (State == PluginStateStop) {
      
      _save_session ();
      close ();
   }
}


void
dmz::RenderExtWindowOgreQt::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOgre::cast (PluginPtr);

         if (_core && _window) {

            Ogre::SceneManager *sceneManager = _core->get_scene_manager ();

            if (sceneManager) {

               _camera = sceneManager->createCamera (_portalName.get_buffer ());

               if (_camera) {

                  _core->add_camera (_portalName, _camera);

                  _viewport = _window->addViewport (_camera); 

                  if (_viewport) {

                     _camera->setAspectRatio (
                        Ogre::Real(_viewport->getActualWidth ()) /
                        Ogre::Real(_viewport->getActualHeight ()));
                  }
               }
            }
         }
      }

      if (!_channels) {

         _channels = InputModule::cast (PluginPtr);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOgre::cast (PluginPtr))) {

         _core->remove_camera (_portalName);

         if (_window && _viewport) {

            _window->removeViewport (_viewport->getZOrder ());
            _viewport = 0;
         }

         if (_camera) {

            Ogre::SceneManager *sceneManager = _camera->getSceneManager ();

            if (sceneManager) {

               sceneManager->destroyCamera (_camera);
               _camera = 0;
            }
         }

         _core = 0;
      }

      if (_channels && (_channels == InputModule::cast (PluginPtr))) {

         _keyEvent.set_source_handle (0);
         _mouseEvent.set_source_handle (0);
         _channels = 0;
      }
   }
}


dmz::Boolean
dmz::RenderExtWindowOgreQt::init_render_window () { 
   
   Boolean retVal (False);
   
   Ogre::Root &root = Ogre::Root::getSingleton ();
      
   Ogre::NameValuePairList params;
   
   if (_fsaa) {
      
      params["FSAA"] = Ogre::StringConverter::toString (_fsaa);
   }

#if defined(Q_WS_X11)        

   QWidget *widget = dynamic_cast<QWidget *> (parent());
   if (!widget) { widget = this; }
          
   QX11Info xInfo = x11Info ();
    
   Ogre::String arg1 = Ogre::StringConverter::toString ((unsigned long) xInfo.display ());
   Ogre::String arg2 = Ogre::StringConverter::toString ((unsigned int) xInfo.screen ());
   Ogre::String arg3 =
      Ogre::StringConverter::toString ((unsigned long) (widget->winId ()));      
    
   params["parentWindowHandle"] = arg1 + ":" + arg2 + ":" + arg3;
#else   
   params["externalWindowHandle"] =
      Ogre::StringConverter::toString ((size_t)winId ());
#endif

   _window =
      root.createRenderWindow (
         _portalName.get_buffer (),
         width (),
         height (),
         false,
         &params);

   if (_window) {
      
      _window->windowMovedOrResized ();
      retVal = True;
   }

   return retVal;
}


dmz::String
dmz::RenderExtWindowOgreQt::get_render_window_name () const { return _portalName; }


void
dmz::RenderExtWindowOgreQt::closeEvent (QCloseEvent *event) {
   
   _exit.request_exit (dmz::ExitStatusNormal, "RenderExtWindowOgreQt Closed");
}


void
dmz::RenderExtWindowOgreQt::focusOutEvent (QFocusEvent *event) {

   // when widget looses focus send a keyup event for all keys that are known to be down
   // see _handle_key_event for where the keydown events get stored

   HashTableUInt32Iterator it;
   InputEventKey *keyEvent (_keyDownTable.get_first (it));

   while (keyEvent) {

      if (_channels) { _channels->send_key_event (*keyEvent); }
      keyEvent = _keyDownTable.get_next (it);
   }

   _keyDownTable.empty ();
}


void
dmz::RenderExtWindowOgreQt::resizeEvent (QResizeEvent *event) {
   
   if (event) {

      if (_window) { _window->windowMovedOrResized (); }
      
      if (_camera && _viewport) {
         
         _camera->setAspectRatio (
            Ogre::Real (_viewport->getActualWidth ()) /
            Ogre::Real (_viewport->getActualHeight ()));
      }
      
      _handle_mouse_event (0, 0);
   }
}


void
dmz::RenderExtWindowOgreQt::keyPressEvent (QKeyEvent *event) {

   if (event) { _handle_key_event (*event, True); }
}


void
dmz::RenderExtWindowOgreQt::keyReleaseEvent (QKeyEvent *event) {

   if (event) { _handle_key_event (*event, False); }
}


void
dmz::RenderExtWindowOgreQt::mousePressEvent (QMouseEvent *event) {

   _handle_mouse_event (event, 0);
}


void
dmz::RenderExtWindowOgreQt::mouseReleaseEvent (QMouseEvent *event) {

   _handle_mouse_event (event, 0);
}


void
dmz::RenderExtWindowOgreQt::mouseMoveEvent (QMouseEvent *event) {

   _handle_mouse_event (event, 0);
}


void
dmz::RenderExtWindowOgreQt::wheelEvent (QWheelEvent *event) {

   _handle_mouse_event (0, event);
}


void
dmz::RenderExtWindowOgreQt::_save_session () {

   String data;
   Config session (get_plugin_name ());
   session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));
   set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::RenderExtWindowOgreQt::_load_session () {

   Config session (
      get_session_config (get_plugin_name (), get_plugin_runtime_context ()));

   QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
   restoreGeometry (geometry);
}


void
dmz::RenderExtWindowOgreQt::_init (const Config &Local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _portalName = config_to_string ("portal.name", Local, _portalName);

   const Handle SourceHandle (defs.create_named_handle (_portalName));

   _keyEvent.set_source_handle (SourceHandle);
   _mouseEvent.set_source_handle (SourceHandle);

   Config windowData;

   if (Local.lookup_all_config ("window", windowData)) {

      ConfigIterator it;
      Config cd;

      Boolean found (windowData.get_first_config (it, cd));
      if (found)  {

         UInt32 width = config_to_uint32 ("width", cd, 800);
         UInt32 height = config_to_uint32 ("height", cd, 600);
         resize (width, height);
         
      }
      
      _fsaa = config_to_uint32 ("window.fsaa", Local, _fsaa);

      if (_fsaa % 2) {
         
         _log.warn << "fsaa not even: " << _fsaa <<  " using " << (_fsaa-1) << " instead"
            << endl;
         _fsaa--;
      }
   }
}


void
dmz::RenderExtWindowOgreQt::_handle_key_event (
      const QKeyEvent &Event, const Boolean KeyState) {

   if (!Event.isAutoRepeat ()) {

      UInt32 theKey (0);

      switch (Event.key ()) {

         case Qt::Key_F1: theKey = KeyF1; break;
         case Qt::Key_F2: theKey = KeyF2; break;
         case Qt::Key_F3: theKey = KeyF3; break;
         case Qt::Key_F4: theKey = KeyF4; break;
         case Qt::Key_F5: theKey = KeyF5; break;
         case Qt::Key_F6: theKey = KeyF6; break;
         case Qt::Key_F7: theKey = KeyF7; break;
         case Qt::Key_F8: theKey = KeyF8; break;
         case Qt::Key_F9: theKey = KeyF9; break;
         case Qt::Key_F10: theKey = KeyF10; break;
         case Qt::Key_F11: theKey = KeyF11; break;
         case Qt::Key_F12: theKey = KeyF12; break;
         case Qt::Key_Left: theKey = KeyLeftArrow; break;
         case Qt::Key_Up: theKey = KeyUpArrow; break;
         case Qt::Key_Right: theKey = KeyRightArrow; break;
         case Qt::Key_Down: theKey = KeyDownArrow; break;
         case Qt::Key_PageUp: theKey = KeyPageUp; break;
         case Qt::Key_PageDown: theKey = KeyPageDown; break;
         case Qt::Key_Home: theKey = KeyHome; break;
         case Qt::Key_End: theKey = KeyEnd; break;
         case Qt::Key_Insert: theKey = KeyInsert; break;
         case Qt::Key_Space: theKey = KeySpace; break;
         case Qt::Key_Escape: theKey = KeyEsc; break;
         case Qt::Key_Tab: theKey = KeyTab; break;
         case Qt::Key_Backspace: theKey = KeyBackspace; break;
         case Qt::Key_Enter: theKey = KeyEnter; break;
         case Qt::Key_Return: theKey = KeyEnter; break;
         case Qt::Key_Delete: theKey = KeyDelete; break;
         case Qt::Key_Shift : theKey = KeyShift; break;
         case Qt::Key_Control : theKey = KeyControl; break;
         case Qt::Key_Meta : theKey = KeyMeta; break;
         case Qt::Key_Alt : theKey = KeyAlt; break;

         default:

            if (!(Event.text ().isEmpty ())) {

               theKey = Event.text ().at (0).toAscii ();
            }
      }

      _keyEvent.set_key (theKey);
      _keyEvent.set_key_state (KeyState);
      
      if (_channels) { _channels->send_key_event (_keyEvent); }

      // keep track of all keys that are pressed when widget gets a focusOut event
      // all pressed keys need to be sent out as released
      
      if (KeyState) {

         InputEventKey *downKeyEvent (_keyDownTable.lookup (theKey));

         if (!downKeyEvent) {

            downKeyEvent = new InputEventKey;
            _keyDownTable.store (theKey, downKeyEvent);
         }

         // store as a KeyUp event
         downKeyEvent->set_key_state (False);
         downKeyEvent->set_key (theKey);
      }
      else {

         InputEventKey *downKeyEvent (_keyDownTable.remove (theKey));
         if (downKeyEvent) { delete downKeyEvent; downKeyEvent = 0; }
      }
   }
}


void
dmz::RenderExtWindowOgreQt::_handle_mouse_event (QMouseEvent *me, QWheelEvent *we) {
   
   if (_channels) {
      
      InputEventMouse event (_mouseEvent);
      
      QPoint pointOnWindow (event.get_mouse_x (), event.get_mouse_y ());
      QPoint pointOnScreen (event.get_mouse_screen_x (), event.get_mouse_screen_y ());
      
      Qt::MouseButtons buttons;
      
      if (me) {
         
         pointOnWindow = me->pos ();
         pointOnScreen = me->globalPos ();

         buttons = me->buttons ();
      }
      else if (we) {

         pointOnWindow = we->pos ();
         pointOnScreen = we->globalPos ();
         
         buttons = we->buttons ();
      }

      event.set_mouse_position (pointOnWindow.x (), pointOnWindow.y ());
      event.set_mouse_screen_position (pointOnScreen.x (), pointOnScreen.y ());

      UInt32 mask (0);
      if (buttons & Qt::LeftButton) { mask |= 0x01 << 0; }
      if (buttons & Qt::RightButton) { mask |= 0x01 << 1; }
      if (buttons & Qt::MidButton) { mask |= 0x01 << 2; }

      event.set_button_mask (mask);

      Int32 deltaX (0), deltaY (0);

      if (we) {

         if (we->orientation () == Qt::Vertical) { deltaY = we->delta (); }
         else if (we->orientation () == Qt::Horizontal) { deltaX = we->delta (); }
      }

      event.set_scroll_delta (deltaX, deltaY);

      event.set_window_size (width (), height ());
      
      if (_mouseEvent.update (event)) {
         
         _channels->send_mouse_event (_mouseEvent);
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderExtWindowOgreQt (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderExtWindowOgreQt (Info, local);
}

};
