#ifndef DMZ_RENDER_OBJECT_DATA_OSG_DOT_H
#define DMZ_RENDER_OBJECT_DATA_OSG_DOT_H

#include <dmzRenderUtilOSGExport.h>
#include <dmzTypesBase.h>

#include <osg/Referenced>

namespace dmz {

   class DMZ_RENDER_UTIL_OSG_LINK_SYMBOL RenderObjectDataOSG : public osg::Referenced {

      public:
         RenderObjectDataOSG (const Handle ObjectHandle);

         Handle get_handle () const;

         void set_mask (const UInt32 Mask);
         UInt32 get_mask () const;
         Boolean do_isect () const;
         Int32 disable_isect ();
         Int32 enable_isect ();

      protected:
         virtual ~RenderObjectDataOSG () {;}

         const Handle _ObjectHandle;
         UInt32 _mask;
         Int32 _isectCount;
   };
};

#endif // DMZ_RENDER_OBJECT_DATA_OSG_DOT_H
