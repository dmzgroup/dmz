local function print_doc (doc, count)

   local title = doc.t
   if not title then title = "" end
   local param = ""
   if doc.p or doc.o then
      if doc.p then
         for i, v in ipairs (doc.p) do
            if #param > 0 then param = param .. ", " end
            param = param .. v
         end
      end
      if doc.o then
         for i, v in ipairs (doc.o) do
            if #param > 0 then param = param .. ", " end
            param = param .. "[" .. v .. "]"
         end
      end
      param = " (" ..  param .. ")"
   end
   print ("<h" .. tostring (count) .. ">" .. title .. param .. "</h" .. tostring (count)
      .. ">")
   if doc.b then
      print (doc.b)
      print ("<hr>")
   end

   local current = 1

   while doc[current] do
      print_doc (doc[current], count + 1)
      current = current + 1
   end 
end

local doc = dofile (arg[1])

local count = 1

print ([[
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html><head><meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
<title>]] .. doc.t .. [[</title>
</head>
<body>]])
print_doc (doc, count)
print ([[
</body>
</html> ]])
