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
        ssize_t readfd(int fd, int *savedErrno);
    };
}
#endif