#include <log/log.h>
#include <net/channel.h>
#include <net/common.h>
#include <net/epoll_poller.h>
#include <net/event_loop.h>

namespace talko::net {
/** 将通道状态转换为字符串形式 */
std::string toString(Status status) {
    switch (status) {
    case Status::New:
        return "New";
    case Status::Added:
        return "Added";
    case Status::Deleted:
        return "Deleted";
    default:
        assert(0);
        return "Unknown";
    }
}

EpollPoller::EpollPoller(EventLoop* loop)
    : owner_loop_(loop)
    , ep_fd_(common::createEpoll())
    , events_(kInitEventListSize) {
    LOG_TRACE("Create epoll fd {}", ep_fd_);
}

EpollPoller::~EpollPoller() {
    common::close(ep_fd_);
}

TimePoint EpollPoller::poll(int timeout, ChannelList* active_channels) {
    LOG_TRACE("Totoal count of fd: {}", channels_.size());

    // 阻塞等待事件发生
    int num_events  = ::epoll_wait(ep_fd_, &*events_.begin(), events_.size(), timeout);
    int saved_errno = errno;

    // 获取当前时间点
    TimePoint now = std::chrono::high_resolution_clock::now();

    if (num_events < 0) { // 发生错误
        if (saved_errno != EINTR) {
            LOG_ERROR("Failed to epoll wait: {}", std::strerror(saved_errno));
        }
    } else if (num_events > 0) { // 发生事件
        LOG_TRACE("{} events happened", num_events);
        fillActiveChannels(num_events, active_channels);
        // 容量不够则进行2倍扩容
        if (num_events == static_cast<int>(events_.size())) {
            events_.resize(events_.size() * 2);
        }
    } else { // 无事件发生
        LOG_TRACE("Nothing happened");
    }

    return now;
}

void EpollPoller::updateChannel(Channel* channel) {
    owner_loop_->checkIsInCreatorThread();

    Status status = channel->status();
    LOG_TRACE("fd {} modify event {}, current status is {}", channel->fd(),
        common::eventsToString(channel->events()), toString(status));

    if (status == Status::New || status == Status::Deleted) {
        // 如果该通道未在epoll中 则将其添加到epoll中
        int fd = channel->fd();
        if (status == Status::New) {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        } else {
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }

        channel->setStatus(Status::Added);
        update(EPOLL_CTL_ADD, channel);
    } else {
        // 如果该通道在epoll中 则修改之
        int fd = channel->fd();
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        // 无事件则删除 有事件则修改
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setStatus(Status::Deleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel* channel) {
    owner_loop_->checkIsInCreatorThread();

    int fd = channel->fd();
    LOG_TRACE("Remove fd {} from epoll", fd);
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    Status status = channel->status();
    assert(status == Status::Added || status == Status::Deleted);

    channels_.erase(channel->fd());
    if (channel->status() == Status::Added) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setStatus(Status::New);
}

bool EpollPoller::hasChannel(Channel* channel) const {
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

void EpollPoller::update(int operation, Channel* channel) {
    epoll_event event;
    ::bzero(&event, sizeof(event));
    event.events   = channel->events();
    event.data.ptr = channel;
    int fd         = channel->fd();

    LOG_TRACE("Control fd {} with {} operation, events: {}", fd,
        common::operationToString(operation), common::eventsToString(channel->events()));

    if (::epoll_ctl(ep_fd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_ERROR("Failed to {} fd {}: {}", common::operationToString(operation), fd, std::strerror(errno));
        } else {
            LOG_FATAL("Failed to {} fd {}: {}", common::operationToString(operation), fd, std::strerror(errno));
        }
    }
}

void EpollPoller::fillActiveChannels(int num_events, ChannelList* active_channels) const {
    assert(num_events <= static_cast<int>(events_.size()));
    for (int i = 0; i < num_events; ++i) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        // 设置具体发生的事件类型
        channel->setRevents(events_[i].events);
        active_channels->push_back(channel);
    }
}
} // namespace talko::net