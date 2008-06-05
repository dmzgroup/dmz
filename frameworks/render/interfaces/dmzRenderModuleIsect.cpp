/*!

\defgroup Render DMZ Render Framework

\class dmz::RenderModuleIsect
\ingroup Render
\brief Render intersection testing interface.
\details The isect module provides an interface for performing intersection testing
on the scene.

\fn dmz::RenderModuleIsect::RenderModuleIsect (const PluginInfo &Info)
\brief Constructor.

\fn dmz::RenderModuleIsect::~RenderModuleIsect ()
\brief Destructor.

\fn dmz::RenderModuleIsect *dmz::RenderModuleIsect::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an RenderModuleIsect.
\details If the Plugin object implements the RenderModuleIsect interface, a pointer to
the RenderModuleIsect interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired RenderModuleIsect
\return Returns pointer to the RenderModuleIsect. Return NULL if the PluginPtr does not
implement the RenderModuleIsect interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Boolean dmz::RenderModuleIsect::do_isect (
const IsectParameters &Parameters,
const IsectTestContainer &TestValues,
IsectResultContainer &result)
\brief Performs intersection test.
\details Performs ray and segment based intersection test with the scene's geometry.
\param[in] Parameters Intersection test parameters.
\param[in] TestValues Container with the intersection tests to perform.
\param[out] result Contains results from the intersection tests.
\return Returns dmz::True if any of the tests resulted in an intersection.

\fn dmz::UInt32 dmz::RenderModuleIsect::enable_isect (const Handle ObjectHandle)
\brief Enables intersection for an object in the scene.A
\param[in] ObjectHandle Handle of object in scene to enable intersection testing for
its geometry.
\return Returns a count for the number of times the geometry has had the
intersection test enabled.

\fn dmz::UInt32 dmz::RenderModuleIsect::disable_isect (const Handle ObjectHandle)
\brief Disable intersection for an object in the scene.
\param[in] ObjectHandle Handle of object in scene to disable intersection testing for
its geometry.
\return Returns a count for the number of times the geometry has had the
intersection test disabled.

*/
