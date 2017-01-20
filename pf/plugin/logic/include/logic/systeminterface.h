/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id system.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/04/15 18:40
 * @uses logic system plugin core class
 */
#ifndef LOGIC_SYSTEMINTERFACE_H_
#define LOGIC_SYSTEMINTERFACE_H_

#include "logic/interface.h"

namespace logic {

class SystemInterface {

 public:
   SystemInterface() {};
   virtual ~SystemInterface() {};

 public:

   //Get logic system id.
   virtual uint16_t getid() const = 0;

   //Register a logic to system.
   virtual void _register(Interface *logic) = 0;

   //Unregister a logic.
   virtual void unregister(Interface *logic) = 0;

   //Get a logic by id.
   virtual Interface *get(uint16_t id) = 0;

};

}; //namespace logic

#endif //LOGIC_SYSTEMINTERFACE_H_
