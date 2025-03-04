#include "Buffer.h"
namespace muduo
{
    Buffer::Buffer() : buffer_(kCheapPrepend + kInitialSize),
                       readIndex_(kCheapPrepend),
                       writeIndex_(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == kInitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    Buffer::~Buffer()
    {
    }

    ssize_t Buffer::readfd(int fd, int *savedErrno)
    {
        return ssize_t();
    }
}