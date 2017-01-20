/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id view.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/04/15 14:53
 * @uses logic system plugin view class
 */
#ifndef LOGIC_VIEWINTERFACE_H_
#define LOGIC_VIEWINTERFACE_H_

#include "pf/base/hashmap/template.h"

namespace logic {

class ViewInterface {

 public:
   ViewInterface() { handles_.init(2048); };
   virtual ~ViewInterface() {};

 public:

   //Make the ui page with data values.
   virtual bool make(uint16_t id, void *data) {
     if (!handles_.isfind(id)) return false;
     function_view function = handles_.get(id);
     function(data);
     return true;
   };

   //Register ui make handle.
   virtual bool _register(uint16_t id, function_view function) {
     if (handles_.isfind(id)) return false;
     handles_.add(id, function);
     return true;
   };

   //Get the ui handle.
   virtual function_view get(uint16_t id) {
     if (!handles_.isfind(id)) return NULL;
     return handles_.get(id);
   };

 private:
   pf_base::hashmap::Template<uint16_t, function_view> handles_;

};

}; //namespace logic 

#endif //LOGIC_VIEWINTERFACE_H_
