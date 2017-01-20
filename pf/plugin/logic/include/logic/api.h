/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id api.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/04/29 12:51
 * @uses all logic interface api
 */
#ifndef LOGIC_API_H_
#define LOGIC_API_H_

#define LOGIC_MODULENAME "logic"

namespace logic {

class ModuleInterface;
class ViewInterface;
class ControllerInterface;
class Interface;
class SystemInterface;
class ManagerInterface;

//The view handle.
typedef bool (__stdcall *function_view)(void *data);

}; //namespace logic

#include "logic/moduleinterface.h"
#include "logic/viewinterface.h"
#include "logic/controllerinterface.h"
#include "logic/interface.h"
#include "logic/systeminterface.h"
#include "logic/managerinterface.h"

#endif //LOGIC_API_H_
