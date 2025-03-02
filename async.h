#pragma once

#include <cstddef>
#include "lib.h"
#include <unordered_map>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <chrono>


namespace async {
//Multithread wrappers for async calls
using handle_t = size_t;
inline std::unordered_map<handle_t, std::unique_ptr<Application>> contexts; //global variable for context swtiching
inline handle_t las_unused = 0;

class AsyncFileLoggerPull : public ILogger
{
private:
    std::queue<std::list<ICommandPtr>> message_queue;
    std::mutex queue_mutex;
    std::condition_variable cv;
    std::vector<std::thread> workers;
    bool stop_flag;
    void worker_thread();
public:
    void log(std::list<ICommandPtr> comms);
    AsyncFileLoggerPull(size_t N_workers = 2);
    ~AsyncFileLoggerPull();
};

class AsyncConsoleLogger : public ILogger
{
private:
    std::queue<std::list<ICommandPtr>> message_queue;
    std::mutex queue_mutex;
    std::condition_variable cv;
    std::thread worker;
    bool stop_flag;
    void worker_thread();
public:
    void log(std::list<ICommandPtr> comms);
    AsyncConsoleLogger();
    ~AsyncConsoleLogger();
};

inline ILoggerPtr afpull = ILoggerPtr{new AsyncFileLoggerPull()};
inline ILoggerPtr acLogger = ILoggerPtr{new AsyncConsoleLogger()};


//interfaces from the task statement
handle_t connect(std::size_t bulk);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

}
