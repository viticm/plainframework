/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id controller.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/15 14:49
 * @uses logic system plugin controller class
 */
#ifndef LOGIC_CONTROLLERINTERFACE_H_
#define LOGIC_CONTROLLERINTERFACE_H_

namespace logic {

class ControllerInterface {

 public:
   ControllerInterface() {};
   virtual ~ControllerInterface() {};

 public:

   //Setup layout and show ui page.
   virtual bool setuplayout() = 0;

   //Request events.
   virtual int32_t request(uint8_t type, void *data) = 0;

   //Set core.
   virtual void set_core(Interface *core) = 0;

   //Get core.
   virtual Interface *get_core() = 0;

};

}; //namespace logic

#endif //LOGIC_CONTROLLERINTERFACE_H_
