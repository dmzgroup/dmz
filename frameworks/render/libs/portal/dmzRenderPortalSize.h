#ifndef DMZ_RENDER_PORTAL_SIZE_DOT_H
#define DMZ_RENDER_PORTAL_SIZE_DOT_H

#include <dmzRenderPortalExport.h>
#include <dmzTypesBase.h>

namespace dmz {

class Config;
class PluginInfo;
class RuntimeContext;

class DMZ_RENDER_PORTAL_LINK_SYMBOL PortalSize {

   public:
      PortalSize (RuntimeContext *context);
      PortalSize (RuntimeContext *context, Config &init);
      PortalSize (const PluginInfo &Info);
      PortalSize (const PluginInfo &Info, Config &init);
      ~PortalSize ();

      void configure (Config &init);

      void set_portal_handle (const Handle PortalHandle);
      Handle get_portal_handle () const;

      void set_size (const Int32 ValueX, const Int32 ValueY);

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond

   private:
      PortalSize ();
      PortalSize (const PortalSize &);
      PortalSize &operator= (const PortalSize &);
};

class DMZ_RENDER_PORTAL_LINK_SYMBOL PortalSizeObserver {

   public:
      PortalSizeObserver (RuntimeContext *context);
      PortalSizeObserver (RuntimeContext *context, Config &init);
      PortalSizeObserver (const PluginInfo &Info);
      PortalSizeObserver (const PluginInfo &Info, Config &init);
      virtual ~PortalSizeObserver ();

      void configure_portal (Config &init);

      void set_portal_handle (const Handle PortalHandle);
      Handle get_portal_handle () const;

      Int32 get_portal_x () const;
      Int32 get_portal_y () const;

      virtual void update_portal_size (
         const Handle PortalHandle,
         const Int32 ValueX,
         const Int32 ValueY) = 0;

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond

   private:
      PortalSizeObserver ();
      PortalSizeObserver (const PortalSizeObserver &);
      PortalSizeObserver &operator= (const PortalSizeObserver &);

};

class DMZ_RENDER_PORTAL_LINK_SYMBOL PortalSizeObserverSimple :
      public PortalSizeObserver {

   public:
      PortalSizeObserverSimple (RuntimeContext *context);
      PortalSizeObserverSimple (RuntimeContext *context, Config &init);
      PortalSizeObserverSimple (const PluginInfo &Info);
      PortalSizeObserverSimple (const PluginInfo &Info, Config &init);
      virtual ~PortalSizeObserverSimple ();

      virtual void update_portal_size (
         const Handle PortalHandle,
         const Int32 ValueX,
         const Int32 ValueY);

      void bind_x (Int32 &valueX);
      void bind_y (Int32 &valueY);

      void unbind ();

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond

   private:
      PortalSizeObserverSimple ();
      PortalSizeObserverSimple (const PortalSizeObserver &);
      PortalSizeObserverSimple &operator= (const PortalSizeObserver &);
};

};


#endif // DMZ_RENDER_PORTAL_SIZE_DOT_H
