#include "script/lua/net.h"
#include "script/lua/export.h"

namespace script {

namespace lua {

bool export_globals() {
  __ENTER_FUNCTION
    if (!register_functiontable_net()) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace lua 

} //namespace script
