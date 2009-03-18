/*!

\class dmz::RenderPick
\ingroup Render
\brief Render Pick Interface.

\fn dmz::RenderPick *dmz::RenderPick::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Cast Plugin pointer to a RenderPick.
\param[in] PluginPtr Ponter to the Plugin to cast.
\param[in] PluginName String containing the name of the desired RenderPick
\return Returns pointer to the RenderPick. Returns NULL if the PluginPtr does not
implement the RenderPick interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Boolean dmz::RenderPick::is_valid (const Handle PickHandle, RuntimeContext *context)
\brief Tests if runtime handle belongs to a valid pick interface.
\param[in] PickHandle Unique runtime handle.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the runtime handle is valid and the Plugin supports the
pick interface.


\fn dmz::RenderPick::RenderPick (const PluginInfo &Info)
\brief Constructor.

\fn dmz::RenderPick::~RenderPick ()
\brief Destructor.

\fn dmz::String dmz::RenderPick::get_pick_name () const
\brief Gets pick interface's name.
\return Returns as String containing the pick interfaces name.

\fn dmz::Handle dmz::RenderPick::get_pick_handle () const
\brief Gets pick interface's runtime handle.
\return Returns unique runtime handle.

\fn void dmz::RenderPick::store_render_module_pick (
const String &Name,
RenderModulePick &module)
\brief Stores RenderModulePick with pick object.
\param[in] Name String containing the name of the RenderModulePick.
\param[in] module RenderModulePick reference.

\fn void dmz::RenderPick::remove_render_module_pick (
const String &Name,
RenderModulePick &module)
\brief Removes RenderModulePick from pick object.
\param[in] Name String containing the name of the RenderModulePick.
\param[in] module RenderModulePick reference.

\fn dmz::Boolean dmz::RenderPick::screen_to_world (
const Int32 ScreenPosX,
const Int32 ScreenPosY,
Vector &worldPosition,
Vector &normal,
Handle &objectHandle)
\brief Converts screen coordinates to world coordinates.
\note The screen coordinates cover the entire display device and not just the window.
\param[in] ScreenPosX The X position of the screen coordinate.
\param[in] ScreenPosY The Y position of the screen coordinate.
\param[out] worldPosition Vector in which the calculated world coordinates will be stored.
\param[out] normal Vector in which the calculated world normal will be stored.
\param[out] objectHandle Handle in which the object handle of any object that may have
been clicked on in the scene. It will be zero if an object was not clicked on.
\return Returns dmz::True if the screen coordinates can be converted
into world coordinates.

\fn dmz::Boolean dmz::RenderPick::world_to_screen (
const Vector &WorldPosition,
Int32 &screenPosX,
Int32 &screenPosY)
\brief Converts world coordinates to screen coordinates.
\note The screen coordinates cover the entire display device and not just the window.
\param[in] WorldPosition World position to be converted to screen coordinates.
\param[out] screenPosX The X position of the returned screen coordinate.
\param[out] screenPosY The Y position of the returned screen coordinate.
\return Returns dmz::True if the world coordinates can be
converted to screen coordinates.

\fn dmz::Boolean dmz::RenderPick::source_to_world (
const Int32 SourcePosX,
const Int32 SourcePosY,
Vector &worldPosition,
Vector &normal,
Handle &objectHandle)
\brief Converts source coordinates to world coordinates.
\note The source coordinates cover the display window and not the entire display device.
\param[in] SourcePosX The X position of the source coordinate.
\param[in] SourcePosY The Y position of the source coordinate.
\param[out] worldPosition Vector in which the calculated world coordinates will be stored.
\param[out] normal Vector in which the calculated world normal will be stored.
\param[out] objectHandle Handle in which the object handle of any object that may have
been clicked on in the scene. It will be zero if an object was not clicked on.
\return Returns dmz::True if the coordinates can be converted
into world coordinates.

\fn dmz::Boolean dmz::RenderPick::world_to_source (
const Vector &WorldPosition,
Int32 &sourcePosX,
Int32 &sourcePosY)
\brief Converts world coordinates to source coordinates.
\note The source coordinates cover the display window and not the entire display device.
\param[in] WorldPosition World position to be converted to source coordinates.
\param[out] sourcePosX The X position of the returned source coordinate.
\param[out] sourcePosY The Y position of the returned source coordinate.
\return Returns dmz::True if the world coordinates can be
converted to source coordinates.

*/
