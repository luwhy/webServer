
#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <assert.h>
#include <vector>
#include <string>
#include <algorithm>
namespace webs
{
    class Buffer
    {
    public:
        Buffer();

        ~Buffer();

        static const size_t kCheapPrepend = 8;
        static const size_t kInitialSize = 1024;
        ssize_t readfd(int fd, int *savedErrno);

    private:
        std::vector<char> buffer_;
        size_t readIndex_;
        size_t writeIndex_;
    };
}
#endif