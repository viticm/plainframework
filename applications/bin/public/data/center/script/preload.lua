-- 重写日志记录方法
local old_log = {}

old_log.slow = log.slow
function log.slow(...)
  old_log.slow("[script] "..string.format(...))
end

old_log.slow_error = log.slow_error
function log.slow_error(...)
  old_log.slow_error("[script] "..string.format(...))
end

old_log.slow_warning = log.slow_warning
function log.slow_warning(...)
  old_log.slow_warning("[script] "..string.format(...))
end

old_log.slow_debug = log.slow_debug
function log.slow_debug(...)
  old_log.slow_debug("[script] "..string.format(...))
end

old_log.slow_write = log.slow_write
function log.slow_write(...)
  old_log.slow_write("[script] "..string.format)
end

old_log.fast = log.fast
function log.fast(...)
  old_log.fast("[script] "..string.format(...))
end

old_log.fast_error = log.fast_error
function log.fast_error(...)
  old_log.fast_error("[script] "..string.format(...))
end

old_log.fast_warning = log.fast_warning
function log.fast_warning(...)
  old_log.fast_warning("[script] "..string.format(...))
end

old_log.fast_debug = log.fast_debug
function log.fast_debug(...)
  old_log.fast_debug("[script] "..string.format(...))
end

old_log.fast_write = log.fast_write
function log.fast_write(...)
  old_log.fast_write("[script] "..string.format(...))
end

-- 重写文件操作
local old_file = {}

old_file.opentab = file.opentab
function file.opentab(filename)
  local result = old_file.opentab(SETTING_PATH.."/"..filename)
  return result
end

old_file.openini = file.openini
function file.openini(filename)
  local result = old_file.openini(SETTING_PATH.."/"..filename)
  return result
end

function require_ex(module_name)
  local _module_name = SCRIPT_PATH.."/"..module_name
  log.fast("require_ex(%s)", module_name)
  if package.loaded[_module_name] then
    log.fast("require_ex module[%s] will reload", module_name)
  end
  package.loaded[_module_name] = nil
  require(_module_name)
end

function dofile_ex(filename)
  local _filename = SCRIPT_PATH.."/"..filename;
  local func = loadfile(_filename..".lua")
  if nil == func then
    func = loadfile(_filename..".lc")
    if nil == func then
      log.fast_error("dofile_ex(%s) failed", filename)
      os.exit()
      return
    end
  end
end

-- 开始打印脚本日志，基本环境已经准备完毕
log.fast_debug("begin load script files...")

-- 载入游戏模块
local data = file.opentab("scripttable.txt")
local basemodule = { modulename_ = "__ModuleBase" };
local moduleindex = { __index = basemodule }
local moduleset = {}
for _, line in pairs(data) do
  local modulename = line.TableName
  moduleset[modulename] = 1
  _G[modulename] = setmetatable({ modulename_ = modulename }, moduleindex);
end
Env.moduleset_ = moduleset

function mytest(a, b, c ,d)
  print(a)
  print(b)
  print(c)
  print(d)
end

net.globalexcute({"mytest", 1.1444444444444, "abc", 3, 4})
-- 载入脚本变量
local scriptvalue_table = file.opentab("scriptvalue/filelist.txt")
for _, line in pairs(scriptvalue_table) do
  local tablename = line.TableName
  local _table = _G[tablename]
  if not _table then
    _table = {}
    _G[tablename] = _table
  end
  local filepath = line.FilePath
  local tablevalue = file.opentab(filepath)
  if not tablevalue then
    log.fast_error("script value file: %s open error", filepath)
    tablevalue = {}
  end
  for _, line in pairs(tablevalue) do
    local name = line.Name
    local value = tonumber(line.Value) or line.Value
    _table[name] = value
  end
end

-- 之后不允许新的全局变量
local metatable = {
  __newindex = function(_table, key, value)
    if (key == "it" or key == "him" or key == "me") then
      rawset(_G, key, value);
    else
      error("Attempt create global value :"..tostring(key), 2);
    end
  end
}
setmetatable(_G, metatable);
