#include "pool.h"
#include <cstdio>
#include  <iostream>
static int runThreads = 1;
static int tackCounter=0;
static std::mutex taskCounterLock;
static std::condition_variable cv;
taskPool_t TASKER{8};

static void taskCounterInc(int inc){
    std::unique_lock<std::mutex> lock(taskCounterLock);
    tackCounter += inc;
}

static void handler(TSQueue<taskPool_t::Task> *q){
    while(1){
        auto t = q->pop();
        if(!runThreads) break;
        try {
            t.fn();
        } catch (const std::exception& e) {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }
        
        taskCounterInc(-1);
        cv.notify_one();
    }
}

taskPool_t::taskPool_t(int numberOfThreads){
    numberOfThreads = numberOfThreads==-1 ? std::thread::hardware_concurrency() : numberOfThreads;
    nOfThreads = numberOfThreads;
    for(int taskIdx=0;taskIdx<nOfThreads;taskIdx++){
        task.push_back(std::thread(handler,&queue));
    }
}

void taskPool_t::push(Task const& t){
    taskCounterInc(1);
    queue.push(t);
}

void taskPool_t::wait(){
    std::unique_lock<std::mutex> lock(taskCounterLock);
    cv.wait(lock,[](){ return tackCounter==0; });
}

void taskPool_t::kill(){
    runThreads = 0;
    for(int threadId=0;threadId<nOfThreads;threadId++){
        queue.push({[](){}});
    }
    for(int threadId=0;threadId<nOfThreads;threadId++){
        // printf("thread joining %i\n",threadId);
        while(!task[threadId].joinable());
        task[threadId].join();
        // printf("thread joined  %i\n",threadId);
    }
}

int taskPool_t::threads(int free){
    return nOfThreads>free ? nOfThreads-free : 1;
}