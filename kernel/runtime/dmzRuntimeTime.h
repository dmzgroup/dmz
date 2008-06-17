#ifndef DMZ_RUNTIME_TIME_DOT_H
#define DMZ_RUNTIME_TIME_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class RuntimeContext;
   class RuntimeContextTime;
   class PluginInfo;

   class DMZ_KERNEL_LINK_SYMBOL Time {

      public:
         Time (RuntimeContext *context);
         Time (const PluginInfo &Info);
         ~Time ();

         void set_frame_time (const Float64 Value);
         Float64 get_frame_time () const;
         Float64 get_last_frame_time () const;
         Float64 get_frame_delta () const;

         void set_time_factor (const Float64 Value);
         Float64 get_time_factor () const;

         void set_target_frame_frequency (const Float64 Value);
         Float64 get_target_frame_frequency () const;

      protected:
         RuntimeContextTime *_context; //!< Time context pointer.

      private:
         Time ();
         Time (const Time &);
         Time &operator= (const Time &);
   };
};

#endif // DMZ_RUNTIME_TIME_DOT_H
