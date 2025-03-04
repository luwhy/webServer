#include "../src/EventLoop.h"
#include "../src/base/Thread.h"
#include <stdio.h>
webs::EventLoop *g_loop;

void print()
{
    printf("sss");
}
void threadFunc()
{
    g_loop->runAfter(1.0, print);
}
int main()
{
    muduo::EventLoop loop;
    g_loop = &loop;
    muduo::Thread t(threadFunc);
    t.start();
    loop.loop();
    t.join();
    return 0;
}