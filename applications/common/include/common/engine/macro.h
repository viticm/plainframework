/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id macro.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/09/24 13:57
 * @uses macros for engine
*/
#ifndef COMMON_ENGINE_MACRO_H_
#define COMMON_ENGINE_MACRO_H_

//引擎内设置公用路径宏，因为比较通用，故弄成宏减少代码书写
#define ENGINE_SET_PUBLICPATH() { \
  char script_rootpath[FILENAME_MAX] = {0}; \
  snprintf(script_rootpath, \
           sizeof(script_rootpath) - 1, \
           "public/data/%s/script", \
           APPLICATION_NAME); \
  setconfig(ENGINE_CONFIG_SCRIPT_ROOTPATH, script_rootpath); \
  char setting_rootpath[FILENAME_MAX] = {0}; \
  snprintf(setting_rootpath, \
           sizeof(setting_rootpath) - 1, \
           "public/data/%s/setting", \
           APPLICATION_NAME); \
  setconfig(ENGINE_CONFIG_SETTING_ROOTPATH, setting_rootpath); \
}

#define ENGINE_SET_SCRIPTVALUE() { \
  SCRIPT_LUASYSTEM_POINTER \
    ->get_vm() \
    ->register_globalvalue("SCRIPT_PATH", \
        getconfig_stringvalue(ENGINE_CONFIG_SCRIPT_ROOTPATH)); \
  SCRIPT_LUASYSTEM_POINTER \
    ->get_vm() \
    ->register_globalvalue("SETTING_PATH", \
        getconfig_stringvalue(ENGINE_CONFIG_SETTING_ROOTPATH)); \
}

#endif //COMMON_ENGINE_MACRO_H_
