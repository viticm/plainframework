#include "structure/define.h"
#include "structure/objectlist.h"

namespace structure {

ObjectList::ObjectList() {
  __ENTER_FUNCTION
    size_ = 0;
    head_.next = &tail_;
    head_.prev = &head_;
    tail_.next = &tail_;
    tail_.prev = &head_;
  __LEAVE_FUNCTION
}

ObjectList::~ObjectList() {
  //do nothing
}

void ObjectList::cleanup() {
  __ENTER_FUNCTION
    head_.clear();
    tail_.clear();
    size_ = 0;
    head_.next = &tail_;
    head_.prev = &head_;
    tail_.next = &tail_;
    tail_.prev = &head_;
  __LEAVE_FUNCTION
}

bool ObjectList::addnode(objectlist_node_t *node) {
  __ENTER_FUNCTION
    Assert(node);
    Assert(NULL == node->prev);
    Assert(NULL == node->next);
    node->next = &tail_;
    node->prev = tail_.prev;
    tail_.prev->next = node;
    tail_.prev = node;
    ++size_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ObjectList::removenode(objectlist_node_t *node) {
  __ENTER_FUNCTION
    char message[32] = {0};
    snprintf(message, sizeof(message) - 1, "node = %p, size = %d", node, size_);
    AssertEx(node, message);
    AssertEx(node->prev != NULL, message);
    AssertEx(node->next != NULL, message);
    objectlist_node_t *pointer = head_.next;
    AssertEx(pointer, message);
    while (pointer != &tail_) {
      if (pointer == node) {
        pointer->prev->next = pointer->next;
        pointer->next->prev = pointer->prev;
        pointer->next = NULL;
        pointer->prev = NULL;
        --size_;
        break;
      } else {
        pointer = pointer->next;
        AssertEx(pointer, message);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t ObjectList::getsize() const {
  return size_;
}

} //namespace structure
