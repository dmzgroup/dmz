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
      RuntimeContext *context,
      Config &init,
      Handle &portal) {

   const String Prefix ("portal-size.");

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
local_bind (DataBinder &binder, Handle &portal, Int32 &valueX, Int32 &valueY) {

   binder.bind ("DMZ_Render_Portal_Resize_Portal_Handle", 0, portal);
   binder.bind ("DMZ_Render_Portal_Resize_Portal_Size", 0, valueX);
   binder.bind ("DMZ_Render_Portal_Resize_Portal_Size", 1, valueY);
}

};

/*!

\class dmz::PortalSize
\ingroup Render
\brief Transmits the current height and width of a portal to PortalSizeObservers.
\details The PortalSize class sends a message every time the parameter passed into
set_size differ from the current height and width values. The message type defaults to
"DMZ_Render_Portal_Resize_Message" and the portal handle defaults to
dmz::RenderMainPortalName.
\code
<dmz>
<your-namespace>
<!-- Note: your-namespace can be any value and should be the Config passed in -->
   <portal-size>
      <name value="Portal Name"/>
      <message>
         <resize name="Message Name"/>
      </message>
   <portal-size>
</your-namespace>
</dmz>
\endcode

*/

struct dmz::PortalSize::State {

   Message msg;
   
   RuntimeContainer rt;
   DataBinder binder;
   Data data;
   Handle portal;
   Int32 x, y;

   void configure (Config &init) {

      msg = local_configure (rt.get_context (), init, portal);
   }

   State (RuntimeContext *context) :
         rt (context),
         binder (context),
         portal (0),
         x (0),
         y (0) { local_bind (binder, portal, x, y); }
};


/*!

\brief Constructor.
\param[in] context Pointer to the RuntimeContext.

*/
dmz::PortalSize::PortalSize (RuntimeContext *context) : _state (*(new State (context))) {

   Config init;
   _state.configure (init);
}


/*!

\brief Constructor.
\param[in] context Pointer to the RuntimeContext.
\param[in] init Config object used in initialization.

*/
dmz::PortalSize::PortalSize (RuntimeContext *context, Config &init) :
      _state (*(new State (context))) {

   _state.configure (init);
}


/*!

\brief Constructor.
\param[in] Info Reference to the PluginInfo.

*/
dmz::PortalSize::PortalSize (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {

   Config init;
   _state.configure (init);
}


/*!

\brief Constructor.
\param[in] Info Reference to the PluginInfo.
\param[in] init Config object used in initialization.

*/
dmz::PortalSize::PortalSize (const PluginInfo &Info, Config &init) :
      _state (*(new State (Info.get_context ()))) {

   _state.configure (init);
}


//! Destructor.
dmz::PortalSize::~PortalSize () { delete &_state; }


/*!

\brief Reconfigures the objects settings.
\details May be called at any time.
\param[in] init Config object used to configure the object.

*/
void
dmz::PortalSize::configure (Config &init) {

   _state.configure (init);
}


/*!

\brief Sets the portal handle transmitted in the resize message.
\param[in] PortalHandle Handle of the portal to be transmitted in the resize message.

*/
void
dmz::PortalSize::set_portal_handle (const Handle PortalHandle) {

   _state.portal = PortalHandle;
}


/*!

\brief Gets the portal handle transmitted in the resize message.
\return Returns the portal handle used when transmitting the resize message.
*/
dmz::Handle
dmz::PortalSize::get_portal_handle () const { return _state.portal; }


/*!

\brief Set the current portal size.
\details If the values are different from the current values, a resize message is
sent to all PortalSizeObserver objects containing the size and source of the resize.
\param[in] ValueX An Int32 containing the width of the portal.
\param[in] ValueY An Int32 containing the height of the portal.

*/
void
dmz::PortalSize::set_size (const Int32 ValueX, const Int32 ValueY) {

   if ((ValueX != _state.x) || (ValueY != _state.y)) {

      _state.x = ValueX;
      _state.y = ValueY;

      _state.binder.write_data (_state.data);

      _state.msg.send (&(_state.data));
   }
}


/*!

\class dmz::PortalSizeObserver
\ingroup Render
\brief Observers changes in the size of the render portal.
\details Allows variables to be bound to either the X or Y component of the
portal's size. These bound variable are automatically updated when the portal's
size changes.

*/
//! \cond
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

      msg = local_configure (rt.get_context (), init, targetPortal);
      
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
//! \endcond


/*!

\brief Constructor.
\param[in] context Pointer to the RuntimeContext.

*/
dmz::PortalSizeObserver::PortalSizeObserver (RuntimeContext *context) :
      _state (*(new State (context))) {

   Config init;
   _state.set_observer (this);
   _state.configure (init);
}


/*!

\brief Constructor.
\param[in] context Pointer to the RuntimeContext.
\param[in] init Config object used in initialization.

*/
dmz::PortalSizeObserver::PortalSizeObserver (RuntimeContext *context, Config &init) :
      _state (*(new State (context))) {

   _state.set_observer (this);
   _state.configure (init);
}


/*!

\brief Constructor.
\param[in] Info Reference to the PluginInfo.

*/
dmz::PortalSizeObserver::PortalSizeObserver (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {

   Config init;
   _state.set_observer (this);
   _state.configure (init);
}


/*!

\brief Constructor.
\param[in] Info Reference to the PluginInfo.
\param[in] init Config object used in initialization.

*/
dmz::PortalSizeObserver::PortalSizeObserver (const PluginInfo &Info, Config &init) :
      _state (*(new State (Info.get_context ()))) {

   _state.set_observer (this);
   _state.configure (init);
}


//! Destructor.
dmz::PortalSizeObserver::~PortalSizeObserver () { delete &_state; }


/*!

\brief Reconfigures the objects settings.
\details May be called at any time.
\param[in] init Config object used to configure the object.

*/
void
dmz::PortalSizeObserver::configure_portal (Config &init) {

   _state.configure (init);
}


/*!

\brief Sets the Handle of the portal to observer.
\param[in] PortalHandle Handle of the portal to observer.

*/
void
dmz::PortalSizeObserver::set_portal_handle (const Handle PortalHandle) {

   _state.targetPortal = PortalHandle;
}


/*!

\brief Gets the Handle of the portal being observed.
\return Returns the Handle of the portal being observed.

*/
dmz::Handle
dmz::PortalSizeObserver::get_portal_handle () const {

   return _state.targetPortal;
}


/*!

\brief Gets the X component of the portal's size.
\return Returns the X component of the portal's size.

*/
dmz::Int32
dmz::PortalSizeObserver::get_portal_x () const { return _state.x; }


/*!

\brief Gets the Y component of the portal's size.
\return Returns the Y component of the portal's size.

*/
dmz::Int32
dmz::PortalSizeObserver::get_portal_y () const { return _state.y; }


/*!

\fn void dmz::PortalSizeObserver::update_portal_size (
const Handle PortalHandle,
const Int32 ValueX,
const Int32 ValueY)
\brief Pure virtual function that is invoked when the portal size changes.
\param[in] PortalHandle Handle of the portal that is changing size.
\param[in] ValueX The X component of the portal's new size.
\param[in] ValueY The Y component of the portal's new size.

*/

/*!

\class dmz::PortalSizeObserverSimple
\ingroup Render
\brief Simple observers changes in the size of the render portal.

*/
namespace {

struct BindStruct {

   BindStruct *next;
   Int32 &value;
   BindStruct (Int32 &theValue) : next (0), value (theValue) {;}
   ~BindStruct () { if (next) { delete next; next = 0; } }
};

};


//! \cond
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
//! \endcond


/*!

\brief Constructor.
\param[in] context Pointer to the RuntimeContext.

*/
dmz::PortalSizeObserverSimple::PortalSizeObserverSimple (RuntimeContext *context) :
      PortalSizeObserver (context),
      _state (*(new State)) {;}

/*!

\brief Constructor.
\param[in] context Pointer to the RuntimeContext.
\param[in] init Config object used in initialization.

*/
dmz::PortalSizeObserverSimple::PortalSizeObserverSimple (
      RuntimeContext *context,
      Config &init) :
      PortalSizeObserver (context, init),
      _state (*(new State)) {;}


/*!

\brief Constructor.
\param[in] Info Reference to the PluginInfo.

*/
dmz::PortalSizeObserverSimple::PortalSizeObserverSimple (const PluginInfo &Info) :
      PortalSizeObserver (Info),
      _state (*(new State)) {

}


/*!

\brief Constructor.
\param[in] Info Reference to the PluginInfo.
\param[in] init Config object used in initialization.

*/
dmz::PortalSizeObserverSimple::PortalSizeObserverSimple (
      const PluginInfo &Info,
      Config &init) :
      PortalSizeObserver (Info, init),
      _state (*(new State)) {

}


//! Destructor.
dmz::PortalSizeObserverSimple::~PortalSizeObserverSimple () { delete &_state; }


void
dmz::PortalSizeObserverSimple::update_portal_size (
      const Handle PortalHandle,
      const Int32 ValueX,
      const Int32 ValueY) {

   BindStruct *current = _state.xlist;
   while (current) { current->value = ValueX; current = current->next; }

   current = _state.ylist;
   while (current) { current->value = ValueY; current = current->next; }
}


/*!

\brief Binds the value to the X component of the portal's size.
\param[in] valueX The variable to bind.

*/
void
dmz::PortalSizeObserverSimple::bind_x (Int32 &valueX) {

   BindStruct *bs = new BindStruct (valueX);
   bs->next = _state.xlist;
   _state.xlist = bs;
}


/*!

\brief Binds the value to the Y component of the portal's size.
\param[in] valueY The variable to bind.

*/
void
dmz::PortalSizeObserverSimple::bind_y (Int32 &valueY) {

   BindStruct *bs = new BindStruct (valueY);
   bs->next = _state.ylist;
   _state.ylist = bs;
}


//! Unbinds all bound variables.
void
dmz::PortalSizeObserverSimple::unbind () { _state.unbind (); }


