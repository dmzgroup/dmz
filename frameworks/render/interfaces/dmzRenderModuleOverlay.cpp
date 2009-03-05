/*!

\class dmz::RenderModuleOverlay
\ingroup Render
\brief Render overlay interface.
\details The overlay module provides an interface for manipulating 2D overlay objects
rendered on the 3D window. 
\verbatim
node
  |
  +-group
      |
      +- switch
      |
      +- transform
\endverbatim

\fn dmz::RenderModuleOverlay *dmz::RenderModuleOverlay::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an RenderModuleOverlay.
\details If the Plugin object implements the RenderModuleOverlay interface, a pointer to
the RenderModuleOverlay interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired RenderModuleOverlay
\return Returns pointer to the RenderModuleOverlay. Returns NULL if the PluginPtr does not
implement the RenderModuleOverlay interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::RenderModuleOverlay::RenderModuleOverlay (const PluginInfo &Info)
\brief Constructor

\fn dmz::RenderModuleOverlay::~RenderModuleOverlay ()
\brief Destructor.

\fn dmz::String dmz::RenderModuleOverlay::get_render_module_overlay_name () const
\brief Gets the render overlay module's name.

\fn dmz::Handle dmz::RenderModuleOverlay::get_render_module_overlay_handle () const
\brief Gets the render overlay module's handle.

\fn dmz::Handle dmz::RenderModuleOverlay::lookup_overlay_handle (const String &Name)
\brief Looks up the handle of a named overlay node.
\param[in] Name String containing the name of the node.
\return Returns the Handle of the specified node. Returns zero if a node with the given
name is not found.

\fn dmz::Handle dmz::RenderModuleOverlay::lookup_overlay_clone_sub_handle (
const Handle CloneHandle,
const String &Name)
\brief Looks up the Handle of to a sub node of a cloned overlay.
\details Since all cloned overlay nodes will have the same name as the template used
to clone, it is necessary to indicate the root node of the clone in order to find sub
nodes with a given name.
\param[in] CloneHandle Handle of the cloned overlay.
\param[in] Name String containing the name of the sub node.
\return Returns the Handle of the specified node. Returns zero if a node with the
given name can not be found that is a child of the root of the cloned overlay.

\fn dmz::String dmz::RenderModuleOverlay::lookup_overlay_name (const Handle Overlay)
\brief Looks up the name of the specified node.
\param[in] Overlay Handle of the overlay node.
\return Returns a String containing the name of the node. Returns an empty string if
the name of the node can not be found.

\fn dmz::RenderOverlayTypeEnum dmz::RenderModuleOverlay::lookup_overlay_type (const Handle Overlay)
\brief Looks up the type of the specified node.
\param[in] Overlay Handle of the overlay node.
\return Returns the enumeration representing the node type.

\fn dmz::Boolean dmz::RenderModuleOverlay::is_of_overlay_type (
const Handle Overlay,
const RenderOverlayTypeEnum Type)
\brief Tests if a node is of a given type.
\param[in] Overlay Handle of the overlay node.
\param[in] Type Node type to test node against.
\return Returns True if the node is of the given type. Returns False if it is not.

\fn dmz::Handle dmz::RenderModuleOverlay::clone_template (const String &Name)
\brief Clones a template with the given name.
\param[in] Name String containing the name of the template to clone.
\return Returns the Handle of the cloned overlay. Returns zero if the clone fails.

\fn dmz::Boolean dmz::RenderModuleOverlay::add_group_child (const Handle Parent, const Handle Child)
\fn dmz::Boolean dmz::RenderModuleOverlay::remove_group_child (const Handle Parent, const Handle Child)

\fn dmz::Int32 dmz::RenderModuleOverlay::lookup_group_child_count (const Handle Overlay)

\fn dmz::Boolean dmz::RenderModuleOverlay::store_switch_state (
const Handle Overlay,
const Int32 Which,
const Boolean SwitchState)

\fn dmz::Boolean dmz::RenderModuleOverlay::store_all_switch_state (
const Handle Overlay,
const Boolean SwitchState)

\fn dmz::Boolean dmz::RenderModuleOverlay::enable_single_switch_state (
const Handle Overlay,
const Int32 Which)

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_switch_state (
const Handle Overlay,
const Int32 Which)


\fn dmz::Boolean dmz::RenderModuleOverlay::store_transform_position (
const Handle Overlay,
const Float64 ValueX,
const Float64 ValueY)

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_transform_position (
const Handle Overlay,
Float64 &valueX,
Float64 &valueY)

\fn dmz::Boolean dmz::RenderModuleOverlay::store_transform_rotation (
const Handle Overlay,
const Float64 Value)

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_transform_rotation (
const Handle Overlay,
Float64 &value)

\fn dmz::Boolean dmz::RenderModuleOverlay::store_transform_scale (
const Handle Overlay,
const Float64 ValueX,
const Float64 ValueY)

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_transform_scale (
const Handle Overlay,
Float64 &valueX,
Float64 &valueY)

*/
