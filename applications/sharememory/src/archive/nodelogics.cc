#include "archive/nodelogic.h"

namespace archive {

/* global { */
template <>
bool NodeLogic<globaldata_t>::init_after() {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool NodeLogic<globaldata_t>::empty() {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool NodeLogic<globaldata_t>::fullflush(bool force, bool servercrash) {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool NodeLogic<globaldata_t>::tickflush() {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

/* } global */

} //namespace archive
