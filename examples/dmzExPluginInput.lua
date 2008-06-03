
function receive_input_event (self, event)

   if event.state then 

      local active = "unknown"
      
      if event.state.active then active = "Active" else active = "NOT Active" end
      
      self.log:out (
         "[store_channel_state] channel: " .. tostring (event.channel)
         .. " value: " .. active)
   end

   if event.axis then

      self.log:out (
         "[store_axis_event] channel: " .. tostring (event.channel)
         .. " source: " .. tostring (event.axis.source)
         .. " axis: " .. event.axis.which
         .. " value: " .. event.axis.value)
   end
   
   if event.button then
   
      local upOrDown = "unknown"

      if event.button.value then upOrDown = "Down" else upOrDown = "Up" end

      self.log:out (
         "[store_button_event] channel: " .. tostring (event.channel)
         .. " source: " .. tostring (event.button.source)
         .. " button: " .. event.button.which
         .. " value: " .. upOrDown)
   end
   
   if event.key then
   
      local upOrDown = "unknown"
      
      if event.key.state then upOrDown = "True" else upOrDown = "False" end
      
      self.log:out (
         "[store_key_event] channel: " .. tostring (event.channel)
         .. " source: " .. tostring (event.key.source)
         .. " key: " .. event.key.value
         .. " value: " .. upOrDown)
   end
end


function new (config, name)

   local self = {

      counter = 0,
      log = dmz.log.new ("lua." .. name),
      obs = dmz.input_observer.new (),
   }

   self.log:info ("Creating plugin: " .. name)
   
   self.obs:init_channels (
      config,
      dmz.input.ChannelState + dmz.input.Axis + dmz.input.Button + dmz.input.Key,
      receive_input_event,
      self);

   return self
end

