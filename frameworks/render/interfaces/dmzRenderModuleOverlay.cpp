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

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_named_color (const String &Name, Float64 &red, Float64 &green, Float64 &blue, Float64 &alpha)
\brief Looks up named color.
\details. All color components are in the range 0.0 to 1.0.
\param[in] Name String containing color name.
\param[out] red Float64 containing the red component.
\param[out] green Float64 containing the green component.
\param[out] blue Float64 containing the blue component.
\param[out] alpha Float64 containing the alpha component.
\return Returns dmz::True if the named color was found.

\fn dmz::Handle dmz::RenderModuleOverlay::lookup_node_handle (const String &Name)
\brief Looks up the handle of a named overlay node.
\param[in] Name String containing the name of the node.
\return Returns the Handle of the specified node. Returns zero if a node with the given
name is not found.

\fn dmz::Handle dmz::RenderModuleOverlay::lookup_node_clone_sub_handle (
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

\fn dmz::String dmz::RenderModuleOverlay::lookup_node_name (const Handle Overlay)
\brief Looks up the name of the specified node.
\param[in] Overlay Handle of the overlay node.
\return Returns a String containing the name of the node. Returns an empty string if
the name of the node can not be found.

\fn dmz::RenderOverlayTypeEnum dmz::RenderModuleOverlay::lookup_node_type (const Handle Overlay)
\brief Looks up the type of the specified node.
\param[in] Overlay Handle of the overlay node.
\return Returns the enumeration representing the node type.

\fn dmz::Boolean dmz::RenderModuleOverlay::is_of_node_type (
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

\fn dmz::Boolean dmz::RenderModuleOverlay::destroy_node (const Handle Overlay)
\brief Destroys the node and all children.
\param[in] Overlay Handle of the node to destroy.
\return Returns dmz::True if the node was destroyed.

\fn dmz:: Boolean dmz::RenderModuleOverlay::store_color (const Handle Overlay, const Float64 Red, const Float64 Green, const Float64 Blue, const Float64 Alpha)
\brief Sets the color recursively starting at the specified node.
\param[in] Overlay Handle of the node.
\param[in] Red Float64 containing the red color component.
\param[in] Green Float64 containing the green color component.
\param[in] Blue Float64 containing the blue color component.
\param[in] Alpha Float64 containing the alpha color component.
\return Returns dmz:True if the color was successfully set.

\fn dmz::Boolean dmz::RenderModuleOverlay::store_text (const Handle Overlay, const String &Value)
\brief Sets the text value of the text node.
\param[in] Overlay Handle of text node
\param[in] Value String containing the new text value.
\return Returns dmz::True if the text value was successfully set.

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_text (const Handle Overlay, String &value)
\brief Gets the text value of the text node.
\param[in] Overlay Handle of text node
\param[out] value String used to return the current text value.
\return Returns dmz::True if the text value was successfully found.

\fn dmz::Boolean dmz::RenderModuleOverlay::add_group_child (
const Handle Parent,
const Handle Child)
\brief Adds a child node to a group node.
\param[in] Parent Handle of the parent group node.
\param[in] Child Handle of the child node.
\return Returns True if the child node was successfully added to the parent group node.

\fn dmz::Boolean dmz::RenderModuleOverlay::remove_group_child (
const Handle Parent,
const Handle Child)
\brief removes a child node from a group node.
\param[in] Parent Handle of the parent group node.
\param[in] Child Handle of the child node.
\return Returns True if the child node was successfully remove from the parent group node.

\fn dmz::Int32 dmz::RenderModuleOverlay::lookup_group_child_count (const Handle Overlay)
\brief Returns number of children in a group node.
\param[in] Overlay Handle of group node
\return Returns the number of children contained in the group node.

\fn dmz::Boolean dmz::RenderModuleOverlay::store_switch_state (
const Handle Overlay,
const Int32 Which,
const Boolean SwitchState)
\brief Sets the switch state of a specified child node.
\param[in] Overlay Handle of the switch node.
\param[in] Which Specifies which child to set.
\param[in] SwitchState Boolean indicating the switch state.
\return Returns True if the switch state was successfully set.

\fn dmz::Boolean dmz::RenderModuleOverlay::store_switch_state_all (
const Handle Overlay,
const Boolean SwitchState)
\brief Sets all children of the switch to the same specified state.
\param[in] Overlay Handle of the switch node.
\param[in] SwitchState Boolean indicating the switch state.
\return Returns True if the switch state was successfully set.

\fn dmz::Boolean dmz::RenderModuleOverlay::enable_switch_state_single (
const Handle Overlay,
const Int32 Which)
\brief Enables a single child and disables all others.
\param[in] Overlay Handle of the switch node.
\param[in] Which Specifies which child to enable.
\return Returns True if the  child was successfully enabled.


\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_switch_state (
const Handle Overlay,
const Int32 Which)
\brief Looks up the switch sate of a specified child node.
\param[in] Overlay Handle of the switch node.
\param[in] Which Specifies which child to set.
\return Returns True if the specified child node is enabled.

\fn dmz::Boolean dmz::RenderModuleOverlay::store_transform_position (
const Handle Overlay,
const Float64 ValueX,
const Float64 ValueY)
\brief Sets the position of the transform node.
\param[in] Overlay Handle of the transform node.
\param[in] ValueX Float64 X component of the translation.
\param[in] ValueY Float64 Y component of the translation.
\return Returns True if the transform was successfully updated.

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_transform_position (
const Handle Overlay,
Float64 &valueX,
Float64 &valueY)
\brief Looks up the position of the transform node.
\param[in] Overlay Handle of the transform node.
\param[out] valueX Float64 X component of the translation.
\param[out] valueY Float64 Y component of the translation.
\return Returns True if the transform was successfully read.

\fn dmz::Boolean dmz::RenderModuleOverlay::store_transform_rotation (
const Handle Overlay,
const Float64 Value)
\brief Sets the rotation of the transform node.
\param[in] Overlay Handle of the transform node.
\param[in] Value Float64 rotation value in radians.
\return Returns True if the rotation was successfully set.

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_transform_rotation (
const Handle Overlay,
Float64 &value)
\brief Looks up the rotation of the transform node.
\param[in] Overlay Handle of the transform node.
\param[out] value Float64 rotation value in radians.
\return Returns True if teh transform was successfully read.

\fn dmz::Boolean dmz::RenderModuleOverlay::store_transform_scale (
const Handle Overlay,
const Float64 ValueX,
const Float64 ValueY)
\brief Sets the scale of the transform node.
\param[in] Overlay Handle of the transform node.
\param[in] ValueX Float64 X component of the scale.
\param[in] ValueY Float64 Y component of the scale.
\return Returns True if the transform was successfully updated.

\fn dmz::Boolean dmz::RenderModuleOverlay::lookup_transform_scale (
const Handle Overlay,
Float64 &valueX,
Float64 &valueY)
\brief Looks up the scale of the transform node.
\param[in] Overlay Handle of the transform node.
\param[out] valueX Float64 X component of the scale.
\param[out] valueY Float64 Y component of the scale.
\return Returns True if the transform was successfully read.

*/
