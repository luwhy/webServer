
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
        /**
         * @brief在Buffer类的swap函数中，可以直接访问rhs.readIndex_和rhs.writeIndex_，因为：
            swap是Buffer类的成员函数。
            C++的访问控制是基于类的作用域，而不是对象的作用域。
            这种设计允许类的成员函数操作同类对象的内部状态，同时仍然保护封装性。
         *
         * @param rhs
         */
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