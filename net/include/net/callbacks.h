#pragma once

#include <chrono>
#include <functional>
#include <memory>

namespace talko::net {
class TcpConnection;
class ByteBuffer;

using TimePoint = std::chrono::high_resolution_clock::time_point;
using Duration  = std::chrono::duration<int, std::milli>;

using TimerCallback         = std::function<void()>;
using TcpConnectionPtr      = std::shared_ptr<TcpConnection>;
using ConnectionCallback    = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback         = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback       = std::function<void(const TcpConnectionPtr&, ByteBuffer*, TimePoint)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr&, size_t)>;
} // namespace talko::net