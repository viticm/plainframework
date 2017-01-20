#include "structure/define.h"

namespace structure {

position_struct::position_struct() {
  clear();
}

position_struct::position_struct(float _x, float _z) {
  x = _x;
  z = _z;
}

void position_struct::clear() {
  x = z = .0f;
}

position_t &position_struct::operator = (position_t const &position) {
  x = position.x;
  z = position.z;
  return *this;
}

position_t &position_struct::operator = (const position_t *position) {
  if (position) {
    x = position->x;
    z = position->z;
  }
  return *this;
}

bool position_struct::operator == (position_t &position) {
  bool result = (fabs(x - position.x) + fabs(z - position.z)) < 0.0001f;
  return result;
}

bool position_struct::operator != (position_t &position) {
  bool result = (fabs(x - position.x) + fabs(z - position.z)) >= 0.0001f;
  return result;
}

bool position_struct::operator == (const position_t &position) {
  bool result = (fabs(x - position.x) + fabs(z - position.z)) < 0.0001f;
  return result;
}

uint32_t position_struct::getsize() {
  return sizeof(float) + sizeof(float);
}

uint32_t position_struct::get_sizemax() {
  return sizeof(float) + sizeof(float);
}

bool position_struct::write(pf_net::socket::OutputStream &outputstream) {
  __ENTER_FUNCTION
    outputstream.write_float(x);
    outputstream.write_float(z);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool position_struct::read(pf_net::socket::InputStream &inputstream) {
  __ENTER_FUNCTION
    x = inputstream.read_float();
    z = inputstream.read_float();
    return true;
  __LEAVE_FUNCTION
    return false;
}

object_struct::object_struct() {
  clear();
}

void object_struct::clear() {
  position.clear();
  direction = .0f;
  viewgroup = 0;
  data = NULL;
}

objectlist_node_struct::objectlist_node_struct() {
  clear();
}

objectlist_node_struct::objectlist_node_struct(Object *_node) {
  clear();
  node = _node;
}

void objectlist_node_struct::clear() {
  node = NULL;
  next = prev = NULL;
}

campdata_struct::campdata_struct() {
  clear();
}

void campdata_struct::clear() {
  id = ID_INVALID;
  pkmode = 0;
  reserve1 = reserve2 = -1;
}

campid_t campdata_struct::getid() const {
  return id;
}

uint8_t campdata_struct::get_pkmode() const {
  return pkmode;
}

void campdata_struct::set_pkmode(uint8_t mode) {
  pkmode = mode;
}

bool campdata_struct::operator == (const campdata_t &in) {
  if (id != in.id || pkmode != in.pkmode) return false;
  return true;
}

bool campdata_struct::operator != (const campdata_t &in) {
  if (id != in.id || pkmode != in.pkmode) return true;
  return false;
}

campdata_t &campdata_struct::operator = (const campdata_t &in) {
  if (&in != this) {
    id = in.id;
    pkmode = in.pkmode;
  }
  return *this;
}

uint32_t campdata_struct::getsize() const {
  return sizeof(id) + sizeof(pkmode) + sizeof(reserve1) + sizeof(reserve2);
}

bool campdata_struct::write(pf_net::socket::OutputStream &outputstream) {
  __ENTER_FUNCTION
    outputstream.write((char *)&id, sizeof(id));
    outputstream.write_uint8(pkmode);
    outputstream.write_int32(reserve1);
    outputstream.write_int32(reserve2);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool campdata_struct::read(pf_net::socket::InputStream &inputstream) {
  __ENTER_FUNCTION
    inputstream.read((char *)&id, sizeof(id));
    pkmode = inputstream.read_uint8();
    reserve1 = inputstream.read_int32();
    reserve2 = inputstream.read_int32();
    return true;
  __LEAVE_FUNCTION
    return false;
}

vrect_struct::vrect_struct() {
  clear();
}

void vrect_struct::clear() {
  startx = startz = endx = endz = 0;
}

bool vrect_struct::iscontinue(int32_t x, int32_t z) {
  if (x < startx || x > endx || z < startz || z > endz)
    return false;
  else
    return true;
}

objectlist_id_struct::objectlist_id_struct() {
  clear();
}

void objectlist_id_struct::clear() {
  count = 0;
  memset(ids, ID_INVALID, sizeof(ids));
}

uint32_t objectlist_id_struct::getsize() const {
  return sizeof(ids);
}

bool objectlist_id_struct::write(pf_net::socket::OutputStream &outputstream) {
  __ENTER_FUNCTION
    outputstream.write_int32(count);
    for (int32_t i = 0; i < count; ++i) {
      outputstream.write((char *)&ids[i], sizeof(objectid_t));
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool objectlist_id_struct::read(pf_net::socket::InputStream &inputstream) {
  __ENTER_FUNCTION
    count = inputstream.read_int32();
    for (int32_t i = 0; i < count; ++i) {
      inputstream.read((char *)&ids[i], sizeof(objectid_t));
    }
  __LEAVE_FUNCTION
    return false;
}

dueldata_struct::dueldata_struct() {
  clear();
}

void dueldata_struct::clear() {
  __ENTER_FUNCTION
    opponent_objectid = ID_INVALID;
    opponent_guid = ID_INVALID;
    regionid = ID_INVALID;
    sceneid = ID_INVALID;
    starttime = 0;
    openlevel = 0;
  __LEAVE_FUNCTION
}

dueldata_t &dueldata_struct::operator = (const dueldata_t &in) {
  __ENTER_FUNCTION
    opponent_objectid = in.opponent_objectid;
    opponent_guid = in.opponent_guid;
    regionid = in.regionid;
    sceneid = in.sceneid;
    starttime = in.starttime;
    openlevel = in.openlevel;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

bool dueldata_struct::operator == (const dueldata_t &in) {
  __ENTER_FUNCTION
    if (opponent_objectid != in.opponent_objectid &&
        opponent_guid != in.opponent_guid &&
        regionid != in.regionid &&
        sceneid != in.sceneid) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool dueldata_struct::operator != (const dueldata_t &in) {
  __ENTER_FUNCTION
    bool result = !(*this == in);
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint32_t dueldata_struct::getsize() const {
  uint32_t result = sizeof(opponent_objectid) +
                    sizeof(opponent_guid) +
                    sizeof(regionid) +
                    sizeof(sceneid) +
                    sizeof(starttime) +
                    sizeof(openlevel);
  return result;
}

bool dueldata_struct::write(pf_net::socket::OutputStream &outputstream) {
  __ENTER_FUNCTION
    outputstream.write((char *)&opponent_objectid, sizeof(opponent_objectid));
    outputstream.write((char *)&opponent_guid, sizeof(opponent_guid));
    outputstream.write_uint8(regionid);
    outputstream.write((char *)&sceneid, sizeof(sceneid));
    outputstream.write_uint32(starttime);
    outputstream.write_uint8(openlevel);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool dueldata_struct::read(pf_net::socket::InputStream &inputstream) {
  __ENTER_FUNCTION
    inputstream.read((char *)&opponent_objectid, sizeof(opponent_objectid));
    inputstream.read((char *)&opponent_guid, sizeof(opponent_guid));
    regionid = inputstream.read_uint8();
    inputstream.read((char *)&sceneid, sizeof(sceneid));
    starttime = inputstream.read_uint32();
    openlevel = inputstream.read_uint8();
    return true;
  __LEAVE_FUNCTION
    return false;
}

human_campdata_struct::human_campdata_struct() {
  clear();
}

human_campdata_struct::human_campdata_struct(campdata_t *in) {
  __ENTER_FUNCTION
    if (in) {
      id = in->id;
      pkmode = in->pkmode;
    }
    clear();
  __LEAVE_FUNCTION
}

void human_campdata_struct::clear() {
  __ENTER_FUNCTION
    objectid = ID_INVALID;
    level = 0;
    sceneid = ID_INVALID;
    scene_safelevel = 0;
    scene_restrictivemode = 0;
    guid = ID_INVALID;
    teamid = ID_INVALID;
    saferegion_id = 0;
    saferegion_campid = CAMP_INVALID;
    dueldata.clear();
  __LEAVE_FUNCTION
}

human_campdata_t &human_campdata_struct::operator = (const campdata_t &in) {
  __ENTER_FUNCTION
    if (&in) {
      id = in.id;
      pkmode = in.pkmode;
    }
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

human_campdata_t &human_campdata_struct::operator = (
    const human_campdata_t &in) {
  __ENTER_FUNCTION
    objectid = in.objectid;
    id = in.id;
    pkmode = in.pkmode;
    level = in.level;
    sceneid = in.sceneid;
    scene_safelevel = in.scene_safelevel;
    guid = in.guid;
    teamid = in.teamid;
    dueldata = in.dueldata;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

objectlist_struct::objectlist_struct() {
  clear();
}
  
void objectlist_struct::clear() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < STRUCTURE_OBJECTLIST_MAX; ++i)
      list[i] = NULL;
    count = 0;
  __LEAVE_FUNCTION
}

objectsingle_manager_init_struct::objectsingle_manager_init_struct() {
  clear();
}

void objectsingle_manager_init_struct::clear() {
  length = 0;
  scene = NULL;
  logicinterval = 0;
  currenttime = 0;
}

objectmanager_init_struct::objectmanager_init_struct() {
  clear();
}

void objectmanager_init_struct::clear() {
  memset(initcount, 0, sizeof(initcount));
}

aidata_struct::aidata_struct() {
  clear();
}
  
void aidata_struct::clear() {
  __ENTER_FUNCTION
    eventsource = ID_INVALID;
    eventparam = -1;
    targetgroup_count = 0;
    for (int8_t i = 0; i < AI_TARGETGROUP_COUNTMAX; ++i)
      targetgroup[i] = NULL;
  __LEAVE_FUNCTION
}

} //namespace structure
