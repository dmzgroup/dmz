#ifndef DMZ_RENDER_OBJECT_DATA_OSG_DOT_H
#define DMZ_RENDER_OBJECT_DATA_OSG_DOT_H

#include <dmzTypesBase.h>
#include <osg/Referenced>

namespace dmz {

   class RenderObjectDataOSG : public osg::Referenced {

      public:
         RenderObjectDataOSG (const Handle ObjectHandle);

         Handle get_handle () const;

         Boolean do_isect () const;
         Int32 disable_isect ();
         Int32 enable_isect ();

      protected:
         virtual ~RenderObjectDataOSG () {;}

         const Handle _ObjectHandle;
         Int32 _isectCount;
   };
};

#endif // DMZ_RENDER_OBJECT_DATA_OSG_DOT_H
