-- 重写日志记录方法
local old_log = {}
log = {}

old_log.slow = logger.slow
function log.slow(...)
  old_log.slow("[script] "..string.format(...))
end

old_log.slow_error = logger.slow_error
function log.slow_error(...)
  old_log.slow_error("[script] "..string.format(...))
end

old_log.slow_warning = logger.slow_warning
function log.slow_warning(...)
  old_log.slow_warning("[script] "..string.format(...))
end

old_log.slow_debug = logger.slow_debug
function log.slow_debug(...)
  old_log.slow_debug("[script] "..string.format(...))
end

old_log.slow_write = logger.slow_write
function log.slow_write(...)
  old_log.slow_write("[script] "..string.format(...))
end

old_log.fast = logger.fast
function log.fast(...)
  old_log.fast("[script] "..string.format(...))
end

old_log.fast_error = logger.fast_error
function log.fast_error(...)
  old_log.fast_error("[script] "..string.format(...))
end

old_log.fast_warning = logger.fast_warning
function log.fast_warning(...)
  old_log.fast_warning("[script] "..string.format(...))
end

old_log.fast_debug = logger.fast_debug
function log.fast_debug(...)
  old_log.fast_debug("[script] "..string.format(...))
end

old_log.fast_write = logger.fast_write
function log.fast_write(...)
  old_log.fast_write("[script] "..string.format(...))
end

-- 重写文件操作
local old_file = {}

old_file.opentab = file.opentab
function file.opentab(filename)
  local result = old_file.opentab(get_settingfile(filename))
  return result
end

old_file.openini = file.openini
function file.openini(filename)
  local result = old_file.openini(get_settingfile(filename))
  return result
end

-- 模块加载
function require_ex(module_name)
  local _module_name = get_scriptfile(module_name) -- To the true name.
  if package.loaded[_module_name] then
    log.fast_debug("require_ex module[%s] reload", module_name)
  else
    log.fast("require_ex(%s)", module_name)
  end
  package.loaded[_module_name] = nil
  require(module_name)
end

-- 文件加载
function dofile_ex(filename)
  local func = loadfile(filename..".lua")
  if nil == func then
    func = loadfile(filename..".lc")
    if nil == func then
      log.fast_error("dofile_ex(%s) failed", filename)
      os.exit()
      return
    end
  end
  setfenv(func, getfenv(2))
  func()
end
