#include <log/log.h>
#include <net/channel.h>
#include <net/common.h>
#include <net/epoll_poller.h>
#include <net/event_loop.h>
#include <net/timer_queue.h>

namespace talko::net {
EventLoop::EventLoop()
    : poller_(std::make_unique<EpollPoller>(this))
    , thread_id_(utils::os::threadId())
    , wakeup_fd_(common::createEventFd())
    , wakeup_channel_(std::make_unique<Channel>(this, wakeup_fd_))
    , timer_queue_(std::make_unique<TimerQueue>(this)) {
    LOG_DEBUG("Create EventLoop[{}]", fmt::ptr(this));
    // 让唤醒事件描述符监听可读事件
    wakeup_channel_->setReadCallback(std::bind(&EventLoop::handleReadToWakeUp, this));
    wakeup_channel_->enableReading();
}

EventLoop::~EventLoop() {
    LOG_DEBUG("Destory EventLoop[{}] of thread {}", fmt::ptr(this), thread_id_);
    wakeup_channel_->disableAll();
    wakeup_channel_->remove();
    common::close(wakeup_fd_);
}

void EventLoop::loop() {
    assert(!looping_);
    checkIsInCreatorThread();

    looping_ = true;
    quit_    = false;
    LOG_TRACE("EventLoop[{}] start looping", fmt::ptr(this));

    while (!quit_) {
        // 阻塞等待描述符上的事件发生
        active_channels_.clear();
        poll_return_time_ = poller_->poll(10000, &active_channels_);

        // 处理描述符上发生的事件
        handling_events_ = true;
        for (auto* channel : active_channels_) {
            cur_active_channel_ = channel;
            cur_active_channel_->handleEvent(poll_return_time_);
        }
        cur_active_channel_ = nullptr;
        handling_events_    = false;

        // 处理待处理的事件集合
        handlePendingFunctors();
    }

    LOG_TRACE("EventLoop[{}] stop looping", fmt::ptr(this));
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInCreatorThread()) {
        wakeup(); // 如果是在其它线程退出 则通知该事件循环
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t  n   = common::write(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("Wakeup write {} bytes instead of 8", n);
    }
}

void EventLoop::runInLoop(Functor cb) {
    if (isInCreatorThread()) {
        cb();
    } else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        pending_functors_.push_back(std::move(cb));
    }

    if (!isInCreatorThread() || calling_pending_functors_) {
        wakeup();
    }
}

size_t EventLoop::queueSize() {
    std::lock_guard<std::mutex> lock(mtx_);
    return pending_functors_.size();
}

TimerId EventLoop::runAt(TimePoint time, TimerCallback cb) {
    return timer_queue_->append(std::move(cb), time, Duration(0));
}

TimerId EventLoop::runAfter(Duration delay, TimerCallback cb) {
    TimePoint time = std::chrono::high_resolution_clock::now() + delay;
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(Duration interval, TimerCallback cb) {
    TimePoint time = std::chrono::high_resolution_clock::now() + interval;
    return timer_queue_->append(std::move(cb), time, interval);
}

void EventLoop::cancel(TimerId timer_id) {
    timer_queue_->cancel(timer_id);
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    checkIsInCreatorThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    checkIsInCreatorThread();
    if (handling_events_) {
        assert(cur_active_channel_ == channel
            || std::find(active_channels_.begin(), active_channels_.end(), channel) == active_channels_.end());
    }
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) const {
    assert(channel->ownerLoop() == this);
    checkIsInCreatorThread();
    return poller_->hasChannel(channel);
}

bool EventLoop::isHandlingEvent() const {
    return handling_events_;
}

bool EventLoop::isInCreatorThread() const {
    return thread_id_ == utils::os::threadId();
}

void EventLoop::checkIsInCreatorThread() const {
    if (!isInCreatorThread()) {
        LOG_FATAL("EventLoop[{}] was created in thread {} or not current thread",
            fmt::ptr(this), thread_id_);
    }
}

void EventLoop::handleReadToWakeUp() {
    uint64_t one = 1;
    ssize_t  n   = common::read(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("Read {} bytes instead of 8 from wakeup", n);
    }
}

void EventLoop::handlePendingFunctors() {
    std::vector<Functor> functors;
    calling_pending_functors_ = true;

    {
        std::lock_guard<std::mutex> lock(mtx_);
        functors.swap(pending_functors_);
    }

    for (const Functor& functor : functors) {
        functor();
    }
    calling_pending_functors_ = false;
}
} // namespace talko::net