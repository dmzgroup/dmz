
function send (self)

   self.value = self.value .. "."
   self.data:store_string ("value", 1, self.value)
   self.data:store_number ("time", 1, dmz.time.frame_time ())
   self.log:info ('[send] self.message:send (self.message, ' ..
      'self.target, data)')
   self.message:send (self.target, self.data)
end

function new (config, name)

   local self = {

      log = dmz.log.new ("lua." .. name),
      timeSlice = dmz.time_slice.new (),
      message = config:lookup_message ("message.name"),
      target = dmz.handle.new ("dmzExPluginMessageReceive"),
      data = dmz.data.new (),
      value = config:lookup_string ("message.value", "Default message."),
   }

   self.log:info ("Creating plugin: " .. name)

   self.handle = self.timeSlice:create (send, self, name, 1)

   return self

end

