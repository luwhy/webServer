#include "Timer.h"
namespace muduo
{
    Timer::Timer(const TimerCallback &cb, Timestamp when, double interval) : callback_(cb),
                                                                             expiration_(when),
                                                                             interval_(interval),
                                                                             repeat_(interval_ > 0.0)
    {
    }
    void Timer::restart(Timestamp now)
    {
        if (repeat_)
        {
            expiration_ = addTime(now, interval_);
        }
        else
        {
            // 新创建一个timestamp
            expiration_ = Timestamp::invalid();
        }
    }
}
