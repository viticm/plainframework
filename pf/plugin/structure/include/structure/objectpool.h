/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id objectpool.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/03/27 11:03
 * @uses object pool template class
 *       cn: 对象池模板类
*/
#ifndef STRUCTURE_OBJECTPOOL_H_
#define STRUCTURE_OBJECTPOOL_H_

#include "structure/config.h"
#include "pf/sys/thread.h"

namespace structure {

template <class T>
class ObjectPool {
 
 public:
   ObjectPool() {
     __ENTER_FUNCTION
       pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
       objects_ = NULL;
       countmax_ = -1;
       count_ = -1;
     __LEAVE_FUNCTION
   };
   ~ObjectPool() {
     __ENTER_FUNCTION
       destroy();
       Assert(NULL == objects_);
     __LEAVE_FUNCTION
   };

 public:
   bool init(int32_t countmax) {
     __ENTER_FUNCTION
       pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
       Assert(countmax > 0);
       if (countmax < 0) return false;
       countmax_ = countmax;
       count_ = 0;
       objects_ = new T * [countmax_];
       memset(objects_, 0, sizeof(objects_));
       int32_t i;
       for (i = 0; i < countmax_; ++i) {
#if __LINUX__
         objects_[i] = new T;
         if (NULL == objects_[i]) return false;
#elif __WINDOWS__
         objects_[i] = NULL;
#endif
       }
       return true;
     __LEAVE_FUNCTION
       return false;
   };
   
   void destroy() {
     __ENTER_FUNCTION
       pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
       if (objects_ != NULL) {
         for (int32_t i = 0; i < countmax_; ++i) {
           SAFE_DELETE(objects_[i]);
         }
         SAFE_DELETE_ARRAY(objects_);
       }
       countmax_ = -1;
       count_ = -1;
     __LEAVE_FUNCTION
   };

   T *get() {
     __ENTER_FUNCTION
       pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
       Assert(count_ < countmax_);
       if (count_ >= countmax_) return NULL;
       if (NULL == objects_[count_]) {
         objects_[count_] = new T;
         Assert(objects_[count_] != NULL);
       }
       T *object = objects_[count_];
       object->set_poolid(count_);
       ++count_;
       return object;
     __LEAVE_FUNCTION
       return NULL;
   };

   void remove(T *object) {
     __ENTER_FUNCTION
       pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
       Assert(objcet != NULL);
       if (NULL == object) return;
       Assert(count_ >= 0);
       if (count_ < 0) return;
       uint32_t index = object->get_poolid();
       Assert(index < count_);
       if (index > static_cast<uint32_t>(count_)) return;
       --count_;
       T *object = objects_[index];
       objects_[index] = objcets_[count_];
       objcets_[count_] = object;
       objects_[index]->set_poolid(index);
       objcets_[count_]->set_poolid(ID_INVALID);
     __LEAVE_FUNCTION
   };

   int32_t getcount() const {
     return count_;
   };

 private:
   T **objects_;
   int32_t countmax_;
   int32_t count_;
   pf_sys::ThreadLock lock_;

};

}; //namespace structure

#endif //STRUCTURE_OBJECTPOOL_H_
