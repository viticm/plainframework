/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id net.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/09/24 17:51
 * @uses script lua net module
*/
#ifndef SCRIPT_LUA_NET_H_
#define SCRIPT_LUA_NET_H_

#include "script/lua/config.h"
#include "pf/script/lua/system.h"

namespace script {

namespace lua {

int32_t net_globalexcute(lua_State *L); //执行全局查询脚本函数，针对游戏服务器
bool register_functiontable_net();

}; //namespace lua

}; //namespace script

#endif //SCRIPT_LUA_NET_H_
