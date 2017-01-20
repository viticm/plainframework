/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id managerinterface.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/04/29 12:41
 * @uses logic manager interface
 */
#ifndef LOGIC_MANAGERINTERFACE_H_
#define LOGIC_MANAGERINTERFACE_H_

namespace logic {

class ManagerInterface {

 public:
   ManagerInterface() {};
   virtual ~ManagerInterface() {};

 public:
   virtual SystemInterface *get(uint16_t id) = 0;
   virtual void _register(SystemInterface *system) = 0;
   virtual void unregister(SystemInterface *system) = 0;

};

}; //namespace logic

#endif //LOGIC_MANAGERINTERFACE_H_
