@echo off
title rename for visual studio(plain framework)
color 02
cls
echo select your need change model.
echo (1)all (2)pf_simple (3)pf_core(4)pf_cores (5)plugin_lua
set /p select=your select?
echo revert to standard?
set revert=-1
set /p revert=(yes/no)?
set all="pf_core|pf_cores|pf_simple|pf_plugin_lua"
if 1 == %select% php ../php/vcbuild.php %all% %revert%
if 2 == %select% php ../php/vcbuild.php "pf_simple" %revert%
if 3 == %select% php ../php/vcbuild.php "pf_core" %revert%
if 4 == %select% php ../php/vcbuild.php "pf_cores" %revert%
if 5 == %select% php ../php/vcbuild.php "plugin_lua" %revert%

pause