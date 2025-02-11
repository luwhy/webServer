#include "Thread.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
namespace CurrentThread
{
    pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    void catchTid()
    {
        if (t_cachedTid == 0)
        {
            t_cachedTid = gettid();
            t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
        }
    }
}