
function sync_update (self, time)

   self.log:error ("Got update at: " .. dmz.time.frame_time ())

   if self.unreg then
      if dmz.time.frame_time () > 4 then
         self.log:info ("Destroying sync")
         self.sync:destroy (self.handle)
         self.log:info ("Setting sync to nil")
         self.sync = nil
         self.handle = nil
      end
   end
end

function start (self)

   self.handle =
      self.sync:create (sync_update, self, nil, 1.1, dmz.sync.Repeating, dmz.sync.System)
end

function stop (self)

   if self.handle and self.sync then self.sync:destroy (self.handle) end

end

function new (config, name)

   local self = {
      start_plugin = start,
      stop_plugin = stop,
      name = name,
      log = dmz.log.new ("lua." .. name),
      sync = dmz.sync.new (),
      unreg = config:lookup_boolean ("unreg.value"),
   }

   self.log:info ("Creating plugin " .. name)

   if self.unreg then self.log:info (name .. " will unregister")
   else self.log:info (name .. " will not unregister")
   end

   local uuid = dmz.uuid.new ()

   print (uuid)

   return self
end
