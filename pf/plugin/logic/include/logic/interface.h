/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id interface.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/04/29 12:12
 * @uses logic interface class
 */
#ifndef LOGIC_INTERFACE_H_
#define LOGIC_INTERFACE_H_

namespace logic {

class Interface {

 public:
   Interface() {};
   virtual ~Interface() {};

 public:

   //Get logic system id.
   virtual uint16_t getid() const = 0;

   //Data load for logic.
   virtual bool dataload() = 0;

   //Get moudle pointer.
   virtual ModuleInterface *module() = 0;

   //Get view pointer.
   virtual ViewInterface *view() = 0;

   //Get controller pointer.
   virtual ControllerInterface *controller() = 0;

   //Get logic status.
   virtual int8_t getstatus() const = 0;

};

}; //namespace logic

#endif //LOGIC_INTERFACE_H_
