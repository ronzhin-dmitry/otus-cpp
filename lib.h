#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <ctime>
#include <fstream>
#include <sstream>
#include <boost/asio.hpp>
#include <mutex>
/**
 * @brief simple function to return version of the release.
 */
int version();
class Application;

/**
 * @brief general command class, self-contained. Allows serizalization and execution (possible) of a command.
 * Actual simple embodiment of command pattern which is not required in this app.
 */
class ICommand
{
private:
    time_t creation_time;
public:
    virtual std::string serialize() = 0;
    virtual void exec() {};
    ICommand() : creation_time(time(0)){};
    time_t getCreationTime() { return creation_time;}
    virtual ~ICommand() {};
};

class DumbCommand : public ICommand
{
private:
    std::string body; //body can be empty, any string is valid
    DumbCommand() {};

public:
    std::string serialize() override
    {
        return body;
    }

    DumbCommand(std::string cmd) : body(cmd) {};
    ~DumbCommand() {};
};
using ICommandPtr = std::shared_ptr<ICommand>;

/**
 * @brief Virtual class for current state of parser (we model it with finite automaton)
 */
class IState
{
protected:
    std::string buf = "";
public:
    virtual void processInput(Application *) = 0;
    virtual ~IState() {};
};

using IStatePtr = std::unique_ptr<IState>;

class ILogger
{
protected:
    std::string id = "";
public:
    virtual void log(std::list<ICommandPtr>) = 0;
    virtual ~ILogger() {};
    void set_id(std::string& i) { id = i;};
};
enum class ReadState{READY = 0, DONE = 1 , WAIT = 2};
class IReader {
protected:
    std::istream* stream;
    void assign_stream(std::istream* s) { stream = s; }
    
public:
    IReader(std::istream* s) : stream(s) {}
    virtual ReadState read(std::string& str) = 0;
    virtual void recieveString(std::string& str) = 0;
    virtual ~IReader() {}
};

class StringReaderBase {
protected:
    std::istringstream iss;
};

class BasicReader : public StringReaderBase, public IReader {
public:
    BasicReader() : IReader(&iss) {}  // Теперь iss уже существует
    
    BasicReader(const std::string& s) 
        : IReader(&iss)  // Передаем адрес iss в базовый класс
    { 
        iss.str(s);  // Устанавливаем строку
    }

    ReadState read(std::string& str) override;

    void recieveString(std::string& str) override {
        iss.str(str);
        iss.clear();
    }
};

class YieldReader : public StringReaderBase, public IReader {
public:
    YieldReader() : IReader(&iss) {}
    
    YieldReader(const std::string& s) 
        : IReader(&iss) 
    { 
        iss.str(s);
    }

    ReadState read(std::string& str) override;

    void recieveString(std::string& str) override {
        iss.str(str);
        iss.clear();
    }
};
class Application;
using ILoggerPtr = std::shared_ptr<ILogger>;
using IReaderPtr = std::unique_ptr<IReader>;
using IAppPtr = std::shared_ptr<Application>;
/**
 * @brief Base class for context of the automaton, allows for context switching based on states and inputs
 */
class Application
{
private:
    IStatePtr curState;
    IReaderPtr reader;
    Application() {};
    std::list<ICommandPtr> coms;
    std::list<ILoggerPtr> loggers;
    size_t N; // max commands in static mode
    std::mutex coms_static_mtx;
    IAppPtr master_app;
public:
    std::string str;
    ReadState read_res;
    IAppPtr getMaster() {return master_app;}
    void setCurrentState(IStatePtr newState)
    {
        if (newState != curState)
        {
            coms.clear();
        }
        curState = std::move(newState);
    }

    Application(size_t N_);
    Application(size_t N_, IReaderPtr ir, IAppPtr ma = nullptr);
    Application(size_t N_, IStatePtr is, IReaderPtr ir, IAppPtr ma = nullptr);
    void runApp()
    {
        read_res = ReadState::READY;
        while (curState != nullptr && read_res == ReadState::READY)
        {
            read_res = reader->read(str);
            //std::cout << "Read string: " << str << std::endl;
            //std::cout << "Cur read res state " << int(read_res) << std::endl;
            curState->processInput(this);
            if(read_res == ReadState::WAIT)
                break;
        }
    }

    void terminate()
    {
        read_res = ReadState::DONE;
        curState->processInput(this);
        // TODO - consider additional logic
    }

    void subscribeLogger(ILoggerPtr &logger)
    {
        loggers.push_back(logger);
    }

    void flushLogs()
    {
        for (auto &logger : loggers)
            logger->log(coms);
        coms.clear();
    }

    void staticPush(ICommandPtr cmd)
    {
        coms.emplace_back(std::move(cmd));
        if (coms.size() == N)
            flushLogs();
    }

    void dynamicPush(ICommandPtr cmd)
    {
        coms.emplace_back(std::move(cmd));
    }

    void asyncStaticPush(ICommandPtr cmd)
    {
        std::lock_guard<std::mutex> guard(coms_static_mtx);
        //std::cout << "In async push, recieved " << cmd->serialize() << std::endl;
        coms.emplace_back(std::move(cmd));
        if (coms.size() == N)
            flushLogs();
    }

    void pushStringToReader(std::string& str)
    {
        //for the basic reader it will start and finish
        //for yield reader it will wait for new string or manual terminate
        reader->recieveString(str);
        read_res = ReadState::READY;
        this->runApp();
    }
};

class StaticState : public IState
{
public:
    void processInput(Application *);
    ~StaticState() {};
};

class DynamicState : public IState
{
    int openCounter = 1;
public:
    void processInput(Application *);
    ~DynamicState() {};
};

class ConsoleLogger : public ILogger
{
public:
    void log(std::list<ICommandPtr> comms)
    {
        if(comms.size() == 0)
            return;
        for (auto it = comms.begin(); it != comms.end(); it++)
        {
            std::cout << (*it)->serialize();
            if(*it != comms.back())
                std::cout << ",";
        }
        std::cout << std::endl;
        return;
    }
    ConsoleLogger() {};
    ~ConsoleLogger() {};
};

class FileLogger : public ILogger
{
public:
    void log(std::list<ICommandPtr> comms)
    {
        if(comms.size() == 0)
            return;
        int randomNum = rand() % 100001;
        std::string random_id = std::to_string(randomNum);
        time_t tt = (*comms.begin())->getCreationTime();
        std::string createTime = std::to_string((long long)tt);
        std::ofstream out(std::string("bulk") + createTime + "_" + random_id + "_" + id +".log");
        for (auto it = comms.begin(); it != comms.end(); it++)
        {
            out << (*it)->serialize();
            if(*it != comms.back())
                out << ",";
        }
        out.close();
        return;
    }
    FileLogger() {};
    ~FileLogger() {};
};