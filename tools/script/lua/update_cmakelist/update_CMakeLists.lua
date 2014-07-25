require('readvcproj')

function update_CMakeLists(cmakefile, vcprog, include_path)
  local newcontent = ""
  local fp = io.open(cmakefile, "r")
  if not fp then return false end
  local filecontent = fp:read("*all")
  local len = string.len(filecontent)
  local begin_position1, begin_position2 = 
    string.find(filecontent, '##update_begin')
  local begintext = string.sub(filecontent, 1, begin_position2 + 1);
  local makefile = get_vcproj_cmake(vcprog, include_path)
  local endposition = string.find(filecontent, '##update_end')
  local endtext = string.sub(filecontent, endposition - 1);
  newcontent = begintext .. '\n\n' .. makefile .. '\n\n' ..endtext
  local wf = io.open(cmakefile, "w")
  wf:write(newcontent)
  wf:close()
end

local gateway_cmakelists = "../../../../gateway/src/CMakeLists.txt"
local gateway_vcproj = "../../../../gateway/scripts/gateway.vc9.vcproj"

update_CMakeLists(gateway_cmakelists, gateway_vcproj)
