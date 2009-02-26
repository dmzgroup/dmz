#include <dmzRenderConsts.h>
#include <dmzRenderPortalSize.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeContainer.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDataBinder.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePluginInfo.h>

using namespace dmz;

namespace {

Message
local_configure (
      const String &ThePrefix,
      RuntimeContext *context,
      Config &init,
      Handle &portal) {

   const String Prefix (ThePrefix ? ThePrefix + "." : String (""));

   Message result = config_create_message_type (
      Prefix + "message.resize.name",
      init,
      "DMZ_Render_Portal_Resize_Message",
      context,
      0);

   result.set_monostate_mode (MessageMonostateOn);

   portal = config_to_named_handle (
      Prefix + "name.value",
      init,
      RenderMainPortalName,
      context);

   return result;
}

void
local_bind (DataBinder &binder, Handle &portal, Int32 &theX, Int32 &theY) {

   binder.bind ("DMZ_Render_Portal_Resize_Portal_Handle", 0, portal);
   binder.bind ("DMZ_Render_Portal_Resize_Portal_Size", 0, theX);
   binder.bind ("DMZ_Render_Portal_Resize_Portal_Size", 1, theY);
}

};

struct dmz::PortalSize::State {

   Message msg;
   
   RuntimeContainer rt;
   DataBinder binder;
   Data data;
   Handle portal;
   Int32 x, y;

   void configure (Config &init) {

      msg = local_configure ("", rt.get_context (), init, portal);
   }

   State (RuntimeContext *context) :
         rt (context),
         binder (context),
         portal (0),
         x (0),
         y (0) { local_bind (binder, portal, x, y); }
};

dmz::PortalSize::PortalSize (RuntimeContext *context) : _state (*(new State (context))) {

   Config init;
   _state.configure (init);
}


dmz::PortalSize::PortalSize (RuntimeContext *context, Config &init) :
      _state (*(new State (context))) {

   _state.configure (init);
}


dmz::PortalSize::PortalSize (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {

   Config init;
   _state.configure (init);
}


dmz::PortalSize::PortalSize (const PluginInfo &Info, Config &init) :
      _state (*(new State (Info.get_context ()))) {

   _state.configure (init);
}


dmz::PortalSize::~PortalSize () { delete &_state; }


void
dmz::PortalSize::configure (Config &init) {

   _state.configure (init);
}


void
dmz::PortalSize::set_portal_handle (const Handle PortalHandle) {

   _state.portal = PortalHandle;
}


dmz::Handle
dmz::PortalSize::get_portal_handle () const { return _state.portal; }


void
dmz::PortalSize::set_size (const Int32 TheX, const Int32 TheY) {

   if ((TheX != _state.x) || (TheY != _state.y)) {

      _state.x = TheX;
      _state.y = TheY;

      _state.binder.write_data (_state.data);

      _state.msg.send (&(_state.data));
   }
}


struct dmz::PortalSizeObserver::State : public MessageObserver {

   PortalSizeObserver *obs;

   Message msg;
   
   RuntimeContainer rt;
   DataBinder binder;
   Data data;
   Handle portal;
   Handle targetPortal;
   Int32 x, y;

   void set_observer (PortalSizeObserver *theObs) { obs = theObs; }

   void configure (Config &init) {

      if (msg) { unsubscribe_to_message (msg); }

      msg = local_configure ("", rt.get_context (), init, targetPortal);
      
      if (msg) { subscribe_to_message (msg); }
   }

   virtual void receive_message (
         const Message &Type,
         const Handle MessageSendHandle,
         const Handle TargetObserverHandle,
         const Data *InData,
         Data *outData) {

      if (obs && (Type.is_of_type (msg)) && InData && binder.read_data (*InData)) {

         if (portal == targetPortal) { obs->update_portal_size (portal, x, y); }
      }
   }


   State (RuntimeContext *context) :
         MessageObserver (0, "DMZ_Render_Portal_Size_Observer", context),
         obs (0),
         rt (context),
         binder (context),
         portal (0),
         targetPortal (0),
         x (0),
         y (0) { local_bind (binder, portal, x, y); }
};

dmz::PortalSizeObserver::PortalSizeObserver (RuntimeContext *context) :
      _state (*(new State (context))) {

   Config init;
   _state.set_observer (this);
   _state.configure (init);
}


dmz::PortalSizeObserver::PortalSizeObserver (RuntimeContext *context, Config &init) :
      _state (*(new State (context))) {

   _state.set_observer (this);
   _state.configure (init);
}


dmz::PortalSizeObserver::PortalSizeObserver (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {

   Config init;
   _state.set_observer (this);
   _state.configure (init);
}


dmz::PortalSizeObserver::PortalSizeObserver (const PluginInfo &Info, Config &init) :
      _state (*(new State (Info.get_context ()))) {

   _state.set_observer (this);
   _state.configure (init);
}


dmz::PortalSizeObserver::~PortalSizeObserver () { delete &_state; }


void
dmz::PortalSizeObserver::configure (Config &init) {

   _state.configure (init);
}


void
dmz::PortalSizeObserver::set_portal_handle (const Handle PortalHandle) {

   _state.targetPortal = PortalHandle;
}


dmz::Handle
dmz::PortalSizeObserver::get_portal_handle () const {

   return _state.targetPortal;
}


dmz::Int32
dmz::PortalSizeObserver::get_x () const { return _state.x; }


dmz::Int32
dmz::PortalSizeObserver::get_y () const { return _state.y; }


namespace {

struct BindStruct {

   BindStruct *next;
   Int32 &value;
   BindStruct (Int32 &theValue) : next (0), value (theValue) {;}
   ~BindStruct () { if (next) { delete next; next = 0; } }
};

};


struct dmz::PortalSizeObserverSimple::State {

   BindStruct *xlist;
   BindStruct *ylist;

   void unbind () {

      if (xlist) { delete xlist; xlist = 0; }
      if (ylist) { delete ylist; ylist = 0; }
   }

   State () : xlist (0), ylist (0) {;}
   ~State () { unbind (); }
};

dmz::PortalSizeObserverSimple::PortalSizeObserverSimple (RuntimeContext *context) :
      PortalSizeObserver (context),
      _state (*(new State)) {;}


dmz::PortalSizeObserverSimple::PortalSizeObserverSimple (
      RuntimeContext *context,
      Config &init) :
      PortalSizeObserver (context, init),
      _state (*(new State)) {;}


dmz::PortalSizeObserverSimple::PortalSizeObserverSimple (const PluginInfo &Info) :
      PortalSizeObserver (Info),
      _state (*(new State)) {

}


dmz::PortalSizeObserverSimple::PortalSizeObserverSimple (
      const PluginInfo &Info,
      Config &init) :
      PortalSizeObserver (Info, init),
      _state (*(new State)) {

}


dmz::PortalSizeObserverSimple::~PortalSizeObserverSimple () { delete &_state; }


void
dmz::PortalSizeObserverSimple::update_portal_size (
      const Handle PortalHandle,
      const Int32 TheX,
      const Int32 TheY) {

   BindStruct *current = _state.xlist;
   while (current) { current->value = TheX; current = current->next; }

   current = _state.ylist;
   while (current) { current->value = TheY; current = current->next; }
}


void
dmz::PortalSizeObserverSimple::bind_x (Int32 &theX) {

   BindStruct *bs = new BindStruct (theX);
   bs->next = _state.xlist;
   _state.xlist = bs;
}


void
dmz::PortalSizeObserverSimple::bind_y (Int32 &theY) {

   BindStruct *bs = new BindStruct (theY);
   bs->next = _state.ylist;
   _state.ylist = bs;
}


void
dmz::PortalSizeObserverSimple::unbind () { _state.unbind (); }


