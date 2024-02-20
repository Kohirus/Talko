#pragma once

#include <cassert>
#include <cstring>
#include <net/common.h>
#include <string>
#include <vector>

namespace talko::net {
/**
 * @brief 缓冲区
 *
 */
class ByteBuffer {
public:
    using Iter      = std::vector<char>::iterator;
    using ConstIter = std::vector<char>::const_iterator;

    ByteBuffer(size_t initial_size = kInitialSize);
    ~ByteBuffer() = default;

    ByteBuffer(const ByteBuffer&)            = default;
    ByteBuffer& operator=(const ByteBuffer&) = default;

    /** 交换缓冲区 */
    void swap(ByteBuffer& other);

    /** 获取读指针 */    
    const char* readerPtr() const;

    /** 获取读指针 */
    char* readerPtr();

    /** 获取写指针 */
    const char* writerPtr() const;

    /** 获取写指针 */
    char* writerPtr();
    
    /** 标记读指针位置 */
    void markReaderIndex();

    /** 标记写指针位置 */
    void markWriterIndex();

    /** 获取可读区域大小 */
    size_t readableBytes() const;

    /** 获取可写区域大小 */
    size_t writableBytes() const;

    /** 重置读指针位置 */
    void resetReaderIndex();

    /** 重置写指针位置 */
    void resetWriterIndex();

    /** 是否具有可读数据 */
    bool isReadable() const;

    /** 获取读指针索引 */
    size_t readerIndex() const;

    /** 获取写指针索引 */
    size_t writerIndex() const;

    /** 读指针跳过指定的字节数 */
    void skipBytes(size_t bytes);

    /** 跳过所有的可读区域 */
    void skipAllBytes();

    /** 将可读区域中的数据读取到 dst 中 */
    void readBytes(std::vector<char>& dst);

    /** 将可读区域中的数据读取到 dst 中 */
    void readBytes(std::string& dst);

    /**
     * @brief 读取字节并移动读指针
     *
     * @tparam T 要读取的数据类型
     * @return T 返回读取的结果
     */
    template <typename T>
    T readBytes() {
        assert(sizeof(T) <= readableBytes());
        T res;
        ::memcpy(&res, *reader(), sizeof(T));
        reader_idx_ += sizeof(T);
        return common::networkToHost(res);
    }

    /** 将 src 中的数据写入到可写区域中 */
    void writeBytes(const std::vector<char>& src);

    /** 将 src 中的数据写入到可写区域中 */
    void writeBytes(std::string_view src);

    /**
     * @brief 写入字节并移动读指针
     *
     * @tparam T 要写入的数据类型
     */
    template <typename T>
    void wrtieBytes(T val) {
        if (sizeof(T) > writableBytes()) {
            expand(sizeof(T));
        }
        T tmp = common::hostToNetwork(val);
        ::memcpy(*writer(), &tmp, sizeof(T));
        writer_idx_ += sizeof(T);
    }

    /**
     * @brief 获取从索引 idx 起的可读数据，但不移动读指针
     *
     * @tparam T 要获取的数据类型
     * @param idx 起始索引
     * @return T 返回获取到的数据
     */
    template <typename T>
    T getBytes(size_t idx) {
        assert(sizeof(T) + idx < writerIndex());
        T res;
        ::memcpy(&res, *reader(), sizeof(T));
        return common::networkToHost(res);
    }

    /**
     * @brief 从指定索引 idx 开始写入数据 val，但不移动写指针
     *
     * @tparam T 要写入的数据类型
     * @param idx 写入数据的起始索引
     * @param val 要写入的数据
     */
    template <typename T>
    void setBytes(size_t idx, T val) {
        assert(idx >= writerIndex());
        if (sizeof(T) > writableBytes()) {
            expand(sizeof(T));
        }
        T tmp = common::hostToNetwork(val);
        ::memcpy(*writer(), &tmp, sizeof(T));
    }

    /**
     * @brief 将文件描述符中的可读数据写入到缓冲区中
     *
     * @param fd 要读取的文件描述符
     * @param saved_errno 错误码
     * @return ssize_t 返回实际读取的数据大小
     */
    ssize_t recvFromFd(int fd, int* saved_errno);

    /** 获取读指针 */
    ConstIter reader() const;

    /** 获取读指针 */
    Iter reader();

    /** 获取写指针 */
    ConstIter writer() const;

    /** 获取写指针 */
    Iter writer();

private:
    /** 扩容确保可写区域最少有 len 个字节的空间 */
    void expand(size_t len);

private:
    static const size_t kInitialSize { 1024 }; ///< 初始化大小

    std::vector<char> buffer_; ///< 缓冲区

    size_t reader_idx_ { 0 };    ///< 读索引
    size_t writer_idx_ { 0 };    ///< 写索引
    size_t reader_mark_ { 0 };   ///< 读指针标记位置
    size_t writer_mark_ { 0 };   ///< 写指针标记位置
    size_t expand_factor_ { 2 }; ///< 扩容因子
};
} // namespace talko::net