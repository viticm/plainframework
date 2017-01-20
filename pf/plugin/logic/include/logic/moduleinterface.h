/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id module.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/15 14:48
 * @uses logic system plugin module class
 */
#ifndef LOGIC_MODULEINTERFACE_H_
#define LOGIC_MODULEINTERFACE_H_

namespace logic {

class ModuleInterface {

 public:
   ModuleInterface() {};
   virtual ~ModuleInterface() {};

 public:

   //Get data pointer, cache or result from database.
   virtual void *getdata() = 0;

   //Set data pointer.
   virtual void setdata(void *data) = 0;

};

}; //namespace logic

#endif //LOGIC_MODULEINTERFACE_H_
