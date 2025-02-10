#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>
#include <thread>
namespace CurrentThread
{
    __thread pid_t t_cachedTid;

        void cacheTid();
    inline pid_t tid()
    {
        // 期望t_cachedTid==0的可能性接近0，所以cacheTid的执行可能性非常低
        if (__builtin_expect(t_cachedTid == 0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }
}

#endif