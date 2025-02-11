#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>
#include <thread>
#include <thread>
namespace CurrentThread
{
    __thread pid_t t_cachedTid;
    __thread int t_tidStringLength = 6;
    __thread char t_tidString[32];
    pid_t gettid();

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