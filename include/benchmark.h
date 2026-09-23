#pragma once
#include <chrono>
class benchmark_t
{
    private:
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> count_t;
    const char* m_Name;
    count_t m_start;
    count_t m_stop;
    
    count_t now(){
        return std::chrono::high_resolution_clock::now();
    }
    
    public:
    long long millis(){
        auto a = std::chrono::time_point_cast<std::chrono::milliseconds>(m_start).time_since_epoch().count();
        auto b = std::chrono::time_point_cast<std::chrono::milliseconds>(m_stop).time_since_epoch().count();
        return b-a;
    }
    long long micros(){
        auto a = std::chrono::time_point_cast<std::chrono::microseconds>(m_start).time_since_epoch().count();
        auto b = std::chrono::time_point_cast<std::chrono::microseconds>(m_stop).time_since_epoch().count();
        return b-a;
    }
    long long nano(){
        auto a = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_start).time_since_epoch().count();
        auto b = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_stop).time_since_epoch().count();
        return b-a;
    }

    benchmark_t()
    {
    }

    ~benchmark_t()
    {
    }

    void start(){
        m_start = now();
    }

    void stop()
    {
        m_stop = now();
    }
};