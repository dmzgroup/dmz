dmz.test.start ("dmzLuaKernelHandleTest")

handle1 = dmz.handle.new ("foo");
handle2 = dmz.handle.new ("foo");
handle3 = dmz.handle.new ("goo");

dmz.test.validate (handle1 == handle2, "Handles with same name are same object")
dmz.test.validate (handle1 ~= handle3, "Handles with different names are not the same")

str = "Hello string."

t = { [handle1] = str, [handle3] = "Some other string" }

dmz.test.validate (
   t[handle2] == str and t[handle3] ~= str,
   "Handles can be used as table keys")

dmz.test.stop ()

