#include "pf/base/global.h"

namespace pf_base {

namespace global {

char net_modulename[FILENAME_MAX] = {0};
char script_modulename[FILENAME_MAX] = {0};
char performance_modulename[FILENAME_MAX] = {0};
char db_modulename[FILENAME_MAX] =  {0};
char engine_modulename[FILENAME_MAX] = {0};

const char *get_net_modulename() {
  __ENTER_FUNCTION
    if (strlen(net_modulename) > 0) return net_modulename;
    snprintf(net_modulename, 
             sizeof(net_modulename) - 1, 
             "net%s%s", 
             strlen(g_applicationname) > 0 ? "_" : "", 
             g_applicationname);
    return net_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_script_modulename() {
  __ENTER_FUNCTION
    if (strlen(script_modulename) > 0) return script_modulename;
    snprintf(script_modulename, 
             sizeof(script_modulename) - 1, 
             "net%s%s", 
             strlen(g_applicationname) > 0 ? "_" : "", 
             g_applicationname);
    return script_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_performanace_modulename() {
  __ENTER_FUNCTION
    if (strlen(performance_modulename) > 0) return performance_modulename;
    snprintf(performance_modulename, 
             sizeof(performance_modulename) - 1, 
             "net%s%s", 
             strlen(g_applicationname) > 0 ? "_" : "", 
             g_applicationname);
    return performance_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_db_modulename() {
  __ENTER_FUNCTION
    if (strlen(db_modulename) > 0) return db_modulename;
    snprintf(db_modulename, 
             sizeof(db_modulename) - 1, 
             "net%s%s", 
             strlen(g_applicationname) > 0 ? "_" : "", 
             g_applicationname);
    return db_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_engine_modulename() {
  __ENTER_FUNCTION
    if (strlen(engine_modulename) > 0) return engine_modulename;
    snprintf(engine_modulename, 
             sizeof(engine_modulename) - 1, 
             "net%s%s", 
             strlen(g_applicationname) > 0 ? "_" : "", 
             g_applicationname);
    return engine_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

}; //namespace global

} //namespace pf_base
