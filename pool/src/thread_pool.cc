#include <pool/thread_pool.h>

namespace talko::tp {
size_t ThreadPool::generated_id_ { 0 };

ThreadPool::~ThreadPool() {
    stop();
}

ThreadPool& ThreadPool::instance() {
    static ThreadPool tp;
    return tp;
}

void ThreadPool::start(size_t thread_num) {
    running_ = true;

    init_thread_size_ = thread_num;

    // 创建线程并启动
    for (int i = 0; i < init_thread_size_; ++i) {
        size_t cur_id = generated_id_++;
        auto   th     = std::make_unique<std::thread>(
            std::bind(&ThreadPool::handleTaskQueue, this, cur_id));
        threads_.emplace(cur_id, std::move(th));

        threads_[cur_id]->detach();
        ++idle_thread_size_;
    }
}

void ThreadPool::stop() {
    running_ = false;
    std::unique_lock<std::mutex> lock(que_mtx_);
    cond_non_empty_.notify_all();
    cond_exit_.wait(lock, [&]() -> bool {
        return threads_.empty();
    });
}

bool ThreadPool::isRunning() const {
    return running_.load();
}

void ThreadPool::setMode(ThreadPoolMode mode) {
    if (running_) return;
    mode_ = mode;
}

void ThreadPool::setMaxTaskSize(size_t max_size) {
    if (running_ || mode_ != ThreadPoolMode::dynamic) return;
    max_task_size_ = max_size;
}

void ThreadPool::setMaxThreadSize(size_t max_size) {
    if (running_) return;
    max_thread_size_ = max_size;
}

void ThreadPool::handleTaskQueue(size_t thread_id) {
    auto last_time = std::chrono::high_resolution_clock().now();

    while (true) {
        Task task = nullptr;

        {
            std::unique_lock<std::mutex> lock(que_mtx_);

            // 线程空闲时间如果超过60s 则回收线程
            while (tasks_.empty()) {
                if (!running_) {
                    threads_.erase(thread_id); // 删除当前线程
                    cond_exit_.notify_all();
                    return;
                }

                if (mode_ == ThreadPoolMode::dynamic) {
                    if (std::cv_status::timeout
                        == cond_non_empty_.wait_for(lock, std::chrono::seconds(1))) {
                        auto now = std::chrono::high_resolution_clock().now();
                        auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - last_time);
                        if (dur.count() >= 5 && threads_.size() > init_thread_size_) {
                            // 回收当前线程
                            threads_.erase(thread_id);
                            --idle_thread_size_;
                            return;
                        }
                    }
                } else {
                    // 等待任务队列不为空
                    cond_non_empty_.wait(lock);
                }
            }

            --idle_thread_size_;

            // 从队列中取出任务
            task = tasks_.front();
            tasks_.pop();

            // 如果仍然存在剩余任务 则通知消费者处理任务
            if (tasks_.size() > 0) {
                cond_non_empty_.notify_all();
            }

            // 通知生产者提交任务
            cond_non_full_.notify_all();
        }

        // 执行任务
        if (task != nullptr) {
            task();
        }

        ++idle_thread_size_;
        last_time = std::chrono::high_resolution_clock().now();
    }
}

bool ThreadPool::addTask(Task task) {
    std::unique_lock<std::mutex> lock(que_mtx_);

    // 等待任务队列有空余位置
    if (!cond_non_full_.wait_for(lock, std::chrono::seconds(1),
            [&]() -> bool { return tasks_.size() < max_task_size_; })) {
        return false;
    }

    // 将新任务加入任务队列
    tasks_.emplace(std::move(task));

    // 通知消费者处理任务
    cond_non_empty_.notify_all();

    // 根据任务数量和空闲线程数量 动态调整线程数量
    if (mode_ == ThreadPoolMode::dynamic && tasks_.size() > idle_thread_size_
        && threads_.size() < max_thread_size_) {
        size_t cur_id = generated_id_++;
        auto   th     = std::make_unique<std::thread>(
            std::bind(&ThreadPool::handleTaskQueue, this, cur_id));
        threads_.emplace(cur_id, std::move(th));
        threads_[cur_id]->detach();

        // ++cur_thread_size_;
        ++idle_thread_size_;
    }

    return true;
}

void setThreadPoolMode(ThreadPoolMode mode) {
    ThreadPool::instance().setMode(mode);
}

void setMaxTaskSize(size_t max_size) {
    ThreadPool::instance().setMaxTaskSize(max_size);
}

void setMaxThreadSize(size_t max_size) {
    ThreadPool::instance().setMaxThreadSize(max_size);
}

void start(size_t thread_num) {
    ThreadPool::instance().start(thread_num);
}

void stop() {
    ThreadPool::instance().stop();
}

bool isRunning() {
    return ThreadPool::instance().isRunning();
}
} // namespace talko::tp