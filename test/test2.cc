#include <sys/timerfd.h>
#include "../src/EventLoop.h"
#include "../src/Channel.h"
#include <functional>
muduo::EventLoop *g_loop;
void timeout()
{
    printf("Timeout\n");
    g_loop->quit();
}
int main()
{

    // muduo::EventLoop loop;
    // g_loop = &loop;
    // int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    // // 因为Channel使用了enable_shared_from_this，所以只能这样子构造
    // std::shared_ptr<muduo::Channel> channel(new muduo::Channel(&loop, timerfd));
    // channel->setReadCallback(std::function<void()>(std::bind(timeout)));
    // channel->enableReading();

    // struct itimerspec howlong;
    // bzero(&howlong, sizeof howlong);
    // howlong.it_value.tv_sec = 5;
    // ::timerfd_settime(timerfd, 0, &howlong, NULL);
    // loop.loop();
    // close(timerfd);
}