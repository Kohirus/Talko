#include <log/log.h>
#include <net/channel.h>
#include <net/common.h>
#include <net/event_loop.h>
#include <poll.h>

namespace talko::net {
const int Channel::kNoneEvent  = 0;
const int Channel::kReadEvent  = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop)
    , fd_(fd)
    , events_(kNoneEvent)
    , revents_(kNoneEvent) {
}

Channel::~Channel() {
    assert(!event_handling_);
    assert(!added_to_loop_);
    if (loop_->isInCreatorThread()) {
        assert(!loop_->hasChannel(this));
    }
}

void Channel::handleEvent(TimePoint receive_time) {
    std::shared_ptr<void> obj;
    if (prolonged_) {
        // 延长Channel对象的生命周期
        obj = prolong_.lock();
        if (obj) {
            handleEvent_(receive_time);
        }
    } else {
        handleEvent_(receive_time);
    }
}

void Channel::setReadCallback(ReadEventCallback cb) {
    read_callback_ = std::move(cb);
}

void Channel::setWriteCallback(EventCallback cb) {
    write_callback_ = std::move(cb);
}

void Channel::setCloseCallback(EventCallback cb) {
    close_callback_ = std::move(cb);
}

void Channel::setErrorCallback(EventCallback cb) {
    error_callback_ = std::move(cb);
}

int Channel::fd() const {
    return fd_;
}

int Channel::events() const {
    return events_;
}

void Channel::setRevents(int revt) {
    revents_ = revt;
}

bool Channel::isNoneEvent() const {
    return events_ == kNoneEvent;
}

bool Channel::isReading() const {
    return events_ & kReadEvent;
}

bool Channel::isWriting() const {
    return events_ & kWriteEvent;
}

void Channel::enableReading(bool enabled) {
    if (enabled) {
        events_ |= kReadEvent;
    } else {
        events_ &= ~kReadEvent;
    }
    update();
}

void Channel::enableWriting(bool enabled) {
    if (enabled) {
        events_ |= kWriteEvent;
    } else {
        events_ &= ~kWriteEvent;
    }
    update();
}

void Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}

Status Channel::status() const {
    return status_;
}

void Channel::setStatus(Status status) {
    status_ = status;
}

void Channel::ensureAlive(const std::shared_ptr<void>& obj) {
    prolong_   = obj;
    prolonged_ = true;
}

void Channel::remove() {
    assert(isNoneEvent());
    added_to_loop_ = false;
    loop_->removeChannel(this);
}

EventLoop* Channel::ownerLoop() const {
    return loop_;
}

void Channel::update() {
    added_to_loop_ = true;
    loop_->updateChannel(this);
}

void Channel::handleEvent_(TimePoint receive_time) {
    event_handling_ = true;
    LOG_TRACE("Resultant events: {}", common::eventsToString(revents_));

    // 发生套接字挂起事件 即远端关闭连接或主动关闭套接字
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        log::warn("fd {} happend POLLHUP event", fd_);
        if (close_callback_) close_callback_();
    }

    // 发生套接字无效事件
    if (revents_ & POLLNVAL) {
        log::warn("fd {} happend POLLNVAL event", fd_);
    }

    // 发生套接字错误事件
    if (revents_ & (POLLERR | POLLNVAL)) {
        if (error_callback_) error_callback_();
    }

    // 发生套接字可读事件
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (read_callback_) read_callback_(receive_time);
    }

    // 发生套接字可写事件
    if (revents_ & POLLOUT) {
        if (write_callback_) write_callback_();
    }

    event_handling_ = false;
}
} // namespace talko::net