/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id thread.tcc
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2016/05/20 20:40
 * @uses Thread implement.
*/
#ifndef PF_SYS_THREAD_TCC_
#define PF_SYS_THREAD_TCC_

#include "pf/sys/thread.h"
#include "pf/basic/io.tcc"

namespace pf_sys {

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
  : stop_(false) {
  for(size_t i = 0; i < threads; ++i)
    workers_.emplace_back(
      [this] {
        for(;;) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(this->queue_mutex_);
            this->condition_.wait(lock,
              [this]{ return this->stop_ || !this->tasks_.empty(); });
            if(this->stop_ && this->tasks_.empty())
              return;
            task = std::move(this->tasks_.front());
            this->tasks_.pop();
          }
          task();
        }
      }
    );
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
  -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;
  auto task = std::make_shared< std::packaged_task<return_type()> >(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);

    // don't allow enqueueing after stopping the pool
    if (stop_)
      throw std::runtime_error("enqueue on stopped ThreadPool");

    tasks_.emplace([task](){ (*task)(); });
  }
  condition_.notify_one();
  return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    stop_ = true;
  }
  condition_.notify_all();
  for(std::thread & worker : workers_)
    worker.join();
}

inline ThreadCollect::ThreadCollect() {
  ++GLOBALS["thread.collects"];
}

inline ThreadCollect::~ThreadCollect() {
  --GLOBALS["thread.collects"];
  if (GLOBALS["app.debug"] == true) {
    pf_basic::io_cdebug(
        "[%s] thread collect wait exit: %d", 
        GLOBALS["app.name"].string(), 
        ThreadCollect::count());
  }
}

inline int32_t ThreadCollect::count() {
  return GLOBALS["thread.collects"].int32();
}

namespace thread {

inline uint64_t get_id() {
  uint64_t id = 0;
#if OS_UNIX
  id = static_cast<uint64_t>(pthread_self());
#elif OS_WIN
  id = static_cast<uint64_t>(GetCurrentThreadId() * 1000);
#endif
  return id;
}

//用下面的方法的线程可以启动与停止
inline const std::string status_key(std::thread &thread) {
  std::string _status_key{"thread.status."};
  _status_key += std::to_string(thread.native_handle());
  return _status_key;
}

inline const std::string status_key(uint64_t id) {
  std::string _status_key{"thread.status."};
  _status_key += std::to_string(id);
  return _status_key;
}

template<typename _Callable, typename... _Args>
inline void start(std::thread &thread, _Callable &&__f, _Args...__args) {
  thread = std::move(std::thread(
        std::forward<_Callable>(__f), 
        std::forward<_Args>(__args)...));
  const std::string _status_key = status_key(thread);
  GLOBALS[_status_key] = kThreadStatusRun;
}

inline void start(std::thread &thread) {
  const std::string _status_key = status_key(thread);
  GLOBALS[_status_key] = kThreadStatusRun;
}

inline void start() {
  const std::string _status_key = status_key(get_id());
  GLOBALS[_status_key] = kThreadStatusRun;
}

inline void stop() {
  const std::string _status_key = status_key(get_id());
  GLOBALS[_status_key] = kThreadStatusStop;
}

inline void stop(std::thread &thread) {
  const std::string _status_key = status_key(thread);
  if (GLOBALS[_status_key] == kThreadStatusRun) 
    GLOBALS[_status_key] = kThreadStatusStop;
}

inline uint8_t status(std::thread &thread) {
  const std::string _status_key = status_key(thread);
  return GLOBALS[_status_key].uint8();
}

inline bool is_running(std::thread &thread) {
  return kThreadStatusRun == status(thread);
}

inline bool is_running(uint64_t id) {
  const std::string _status_key = status_key(id);
  return kThreadStatusRun == GLOBALS[_status_key].uint8();
}

inline bool is_running() {
  return is_running(get_id());
}

inline bool is_stopping(std::thread &thread) {
  return kThreadStatusStop == status(thread);
}

inline bool is_stopping(uint64_t id) {
  const std::string _status_key = status_key(id);
  return kThreadStatusStop == GLOBALS[_status_key].uint8();
}

inline bool is_stopping() {
  return is_stopping(get_id());
}

}; //namespace thread


}; //namespace pf_sys

#endif //PF_SYS_THREAD_TCC_
