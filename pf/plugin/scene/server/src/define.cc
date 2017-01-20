#include "scene/define.h"
#include "pf/base/string.h"
#include "pf/net/socket/inputstream.h"
#include "pf/net/socket/outputstream.h"

namespace scene {

rect_struct::rect_struct() {
  clear();
}

void rect_struct::clear() {
  left = right = top = bottom = .0f;
}

bool rect_struct::iscontain(float x, float z) const {
  bool result = ((x >= left) && (x < right) && (z >= top) && (z < bottom));
  return result;
}

region_struct::region_struct() {
  clear();
}

void region_struct::clear() {
  id = scriptid = static_cast<uint32_t>(ID_INVALID);
  rect.clear();
}

bool region_struct::iscontain(float x, float z) const {
  bool result = rect.iscontain(x, z);
  return result;
}

regionconfig_struct::regionconfig_struct() {
  memset(filename, 0, sizeof(filename));
  region = NULL;
  count = 0;
}

regionconfig_struct::~regionconfig_struct() {
  clear();
}

void regionconfig_t::clear() {
  memset(filename, 0, sizeof(filename));
  SAFE_DELETE_ARRAY(region);
  count = 0;
}

load_struct::load_struct() {
  clear();
}

void load_struct::clear() {
  memset(map_filename, 0, sizeof(map_filename));
  memset(monster_filename, 0, sizeof(monster_filename));
  memset(region_filename, 0, sizeof(region_filename));
  memset(pkregion_filename, 0, sizeof(pkregion_filename));
  is_pkregion_exist = false;
}

initdata_struct::initdata_struct() {
  clear();
}

void initdata_struct::clear() {
  datatype = kTypeLogic;
}
  
bool initdata_struct::write(pf_net::socket::OutputStream &outputstream) {
  __ENTER_FUNCTION
    outputstream.write_int8(datatype);
    return true;
  __LEAVE_FUNCTION
    return false;
}
  
bool initdata_struct::read(pf_net::socket::InputStream &inputstream) {
  __ENTER_FUNCTION
    datatype = inputstream.read_int8();
    return true;
  __LEAVE_FUNCTION
    return false;
}
  
uint32_t initdata_struct::getsize() const {
  uint32_t result = sizeof(datatype);
  return result;
}

variable_struct::variable_struct() {
  clear();
}

void variable_struct::clear() {
  memset(this, 0, sizeof(*this));
}

variableopt_struct::variableopt_struct() {
  clear();
}

void variableopt_struct::clear() {
  variable.clear();
  dirty = false;
}

int32_t variableopt_struct::get(int32_t index) {
  Assert(index >= 0);
  Assert(index < SCENE_VARIABLE_MAX);
  return variable.values[index];
}

void variableopt_struct::set(int32_t index, int32_t value) {
  Assert(index >= 0);
  Assert(index < SCENE_VARIABLE_MAX);
  variable.values[index] = value;
}

pkregion_struct::pkregion_struct() {
  clear();
}

void pkregion_struct::clear() {
  id = ID_INVALID;
  type = ID_INVALID;
  base_campid = CAMP_INVALID;
  rect.clear();
}

bool pkregion_struct::iscontain(float x, float z) const {
  bool result = rect.iscontain(x, z);
  return result;
}

pkregion_config_struct::pkregion_config_struct() {
  memset(filename, 0, sizeof(filename));
  count = 0;
  sceneid = -1;
  pkregion = NULL;
}

pkregion_config_struct::~pkregion_config_struct() {
  clear();
}

void pkregion_config_struct::clear() {
  __ENTER_FUNCTION
    memset(filename, 0, sizeof(filename));
    SAFE_DELETE_ARRAY(pkregion);
    count = 0;
    sceneid = -1;
  __LEAVE_FUNCTION
}

fuben_data_struct::fuben_data_struct() {
  clear();
}

void fuben_data_struct::clear() {
  __ENTER_FUNCTION
    source_sceneid = ID_INVALID;
    teamleader = ID_INVALID;
    teamid = ID_INVALID;
    nouser_closetime = 10000;
    scenetimer = 0;
    memset(param, 0, sizeof(param));
    for (int32_t i = 0; i < SCENE_FUBEN_DATA_COPYMEMBER_MAX; ++i) {
      memberguid[i] = ID_INVALID;
    }
    worldid = ID_INVALID;
    bakscene = ID_INVALID;
    bakposition.clear();
    playerdisconnet = false;
  __LEAVE_FUNCTION
}

fuben_data_t &fuben_data_struct::operator = (const fuben_data_t &data) {
  __ENTER_FUNCTION
    if (this == &data) return *this;
    clear();
    source_sceneid = data.source_sceneid;
    teamleader = data.teamleader;
    teamid = data.teamid;
    nouser_closetime = data.nouser_closetime;
    scenetimer = data.scenetimer;
    memcpy(param, data.param, sizeof(param));
    memcpy(memberguid, data.memberguid, sizeof(memberguid));
    worldid = data.worldid;
    bakscene = data.bakscene;
    bakposition = data.bakposition;
    playerdisconnet = data.playerdisconnet;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

bool fuben_data_struct::is_havemember(guid_t guid) {
  __ENTER_FUNCTION
    bool result = false;
    for (int32_t i = 0; i < SCENE_FUBEN_DATA_COPYMEMBER_MAX; ++i) {
      if (memberguid[i] == guid) {
        result = true;
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

void fuben_data_struct::add_memberguid(guid_t guid) {
  __ENTER_FUNCTION
    for (int32_t i = 0; i < SCENE_FUBEN_DATA_COPYMEMBER_MAX; ++i) { 
      if (ID_INVALID == memberguid[i]) {
        memberguid[i] = guid;
        break;
      }
    }
  __LEAVE_FUNCTION
}

fuben_select_struct::fuben_select_struct() {
  clear();
}

void fuben_select_struct::clear() {
  sceneload.clear();
  scene_datatype = 0;
  fuben_data.clear();
}

void fuben_select_struct::set_sceneload(const char *filename) {
  __ENTER_FUNCTION
    Assert(filename != NULL);
    memset(sceneload.map_filename, 0, FILENAME_MAX);
    pf_base::string::safecopy(sceneload.map_filename, filename, FILENAME_MAX);
  __LEAVE_FUNCTION
}

performance_struct::performance_struct() {
  clear();
}

void performance_struct::clear() {
  sceneid = ID_INVALID;
  memset(ticks_, 0, sizeof(ticks_));
}

config_struct::config_struct() {
  memset(filename, 0, sizeof(filename));
}

clientres_struct::clientres_struct() {
  memset(nav_filename, 0, sizeof(nav_filename));
  id = ID_INVALID;
}

monsterconfig_struct::monsterconfig_struct() {
  memset(filename, 0, sizeof(filename));
  initdata = NULL;
  count = 0;
}

monsterconfig_struct::~monsterconfig_struct() {
  clear();
}

void monsterconfig_struct::clear() {
  memset(filename, 0, sizeof(filename));
  SAFE_DELETE_ARRAY(initdata);
  count = 0;
}

setting_struct::setting_struct() {
  clear();
}
  
void setting_struct::clear() {
  __ENTER_FUNCTION
    id = ID_INVALID;
    isactive = false;
    memset(name, 0, sizeof(name));
    memset(filename, 0, sizeof(filename));
    serverid = ID_INVALID;
    type = static_cast<uint8_t>(-1);
    threadindex = -1;
    clientres = -1;
    securitylevel = 0;
    restrictivemode = 0;
    safelevel = 0;
    punish = false;
    defaultx = 0;
    defaultz = 0;
    default_campid = CAMP_INVALID;
    broadcast_radius = 2;
  __LEAVE_FUNCTION
}
  
settings_struct::settings_struct() {
  __ENTER_FUNCTION
    scenes = NULL;
    count = 0;
    timercount_max = 0;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) hash[i] = ID_INVALID;
  __LEAVE_FUNCTION
}
  
settings_struct::~settings_struct() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(scenes);
    count = 0;
  __LEAVE_FUNCTION
}

chatitem_struct::chatitem_struct() {
  clear();
}

void chatitem_struct::clear() {
  packet = NULL;
  sourceid = targetid = static_cast<objectid_t>(ID_INVALID);
}

scaner_init_struct::scaner_init_struct() {
  clear();
}

void scaner_init_struct::clear() {
  __ENTER_FUNCTION
    scene = NULL;
    gridid = ID_INVALID;
    gridradius = 0;
    scanhuman = false;
  __LEAVE_FUNCTION
}

gridinfo_sturct::gridinfo_sturct() {
  clear();
}

void gridinfo_sturct::clear() {
  width = height = size = 0;
}

} //namespace scene
