#pragma once
#include <thread>
#include <functional>
#include "queue.h"
 
class taskPool_t
{
    public:
    struct Task {
        std::function<void()> fn;
    };
    taskPool_t(int numberOfThreads = -1);
    void push(Task const& t);
    void wait();
    void kill();
    int threads(int free=0);
    private:
    int nOfThreads;
    std::vector<std::thread> task;
    TSQueue<Task> queue;
};

extern taskPool_t TASKER;