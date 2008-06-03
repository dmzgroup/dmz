dmz.test.start ("dmzLuaKernelVectorTest")

vec0 = dmz.vector.new ()

dmz.test.validate (
   0 == vec0:get_x () and 0 == vec0:get_y () and 0 == vec0:get_z (),
   "Vector default constructor")

vec1 = dmz.vector.new {1, 2, 3}

dmz.test.validate (
   1 == vec1:get_x () and 2 == vec1:get_y () and 3 == vec1:get_z (),
   "Vector table constructor")

vec2 = dmz.vector.new (vec1)

dmz.test.validate (
   1 == vec2:get_x () and 2 == vec2:get_y () and 3 == vec2:get_z (),
   "Vector copy constructor")

dmz.test.stop ()

