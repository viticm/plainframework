@echo off
title rename for visual studio(pap)
color 02
cls
echo select your need change model.
echo (1)all (2)gateway (3)pf_simple (4)pf_core (5)sharememory (6)login (7)center
set /p select=your select?
echo revert to standard?
set revert=-1
set /p revert=(yes/no)?
set all="billing sharememory login world server"
if 1 == %select% echo "php ../php/vcbuild.php" %all% %revert%
if 2 == %select% php ../php/vcbuild.php "gateway" %revert%
if 3 == %select% php ../php/vcbuild.php "pf_simple" %revert%
if 4 == %select% php ../php/vcbuild.php "pf_core" %revert%
if 5 == %select% php ../php/vcbuild.php "sharememory" %revert%
if 6 == %select% php ../php/vcbuild.php "login" %revert%
if 7 == %select% php ../php/vcbuild.php "center" %revert%

pause