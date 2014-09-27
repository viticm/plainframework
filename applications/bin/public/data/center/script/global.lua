g_debug = true

if g_debug then
  package.path = "./?.lua"
else
  package.path = "./?.lc"
end
