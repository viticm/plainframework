#include "pf/sys/memory/sharemap.h"

using namespace pf_sys::memory::share;
_map_node_struct::_map_node_struct() :
  hash{0},
  prev{INDEX_INVALID},
  next{INDEX_INVALID} {
}
  
void _map_node_struct::clear() {
  prev = INDEX_INVALID;
  hash = 0;
  next = INDEX_INVALID;
}  

map_bucket_struct::map_bucket_struct() :
  cur(INDEX_INVALID) {
}
  
void map_bucket_struct::clear() {
  cur = INDEX_INVALID;
}

MapPool::MapPool() {
  //do nothing
}

MapPool::~MapPool() {
  //do nothing
}

bool MapPool::init(uint32_t key, 
                   size_t size, 
                   size_t datasize, 
                   bool create) {
  if (ready_) return true;
  set_data_extend_size(datasize);
  ref_obj_pointer_ = std::shared_ptr<share::Base>(new Base());
  Assert(ref_obj_pointer_);
  if (!ref_obj_pointer_) return false;
  bool result = true;
  bool needinit = false;
  auto headersize = sizeof(header_t);
  auto bucketsize = sizeof(map_bucket_t) * size;
  auto full_datasize = (sizeof(map_node_t) + data_extend_size_) * size;
  auto memorysize = headersize + bucketsize + full_datasize;
  result = ref_obj_pointer_->attach(key, memorysize);
  if (create && !result) {
    result = ref_obj_pointer_->create(key, memorysize);
    needinit = true;
  } else if (!result) {
    return false;
  }
  if (!result && GLOBALS["app.cmdmodel"] == kCmdModelClearAll) {
    return true;
  } else if (!result) {
    SLOW_ERRORLOG(
        "sharememory", 
        "[sys][sharememory] (UnitPool::init) failed");
    Assert(result);
    return result;
  }    
  size_ = size;
  objs_ = new map_node_t * [size_];
  if (is_null(objs_)) return false;
  map_bucket_t *buckets = reinterpret_cast<map_bucket_t *>(getbuckets());
  for (decltype(size_) i = 0; i < size_; ++i) {
    char *pointer = getdata(sizeof(map_node_t) + data_extend_size_, i);
    objs_[i] = reinterpret_cast<map_node_t *>(pointer);
    if (is_null(objs_[i])) {
      Assert(false);
      return false;
    }
    objs_[i]->set_extend_size(data_extend_size_);
    if (data_extend_size_ > 0 && needinit) {
      memset(&pointer[sizeof(map_node_t)], 0, data_extend_size_);
    }
    if (needinit) {
      buckets[i].clear();
      objs_[i]->init();
    }
  }    
  key_ = key;
  ready_ = true;
  return true;
}

void MapPool::delete_obj(map_node_t *obj) {
  Assert(obj != nullptr && ref_obj_pointer_ != nullptr);
  header_t *header = ref_obj_pointer_->header();
  Assert(header);
  unique_lock<header_t> auto_lock(*header, kFlagMixedWrite);
  Assert(header->pool_position > 0);
  if (is_null(obj) || header->pool_position <= 0) {
    return;
  }
  uint32_t delete_index = obj->get_pool_id(); //this function 
                                              //must define in T*
  Assert(delete_index < header->pool_position);
  if (delete_index >= header->pool_position) {
    return;
  }
  --(header->pool_position);
  map_node_t *node = objs_[delete_index];

  //Safe to swap list.
  map_node_t *swapnode = objs_[header->pool_position];
  uint32_t datasize = sizeof(map_node_t) + data_extend_size_;
  char *pointer = reinterpret_cast<char *>(node);
  char *swappointer = reinterpret_cast<char *>(swapnode);
  memcpy(pointer, swappointer, datasize);
  if (node) {
    if (node->data.prev != ID_INVALID) {
      map_node_t *prevnode = get_obj(node->data.prev);
      unique_lock< map_node_t > auto_lock(*prevnode, kFlagMixedWrite);
      prevnode->data.next = delete_index;
    }
  }

  //Safe to swap bucket.
  uint32_t _bucketindex = bucketindex(objs_[delete_index]->data.hash);
  char *_buckets = getbuckets();
  map_bucket_t *buckets = reinterpret_cast<map_bucket_t *>(_buckets);
  if (buckets[_bucketindex].cur == 
      static_cast<int32_t>(header->pool_position)) {
    buckets[_bucketindex].cur = delete_index;
  }
}
uint32_t MapPool::bucketindex(uint32_t hash) {
  uint32_t index = hash & (size() - 1);
  return index;
  return 0;
}
   
uint32_t MapPool::hashkey(const char *str) {
  uint32_t hash = 5381;
  while (*str) {
    hash = ((hash << 5) + hash) ^ *str++;
  }
  return hash;
}

char *MapPool::getdata(uint32_t size, uint32_t index) {
  char *result = nullptr;
  if (!ref_obj_pointer_) return result;
  char *data = ref_obj_pointer_->get();
  uint32_t bucketsize = sizeof(map_bucket_t) * size_;
  char *realdata = data + bucketsize;
  uint32_t data_fullsize = (sizeof(map_node_t) + data_extend_size_) * size_;
  Assert(size > 0);
  Assert(size * index <= data_fullsize - size);
  result = (size <= 0 || size * index > data_fullsize - size) ? 
            nullptr : 
            realdata + size * index;
  return result;
}

char *MapPool::getbuckets() {
  char *data = ref_obj_pointer_->get();
  return data;
}

//Map class.
 size_t keysize_;
   size_t valuesize_;
   MapPool *pool_;
   map_bucket_t *buckets_;
   bool ready_;

Map::Map() :
  keysize_{0},
  valuesize_{0},
  pool_{nullptr},
  buckets_{nullptr},
  ready_{false} {
}

Map::~Map() {
}

bool Map::init(uint32_t key, 
               size_t size, 
               size_t keysize, 
               size_t valuesize,
               bool create) {
  if (ready_) return true;
  pool_ = new MapPool;
  Assert(pool_);
  uint32_t datasize = (keysize + 1) + (valuesize + 1);
  bool result = pool_->init(key, size, datasize, create);
  if (!result) return result;
  buckets_ = reinterpret_cast<map_bucket_t *>(pool_->getbuckets());
  Assert(buckets_);
  keysize_ = keysize;
  valuesize_ = valuesize;
  ready_ = true;
  return true;
}

void Map::clear() {
  keysize_ = 0;
  valuesize_ = 0;
  buckets_ = nullptr;
}
   
const char *Map::get(const char *key) {
  const char *result = nullptr;
  int32_t index = getref(key);
  if (index != INDEX_INVALID) {
    map_node_t *node = pool_->get_obj(index);
    uint32_t valuepos = sizeof(map_node_t) + keysize_ + 2;
    result = reinterpret_cast<char *>(node) + valuepos;
  }
  return result;
}
   
bool Map::set(const char *key, const char *value) {
  auto index = getref(key);
  auto valuesize = static_cast<uint32_t>(strlen(value));
  map_node_t *node = nullptr;
  auto valuepos = sizeof(map_node_t) + keysize_ + 2;
  valuesize = valuesize > valuesize_ ? valuesize_ : valuesize;
  if (index != INDEX_INVALID) {
    node = pool_->get_obj(index);
    Assert(node);
    if (is_null(node)) return false;
    unique_lock< map_node_t > auto_lock(*node, kFlagMixedWrite);
    char *pointer = reinterpret_cast<char *>(node) + valuepos;
    memset(pointer, 0, valuesize_ + 1);
    memcpy(pointer, value, valuesize);
  } else {
    node = newnode(key, value);
    if (is_null(node)) return false;
    addnode(node);
  }
  return true;
}

void Map::remove(const char *key) {
    map_node_t *node = nullptr;
    int32_t index = getref(key);
    map_bucket_t *buckets = reinterpret_cast<map_bucket_t *>(pool_->getbuckets());
    header_t *header = pool_->get_header();
    unique_lock<header_t> auto_lock(*header, kFlagSelfWrite);
    uint32_t hash = 0;
    if (index != INDEX_INVALID) {
      node = pool_->get_obj(index);
      hash = node->data.hash;
      if (node->data.prev != INDEX_INVALID) {
        map_node_t *prevnode = pool_->get_obj(node->data.prev);
        unique_lock< map_node_t > auto_lock(*prevnode, kFlagMixedWrite);
        if (prevnode) prevnode->data.next = node->data.next;
      }
      uint32_t _bucketindex = pool_->bucketindex(hash);
      if (_bucketindex >= 0 && _bucketindex < pool_->size()) {
        unique_lock< header_t > auto_lock(*header, kFlagMixedWrite);
        map_bucket_t *bucket = &buckets[_bucketindex];
        if (bucket->cur == static_cast<int32_t>(node->get_pool_id())) 
          bucket->cur = node->data.next;
      }
      pool_->delete_obj(node);
      //node->data.clear();
    }
}
   
MapPool *Map::getpool() {
  return pool_;
}   

map_node_t *Map::newnode(const char *key, const char *value) {
    map_node_t *node = nullptr;
    uint32_t keypos = sizeof(map_node_t);
    uint32_t valuepos = keypos + keysize_ + 2;
    node = pool_->new_obj();
    if (is_null(node)) return node;
    char *pointer = reinterpret_cast<char *>(node);
    uint32_t keysize = static_cast<uint32_t>(strlen(key));
    uint32_t valuesize = static_cast<uint32_t>(strlen(value));
    uint32_t hash = pool_->hashkey(key);
    keysize = keysize > keysize_ ? keysize_ : keysize;
    valuesize = valuesize > valuesize_ ? valuesize_ : valuesize;
    unique_lock< map_node_t > auto_lock(*node, kFlagMixedWrite);
    node->data.hash = hash;
    memset(pointer + keypos, 0, keysize_ + 1);
    memcpy(pointer + keypos, key, keysize);
    memset(pointer + valuepos, 0, valuesize + 1);
    memcpy(pointer + valuepos, value, valuesize);
    return node;
}
   
void Map::addnode(map_node_t *node) {
  unique_lock< map_node_t > auto_lock(*node, kFlagMixedWrite);
  int32_t n = pool_->bucketindex(node->data.hash);
  map_bucket_t *buckets = reinterpret_cast<map_bucket_t *>(pool_->getbuckets());
  node->data.next = buckets[n].cur;
  if (node->data.next != INDEX_INVALID) {
    map_node_t *_node = pool_->get_obj(node->data.next);
    _node->data.prev = node->get_pool_id();
  }
  buckets[n].cur = node->get_pool_id();
}
   
int32_t Map::getref(const char *key) {
  int32_t result = INDEX_INVALID;
  auto hash = pool_->hashkey(key);
  map_bucket_t *buckets = reinterpret_cast<map_bucket_t *>(pool_->getbuckets());
  if (pool_->get_position() > 0) {
    uint32_t _bucketindex = pool_->bucketindex(hash);
    Assert(_bucketindex >= 0 && _bucketindex < pool_->size());
    map_bucket_t *bucket = &buckets[_bucketindex];
    uint32_t keypos = sizeof(map_node_t);
    int32_t index = bucket->cur;
    while (index != INDEX_INVALID) {
      map_node_t *node = pool_->get_obj(index);
      Assert(node);
      char *pointer = reinterpret_cast<char *>(node);
      if (node->data.hash == hash && 0 == strcmp(pointer + keypos, key)) {
        result = index;
        break;
      }
      index = node->data.next;
    }
  }
  return result;
}

void map_iterator::generate_data() {
  if (is_null(map_)) return;
  map_node_t *node = map_->getpool()->get_obj(current_);
  first = reinterpret_cast<char *>(node) + sizeof(map_node_t);
  uint32_t valuepos = sizeof(map_node_t) + map_->key_size() + 2;
  second = reinterpret_cast<char *>(node) + valuepos;
}

void map_reverse_iterator::generate_data() {
  if (is_null(map_)) return;
  map_node_t *node = map_->getpool()->get_obj(current_);
  first = reinterpret_cast<char *>(node) + sizeof(map_node_t);
  uint32_t valuepos = sizeof(map_node_t) + map_->key_size() + 2;
  second = reinterpret_cast<char *>(node) + valuepos;
}
