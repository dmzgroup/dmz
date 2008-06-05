/*!

\class dmz::RenderModulePick
\ingroup Render
\brief Converts screen coordinates into world position from either a 2D or 3D source.
\details The module can convert to and from screen coordinates from a 2D or 3D source.
In order for the module to convert the coordinate, the source needs to have registered
the appropriate pick interface with the module. The pick module will also return
the Handle of the object at the given screen coordinates if an object is present.

\fn dmz::RenderModulePick *dmz::RenderModulePick::cast (const Plugin *PluginPtr)A
\brief Cast Plugin pointer to a RenderModulePick.
\param[in] PluginPtr Ponter to the Plugin to cast.
\param[in] PluginName String containing the name of the desired RenderModulePick
\return Returns pointer to the RenderModulePick. Returns NULL if the PluginPtr does not
implement the RenderModulePick interface or the \a PluginName is not empty
and not equal to the Plugin's name.
        
\fn dmz::RenderModulePick::RenderModulePick (const PluginInfo &Info);
\brief Constructor.
\para[in] Info Reference to the PluginInfo.

\fn dmz::RenderModulePick::~RenderModulePick ();
\brief Destructor
         
\fn dmz::Boolean dmz::RenderModulePick::register_pick_2d (const Handle Source, RenderPick2d &pick)

\fn dmz::Boolean dmz::RenderModulePick::release_pick_2d (const Handle Source, RenderPick2d &pick)
         
\fn dmz::Boolean dmz::RenderModulePick::screen_to_world (
const Handle Source,
const Int32 ScreenPosX,
const Int32 ScreenPosY,
Vector &worldPosition,
Handle &objectHandle)
         
\fn dmz::Boolean dmz::RenderModulePick::world_to_screen (
const Handle Source,
const Vector &WorldPosition,
Int32 &screenPosX,
Int32 &screenPosY)
         
*/
