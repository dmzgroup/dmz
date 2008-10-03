return {

t = "DMZ Lua API Reference Guide",

{

t = "dmz.config",
b = [[
The dmz.config class is a wrapper for the dmz::Config class.
]],

{
t = "dmz.config.get_global",
p = {},
b = [[
Returns a dmz.config object containing the global config data.
]],
},{
t = "dmz.config.new",
p = {},
o = {"name",},
b = [[
Returns a new dmz.config object. The name is used to set the new dmz.config
objects tag.
]],
},{
t = "dmz.config.is_a",
p = {"value",},
b = [[
Returns a dmz.config if value is a dmz.config object. Returns nil if it is not.
]],
},{
t = "dmz.config:get_name",
p = {},
b = [[
Returns the name of the dmz.config object. Returns nil if the dmz.config object is empty.
]],
},{
t = "dmz.config:get_attributes",
p = {},
b = [[
Returns a table containing the attributes stored in the dmz.config object.
Returns nil if the dmz.config object is empty.
]],
},{
t = "dmz.config:store_attribute",
p = {"name", "value",},
b = [[
Store an attribute of name with value in the dmz.config object.
Returns true if the attribute was successfully stored. ]],
},{
t = "dmz.config:lookup_attribute",
p = {"name",},
b = [[
Returns the value of the attribute name. Returns nil if
the dmz.config object does not contain an attribute with the given name.
]],
},{
t = "dmz.config:lookup_boolean",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a boolean.
If the attribute name is not found, then default is returned.
]],
},{
t = "dmz.config:lookup_string",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a string.
If the attribute name is not found, then default is returned.
]],
-- [string]
},{
t = "dmz.config:lookup_number",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a number.
If the attribute name is not found, then default is returned.
]],
-- [number]
},{
t = "dmz.config:lookup_handle",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.handle.
If the attribute name is not found, then default is returned.
]],
},{
t = "dmz.config:lookup_matrix",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.matrix.
If the attribute name is not found, then default is returned.
]],
-- [dmz.matrix]
},{
t = "dmz.config:lookup_event_type",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.event_type.
If the attribute name is not found, then default is returned. Returns nil if
a dmz.event_type can not be found.
]],
},{
t = "dmz.config:lookup_message",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.message.
If the attribute name is not found, then default is returned. Returns nil if
a dmz.message can not be found.
]],
},{
t = "dmz.config:lookup_object_type",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.object_type.
If the attribute name is not found, then default is returned. Returns nil if
a dmz.object_type can not be found.
]],
-- [dmz.object_type | nil]
},{
t = "dmz.config:lookup_state",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.mask.
If the attribute name is not found, then default is returned.
]],
-- [dmz.mask]
},{
t = "dmz.config:lookup_uuid",
p = {"name",},
o = {"default",},
b = [[
]],
--  [dmz.uuid]
},{
t = "dmz.config:lookup_vector",
p = {"name",},
o = {"default",},
b = [[
]],
-- [dmz.vector]
},{
t = "dmz.config:set_value",
p = {"value",},
b = [[
]],
-- [boolean]
},{
t = "dmz.config:append_value",
p = {"value",},
b = [[
]],
-- [boolean]
},{
t = "dmz.config:get_value",
p = {},
b = [[
]],
-- [string | nil]
},{
t = "dmz.config:get_children",
p = {},
b = [[
]],
-- [{dmz.config children}]
},{
t = "dmz.config:add_config",
p = {"scope", "dmz.config | {dmz.config})",},
b = [[
]],
--[boolean]
},{
t = "dmz.config:lookup_config",
p = {"scope",},
b = [[
]],
-- [dmz.config | nil]
},{
t = "dmz.config:lookup_all_config",
p = {"scope",},
b = [[
]],
-- [{dmz.config} | nil]
},{
t = "dmz.config:lookup_all_config_merged",
p = {"scope",},
b = [[
]],
-- [dmz.config | nil]
},
--[[
dmz.data.new ([dmz.data]) [dmz.data]
dmz.data.is_a (value) [dmz.data | nil]

dmz.data:get_attributes () [{dmz.handle}]
dmz.data:store_string (dmz.handle attribute, int offset, string value)
dmz.data:lookup_string (dmz.handle attribute, int offset) [string | nil]
dmz.data:store_number (dmz.handle attribute, int offset, number value)
dmz.data:lookup_number (dmz.handle attribute, int offset) [number | nil]
dmz.data:store_handle (dmz.handle attribute, int offset, dmz.handle value)
dmz.data:lookup_handle (dmz.handle attribute, int offset) [dmz.handle | nil]
dmz.data:store_vector (dmz.handle attribute, int offset, dmz.vector value)
dmz.data:lookup_vector (dmz.handle attribute, int offset) [dmz.vector | nil]


dmz.event_type.new ([dmz.event_type]) [dmz.event_type]
dmz.event_type.is_a (value) [dmz.event_type | nil]

dmz.event_type:is_of_type (dmz.event_type event) [boolean]
dmz.event_type:get_name () [string]
dmz.event_type:get_handle () [dmz.handle | nil]
dmz.event_type:get_parent () [dmz.event_type]
dmz.event_type:become_parent () [boolean]
dmz.event_type:get_children () [{dmz.event_type} | nil]


dmz.handle.new ([string name | dmz.handle value]) [dmz.handle | nil]
dmz.handle.is_a (value) [dmz.handle | nil]


dmz.input.ChannelState
dmz.input.Axis
dmz.input.Button
dmz.input.Switch
dmz.input.Key
dmz.input.Mouse
dmz.input.Data
dmz.input.All

dmz.input.has_buttons_changed (table buttonEvent) [boolean]
dmz.input.is_button_changed (number buttonNumber, table buttonEvent) [boolean]
dmz.input.is_button_pressed (number buttonNumber, table buttonEvent) [boolean]

dmz.input_observer.new () [dmz.input_observer]
dmz.input_observer.is_a (value) [dmz.input_observer | nil]

dmz.input_observer:init_channels (dmz.config config, dmz.mask mask, function callback, table self)
dmz.input_observer:register_channel (string channelName, dmz.mask mask, function callback, table self)
dmz.input_observer:register_default_channel (dmz.mask, function callback, table self)
dmz.input_observer:release_channel (string channelName)
dmz.input_observer:release_default_channel ()
dmz.input_observer:release_all ()

dmz.input_observer.callback (self, {
[axis = {
   dmz.handle source,
   integer which,
   number value,
   number previous_value,
   number delta,
}],
[button = {
   dmz.handle source,
   integer which,
   boolean value,
   boolean previous_value,
   boolean changed,
}],
[switch = {
   dmz.handle source,
   integer which,
   integer value,
   integer previous_value,
   boolean changed,
}],
[key = {
   dmz.handle source,
   integer value,
   boolean state,
}],
[mouse = {
   dmz.handle source,
   integer window_size_x, 
   integer window_size_y, 
   integer x,
   integer y,
   integer previous_x,
   integer previous_y,
   integer delta_x,
   integer delta_y,
   integer scroll_delta_x,
   integer scroll_delta_y,
   dmz.mask buttons,
   dmz.mask previous_buttons,
}],
})


dmz.isect.RayTest -- Test type
dmz.isect.SegmentTest -- Test type
dmz.isect.FirstPoint -- Param type
dmz.isect.ClosestPoint -- Param type
dmz.isect.AllPoints -- Param type
dmz.isect.BackCull -- Cull mask
dmz.isect.FrontCull -- Cull mask
dmz.isect.Invisible -- Cull mask
dmz.isect.clamp (dmz.vector point) [(dmz.vector point, dmz.vector normal) | nil]
dmz.isect.enable_isect (dmz.handle handle) [boolean]
dmz.isect.disable_isect (dmz.handle handle) [boolean]

dmz.isect.do_isect (
{ integer type, dmz.vector start, dmz.vector vector },
[{
   [integer type],
   [boolean normal],
   [boolean object_handle],
   [boolean distance],
   [boolean cull]
}]
)
[{
   {
      integer type,
      [dmz.vector point],
      [dmz.vector normal],
      [dmz.handle object],
      [number distance],
      [dmz.mask cull],
   }
}]

dmz.isect.do_batch_isect (
{
   { integer type, dmz.vector start, dmz.vector vector }
},
[{
   [integer type],
   [boolean normal],
   [boolean object_handle],
   [boolean distance],
   [boolean cull]
}]
)
[{
   {
      integer type,
      [dmz.vector point],
      [dmz.vector normal],
      [dmz.handle object],
      [number distance],
      [dmz.mask cull],
   }
}]


dmz.log.new ([string name]) [dmz.log]
dmz.log.is_a (value) [dmz.log | nil]

dmz.log:out (...)
dmz.log:error (...)
dmz.log:warn (...)
dmz.log:info (...)
dmz.log:debug (...)


dmz.mask.new ([dmz.mask value]) [dmz.mask]
dmz.mask.is_a (value) [dmz.mask | nil]

dmz.mask:clear ()
dmz.mask:contains (dmz.mask value) [boolean]
dmz.mask:unset (dmz.mask value) [boolean]
dmz.mask:bit_and (dmz.mask value) [dmz.mask]
dmz.mask:bit_or (dmz.mask value) [dmz.mask]
dmz.mask:bit_xor (dmz.mask value) [dmz.mask]
dmz.mask:bit_set (int value) [dmz.mask]
dmz.mask:bit_unset (int value) [dmz.mask]
dmz.mask:bit_not () [dmz.mask]
dmz.mask:bit_shift (int shift) [dmz.mask]


dmz.math.is_zero (number value) [boolean]
dmz.math.to_degrees (number value) [number]
dmz.math.to_radians (number value) [number]
dmz.math.up () [dmz.vector]
dmz.math.down () [dmz.vector]
dmz.math.right () [dmz.vector]
dmz.math.left () [dmz.vector]
dmz.math.forward () [dmz.vector]
dmz.math.backward () [dmz.vector]


dmz.matrix.new ([dmz.matrix | {number}] | number v1, number v2, number v3, number v4, number v5, number v6, number v7, number v8, number v9) [dmz.matrix]
dmz.matrix.is_a (value) [dmz.matrix | nil]

dmz.matrix:transpose () [dmz.matrix]
dmz.matrix:invert () [dmz.matrix]
dmz.matrix:transform (dmz.vector) [dmz.vector]
dmz.matrix:from_table ({number}) [dmz.matrix]
dmz.matrix:to_table () [{number}]
dmz.matrix:from_two_vectors (dmz.vector from, dmz.vector to) [dmz.matrix]
dmz.matrix:from_axis_and_angle (dmz.vector axis, number angle) [dmz.matrix]
dmz.matrix:is_identity () [boolean]
dmz.matrix:set_identity () [dmz.matrix]


dmz.message_observer.new (string name) [dmz.message_observer]
dmz.message_observer.is_a (value) [dmz.message_observer | nil]

dmz.message_observer:register (dmz.message_type type, function callback, table self) [boolean]
dmz.message_observer:release (dmz.message_type type) [boolean]
dmz.message_observer:release_all () [boolean]


dmz.message_type.new ([string name]) [dmz.message_type]
dmz.message_type.is_a (value) [dmz.message_type | nil]

dmz.message_type:is_of_type (dmz.message_type type) [boolean | nil]
dmz.message_type:get_name () [string | nil]
dmz.message_type:get_handle () [dmz.handle | nil]
dmz.message_type:get_parent () [dmz.messaget_type | nil]
dmz.message_type:become_parent () [boolean | nil]


dmz.object.Local -- Locality
dmz.object.Remote -- Locality
dmz.object.RetainLinks -- Mode
dmz.object.IgnoreLinks -- Mode
dmz.object.SelectNew -- Mode
dmz.object.SelectAdd -- Mode
dmz.object.CreateMask -- dmz.mask
dmz.object.DestroyMask -- dmz.mask
dmz.object.UUIDMask -- dmz.mask
dmz.object.RemoveAtrributeMask -- dmz.mask
dmz.object.LinkMask -- dmz.mask
dmz.object.UnlinkMask -- dmz.mask
dmz.object.LinkAttriuteObjectMaks -- dmz.mask
dmz.object.LocalityMask -- dmz.mask
dmz.object.TypeMask -- dmz.mask
dmz.object.StateMask -- dmz.mask
dmz.object.FlagMask -- dmz.mask
dmz.object.TimeStampMask -- dmz.mask
dmz.object.PositionMask -- dmz.mask
dmz.object.OrientationMask -- dmz.mask
dmz.object.VelocityMask -- dmz.mask
dmz.object.AccelerationMask -- dmz.mask
dmz.object.ScaleMask -- dmz.mask
dmz.object.VectorMask -- dmz.mask
dmz.object.ScalarMask -- dmz.mask
dmz.object.TextMask -- dmz.mask
dmz.object.DataMask -- dmz.mask
dmz.object.AllMask -- dmz.mask
dmz.object.AttributeDefaultName -- string
dmz.object.AttributeLastNetworkValueName -- string
dmz.object.AttributeHumanInTheLoopName -- string
dmz.object.AttributeSelectName -- string
dmz.object.CreateMessage -- dmz.message_type
dmz.object.DestroyMessage -- dmz.message_type
dmz.object.create_last_network_value_name (string name) [string]
dmz.object.get_human_in_the_loop () [dmz.handle | nil]
dmz.object.is_object (dmz.handle) [boolean | nil]
dmz.object.is_link (dmz.handle) [boolean | nil]
dmz.object.get_all () [{dmz.handle} | nil]
dmz.object.create (dmz.object_type type, [integer locality]) [dmz.handle | nil]
dmz.object.activate (dmz.handle handle) [boolean | nil]
dmz.object.destroy (dmz.handle handle) [boolean | nil]
dmz.object.clone (dmz.handle handle, [integer mode]) [dmz.handle | nil]
dmz.object.uuid_to_handle (dmz.uuid uuid) [dmz.handle | nil]
dmz.object.uuid (dmz.handle, [dmz.uuid uuid]) [dmz.uuid | nil]
dmz.object.remove_attribute (dmz.handle handle, dmz.mask mask) [boolean | nil]
dmz.object.link (dmz.handle attr_handle, dmz.handle super_handle, dmz.handle sub_handle) [dmz.handle | nil]
dmz.object.lookup_link_handle (dmz.handle attr_handle, dmz.handle super_handle, dmz.handle sub_handle) [dmz.handle | nil]
dmz.object.lookup_linked_objects (dmz.handle link_handle) [dmz.handle attr_handle, dmz.handle super_handle, dmz.handle sub_handle | nil]
dmz.object.unlink (dmz.handle link_handle) [boolean | nil]
dmz.object.link_attribute_object (dmz.handle link_handle, dmz.handle attr_object_handle) [boolean | nil]
dmz.object.super_links (dmz.handle obj_handle, dmz.handle attr_handle) [{dmz.handle} | nil]
dmz.object.sub_links (dmz.handle obj_handle, dmz.handle attr_handle) [{dmz.handle} | nil]
dmz.object.locality (dmz.handle obj_handle, [integer locality]) [integer | nil]
dmz.object.type (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.object_type type]) [dmz.object_type | nil]
dmz.object.state (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.mask state]) [dmz.mask | nil]
dmz.object.flag (dmz.handle obj_handle, dmz.handle attr_handle, [boolean flag]) [boolean | nil]
dmz.object.time_stamp (dmz.handle obj_handle, dmz.handle attr_handle, [number time_stamp]) [number | nil]
dmz.object.position (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector position]) [dmz.vector | nil]
dmz.object.orientation (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.matrix orientation]) [dmz.matrix | nil]
dmz.object.velocity (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector velocity]) [dmz.vector | nil]
dmz.object.acceleration (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector acceleration]) [dmz.vector | nil]
dmz.object.scale (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector scale]) [dmz.vector | nil]
dmz.object.vector (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector vector]) [dmz.vector | nil]
dmz.object.scalar (dmz.handle obj_handle, dmz.handle attr_handle, [number scalar]) [number | nil]
dmz.object.text (dmz.handle obj_handle, dmz.handle attr_handle, [string value]) [string | nil]
dmz.object.data (dmz.handle obj_handle, dmz.handle attr_handle, [dmz.data data]) [dmz.data | nil]
dmz.object.get_selected () [{dmz.handle}]
dmz.object.is_selected (dmz.handle object) [boolean | nil]
dmz.object.select (dmz.handle object, [int mode]) [boolean | nil]
dmz.object.unselect (dmz.handle object) [boolean | nil]
dmz.object.unselect_all ()

dmz.object_observer.new () [dmz.object_observer]

dmz.object_observer:register (dmz.handle attr_handle, table callbacks, self) [boolean | nil]
dmz.object_observer:release (dmz.handle attr_handle, dmz.mask mask) [boolean | nil]

-- object_observer callback table function names and signatures
callbacks = {
   create_object (self, dmz.handle obj_handle, dmz.object_type type, integer locality),
   destroy_object (self, dmz.handle obj_handle),
   update_object_uuid (self, dmz.handle obj_handle, dmz.uuid uuid, dmz.uuid prev_uuid),
   remove_object_attribute (self, dmz.handle obj_handle, dmz.handle attr_handle, dmz.mask mask),
   link_object (self, dmz.handle link_handle, dmz.handle attr_handle, dmz.handle super_handle, dmz.handle sub_handle),
   unlink_object (self, dmz.handle link_handle, dmz.handle attr_handle, dmz.handle super_handle, dmz.handle sub_handle),
   update_link_object  (self, dmz.handle link_handle, dmz.handle attr_handle, dmz.handle super_handle, dmz.handle sub_handle, dmz.handle attr_obj_handle),
   update_object_locality (self, dmz.handle obj_handle, integer locality, [integer prev_locality]),
   update_object_type (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.type type, [dmz.type prev_type]),
   update_object_state (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.mask state, [dmz.mask prev_state]),
   update_object_flag (self, dmz.handle object_handle, dmz.handle attr_handle, boolean flag, [boolean prev_flag]),
   update_object_time_stamp (self, dmz.handle object_handle, dmz.handle attr_handle, number time_stamp, [number prev_time_stamp]),
   update_object_position (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.vector value, [dmz.vector prev_value])
   update_object_orientation (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.matrix value, [dmz.matrix prev_value]),
   update_object_velocity (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.vector value, [dmz.vector prev_value])
   update_object_acceleration (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.vector value, [dmz.vector prev_value])
   update_object_scale (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.vector value, [dmz.vector prev_value])
   update_object_vector (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.vector value, [dmz.vector prev_value])
   update_object_scalar (self, dmz.handle object_handle, dmz.handle attr_handle, number value, [number prev_value]),
   update_object_text (self, dmz.handle object_handle, dmz.handle attr_handle, string value, [string prev_value]),
   update_object_data (self, dmz.handle object_handle, dmz.handle attr_handle, dmz.data value, [dmz.data prev_value]),
}


dmz.object_type.new ([string name]) [dmz.object_type]
dmz.object_type.is_a (value) [dmz.object_type | nil]

dmz.object_type:is_of_type (dmz.object_type type) [boolean | nil]
dmz.object_type:get_name () [string | nil]
dmz.object_type:get_handle () [dmz.handle | nil]
dmz.object_type:get_parent () [dmz.object_type | nil]
dmz.object_type:become_parent () [boolean | nil]
dmz.object_type:get_children () [{dmz.object_type}]


dmz.portal.set_view (dmz.vector position, dmz.matrix orientation, [string name])
dmz.portal.get_view ([string name]) [dmz.vector, dmz.matrix]


dmz.runtime.lookup_named_handle (string name) [dmz.handle | nil]
dmz.runtime.create_named_handle (string name) [dmz.handle | nil]
dmz.runtime.send_message (dmz.message_type type, [dmz.handle target], [dmz.data in], [dmz.data out]) [number | nil]


dmz.time_slice.new () [dmz.time_slice]
dmz.time_slice.is_a (value) [dmz.time_slice | nil]
dmz.time_slice.System -- Type
dmz.time_slice.Runtime -- Type
dmz.time_slice.Repeating -- Mode
dmz.time_slice.Single -- Mode

dmz.time_slice:create (function callback, table self, [string name], [number interval], [integer mode], [integer type]) [dmz.handle | nil]
dmz.time_slice:start (dmz.handle handle) [boolean | nil]
dmz.time_slice:stop (dmz.handle handle) [boolean | nil]
dmz.time_slice:set_interval (dmz.handle handle, number interval) [boolean | nil]
dmz.time_slice:get_interval (dmz.handle handle) [number | nil]
dmz.time_slice:get_type (dmz.handle handle) [integer | nil]
dmz.time_slice:set_mode (dmz.handle handle, integer mode) [boolean | nil]
dmz.time_slice:get_mode (dmz.handle handle) [integer | nil]
dmz.time_slice:destroy (dmz.handle handle) [boolean | nil]


dmz.time.frame_time ([number frameTime]) [number]
dmz.time.last_frame_time () [number]
dmz.time.frame_delta () [number]
dmz.time.time_factor ([number factor]) [number]
dmz.time.target_frame_frequency ([number frequency]) [number]


dmz.uuid.new ([string name | dmz.uuid uuid]) [dmz.uuid | nil]
dmz.uuid.is_a (value) [dmz.uuid | nil]


dmz.undo.Undo -- Type
dmz.undo.Redo -- Type
dmz.undo.is_recording () [boolean]
dmz.undo.do_next (integer type) [boolean]
dmz.undo.start_record () [dmz.handle | nil]
dmz.undo.store_action (dmz.message_type type, [dmz.handle target], [dmz.data data]) [boolean | nil]
dmz.undo.stop_record (dmz.handle handle) [boolean | nil]
dmz.undo.abort_record (dmz.handle handle) [boolean | nil]


dmz.vector.new ([dmz.vector value | {number} | number x, number y, number z]) [dmz.vector]
dmz.vector.is_a (value) [dmz.vector | nil]

dmz.vector:set_x (number x) [self | nil]
dmz.vector:set_y (number y) [self | nil]
dmz.vector:set_z (number z) [self | nil]
dmz.vector:set_xyz (number x, number y, number z) [self | nil]
dmz.vector:get_x () [number | nil]
dmz.vector:get_y () [number | nil]
dmz.vector:get_z () [number | nil]
dmz.vector:get_xyz () [(number, number, number) | nil]
dmz.vector:magnitude () [number | nil]
dmz.vector:normalize () [dmz.vector | nil]
dmz.vector:dot (dmz.vector value) [number | nil]
dmz.vector:cross (dmz.vector value) [dmz.vector | nil]
dmz.vector:is_zero ([number epsilon]) [boolean | nil]
dmz.vector:get_angle (dmz.vector value) [number | nil]
--]]
}
}
