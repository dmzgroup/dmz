/*!

\class dmz::RenderModulePick
\ingroup Render
\brief Converts screen coordinates into world position from either a 2D or 3D source.
\details The module can convert to and from screen coordinates from a 2D or 3D source.
In order for the module to convert the coordinate, the source needs to have registered
the appropriate pick interface with the module. The pick module will also return
the Handle of the object at the given screen coordinates if an object is present.

\fn dmz::RenderModulePick *dmz::RenderModulePick::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Cast Plugin pointer to a RenderModulePick.
\param[in] PluginPtr Ponter to the Plugin to cast.
\param[in] PluginName String containing the name of the desired RenderModulePick
\return Returns pointer to the RenderModulePick. Returns NULL if the PluginPtr does not
implement the RenderModulePick interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::RenderModulePick::RenderModulePick (const PluginInfo &Info);
\brief Constructor.
\param[in] Info Reference to the PluginInfo.

\fn dmz::RenderModulePick::~RenderModulePick ();
\brief Destructor

\fn dmz::Boolean dmz::RenderModulePick::register_pick (
const Handle Source,
RenderPick &pick)
\brief Registers a pick source with the module.
\param[in] Source Unique runtime handle of the source.
\param[in] pick Reference to the pick interface.
\return Returns dmz::True if the pick interface was successfully registered.

\fn dmz::Boolean dmz::RenderModulePick::release_pick (
const Handle Source,
RenderPick &pick)
\brief Releases a pick source from the module.
\param[in] Source Unique runtime handle of the source.
\param[in] pick Reference to the pick interface to release.
\return Returns dmz::True if the pick interface was successfully released.

\fn dmz::Boolean dmz::RenderModulePick::screen_to_world (
const Handle Source,
const Int32 ScreenPosX,
const Int32 ScreenPosY,
Vector &worldPosition,
Vector &normal,
Handle &objectHandle)
\brief Converts screen coordinates to world coordinates.
\note The screen coordinates cover the entire display device and not just the window.
\param[in] Source Handle of the source to use in the conversion.
\param[in] ScreenPosX The X position of the screen coordinate.
\param[in] ScreenPosY The Y position of the screen coordinate.
\param[out] worldPosition Vector in which the calculated world coordinates will be stored.
\param[out] normal Vector in which the calculated world normal will be stored.
\param[out] objectHandle Handle in which the object handle of any object that may have
been clicked on in the scene. It will be zero if an object was not clicked on.
\return Returns dmz::True if the source is known and screen coordinates can be converted
into world coordinates.

\fn dmz::Boolean dmz::RenderModulePick::world_to_screen (
const Handle Source,
const Vector &WorldPosition,
Int32 &screenPosX,
Int32 &screenPosY)
\brief Converts world coordinates to screen coordinates.
\note The screen coordinates cover the entire display device and not just the window.
\param[in] Source Handle of the source to use in the conversion.
\param[in] WorldPosition World position to be converted to screen coordinates.
\param[out] screenPosX The X position of the returned screen coordinate.
\param[out] screenPosY The Y position of the returned screen coordinate.
\return Returns dmz::True if the source is known and the world coordinates can be
converted to screen coordinates.

\fn dmz::Boolean dmz::RenderModulePick::source_to_world (
const Handle Source,
const Int32 ScreenPosX,
const Int32 ScreenPosY,
Vector &worldPosition,
Vector &normal,
Handle &objectHandle)
\brief Converts source coordinates to world coordinates.
\note The source coordinates cover the display window and not the entire display device.
\param[in] Source Handle of the source to use in the conversion.
\param[in] ScreenPosX The X position of the source coordinate.
\param[in] ScreenPosY The Y position of the source coordinate.
\param[out] worldPosition Vector in which the calculated world coordinates will be stored.
\param[out] normal Vector in which the calculated world normal will be stored.
\param[out] objectHandle Handle in which the object handle of any object that may have
been clicked on in the scene. It will be zero if an object was not clicked on.
\return Returns dmz::True if the source is known and the source coordinates can be
converted into world coordinates.

\fn dmz::Boolean dmz::RenderModulePick::world_to_source (
const Handle Source,
const Vector &WorldPosition,
Int32 &screenPosX,
Int32 &screenPosY)
\brief Converts world coordinates to source coordinates.
\note The source coordinates cover the display window and not the entire display device.
\param[in] Source Handle of the source to use in the conversion.
\param[in] WorldPosition World position to be converted to source coordinates.
\param[out] screenPosX The X position of the returned source coordinate.
\param[out] screenPosY The Y position of the returned source coordinate.
\return Returns dmz::True if the source is known and the world coordinates can be
converted to source coordinates.

*/
