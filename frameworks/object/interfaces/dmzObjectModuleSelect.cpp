/*!

\class dmz::ObjectModuleSelect
\ingroup Object
\brief Object selection module interface.
\details Provides an interface for selecting and unselecting objects in the
dmz::ObjectModule. When an object is selected, the object attribute flag specified by
dmz::ObjectAttributeSelectName is set to dmz::True. This attribute may be removed or
set to dmz::False when the object is not selected.

\fn dmz::ObjectModuleSelect::ObjectModuleSelect (const PluginInfo &Info);
\brief Constructor.
\param[in] Info  PluginInfo containing initialization data.

\fn dmz::ObjectModuleSelect::~ObjectModuleSelect ()
\brief Destructor.

\fn dmz::ObjectModuleSelect *dmz::ObjectModuleSelect::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an ObjectModuleSelect.
\details If the Plugin object implements the ObjectModuleSelect interface, a pointer
to the ObjectModuleSelect interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired ObjectModuleSelect.
\return Returns pointer to the ObjectModuleSelect. Return NULL if the PluginPtr does not
implement the ObjectModuleSelect interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn void dmz::ObjectModuleSelect::get_selected_objects (HandleContainer &container)
\brief Creates a list of all selected objects.
\param[out] container HandleContainer used to store all selected object handles.

\fn dmz::Boolean dmz::ObjectModuleSelect::is_selected (const Handle ObjectHandle)
\brief Determines if an object is selected.
\param[in] ObjectHandle Handle of object to test for selection.
\return Returns dmz::True if the object is currently selected. Return dmz::False if
the object is not currently selected.

\fn dmz::Boolean dmz::ObjectModuleSelect::select_object (
const Handle ObjectHandle,
const ObjectSelectModeEnum Mode)
\brief Selects an object.
\param[in] ObjectHandle Handle of the object to select.
\param[in] Mode ObjectSelectinoModeEnum specifies if this is a new selection of if the
object should be added to the current list of selected object. If an object is
a new selection, all currently selected objects will be unselected first. If an object
is added to the selection, all existing selections are retained.
\return Returns dmz::True if the object was selected.

\fn dmz::Boolean dmz::ObjectModuleSelect::unselect_object (const Handle ObjectHandle)
\brief Unselects an object.
\param ObjectHandle Handle of object to unselect.
\return Returns dmz::True if the object was unselected.

\fn void dmz::ObjectModuleSelect::unselect_all_objects ()
\brief Unselects all currently selected objects.

*/
