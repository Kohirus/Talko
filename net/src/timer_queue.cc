#include <log/log.h>
#include <net/channel.h>
#include <net/common.h>
#include <net/event_loop.h>
#include <net/timer.h>
#include <net/timer_queue.h>

namespace talko::net {
TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop)
    , timer_fd_(common::createTimerFd())
    , timer_channel_(std::make_unique<Channel>(loop, timer_fd_))
    , handling_expired_timers_(false) {
    timer_channel_->setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timer_channel_->enableReading();
}

TimerQueue::~TimerQueue() {
    timer_channel_->disableAll();
    timer_channel_->remove();
    common::close(timer_fd_);
}

TimerId TimerQueue::append(TimerCallback cb, TimePoint when, Duration interval) {
    loop_->runInLoop(std::bind(&TimerQueue::append_, this, sequence_.load(), cb, when, interval));
    return TimerId(when, sequence_++);
}

void TimerQueue::cancel(TimerId timer_id) {
    loop_->runInLoop(std::bind(&TimerQueue::cancel_, this, timer_id));
}

void TimerQueue::handleRead() {
    loop_->checkIsInCreatorThread();
    TimePoint now = std::chrono::high_resolution_clock::now();
    common::readTimerFd(timer_fd_, now);

    // 获取所有已过期的定时器 这些定时器不在当前的定时器序列中
    auto expired = getExpiredTimers(now);

    handling_expired_timers_ = true;
    canceling_timers_.clear();
    for (auto& timer : expired) {
        timer->run(); // 执行定时器任务
    }
    handling_expired_timers_ = false;

    // 如果过期定时器需要重启 则重新加入到定时器序列中
    // 否则函数执行完后会自动销毁已过期的定时器
    reset(expired, now);
}

void TimerQueue::append_(uint64_t sequence, TimerCallback cb, TimePoint when, Duration interval) {
    loop_->checkIsInCreatorThread();
    // 创建定时器对象
    auto timer = std::make_unique<Timer>(sequence, std::move(cb), when, interval);

    // 插入一个定时器有可能使得最早到期的定时器发生改变
    bool earliest_changed = insert(std::move(timer));

    // 如果发生改变则需要重置定时器的超时时刻
    if (earliest_changed) {
        common::resetTimerFd(timer_fd_, when);
    }
}

void TimerQueue::cancel_(TimerId timer_id) {
    loop_->checkIsInCreatorThread();

    // 从定时器序列中删除指定的定时器
    // 如果在定时器序列中没有找到该定时器 那么可能存在于过期的定时器中
    // 因为定时器过期时已经被移除定时器序列了
    // 那么就将需要取消的定时器编号加入到取消序列中
    auto it = handlers_.find(timer_id.sequence_);
    if (it != handlers_.end()) {
        timers_.erase(it->second);
        handlers_.erase(it);
    } else if (handling_expired_timers_) {
        canceling_timers_.insert(timer_id.sequence_);
    }
}

bool TimerQueue::insert(TimerPtr timer) {
    loop_->checkIsInCreatorThread();
    bool earliest_changed = false;

    TimePoint when = timer->expiration();

    // 由于该序列是按照定时器的到期时间和序号进行排序的
    // 到期时间小的在前 如果到期时间相同 则创建时间小的在前
    // 因此当序列为空或者小于序列中第一个到期时间点时
    // 最早的到期时间需要发生改变
    auto it = timers_.begin();
    if (it == timers_.end() || when < it->first.expiration_) {
        earliest_changed = true;
    }

    // 将其添加到定时器序列中
    TimerId timer_id(timer->expiration(), timer->sequence());
    auto    res = timers_.insert(std::make_pair(timer_id, std::move(timer)));
    assert(res.second);

    // 插入到定时器句柄序列
    handlers_[timer_id.sequence_] = res.first;

    return earliest_changed;
}

std::vector<TimerQueue::TimerPtr> TimerQueue::getExpiredTimers(TimePoint now) {
    std::vector<TimerPtr> expired;

    // 获取最大的过期定时器
    TimerId timer_id(now, std::numeric_limits<uint64_t>::max());
    auto    end = timers_.lower_bound(std::move(timer_id));
    assert(end == timers_.end() || now < end->first.expiration_);

    // 从当前的定时器序列中删除过期定时器
    for (auto it = timers_.begin(); it != end; ++it) {
        expired.push_back(std::move(it->second));
    }
    timers_.erase(timers_.begin(), end);

    // 从定时器句柄序列中删除过期定时器
    for (auto& timer : expired) {
        handlers_.erase(timer->sequence());
    }

    return expired;
}

void TimerQueue::reset(std::vector<TimerPtr>& expired, TimePoint now) {
    // 遍历到期的定时器序列 如果定时器需要重复执行且该定时器没有被取消 则重启定时器
    for (auto& timer : expired) {
        if (timer->repeat() && canceling_timers_.find(timer->sequence()) == canceling_timers_.end()) {
            timer->restart(now);
            insert(std::move(timer));
        }
    }

    // 获取新的过期时间 重置定时器超时时刻
    if (!timers_.empty()) {
        TimePoint nxt_expiration = timers_.begin()->first.expiration_;
        common::resetTimerFd(timer_fd_, nxt_expiration);
    }
}
} // namespace talko::net