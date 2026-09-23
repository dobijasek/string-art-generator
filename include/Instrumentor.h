//
// Basic instrumentation profiler by Cherno

// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");        // Begin session 
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like this in scopes you'd like to include in profiling
//     // Code
// }
// Instrumentor::Get().EndSession();                        // End Session
//
// You will probably want to macro-fy this, to switch on/off easily and use things like __FUNCSIG__ for the profile name.
//
#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

struct ProfileResult
{
    const char *Name;
    long long Start, End;
    uint32_t ThreadID;
    int printable;
};

struct InstrumentationSession
{
    std::string Name;
};
extern volatile int runThreads;

static void handler(void *p);

class Instrumentor
{
private:
    InstrumentationSession* m_CurrentSession;
    std::ofstream m_OutputStream;
    std::mutex m_mutex;
    int m_ProfileCount;
    std::thread task;
public:
    // std::vector<ProfileResult> qqq;
    std::queue<ProfileResult> qqq;
    Instrumentor()
        : m_CurrentSession(nullptr), m_ProfileCount(0)
    {
        task = std::thread(handler,nullptr);
        // printf("Instrumentator constuctor\n");
    }

    void BeginSession(const std::string& name, const std::string& filepath = "results.json")
    {
        // std::unique_lock<std::mutex> lock(m_mutex);
        // printf("Instrumentator START");
        m_OutputStream.open(filepath);
        // m_OutputStream.rdbuf()->pubsetbuf(mybuffer,sizeof(mybuffer));
        WriteHeader();
        m_CurrentSession = new InstrumentationSession{ name };
    }

    void store(){
        // printf("%i %i \n",qqq.size(),runThreads);
        for(int i=0;i<qqq.size();i++){
            WriteProfile(qqq.front());
            qqq.pop();
        }
        // for(const auto &q : qqq){
        //     WriteProfile(q);
        // }
    }

    void EndSession()
    {
        // printf("Instrumentator END");
        runThreads = 0;
        while(!task.joinable());
        task.join();
        // std::unique_lock<std::mutex> lock(m_mutex);
        store();
        WriteFooter();
        m_OutputStream.close();
        delete m_CurrentSession;
        m_CurrentSession = nullptr;
        m_ProfileCount = 0;
        // printf(" DONE\n");
    }

    

    void push(const ProfileResult&& result)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(!m_OutputStream.is_open()) return;
        qqq.push(std::move(result));
    }

    void WriteProfile(const ProfileResult& result)
    {
       std::unique_lock<std::mutex> lock(m_mutex);
       if(!m_OutputStream.is_open()) return;
        if (m_ProfileCount++ > 0)
            m_OutputStream << ",";

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');
        auto dur = result.End - result.Start;

        m_OutputStream << "{";
        m_OutputStream << "\"cat\":\"function\",";
        m_OutputStream << "\"dur\":" << dur << ',';
        m_OutputStream << "\"name\":\"" << name << "\",";
        m_OutputStream << "\"ph\":\"X\",";
        m_OutputStream << "\"pid\":0,";
        m_OutputStream << "\"tid\":" << result.ThreadID << ",";
        m_OutputStream << "\"ts\":" << result.Start;
        m_OutputStream << "}\n";

        if(result.Name[0]=='_'){
            double q = dur/1000000.0;
            printf("%s: %f\n",result.Name,q);
        }

        // m_OutputStream.flush();
    }

    void WriteHeader()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
        // m_OutputStream.flush();
    }

    void WriteFooter()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_OutputStream << "]}";
        // m_OutputStream.flush();
    }

    static Instrumentor& Get()
    {
        static Instrumentor instance;
        return instance;
    }
};

static void handler(void *p){
    while(runThreads){
        if(!runThreads) break;
        Instrumentor::Get().store();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // printf("INTRUMENTATOR DETACH\n");
}

class InstrumentationTimer
{
public:
    InstrumentationTimer(const char* name)
        : m_Name(name), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer()
    {
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_StartTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch().count();

        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        const ProfileResult result = { m_Name, start, end, threadID };
        Instrumentor::Get().push(std::move(result));

        m_Stopped = true;
    }
private:
    const char* m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    bool m_Stopped;
};

/*
#include "Instrumentor.h"
PROFILE_SCOPE(name);
PROFILE_FUNCTION();
*/

#define PROFILING 0
#if PROFILING
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() static const char *__PROFILE_FNAME__ = __FUNCTION__; PROFILE_SCOPE(__PROFILE_FNAME__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif