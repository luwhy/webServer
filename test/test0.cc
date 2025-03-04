#include <sys/eventfd.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

int main()
{

    int evtfd = eventfd(0, 0);
    ssize_t t = 1;
    int n = ::read(evtfd, &t, sizeof t);
    std::cout << n;
}