function receive_input_event (self, event)
   self.log:error ("-=-=-=-=-=-=-=-=-=- Start Mouse Event")
   if event.mouse then
      for t, v in pairs (event.mouse) do
         self.log:warn (tostring (t) .. " : " .. tostring (v))
      end
   end
end

function start (self)
   self.input:init_channels (
      self.config,
      dmz.input.Mouse,
      receive_input_event,
      self);
end

function stop (self)
   if self.handle and self.timeSlice then self.timeSlice:destroy (self.handle) end
   self.input:release_all ()
end

function new (config, name)
   local self = {
      start_plugin = start,
      stop_plugin = stop,
      name = name,
      config = config,
      log = dmz.log.new ("lua." .. name),
      input = dmz.input_observer.new (),
   }
   self.log:info ("Creating plugin: " .. name)
   return self
end
