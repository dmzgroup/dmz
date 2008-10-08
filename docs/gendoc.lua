local function build_index (name, doc, index)
   if doc.t then
      local href = name .. ".html#" .. doc.t
      doc.title = '<a name="' .. doc.t .. '">' .. doc.t .. "</a>"
      doc.link = '<a href=' .. href .. '>' .. doc.t .. '</a>'
      index[#index + 1] = { t = doc.t, link = doc.link }
   end
   local current = 1
   while doc[current] do build_index (name, doc[current], index) current = current + 1 end
end

local function create_contents (name, doc)
   local index = {}
   local current = 1
   while doc[current] do build_index (name, doc[current], index) current = current + 1 end
   local file =  io.open (name .. "-contents.html", "w")
   table.sort (index, function (a, b) return a.t < b.t end)
   file:write ([[
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html><head><meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
<title>]] .. doc.t .. " Contents" .. [[</title>
</head>
<body>]] .. "\n")
   for i, v in ipairs (index) do
      file:write (v.link .. "<br>\n")
   end
   file:write ([[
</body>
</html>]] .. "\n")
  file:close ()  
end

local function create_class_index (doc)
   local index = {}
   local count = 1
   while doc[count] do
      if doc[count].link then index[#index + 1] = doc[count].t end
      count = count + 1
   end
   return index
end

local function add_code_blocks (body, keys)
   if keys then
      local count = 1
      while keys[count] do
         body = string.gsub (body, "([ \n])(" .. keys[count] .. ")([ .,\n])",
            function (s1, s2, s3)
               return s1 .. "<code>" .. s2 .. "</code>" .. s3
            end)
         count = count + 1
      end
   end
   return body
end

local emList = {"nil", "number", "boolean", "string"}

local function add_em_blocks (body)
    for i, v in ipairs (emList) do
      body = string.gsub (body, "([ \n])(" .. v .. ")([ .,\n])", function (s1, s2, s3)
         return s1 ..'<em>' .. s2 .. '</em>' .. s3
      end)
   end
   return body
end

local function add_class_links (body, classIndex)
   for i, v in ipairs (classIndex) do
      body = string.gsub (body, "([ \n])(" .. v .. ")([ .,\n])", function (s1, s2, s3)
         return s1 .. '<a href="#' .. s2 .. '">' .. s2 .. '</a>' .. s3
      end)
   end
   return body
end

local function print_doc (file, doc, classIndex, count)
   local title = doc.title
   if not title then title = doc.t end
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
         local closeCount = 0
         for i, v in ipairs (doc.o) do
            if #param > 0 then param = param .. " [, "
            else  param = param .. "["
            end
            closeCount = closeCount + 1
            param = param .. v
         end
         param = param .. string.rep ("]", closeCount)
      end
      param = " (" ..  param .. ")"
   end
   file:write ("<h" .. tostring (count) .. ">" .. title .. param .. "</h"
      .. tostring (count) .. ">\n")
   if doc.b then
      doc.body = add_code_blocks (doc.b, doc.p)
      doc.body = add_code_blocks (doc.body, doc.o)
      doc.body = add_em_blocks (doc.body)
      doc.body = add_class_links (doc.body, classIndex)
      file:write (doc.body)
      if count >= 2 then file:write ("<hr>\n") end
   end

   local current = 1

   while doc[current] do
      print_doc (file, doc[current], classIndex, count + 1)
      current = current + 1
   end 
end

local doc = dofile (arg[1])

create_contents (arg[2], doc)
local classIndex = create_class_index (doc)

local count = 1
local file = io.open (arg[2] .. ".html", "w")
file:write ([[
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html><head><meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
<title>]] .. doc.t .. [[</title>
</head>
<body>]] .. "\n")
print_doc (file, doc, classIndex, count)
file:write ([[
</body>
</html>]] .. "\n")
file:close ()
