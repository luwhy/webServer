#ifndef TIMER__H
#define TIMER__H
#include "base/Timestamp.h"
#include "Callbacks.h"
using namespace muduo;
namespace webs
{
    class Timer
    {
    public:
        Timer(const TimerCallback &cb, Timestamp when, double interval);
        Timer(Timer &t) = delete;

        void run() const
        {
            callback_();
        }

        Timestamp expiration() const { return expiration_; }

        bool repeat() const { return repeat_; }

        void restart(Timestamp now);

    private:
        const TimerCallback callback_;
        Timestamp expiration_;
        const double interval_;
        const bool repeat_;
    };
}
#endif