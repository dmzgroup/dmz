return {

t = "DMZ Lua API Reference Guide",

{

t = "dmz.vector",

b = [[
The dmz.vector class provides bindings for the dmz::Vector class.
]],
{
t = "dmz.vector.new",
p = {"x", "y", "z",},
},{
t = "dmz.vector.new",
p = {"value",},
b = [[
Creates a new dmz.vector. The parameter value may either be a dmz.vector or
an array with three numbers. The parameters x, y, and z are numbers.
]],
},{
t = "dmz.vector.is_a",
p = {"value",},
b = [[
Tests if value is a dmz.vector. Returns a dmz.vector if true and nil if false.
]],
},{
t = "dmz.vector:set_x",
p = {"value"},
b = [[
Sets the x value of the dmz.vector. Returns a reference to self.
]],
},{
t = "dmz.vector:set_y",
p = {"value",},
b = [[
Sets the y value of the dmz.vector. Returns a reference to self.
]],
},{
t = "dmz.vector:set_z",
p = {"value",},
b = [[
Sets the z value of the dmz.vector. Returns a reference to self.
]],
},{
t = "dmz.vector:set_xyz",
p = {"x", "y", "z",},
b = [[
Sets the x, y, and z components of the dmz.vector. Returns a reference to self.
]],
},{
t = "dmz.vector:get_x",
p = {},
b = [[
Gets the x value of the dmz.vector. Returns a number.
]],
},{
t = "dmz.vector:get_y",
p = {},
b = [[
Gets the y value of the dmz.vector. Returns a number.
]],
},{
t = "dmz.vector:get_z",
p = {},
b = [[
Gets the z value of the dmz.vector. Returns a number.
]],
},{
t = "dmz.vector:get_xyz",
p = {},
b = [[
Gets the x, y, and z components of the dmz.vector. Returns three numbers.
]],
},{
t = "dmz.vector:magnitude",
p = {},
b = [[
Calculates the magnitude of the vector. Returns a number.
]],
},{
t = "dmz.vector:normalize",
p = {},
b = [[
Creates and returns a normalized version of the vector.
]],
},{
t = "dmz.vector:dot",
p = {"value",},
b = [[
Calculates and returns the dot product of the dmz.vector and value. The parameter value
must evaluate to a dmz.vector.
]],
},{
t = "dmz.vector:cross",
p = {"value",},
b = [[
Calculates and returns the cross product of the dmz.vector and value. The parameter value
must evaluate to a dmz.vector. Returns a dmz.vector.
]],
},{
t = "dmz.vector:is_zero",
o = {"epsilon",},
b = [[
Tests if the x, y, and z components of the dmz.vector are zero. Returns true if the
all components are zero. It will otherwise return false. The optional parameter epsilon
specifies the error to allow when calculating the value of zero.
]],
},{
t = "dmz.vector:get_angle",
p = {"value",},
b = [[
Calculates and returns the angle between the dmz.vector and value. The angle is returned
as a number in radians. The parameter value must evaluate to a dmz.vector.
]],
},
}, -- dmz.vector
{

t = "dmz.matrix",
b = [[
The dmz.matrix class provides bindings for the dmz::Matrix class.
]],
{
t = "dmz.matrix.new",
p = {"v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",},
},{
t = "dmz.matrix.new",
p = {"value",},
b = [[
Creates a new dmz.matrix. The parameter value may either be a dmz.matrix or an array
with nine numbers. The parameters v1 through v9 are numbers.
]],
},{
t = "dmz.matrix.is_a",
p = {"value",},
b = [[
Tests if value is a dmz.matrix. Returns a dmz.matrix if value is a dmz.matrix and nil
if it is not.
]],
},{
t = "dmz.matrix:transpose",
p = {},
b = [[
Creates and returns a dmz.matrix that is a transpose of the current dmz.matrix.
]],
},{
t = "dmz.matrix:invert",
p = {},
b = [[
Creates and returns a dmz.matrix that is an inverse of the current dmz.matrix.
]],
},{
t = "dmz.matrix:transform",
p = {"value",},
b = [[
Creates and returns a dmz.vector that is the transformed value. The parameter
value must evaluate to a dmz.vector.
]],
},{
t = "dmz.matrix:from_table",
p = {"value"},
b = [[
Updates the dmz.matrix to use the array of numbers stored in value. The parameter
value is an array of nine numbers. A reference to self is returned.
]],
},{
t = "dmz.matrix:to_table",
p = {},
b = [[
The nine components of the dmz.matrix are returned in an array.
]],
},{
t = "dmz.matrix:from_two_vectors",
p = {"fromVec", "toVec", },
b = [[
Use the fromVec and toVec to compose new values for the dmz.matrix. The parameters
fromVec and toVec must evaluate to dmz.vectors. A reference to self is returned.
]],
},{
t = "dmz.matrix:from_axis_and_angle",
p = {"axis", "angle",},
b = [[
Use the axis and angle to compose new values for the dmz.matrix. The parameter
axis must evaluate to a dmz.vector. The parameter angle is a number.
A reference to self is returned.
]],
},{
t = "dmz.matrix:is_identity",
p = {},
b = [[
Test if the dmz.matrix is an identity matrix. Returns true if it is an identity matrix.
]],
},{
t = "dmz.matrix:set_identity",
p = {},
b = [[
Coverts the matrix to an identity matrix. A reference to self is returned.
]],
},
}, -- dmz.matrix
{

t = "dmz.mask",
b = [[
The dmz.mask class provides bindings for the dmz::Mask class.
]],

{
t = "dmz.mask.new",
o = {"value"},
b = [[
Creates and returns a new dmz.mask. The optional value may be a dmz.mask which is used
to initialized the new dmz.mask.
]],
},{
t = "dmz.mask.is_a",
p = {"value",},
b = [[
Tests if value is a dmz.mask. Returns a dmz.mask if value is a dmz.mask and nil
if it is not.
]],
},{
t = "dmz.mask:clear",
p = {},
b = [[
Clears the mask. Returns a reference to self.
]],
},{
t = "dmz.mask:contains",
p = {"value",},
b = [[
Test if the dmz.mask contains value. The parameter value must evaluate to a dmz.mask.
Returns true if the mask contains value.
]],
},{
t = "dmz.mask:unset",
p = {"value",},
b = [[
The dmz.mask has all bits defined in value unset. The parameter must evaluate to a 
dmz.mask. Returns true if the mask is successfully unset. 
]],
},{
t = "dmz.mask:bit_and",
p = {"value",},
b = [[
Creates and returns a dmz.mask that is a bitwise and of the dmz.mask and value. The
parameter value must evaluate to a dmz.mask.
]],
},{
t = "dmz.mask:bit_or",
p = {"value",},
b = [[
Creates and returns a dmz.mask that is a bitwise or of the dmz.mask and value. The
parameter value must evaluate to a dmz.mask.
]],
},{
t = "dmz.mask:bit_xor",
p = {"value",},
b = [[
Creates and returns a dmz.mask that is a bitwise exclusive-or  (i.e. xor) of the
dmz.mask and value.  The parameter value must evaluate to a dmz.mask.
]],
},{
t = "dmz.mask:bit_set",
p = {"value",},
b = [[
Sets the bit specified by value. The parameter value is a number. Returns a reference
to self.
]],
},{
t = "dmz.mask:bit_unset",
p = {"value",},
b = [[
Unsets the bit specified by value. The parameter value is a number. Returns a reference
to self.
]],
},{
t = "dmz.mask:bit_not",
p = {},
b = [[
Creates and returns a mask that is the bitwise not of the dmz.mask.
]],
},{
t = "dmz.mask:bit_shift",
p = {"shift",},
b = [[
Shifts the mask. A positive value shifts up while a negative value shifts down.
Returns a reference to self.
]],
},
}, -- dmz.mask
{

t = "dmz.handle",
b = [[
The dmz.handle class provides bindings for the dmz::Handle type. Handles are atomic
and maybe be used as key values in tables.
]],
{
t = "dmz.handle.new",
p = {"value",},
b = [[
Creates a dmz.handle. The parameter value must either be a string or number. If value
is a string, a named handle is created. If value is a number, the dmz.handle is
initialized with the number.
]],
},{
t = "dmz.handle.is_a",
p = {"value",},
b = [[
Tests if value is a dmz.handle. Returns a dmz.handle if value is a dmz.handle and returns
nil if it is not.
]],
},
}, -- dmz.handle
{

t = "dmz.uuid",
b = [[
The dmz.uuid class provides bindings for the dmz::UUID class.  UUIDs are atomic
and maybe be used as key values in tables.
]],

{
t = "dmz.uuid.new",
p = {},
b = [[
Creates a new dmz.uuid. The newly created dmz.uuid is initialized with a unique UUID.
]],
},{
t = "dmz.uuid.is_a",
p = {"value"},
b = [[
Tests if value is a dmz.uuid. Returns a dmz.uuid if value is a dmz.uuid and returns
nil if it is not.
]],
},
}, -- dmz.uuid
{

t = "dmz.config",
b = [[
The dmz.config class provides bindings for the dmz::Config class.
]],

{
t = "dmz.config.get_global",
p = {},
b = [[
Returns a dmz.config object containing the global config data.
]],
},{
t = "dmz.config.new",
o = {"name",},
b = [[
Returns a new dmz.config object. The name is used to initalize the new dmz.config
object's tag.
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
},{
t = "dmz.config:lookup_number",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a number.
If the attribute name is not found, then default is returned.
]],
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
},{
t = "dmz.config:lookup_state",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.mask.
If the attribute name is not found, then default is returned.
]],
t = "dmz.config:lookup_uuid",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.uuid.
If the attribute name is not found, then default is returned.
]],
},{
t = "dmz.config:lookup_vector",
p = {"name",},
o = {"default",},
b = [[
Looks up the attribute value name and returns it as a dmz.vector.
If the attribute name is not found, then default is returned.
]],
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
Returns a table of children dmz.config objects.
]],
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

t = "dmz.resources",

{
t = "dmz.resources.find_file",
p = {"fileName",},
b = [[
]],
},{
t = "dmz.resources.store_config",
p = {"name", "config",},
b = [[
]],
},{
t = "dmz.resources.lookup_config",
p = {"name",},
b = [[
]],
},{
t = "dmz.resources.remove_config",
p = {"name",},
b = [[
]],
},
}, -- dmz.resources
{

t = "dmz.object_type",

{
t = "dmz.object_type.new",
p = {"name",},
--  [dmz.object_type]
b = [[
]],
},{
t = "dmz.object_type.is_a",
p = {"value",},
--  [dmz.object_type | nil]
b = [[
]],
},{
t = "dmz.object_type:is_of_type",
p = {"type"},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.object_type:get_name",
p = {},
-- [string | nil]
b = [[
]],
},{
t = "dmz.object_type:get_handle",
p = {},
-- [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.object_type:get_parent",
p = {},
-- [dmz.object_type | nil]
b = [[
]],
},{
t = "dmz.object_type:become_parent",
p = {},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.object_type:get_children",
p = {},
-- [{dmz.object_type}]
b = [[
]],
},
}, -- dmz.object_type

{

t = "dmz.event_type",

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

t = "dmz.message_observer",

{
t = "dmz.message_observer.new",
p = {"name",},
-- [dmz.message_observer]
b = [[
]],
},{
t = "dmz.message_observer.is_a",
p = {"value",},
-- [dmz.message_observer | nil]
b = [[
]],
},{
t = "dmz.message_observer:register",
p = {"message", "callback", "self",},
-- [boolean]
b = [[
]],
},{
t = "dmz.message_observer:release",
p = {"message",},
-- [boolean]
b = [[
]],
},{
t = "dmz.message_observer:release_all",
p = {},
-- [boolean]
b = [[
]],
},
}, -- dmz.message_observer
{

t = "dmz.message",

{
t = "dmz.message.new",
p = {"name",},
-- [dmz.message]
b = [[
]]
},{
t = "dmz.message.is_a",
p = {"value",},
-- [dmz.message | nil]
b = [[
]]
},{
t = "dmz.message:is_of_type",
p = {"message",},
-- [boolean | nil]
b = [[
]]
},{
t = "dmz.message:get_name",
p = {},
-- [string | nil]
b = [[
]]
},{
t = "dmz.message:get_handle",
p = {},
-- [dmz.handle | nil]
b = [[
]]
},{
t = "dmz.message:get_parent",
p = {},
-- [dmz.messaget_type | nil]
b = [[
]]
},{
t = "dmz.message:become_parent",
p = {},
-- [boolean | nil]
b = [[
]]
},{
t = "dmz.message:send",
o = {"target", "inData", "outData",},
b = [[
]],
},
}, -- dmz.message
{

t = "dmz.time_slice",

{
t = "dmz.time_slice.new",
p = {},
-- [dmz.time_slice]
b = [[
]],
},{
t = "dmz.time_slice.is_a",
p = {"value",},
-- [dmz.time_slice | nil]
b = [[
]],
},{
t = "dmz.time_slice.System",
-- Type
b = [[
]],
},{
t = "dmz.time_slice.Runtime",
-- Type
b = [[
]],
},{
t = "dmz.time_slice.Repeating",
-- Mode
b = [[
]],
},{
t = "dmz.time_slice.Single",
-- Mode
b = [[
]],
},{
t = "dmz.time_slice:create",
p = {"callback", "self",},
o = {"name", "interval", "mode", "type"},
-- [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.time_slice:start",
p = {"handle",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.time_slice:stop",
p = {"handle",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.time_slice:set_interval",
p = {"handle", "interval",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.time_slice:get_interval",
p = {"handle",},
-- [number | nil]
b = [[
]],
},{
t = "dmz.time_slice:get_type",
p = {"handle",},
-- [integer | nil]
b = [[
]],
},{
t = "dmz.time_slice:set_mode",
p = {"handle", "mode",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.time_slice:get_mode",
p = {"handle"},
-- [integer | nil]
b = [[
]],
},{
t = "dmz.time_slice:destroy",
p = {"handle",},
-- [boolean | nil]
b = [[
]],
},
}, -- dmz.time_slice
{

t = "dmz.time",

{
t = "dmz.time.frame_time",
o = {"value",},
-- [number]
b = [[
]],
},{
t = "dmz.time.last_frame_time",
p = {},
-- [number]
b = [[
]],
},{
t = "dmz.time.frame_delta",
p = {},
-- [number]
b = [[
]],
},{
t = "dmz.time.time_factor",
o = {"value",},
-- [number]
b = [[
]],
},{
t = "dmz.time.target_frame_frequency",
o = {"value",},
--  [number]
b = [[
]],
},
}, -- dmz.time
{

t = "dmz.undo",

{
t = "dmz.undo.Undo",
-- Type
b = [[
]],
},{
t = "dmz.undo.Redo",
-- Type
b = [[
]],
},{
t = "dmz.undo.is_recording",
p = {},
-- [boolean]
b = [[
]],
},{
t = "dmz.undo.do_next",
p = {"type",},
-- [boolean]
b = [[
]],
},{
t = "dmz.undo.start_record",
p = {},
-- [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.undo.store_action",
p = {"type",},
o = {"target", "data",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.undo.stop_record",
p = {"handle",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.undo.abort_record",
p = {"handle",},
-- [boolean | nil]
b = [[
]],
},
}, -- dmz.undo
{

t = "dmz.object_observer",

{
t = "dmz.object_observer.new",
p = {},
--  [dmz.object_observer]
b = [[
]],
},{
t = "dmz.object_observer:register",
p = {"attribute", "callbacks", "self",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.object_observer:release",
p = {"attribute", "mask",},
-- [boolean | nil]
b = [[
]],
},
}, -- dmz.object_observer
{

t = "dmz.object",

{
t = "dmz.object.Local",
-- Locality
b = [[
]],
},{
t = "dmz.object.Remote",
-- Locality
b = [[
]],
},{
t = "dmz.object.RetainLinks",
-- Mode
b = [[
]],
},{
t = "dmz.object.IgnoreLinks",
-- Mode
b = [[
]],
},{
t = "dmz.object.SelectNew",
-- Mode
b = [[
]],
},{
t = "dmz.object.SelectAdd",
-- Mode
b = [[
]],
},{
t = "dmz.object.CreateMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.DestroyMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.UUIDMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.RemoveAtrributeMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.LinkMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.UnlinkMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.LinkAttriuteObjectMaks",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.LocalityMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.TypeMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.StateMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.FlagMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.TimeStampMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.PositionMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.OrientationMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.VelocityMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.AccelerationMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.ScaleMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.VectorMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.ScalarMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.TextMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.DataMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.AllMask",
-- dmz.mask
b = [[
]],
},{
t = "dmz.object.AttributeDefaultName",
-- string
b = [[
]],
},{
t = "dmz.object.AttributeLastNetworkValueName",
-- string
b = [[
]],
},{
t = "dmz.object.AttributeHumanInTheLoopName",
-- string
b = [[
]],
},{
t = "dmz.object.AttributeSelectName",
-- string
b = [[
]],
},{
t = "dmz.object.CreateMessage",
-- dmz.message
b = [[
]],
},{
t = "dmz.object.DestroyMessage",
-- dmz.message
b = [[
]],
},{
t = "dmz.object.create_last_network_value_name",
p = {"name",},
-- [string]
b = [[
]],
},{
t = "dmz.object.get_human_in_the_loop",
p = {},
--) [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.object.is_object",
p = {"value",},
-- (dmz.handle) [boolean | nil]
b = [[
]],
},{
t = "dmz.object.is_link",
p = {"value",},
-- (dmz.handle) [boolean | nil]
b = [[
]],
},{
t = "dmz.object.get_all",
p = {},
-- [{dmz.handle} | nil]
b = [[
]],
},{
t = "dmz.object.create",
p = {"type",},
o = {"locality",},
-- (dmz.object_type type, [integer locality]) [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.object.activate",
p = {"object"},
-- (dmz.handle handle) [boolean | nil]
b = [[
]],
},{
t = "dmz.object.destroy",
p = {"object",},
-- (dmz.handle handle) [boolean | nil]
b = [[
]],
},{
t = "dmz.object.clone",
p = {"object",},
o = {"mode",},
--  (dmz.handle handle, [integer mode]) [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.object.uuid_to_handle",
p = {"uuid",},
-- (dmz.uuid uuid) [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.object.uuid",
p = {"object",},
o = {"uuid",},
-- (dmz.handle, [dmz.uuid uuid]) [dmz.uuid | nil]
b = [[
]],
},{
t = "dmz.object.remove_attribute",
p = {"object", "mask",},
-- dmz.handle handle, dmz.mask mask) [boolean | nil]
b = [[
]],
},{
t = "dmz.object.link",
p = {"attribute", "super", "sub"},
--(dmz.handle attr_handle, dmz.handle super_handle, dmz.handle sub_handle) [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.object.lookup_link_handle",
p = {"attribute", "super", "sub",},
-- [dmz.handle | nil]
b = [[
]],
},{
t = "dmz.object.lookup_linked_objects",
p = {"link",},
-- [dmz.handle attr_handle, dmz.handle super_handle, dmz.handle sub_handle | nil]
b = [[
]],
},{
t = "dmz.object.unlink",
p = {"link",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.object.link_attribute_object",
p = {"link", "object"},
--  [boolean | nil]
b = [[
]],
},{
t = "dmz.object.super_links",
p = {"object", "attribute",},
-- [{dmz.handle} | nil]
b = [[
]],
},{
t = "dmz.object.sub_links",
p = {"object", "attribute",},
-- [{dmz.handle} | nil]
b = [[
]],
},{
t = "dmz.object.locality",
p = {"object",},
o = {"locality",},
-- [integer | nil]
b = [[
]],
},{
t = "dmz.object.alternate_type",
p = {"object",},
o = {"attribute", "value",},
-- [dmz.object_type | nil]
b = [[
]],
},{
t = "dmz.object.state",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [dmz.mask state]) [dmz.mask | nil]
b = [[
]],
},{
t = "dmz.object.flag",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [boolean flag]) [boolean | nil]
b = [[
]],
},{
t = "dmz.object.time_stamp",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [number time_stamp]) [number | nil]
b = [[
]],
},{
t = "dmz.object.position",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector position]) [dmz.vector | nil]
b = [[
]],
},{
t = "dmz.object.orientation",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [dmz.matrix orientation]) [dmz.matrix | nil]
b = [[
]],
},{
t = "dmz.object.velocity",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector velocity]) [dmz.vector | nil]
b = [[
]],
},{
t = "dmz.object.acceleration",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector acceleration]) [dmz.vector | nil]
b = [[
]],
},{
t = "dmz.object.scale",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector scale]) [dmz.vector | nil]
b = [[
]],
},{
t = "dmz.object.vector",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [dmz.vector vector]) [dmz.vector | nil]
b = [[
]],
},{
t = "dmz.object.scalar",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [number scalar]) [number | nil]
b = [[
]],
},{
t = "dmz.object.text",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [string value]) [string | nil]
b = [[
]],
},{
t = "dmz.object.data",
p = {"object",},
o = {"attribute", "value",},
--(dmz.handle obj_handle, dmz.handle attr_handle, [dmz.data data]) [dmz.data | nil]
b = [[
]],
},{
t = "dmz.object.get_selected",
p = {},
-- [{dmz.handle}]
b = [[
]],
},{
t = "dmz.object.is_selected",
p = {"object",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.object.select",
p = {"object",},
o = {"mode",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.object.unselect",
p = {"object",},
-- [boolean | nil]
b = [[
]],
},{
t = "dmz.object.unselect_all",
p = {},
b = [[
]],
},
}, -- dmz.object

--[[
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
--]]
{

t = "dmz.event",

{
t = "dmz.event.Local",
b = [[
]],
},{
t = "dmz.event.Remote",
b = [[
]],
},{
t = "dmz.event.CreateMask",
b = [[
]],
},{
t = "dmz.event.CloseMask",
b = [[
]],
},{
t = "dmz.event.SourceHandle",
b = [[
]],
},{
t = "dmz.event.TargetHandle",
b = [[
]],
},{
t = "dmz.event.MunitionsHandle",
b = [[
]],
},{
t = "dmz.event.launch,",
p = {"source", "target",},
b = [[
]],
},{
t = "dmz.event.detonation",
p = {"source", "target",},
b = [[
]],
},{
t = "dmz.event.collision",
p = {"source", "target",},
b = [[
]],
},{
t = "dmz.event.open_launch",
p = {"source", "target",},
b = [[
]],
},{
t = "dmz.event.open_detonation",
p = {"source", "target",},
b = [[
]],
},{
t = "dmz.event.open_collision",
p = {"source", "target",},
b = [[
]],
},{
t = "dmz.event.create",
p = {"type",},
o = {"locality",},
b = [[
]],
},{
t = "dmz.event.close",
p = {"event",},
b = [[
]],
},{
t = "dmz.event.type",
p = {"event",},
b = [[
]],
},{
t = "dmz.event.locality",
p = {"event",},
b = [[
]],
},{
t = "dmz.event.object_handle",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.object_type",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.state",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.time_stamp",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.position",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.orientation",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.velocity",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.acceleration",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.scale",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.vector",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.scalar",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.text",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},{
t = "dmz.event.data",
p = {"event",},
o = {"attribute", "value",},
b = [[
]],
},
}, -- dmz.event
{

t = "dmz.event_observer",
--[[
callbacks:
   create_event (event, type, locality)
   close_event (event, type, locality)
--]]
{
t = "dmz.event_observer.new",
p = {},
b = [[
]],
},{
t = "dmz.event_observer:register",
p = {"type", "callbacks",},
o = {"self",},
b = [[
]],
},{
t = "dmz.event_observer:release",
p = {"type", "mask",},
b = [[
]],
},
}, -- dmz.event_observer
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

t = "dmz.portal",

{
t = "dmz.portal.set_view",
p = {"position", "orientation"},
o = {"name",},
b = [[
]],
},{
t = "dmz.portal.get_view",
o = {"name",},
-- [dmz.vector, dmz.matrix]
b = [[
]],
},
}, -- dmz.portal
{
t = "dmz.audio",
--[[
init = {
   looped, -- boolean
   relative, -- boolean
}

attributes = {
   position, -- dmz.vector
   velocity, -- dmz.vector
   gain, -- number
   pitch, -- number
}
--]]
{
t = "dmz.audio.create",
p = {"fileName"},
b = [[
]],
},{
t = "dmz.audio.destroy",
p = {"audioHandle",},
b = [[
]],
},{
t = "dmz.audio.play",
p = {"audioHandle",},
o = {"init", "attributes",},
b = [[
]],
},{
t = "dmz.audio.update",
p = {"soundHandle", "attributes",},
b = [[
]],
},{
t = "dmz.audio.stop",
p = {"soundHandle",},
b = [[
]],
},
}, -- dmz.audio
} -- main table
