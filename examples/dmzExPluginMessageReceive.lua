
function receive (self, type, data)

   if dmz.data.is_a (data) then

      local value = data:lookup_string ("value", 1)
      local time = data:lookup_number ("time", 1)
      self.log:info ("[receive] time: " .. time .. " value: " .. value)

   end
end

function new (config, name)

   local self = {

      log = dmz.log.new ("lua." .. name),
      message = config:lookup_message_type ("message.name"),
      obs = dmz.message_observer.new (name),
   }

   self.log:info ("Creating plugin: " .. name)

   self.obs:register (self.message, receive, self)

   return self
end
