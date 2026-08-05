#pragma once

#include <mutex>
#include <queue>
#include <utility>

template <typename T>
class ThreadSafeQueue {
public:
  ThreadSafeQueue() = default;
  ~ThreadSafeQueue() = default;

  ThreadSafeQueue(const ThreadSafeQueue&) = delete;
  ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
  ThreadSafeQueue(ThreadSafeQueue&&) = delete;
  ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

  void push(const T& item)
  {
    const std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(item);
  }

  bool tryPop(T& out)
  {
    const std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    out = std::move(queue_.front());
    queue_.pop();
    return true;
  }

private:
  std::queue<T> queue_;
  mutable std::mutex mutex_;
};