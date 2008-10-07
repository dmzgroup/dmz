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
Returns true if the attribute was successfully stored.
]],
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
}, -- dmz.config
{

t = "dmz.data",

{
t = "dmz.data.new",
o = {"data",},
b = [[
]],
-- [dmz.data]
},{
t = "dmz.data.is_a",
p = {"value",},
-- [dmz.data | nil]
b = [[
]],
},{
t = "dmz.data:get_attributes",
p = {},
--  [{dmz.handle}]
b = [[
]],
},{
t = "dmz.data:store_string",
p = {"attribute", "offset", "value",},
b = [[
]],
},{
t = "dmz.data:lookup_string",
p = {"attribute", "offset",},
-- [string | nil]
b = [[
]],
},{
t = "dmz.data:store_number",
p = {"attribute", "offset", "value",},
b = [[
]],
},{
t = "dmz.data:lookup_number",
p = {"attribute", "offset",},
-- [number | nil]
b = [[
]],
},{
t = "dmz.data:store_handle",
p = {"attribute", "offset", "value",},
b = [[
]],
},{
t = "dmz.data:lookup_handle",
p = {"attribute", "offset",},
-- [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.data:store_vector",
p = {"attribute", "offset", "value",},
b = [[
]],
},{
t = "dmz.data:lookup_vector",
p = {"attribute", "offset",},
-- [dmz.vector | nil]
b = [[
]],
},
}, -- dmz.data
{

t = " dmz.event_type",

{
t = "dmz.event_type.new",
o = {"dmz.event_type",},
-- [dmz.event_type]
b = [[
]],
},{
t = "dmz.event_type.is_a",
p = {"value",},
-- [dmz.event_type | nil]
b = [[
]],
},{
t = "dmz.event_type:is_of_type",
p = {"event_type",},
-- [boolean]
b = [[
]],
},{
t = "dmz.event_type:get_name",
p = {},
-- [string]
b = [[
]],
},{
t = "dmz.event_type:get_handle",
p = {},
-- [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.event_type:get_parent",
p = {},
-- [dmz.event_type]
b = [[
]],
},{
t = "dmz.event_type:become_parent",
p = {},
-- [boolean]
b = [[
]],
},{
t = "dmz.event_type:get_children",
p = {},
-- [{dmz.event_type} | nil]
b = [[
]],
},
}, -- dmz.event_type

{

t = "dmz.handle",

{
t = "dmz.handle.new",
p = {"value",},
b = [[
]],
--[dmz.handle | nil]
},{
t = "dmz.handle.is_a",
p = {"value",},
-- [dmz.handle | nil]
b = [[
]],
},
}, -- dmz.handle
{

t = "dmz.input",

{
t = "dmz.input.ChannelState",
b = [[
]],
},{
t = "dmz.input.Axis",
b = [[
]],
},{
t = "dmz.input.Button",
b = [[
]],
},{
t = "dmz.input.Switch",
b = [[
]],
},{
t = "dmz.input.Key",
b = [[
]],
},{
t = "dmz.input.Mouse",
b = [[
]],
},{
t = "dmz.input.Data",
b = [[
]],
},{
t = "dmz.input.All",
b = [[
]],
},{
t = "dmz.input.has_buttons_changed",
p = {"buttonEvent",},
--[boolean]
b = [[
]],
},{
t = "dmz.input.is_button_changed",
p = {"buttonNumber", "buttonEvent",},
-- [boolean]
b = [[
]],
},{
t = "dmz.input.is_button_pressed",
p = {"buttonNumber", "buttonEvent",},
-- [boolean]
b = [[
]],
},
}, -- dmz.input
{

t = "dmz.input_observer",
b = [[

An event is stored as a table and has the following structure:
<code>
<pre>
{
   axis = {
      source, -- dmz.handle
      which, -- number
      value, -- number
      previous_value, -- number
      delta, -- number
   },
   button = {
      source, -- dmz.handle
      which, -- number
      value, -- boolean
      previous_value, -- boolean
      changed, -- boolean
   },
   switch = {
      source, -- dmz.handle
      which, -- number
      value, -- number
      previous_value, -- number
      changed, -- boolean
   },
   key = {
      source, -- dmz.handle
      value, -- number
      state, -- boolean
   },
   mouse = {
      source, -- dmz.handle
      window_size_x, -- number
      window_size_y, -- number
      x, -- number
      y, -- number
      previous_x, -- number
      previous_y, -- number
      delta_x, -- number
      delta_y, -- number
      scroll_delta_x, -- number
      scroll_delta_y, -- number
      buttons, -- dmz.mask
      previous_buttons, -- dmz.mask
   },
}
</pre>
</code>

The input callback should accept two parameters: self and an event.
<code>
<pre>
   function input_callback (self, event)
      if event.button then -- process mouse event
      elseif event.key then -- process key event
      elseif event.mouse then -- process mouse event
      end
   end
</pre>
</code>
]],
{
t = "dmz.input_observer.new",
p = {},
b = [[
]],
-- [dmz.input_observer]
},{
t = "dmz.input_observer.is_a",
p = {"value",},
-- [dmz.input_observer | nil]
b = [[
]],
},{
t = "dmz.input_observer:init_channels",
p = {"config", "mask", "callback", "self",},
b = [[
]],
},{
t = "dmz.input_observer:register_channel",
p = {"channelName", "mask", "callback", "self",},
b = [[
]],
},{
t = "dmz.input_observer:register_default_channel",
p = {"mask", "callback", "self"},
b = [[
]],
},{
t = "dmz.input_observer:release_channel",
p = {"channelName",},
b = [[
]],
},{
t = "dmz.input_observer:release_default_channel",
p = {},
b = [[
]],
},{
t = "dmz.input_observer:release_all",
p = {},
b = [[
]],
},
}, -- dmz.input_observer
{

t = "dmz.isect",

{
t = "dmz.isect.RayTest",
-- Test type
b = [[
]],
},{
t = "dmz.isect.SegmentTest",
-- Test type
b = [[
]],
},{
t = "dmz.isect.FirstPoint",
-- Param type
b = [[
]],
},{
t = "dmz.isect.ClosestPoint",
-- Param type
b = [[
]],
},{
t = "dmz.isect.AllPoints",
-- Param type
b = [[
]],
},{
t = "dmz.isect.BackCull",
-- Cull mask
b = [[
]],
},{
t = "dmz.isect.FrontCull",
-- Cull mask
b = [[
]],
},{
t = "dmz.isect.Invisible",
-- Cull mask
b = [[
]],
},{
t = "dmz.isect.clamp",
p = {"point",},
-- [(dmz.vector point, dmz.vector normal) | nil]
b = [[
]],
},{
t = "dmz.isect.enable_isect",
p = {"handle",},
-- [boolean]
b = [[
]],
},{
t = "dmz.isect.disable_isect",
p = {"handle",},
--  [boolean]
b = [[
]],
},{
t = "dmz.isect.do_isect",
p = {"test"},
o = {"parameters"},
--[[
 (
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
--]]
b = [[
]],
},{
t = "dmz.isect.do_batch_isect",
p = {"tests"},
o = {"parameters"},
--[[
(
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
--]]
b = [[
]],
},
}, -- dmz.isect
{

t = "dmz.log",

{
t = "dmz.log.new",
o = {"name",},
b = [[
]],
-- [dmz.log]
},{
t = "dmz.log.is_a",
p = {"value",},
-- [dmz.log | nil]
b = [[
]],
},{
t = "dmz.log:out",
p = {"...",},
},{
t = "dmz.log:error",
p ={"...",},
},{
t = "dmz.log:warn",
p = {"...",},
},{
t = "dmz.log:info",
p = {"...",},
},{
t = "dmz.log:debug",
p = {"...",},
b = [[
]],
},
}, -- dmz.log
{

t = "dmz.mask",

{
t = "dmz.mask.new",
o = {"value"},
-- [dmz.mask]
b = [[
]],
},{
t = "dmz.mask.is_a",
p = {"value",},
-- (value) [dmz.mask | nil]
b = [[
]],
},{
t = "dmz.mask:clear",
p = {},
b = [[
]],
},{
t = "dmz.mask:contains",
p = {"value",},
--  [boolean]
b = [[
]],
},{
t = "dmz.mask:unset",
p = {"value",},
-- [boolean]
b = [[
]],
},{
t = "dmz.mask:bit_and",
p = {"value",},
--  [dmz.mask]
b = [[
]],
},{
t = "dmz.mask:bit_or",
p = {"value",},
--  [dmz.mask]
b = [[
]],
},{
t = "dmz.mask:bit_xor",
p = {"value",},
--  [dmz.mask]
b = [[
]],
},{
t = "dmz.mask:bit_set",
p = {"value",},
-- [dmz.mask]
b = [[
]],
},{
t = "dmz.mask:bit_unset",
p = {"value",},
-- [dmz.mask]
b = [[
]],
},{
t = "dmz.mask:bit_not",
p = {},
-- [dmz.mask]
b = [[
]],
},{
t = "dmz.mask:bit_shift",
p = {"shift",},
-- (int shift) [dmz.mask]
b = [[
]],
},
}, -- dmz.mask
{

t = "dmz.math",

{
t = "dmz.math.is_zero",
p = {"value",},
-- [boolean]
b = [[
]],
},{
t = "dmz.math.to_degrees",
p = {"value",},
-- [number]
b = [[
]],
},{
t = "dmz.math.to_radians",
p = {"value",},
-- [number]
b = [[
]],
},{
t = "dmz.math.up",
p = {},
-- [dmz.vector]
b = [[
]],
},{
t = "dmz.math.down",
p = {},
-- [dmz.vector]
b = [[
]],
},{
t = "dmz.math.right",
p = {},
-- [dmz.vector]
b = [[
]],
},{
t = "dmz.math.left",
p = {},
-- [dmz.vector]
b = [[
]],
},{
t = "dmz.math.forward",
p = {},
-- [dmz.vector]
b = [[
]],
},{
t = "dmz.math.backward",
p = {},
-- [dmz.vector]
b = [[
]],
},
}, -- dmz.math

{

t = "dmz.matrix",

{
t = "dmz.matrix.new",
p = {"value",},
-- value can be dmz.matrix or array of nine numbers
},{
t = "dmz.matrix.new",
p = {"v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",},
-- [dmz.matrix]
b = [[
]],
},{
t = "dmz.matrix.is_a",
p = {"value",},
-- [dmz.matrix | nil]
b = [[
]],
},{
t = "dmz.matrix:transpose",
p = {},
-- [dmz.matrix]
b = [[
]],
},{
t = "dmz.matrix:invert",
p = {},
-- [dmz.matrix]
b = [[
]],
},{
t = "dmz.matrix:transform",
p = {"value",},
-- (dmz.vector) [dmz.vector]
b = [[
]],
},{
t = "dmz.matrix:from_table",
p = {"value"},
-- table [dmz.matrix]
b = [[
]],
},{
t = "dmz.matrix:to_table",
p = {},
--  [{number}]
b = [[
]],
},{
t = "dmz.matrix:from_two_vectors",
p = {"from", "to", },
-- [dmz.matrix]
b = [[
]],
},{
t = "dmz.matrix:from_axis_and_angle",
p = {"axis", "angle",},
-- [dmz.matrix]
b = [[
]],
},{
t = "dmz.matrix:is_identity",
p = {},
-- [boolean]
b = [[
]],
},{
t = "dmz.matrix:set_identity",
p = {},
-- [dmz.matrix]
b = [[
]],
},
}, -- dmz.matrix
--[[
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
