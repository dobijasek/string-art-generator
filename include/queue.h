#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> 
class TSQueue { 
    public:

    void push(const T item){
        std::unique_lock<std::mutex> lock(m_queueLock);
        m_queue.push(item);
        m_cond.notify_one();
    }

    T pop(){
        std::unique_lock<std::mutex> lock(m_queueLock);
        m_cond.wait(lock,[this]() { return !m_queue.empty(); });
        T item = m_queue.front();
        m_queue.pop();
        return item;
    }

    void wait(){
        std::unique_lock<std::mutex> lock(m_queueLock);
        m_cond.wait(lock,[this]() { return !m_queue.empty(); });
    }
    
    bool empty(){
        std::unique_lock<std::mutex> lock(m_queueLock);
        return m_queue.empty();
    }

    int size(){
        return m_queue.size();
    }

    void clear(){
        std::unique_lock<std::mutex> lock(m_queueLock);
        while(!m_queue.empty()){
            m_queue.pop();
        }
    }

    private:
    std::queue<T> m_queue;
    std::mutex m_queueLock;
    std::condition_variable m_cond;
};