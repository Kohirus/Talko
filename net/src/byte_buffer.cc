#include <array>
#include <cassert>
#include <cstdint>
#include <net/byte_buffer.h>
#include <string_view>

namespace talko::net {
ByteBuffer::ByteBuffer(size_t initial_size)
    : buffer_(initial_size) {
}

void ByteBuffer::swap(ByteBuffer& other) {
    buffer_.swap(other.buffer_);
    std::swap(reader_idx_, other.reader_idx_);
    std::swap(writer_idx_, other.writer_idx_);
    std::swap(reader_mark_, other.reader_mark_);
    std::swap(writer_mark_, other.writer_mark_);
}

const char* ByteBuffer::readerPtr() const {
    return &*buffer_.begin() + reader_idx_;
}

char* ByteBuffer::readerPtr() {
    return &*buffer_.begin() + reader_idx_;
}

const char* ByteBuffer::writerPtr() const {
    return &*buffer_.begin() + writer_idx_;
}

char* ByteBuffer::writerPtr() {
    return &*buffer_.begin() + writer_idx_;
}

void ByteBuffer::markReaderIndex() {
    reader_mark_ = reader_idx_;
}

void ByteBuffer::markWriterIndex() {
    writer_mark_ = writer_idx_;
}

size_t ByteBuffer::readableBytes() const {
    return writer_idx_ - reader_idx_;
}

size_t ByteBuffer::writableBytes() const {
    return buffer_.size() - writer_idx_;
}

void ByteBuffer::resetReaderIndex() {
    reader_idx_ = reader_mark_;
}

void ByteBuffer::resetWriterIndex() {
    writer_idx_ = writer_mark_;
}

bool ByteBuffer::isReadable() const {
    return writer_idx_ > reader_idx_;
}

size_t ByteBuffer::readerIndex() const {
    return reader_idx_;
}

size_t ByteBuffer::writerIndex() const {
    return writer_idx_;
}

void ByteBuffer::skipBytes(size_t bytes) {
    bytes = bytes > readableBytes() ? readableBytes() : bytes;
    reader_idx_ += bytes;
}

void ByteBuffer::skipAllBytes() {
    reader_idx_ = writer_idx_;
}

void ByteBuffer::readBytes(std::vector<char>& dst) {
    if (dst.size() < readableBytes()) {
        dst.resize(readableBytes());
    }
    std::copy(buffer_.begin(), reader(), dst.begin());
    reader_idx_ = writer_idx_;
}

void ByteBuffer::readBytes(std::string& dst) {
    if (dst.size() < readableBytes()) {
        dst.resize(readableBytes());
    }
    std::copy(reader(), writer(), dst.begin());
    reader_idx_ = writer_idx_;
}

void ByteBuffer::writeBytes(const std::vector<char>& src) {
    if (writableBytes() < src.size()) {
        expand(src.size());
    }
    std::copy(src.begin(), src.end(), writer());
}

void ByteBuffer::writeBytes(std::string_view src) {
    if (writableBytes() < src.size()) {
        expand(src.size());
    }
    std::copy(src.begin(), src.end(), writer());
}

ssize_t ByteBuffer::recvFromFd(int fd, int* saved_errno) {
    std::array<char, 65535> extra_buf;
    std::array<iovec, 2>    vec;

    const size_t writable = writableBytes();

    // 设置多个缓冲区
    vec[0].iov_base = writerPtr();
    vec[0].iov_len  = writable;
    vec[1].iov_base = extra_buf.data();
    vec[1].iov_len  = extra_buf.size();

    const int     iovcnt = (writable < extra_buf.size()) ? 2 : 1;
    const ssize_t n      = common::readv(fd, vec.data(), iovcnt);
    if (n < 0) {
        *saved_errno = errno;
    } else if (n <= static_cast<ssize_t>(writable)) {
        writer_idx_ += n;
    } else {
        writer_idx_ = buffer_.size();
        expand(extra_buf.size());
        std::copy(extra_buf.begin(), extra_buf.end(), writer());
    }
    return n;
}

ByteBuffer::ConstIter ByteBuffer::reader() const {
    return buffer_.cbegin() + reader_idx_;
}

ByteBuffer::Iter ByteBuffer::reader() {
    return buffer_.begin() + reader_idx_;
}

ByteBuffer::ConstIter ByteBuffer::writer() const {
    return buffer_.cbegin() + writer_idx_;
}

ByteBuffer::Iter ByteBuffer::writer() {
    return buffer_.begin() + writer_idx_;
}

void ByteBuffer::expand(size_t len) {
    // 先用可读区域填充无效区域
    if (reader_idx_ != 0) {
        size_t len = readableBytes();
        std::copy(reader(), writer(), buffer_.begin());
        reader_idx_ = 0;
        writer_idx_ = len;
    }

    // 如果此时可写区域空间仍然不够则扩容
    if (writableBytes() < len) {
        size_t expand_size = buffer_.size() * expand_factor_;
        while (expand_size - writer_idx_ < len) {
            expand_size *= expand_factor_;
        }
        buffer_.resize(expand_size);
    }
}
} // namespace talko::net