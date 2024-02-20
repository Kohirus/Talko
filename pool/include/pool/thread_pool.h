#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <unordered_map>

namespace talko::tp {
enum class ThreadPoolMode {
    fixed,  ///< 固定数量
    dynamic ///< 动态数量
};

class Thread;

/**
 * @brief 线程池
 *
 */
class ThreadPool {
public:
    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(ThreadPool&&)            = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    /** 返回线程池实例对象 */
    static ThreadPool& instance();

    /** 启动线程池 */
    void start(size_t thread_num = std::thread::hardware_concurrency());

    /** 停止线程池并回收所有线程资源 */
    void stop();

    /** 线程池是否正在运行 */
    bool isRunning() const;

    /** 设置线程池的工作模式 */
    void setMode(ThreadPoolMode mode);

    /** 设置最大任务数量 */
    void setMaxTaskSize(size_t max_size);

    /** 设置动态模式下的最大线程数量 */
    void setMaxThreadSize(size_t max_size);

    /** 获取空闲线程数量 */
    size_t idleThreadSize() const;

    /** 获取当前的线程池模式 */
    ThreadPoolMode mode() const;

    /**
     * @brief 提交任务
     *
     * @tparam TaskFunc 任务函数类型
     * @tparam Args 任务函数参数类型
     * @param func 任务函数
     * @param args 任务函数参数
     * @return std::future<decltype(func(args...))> 返回任务函数的异步结果
     */
    template <typename TaskFunc, typename... Args>
    auto submitTask(TaskFunc&& func, Args&&... args) -> std::future<decltype(func(args...))> {
        using ReturnType = decltype(func(args...));

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<TaskFunc>(func), std::forward<Args>(args)...));
        std::future<ReturnType> result = task->get_future();

        if (!addTask([task]() { (*task)(); })) {
            auto failed_task = std::packaged_task<ReturnType()>(
                []() -> ReturnType { return ReturnType(); });
            failed_task();
            return failed_task.get_future();
        }

        return result;
    }

private:
    using Task      = std::function<void()>;
    using ThreadPtr = std::unique_ptr<std::thread>;
    using ThreadMap = std::unordered_map<size_t, ThreadPtr>;
    using TaskQueue = std::queue<Task>;

    ThreadPool() = default;
    ~ThreadPool();

    /** 处理任务队列 */
    void handleTaskQueue(size_t thread_id);

    /** 向任务队列添加任务 */
    bool addTask(Task task);

private:
    static size_t generated_id_;

    ThreadMap threads_ {};
    TaskQueue tasks_ {}; ///< 任务队列

    std::atomic_size_t idle_thread_size_ { 0 }; ///< 空闲线程数量
    std::atomic_bool   running_ { false };      ///< 线程池是否正在运行

    ThreadPoolMode mode_ { ThreadPoolMode::fixed }; ///< 工作模式

    size_t init_thread_size_ { 4 };  ///< 初始线程数量
    size_t max_task_size_ { 1024 };  ///< 最大任务数量
    size_t max_thread_size_ { 200 }; ///< 最大线程数量

    std::mutex que_mtx_; ///< 保证任务队列的线程安全

    std::condition_variable cond_non_empty_; ///< 非空条件变量
    std::condition_variable cond_non_full_;  ///< 非满条件变量
    std::condition_variable cond_exit_;      ///< 退出线程池条件变量
};

/** 设置线程池模式 */
void setThreadPoolMode(ThreadPoolMode mode);

/** 设置最大线程数目 */
void setMaxThreadSize(size_t max_size);

/** 设置最大任务数目 */
void setMaxTaskSize(size_t max_size);

/** 启动线程池 */
void start(size_t thread_num = std::thread::hardware_concurrency());

/** 停止线程池 */
void stop();

/** 获取空闲线程数量 */
size_t idleThreadSize();

/** 获取当前的线程池模式 */
ThreadPoolMode mode();

/** 线程池是否正在运行 */
bool isRunning();

/**
 * @brief 向线程池提交任务
 *
 * @tparam TaskFunc 任务函数类型
 * @tparam Args 任务函数参数类型
 * @param func 任务函数
 * @param args 任务函数参数
 * @return std::future<decltype(func(args...))> 返回任务函数的异步结果
 */
template <typename TaskFunc, typename... Args>
auto submitTask(TaskFunc&& func, Args&&... args) -> std::future<decltype(func(args...))> {
    return ThreadPool::instance().submitTask(std::forward<TaskFunc>(func), std::forward<Args>(args)...);
}
} // namespace talko::tp