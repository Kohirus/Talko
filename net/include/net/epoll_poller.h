#pragma once

#include <map>
#include <net/callbacks.h>
#include <sys/epoll.h>
#include <vector>

namespace talko::net {
class Channel;
class EventLoop;

/**
 * @brief 基于epoll的I/O复用模型
 *
 */
class EpollPoller {
public:
    using ChannelList = std::vector<Channel*>;

    /**
     * @brief Construct a new Epoll Poller object
     *
     * @param loop 事件循环
     */
    EpollPoller(EventLoop* loop);
    ~EpollPoller();

    /**
     * @brief 阻塞等待描述符上的事件发生
     *
     * @param[in] timeout 超时时间，单位为毫秒
     * @param[out] active_channels 具体发生事件的描述符通道
     * @return TimePoint 返回发生事件的时间点
     */
    TimePoint poll(int timeout, ChannelList* active_channels);

    /** 更新通道 */
    void updateChannel(Channel* channel);

    /** 移除通道 */
    void removeChannel(Channel* channel);

    /** 是否拥有指定通道 */
    bool hasChannel(Channel* channel) const;

private:
    /**
     * @brief 更新指定通道上的事件
     *
     * @param operation 操作类型
     * @param channel 通道
     */
    void update(int operation, Channel* channel);

    /**
     * @brief 填充通道列表
     *
     * @param[in] num_events 发生事件的数量
     * @param[out] active_channels 具体发生事件的描述符通道
     */
    void fillActiveChannels(int num_events, ChannelList* active_channels) const;

private:
    using ChannelMap = std::map<int, Channel*>;
    using EventList  = std::vector<epoll_event>;

    static const int kInitEventListSize { 16 }; ///< 初始化列表大小

    EventLoop* owner_loop_; ///< 事件循环
    int        ep_fd_;      ///< I/O复用模型描述符
    ChannelMap channels_;   ///< 通道映射表
    EventList  events_;     ///< 事件列表
};
} // namespace talko::net