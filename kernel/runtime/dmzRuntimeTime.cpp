#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextThreadKey.h"
#include "dmzRuntimeContextTime.h"
#include <dmzRuntimeHandle.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesString.h>

/*!

\file dmzRuntimeTime.h
\ingroup Runtime
\brief Contains time related classes.


\class dmz::Time
\ingroup Runtime
\brief Provides interface to the runtime time facilities.
\details The Time class provides an interface for manipulating the runtime time.
Time in the runtime is decoupled from system time. It may be set to run in real time,
faster than real time, slower than real time, and even backwards. The current runtime
time may be set as well.

*/

/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::Time::Time (RuntimeContext *context) : _context (0) {

   if (context) {

      _context = context->get_time_context ();

      if (_context) { _context->ref (); }
   }
}


/*!

\brief Constructor.
\param[in] Info PluginInfo that provides the runtime context.

*/
dmz::Time::Time (const PluginInfo &Info) : _context (0) {

   RuntimeContext *context (Info.get_context ());

   if (context) {

      _context = context->get_time_context ();

      if (_context) { _context->ref (); }
   }
}


//! Destructor.
dmz::Time::~Time () { if (_context) { _context->unref (); _context = 0; } }


/*!

\brief Sets current runtime time.
\param[in] Value New value of runtime time in seconds.

*/
void
dmz::Time::set_frame_time (const Float64 Value) {

   if (_context) { _context->set_current_time (Value); }
}


/*!

\brief Gets current frame time.
\details The frame time is calculated once per frame. A frame is a time step in the
simulation and may be thought of as a single pass through the simulations while loop.
\return Returns current runtime frame time in seconds.

*/
dmz::Float64
dmz::Time::get_frame_time () const {

   Float64 result (0.0);

   if (_context) { result = _context->currentTime; }

   return result;
}


/*!

\brief Gets last frame time.
\return Returns frame time from previous frame in seconds.
\sa dmz::Time::get_frame_time

*/
dmz::Float64
dmz::Time::get_last_frame_time () const {

   Float64 result (0.0);

   if (_context) { result = _context->previousTime; }

   return result;
}


/*!

\brief Gets delta time for current time frame.
\details Returns the number of seconds that have elapsed since the last time frame.
\return Returns delta time for current fame in seconds.

*/
dmz::Float64
dmz::Time::get_frame_delta () const {

   Float64 result (0.0);

   if (_context) { result = _context->deltaTime; }

   return result;
}


/*!

\brief Sets time factor.
\details \a Value may be either negative or positive. A time factor of 2.0 would
cause the runtime time to increment at double that of real time. A time factor of
-3.0 would cause the runtime time to decrement at three times that of real time. 
\param[in] Value Time factor in seconds.

*/
void
dmz::Time::set_time_factor (const Float64 Value) {

   if (_context) { _context->set_time_factor (Value); }
}


/*!

\brief Returns current time factor.
\return Returns current time factor in seconds.
\sa dmz::Time::set_time_factor.

*/
dmz::Float64
dmz::Time::get_time_factor () const {

   Float64 result (0.0);

   if (_context) { result = _context->timeFactor; }

   return result;
}


/*!

\brief Sets target frame rate.
\details This function tells the runtime how many frames per second it should try and
generate.  This is only best effort so if any component in the system uses more time
than is alloted for a time frame, the system will run slower and have fewer frames than
were requested. A \a Value of 1.0 would create a new frame every second while a
\a Value of 60.0 would try to create 60 frames pre second. If \a Value is less than
or equal to zero, the runtime will free run and try to create as many time frames
as possible per second.
\param[in] Value Number of frames per second.

*/
void
dmz::Time::set_target_frame_frequency (const Float64 Value) {

   if (_context) { _context->set_target_frequency (Value); }
}


/*!

\brief Gets current target frame frequency.
\return Returns current target frame frequency as number of frames per second.
\sa dmz::Time::set_target_frame_frequency.

*/
dmz::Float64
dmz::Time::get_target_frame_frequency () const {

   Float64 result (0.0);

   if (_context) { result = _context->targetFrequency; }

   return result;
}

