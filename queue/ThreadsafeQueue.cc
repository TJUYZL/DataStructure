#ifndef PS_INTERNAL_THREADSAFE_QUEUE_H_
#define PS_INTERNAL_THREADSAFE_QUEUE_H_
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T> class ThreadsafeQueue {
 public:
        ThreadsafeQueue();
        ~ThreadsafeQueue();

        void Push(T new_value) {
            std::lock_guard<std::mutex> lk(mu_);
            // mu_.lock();
            queue_.push(std::move(new_value));
            // mu_.unlock();
            cond_.notify_all();
        }

        void WaitAndPop(T* value) {
            std::unique_lock<std::mutex> lk(mu_);
            cond_.wait(lk, [this] {return !queue_.empty();});
            value = std::move(queue_.front());
            queue_.pop();
        }

        bool Empty() const {
            std::lock_guard<std::mutex> lk(mu_);
            return queue_.empty();
        }

 private:
        mutable std::mutex mu_;
        std::queue<T> queue_;
        std::condition_variable cond_;
};

#endif  // PS_INTERNAL_THREADSAFE_QUEUE_H_
