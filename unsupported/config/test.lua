print ("Hello world!")

local v1 = dmz.vector.new (1, 2, 3)
local v2 = dmz.vector.new (v1)

if (v1 == v2) then print ("Equal") else print ("NOT Equal") end

local v3 = v1 + v2

local x, y, z = v3:get_xyz ()

print (v3, x .. " " .. y .. " " .. z)

v3 = v3 * 5

print (v3)

v4 = v1 * v2
print (v4)

v5 = v4:normalize ()
print (v5)

local t = {1, 2, 3, 4, 5, 6, 7, 8, 9}

local f = function ()

   local v1 = dmz.vector.new (7, 8, 9)

   print ("In function 'f'", v1)

--   local foo = dmz.foo.new ()
end 

local g = function ()

  f ()
end

g ()

local m1 =dmz.matrix.new (t)
print (m1)
print (-m1)

local u1 = dmz.uuid.new (true)
print (u1)

local mask = dmz.mask.new ();
mask:bit_set (63);
print (mask)

local log = dmz.log.new ("TestLog")
log:warn ("Value:", v1, v3);
log:error ("This is an error")

local c = dmz.config.new ("THIS IS THE NAME")
local c2 = dmz.config.new ("child1")
local c3 = dmz.config.new ("child2")
local ct = { c2, c3 }
c:add_config (ct)
c:add_config ("level1.level2", ct)
log:info ("Name: " .. c:get_name ())

c:store_attribute ("foo", "value1")
c:store_attribute ("goo", "value2")

local at = c:get_attributes ()

log:info (at.foo .. " " .. " " .. at.goo)
log:info ("config: ", c);

log:info (dmz.time.time_factor ())

log:info (dmz.config.get_global ())

test = function ()

   log:error ("This is the test fuction")
end

function new (config)

   log:error (config:lookup_attribute ("value.name"))

   log:error ("NEW CALLED!!!")

end
