
#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <assert.h>
#include <vector>
#include <string>
#include <algorithm>
namespace muduo
{
    class Buffer
    {
    public:
        static const size_t kCheapPrepend = 8;
        static const size_t kInitialSize = 1024;
        Buffer();

        ~Buffer();

        ssize_t readfd(int fd, int *savedErrno);

        // 当前可读（写-读）
        ssize_t readableBytes() const
        {
            return writeIndex_ - readIndex_;
        }
        /**
         * @brief 当前可写，总-写
         *
         * @return ssize_t
         */
        ssize_t writableBytes() const
        {
            return buffer_.size() - writeIndex_;
        }

        /**
         * @brief 初始化前位置
         *
         * @return ssize_t
         */
        ssize_t prependableBytes() const
        {
            return readIndex_;
        }

        void swap(Buffer &rhs)
        {
            buffer_.swap(rhs.buffer_);
            std::swap(readIndex_, rhs.readIndex_);
            std::swap(writeIndex_, rhs.writeIndex_);
        }

    private:
        std::vector<char> buffer_;
        size_t readIndex_;
        size_t writeIndex_;
    };
}
#endif