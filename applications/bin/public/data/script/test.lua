function mytest()
  log.fast("mytest function is done: %d", os.time())
end

mytest = {}
mytest.mytest = function()
  log.fast_debug("test function...");
end
