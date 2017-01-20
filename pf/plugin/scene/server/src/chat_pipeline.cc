#include "pf/sys/thread.h"
#include "structure/define.h"
#include "scene/chat_pipeline.h"

namespace scene {

ChatPipeline::ChatPipeline() {    
  __ENTER_FUNCTION
    validcount_ = 0;
    scene_ = NULL;
    chatitems_ = NULL;
    chathead_ = 0;
    chattail_ = 0;
    tipsitems_ = NULL;
    tipshead_ = 0;
    tipstail_ = 0;
  __LEAVE_FUNCTION
}

ChatPipeline::~ChatPipeline() {
  __ENTER_FUNCTION
    clear();
    scene_ = NULL;
    SAFE_DELETE_ARRAY(chatitems_);
    SAFE_DELETE_ARRAY(tipsitems_);
  __LEAVE_FUNCTION
}

void ChatPipeline::clear() {    
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    validcount_ = 0;
    chathead_ = 0;
    chattail_ = 0;
    for (int32_t i = 0; i < SCENE_CHAT_ITEM_SIZE; ++i) {
      chatitems_[i].packet = NULL;
      chatitems_[i].sourceid = static_cast<objectid_t>(ID_INVALID);
      chatitems_[i].targetid = static_cast<objectid_t>(ID_INVALID);
    }
    tipshead_ = 0;
    tipstail_ = 0;
    for (int32_t i = 0; i < SCENE_CHAT_TIPS_SIZE; ++i) {
      tipsitems_[i].packet = NULL;
      tipsitems_[i].sourceid = static_cast<objectid_t>(ID_INVALID);
      tipsitems_[i].targetid = static_cast<objectid_t>(ID_INVALID);
    }
  __LEAVE_FUNCTION
}

bool ChatPipeline::init(Scene *scene) {
  __ENTER_FUNCTION
    chatitems_ = new chatitem_t[SCENE_CHAT_ITEM_SIZE]; 
    if (NULL == chatitems_) return false;
    tipsitems_ = new chatitem_t[SCENE_CHAT_TIPS_SIZE];
    if (NULL == tipsitems_) return false;
    scene_ = scene;
    Assert(scene_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ChatPipeline::heartbeat(uint32_t time) {
  USE_PARAM(time);
  return true;
}

Scene *ChatPipeline::getscene() {
  return scene_;
}

void ChatPipeline::dec_validcount(int32_t count) {
  validcount_ -= count;
}

bool ChatPipeline::packet_send(pf_net::packet::Base *packet, 
                             objectid_t sourceid, 
                             objectid_t targetid, 
                             int32_t type) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    if (structure::kMessageTypeChat == type) {
      Assert(chathead_ >= 0 && chathead_ < SCENE_CHAT_ITEM_SIZE);
      Assert(chattail_ >= 0 && chattail_ < SCENE_CHAT_ITEM_SIZE);
      if (chatitems_[chathead_].packet != NULL) return false;
      chatitems_[chathead_].packet = packet;
      chatitems_[chathead_].sourceid = sourceid;
      chatitems_[chathead_].targetid = targetid;
      ++chathead_;
      if (chathead_ > SCENE_CHAT_ITEM_SIZE) chathead_ = 0;
    }
    if (structure::kMessageTypeTips == type) {
      Assert(tipshead_ >= 0 && tipshead_ < SCENE_CHAT_TIPS_SIZE);
      Assert(tipstail_ >= 0 && tipstail_ < SCENE_CHAT_TIPS_SIZE);
      if (tipsitems_[tipshead_].packet != NULL) return false;
      tipsitems_[tipshead_].packet = packet;
      tipsitems_[tipshead_].sourceid = sourceid;
      tipsitems_[tipshead_].targetid = targetid;
      ++tipshead_;
      if (tipshead_ > SCENE_CHAT_TIPS_SIZE) tipshead_ = 0;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ChatPipeline::packet_chatrecv(pf_net::packet::Base *&packet, 
                                   objectid_t &sourceid, 
                                   objectid_t &targetid) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    Assert(chathead_ >= 0 && chathead_ < SCENE_CHAT_ITEM_SIZE);
    Assert(chattail_ >= 0 && chattail_ < SCENE_CHAT_ITEM_SIZE);
    if (NULL == chatitems_[chattail_].packet) return false;
    packet = chatitems_[chattail_].packet;
    sourceid = chatitems_[chattail_].sourceid;
    targetid = chatitems_[chattail_].targetid;
    chatitems_[chattail_].packet = NULL;
    chatitems_[chattail_].sourceid = static_cast<objectid_t>(ID_INVALID);
    chatitems_[chattail_].targetid = static_cast<objectid_t>(ID_INVALID);
    ++chattail_;
    if (chattail_ > SCENE_CHAT_ITEM_SIZE) chattail_ = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ChatPipeline::packet_tipsrecv(pf_net::packet::Base *&packet, 
                                   objectid_t &sourceid, 
                                   objectid_t &targetid) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    Assert(tipshead_ >= 0 && tipshead_ < SCENE_CHAT_TIPS_SIZE);
    Assert(tipstail_ >= 0 && tipstail_ < SCENE_CHAT_TIPS_SIZE);
    if (NULL == tipsitems_[tipstail_].packet) return false;
    packet = tipsitems_[tipstail_].packet;
    sourceid = tipsitems_[tipstail_].sourceid;
    targetid = tipsitems_[tipstail_].targetid;
    tipsitems_[tipstail_].packet = NULL;
    tipsitems_[tipstail_].sourceid = static_cast<objectid_t>(ID_INVALID);
    tipsitems_[tipstail_].targetid = static_cast<objectid_t>(ID_INVALID);
    ++tipstail_;
    if (tipstail_ > SCENE_CHAT_TIPS_SIZE) tipstail_ = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace scene
