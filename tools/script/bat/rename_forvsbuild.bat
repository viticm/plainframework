@echo off
title rename for visual studio(plain framework)
color 02
cls
echo select your need change model.
echo (1)all (2)billing (3)pf_simple (4)pf_core (5)share (6)login (7)center (8) plugin_pak (9) plugin_scene (10) plugin_structure (11)database (12)server (13)client (14)pf_cores (15)libclient
set /p select=your select?
echo revert to standard?
set revert=-1
set /p revert=(yes/no)?
set all="pf_core|pf_cores|pf_simple|billing|share|login|center|plugin_pak|plugin_scene|plugin_structure|database|server|client|libclient"
if 1 == %select% php ../php/vcbuild.php %all% %revert%
if 2 == %select% php ../php/vcbuild.php "billing" %revert%
if 3 == %select% php ../php/vcbuild.php "pf_simple" %revert%
if 4 == %select% php ../php/vcbuild.php "pf_core" %revert%
if 5 == %select% php ../php/vcbuild.php "share" %revert%
if 6 == %select% php ../php/vcbuild.php "login" %revert%
if 7 == %select% php ../php/vcbuild.php "center" %revert%
if 8 == %select% php ../php/vcbuild.php "plugin_pak" %revert%
if 9 == %select% php ../php/vcbuild.php "plugin_scene" %revert%
if 10 == %select% php ../php/vcbuild.php "plugin_structure" %revert%
if 11 == %select% php ../php/vcbuild.php "database" %revert%
if 12 == %select% php ../php/vcbuild.php "server" %revert%
if 13 == %select% php ../php/vcbuild.php "client" %revert%
if 14 == %select% php ../php/vcbuild.php "pf_cores" %revert%
if 15 == %select% php ../php/vcbuild.php "libclient" %revert%

pause