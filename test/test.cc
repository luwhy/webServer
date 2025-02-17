#include "../src/EventLoop.h"
#include "../src/base/Thread.h"
webs::EventLoop *g_loop;

void threadFunc()
{
    g_loop->loop();
}
int main()
{
    webs::EventLoop loop;
    g_loop = &loop;
    muduo::Thread t(threadFunc);
    t.start();
    t.join();
    return 0;
}