#ifndef TIMER__ID__H
#define TIMER__ID__H
#include "Timer.h"
namespace webs
{
    class TimerId
    {
    public:
        explicit TimerId(Timer *timer) : value_(timer)
        {
        }

    private:
        Timer *value_;
    };
}
#endif