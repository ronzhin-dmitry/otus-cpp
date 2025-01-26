#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <ctime>
#include <fstream>

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
using ICommandPtr = std::unique_ptr<ICommand>;

/**
 * @brief Virtual class for current state of parser (we model it with finite automaton)
 */
class IState
{
public:
    virtual void processInput(Application *) = 0;
    virtual ~IState() {};
};

using IStatePtr = std::unique_ptr<IState>;

class ILogger
{
public:
    virtual void log(const std::list<ICommandPtr> &) = 0;
    virtual ~ILogger() {};
};

using ILoggerPtr = std::unique_ptr<ILogger>;

/**
 * @brief Base class for context of the automaton, allows for context switching based on states and inputs
 */
class Application
{
private:
    IStatePtr curState;
    Application() {};
    std::list<ICommandPtr> coms;
    std::list<ILoggerPtr> loggers;
    size_t N; // max commands in static mode
public:
    void setCurrentState(IStatePtr newState)
    {
        if (newState != curState)
        {
            coms.clear();
        }
        curState = std::move(newState);
    }

    Application(size_t N_);

    void runApp()
    {
        while (curState != nullptr)
        {
            curState->processInput(this);
        }
    }

    void terminate()
    {
        curState = nullptr;
        // TODO - consider additional logic
    }

    void subscribeLogger(ILoggerPtr &logger)
    {
        loggers.emplace_back(std::move(logger));
    }

    void flushLogs()
    {
        for (auto &logger : loggers)
            logger->log(coms);
        coms.clear();
    }

    void staticPush(ICommandPtr &cmd)
    {
        coms.emplace_back(std::move(cmd));
        if (coms.size() == N)
            flushLogs();
    }

    void dynamicPush(ICommandPtr &cmd)
    {
        coms.emplace_back(std::move(cmd));
    }
};

class StaticState : public IState
{
public:
    void processInput(Application *app);
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
    void log(const std::list<ICommandPtr> &comms)
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
    void log(const std::list<ICommandPtr> & comms)
    {
        if(comms.size() == 0)
            return;
        time_t tt = (*comms.begin())->getCreationTime();
        std::string createTime = std::to_string((long long)tt);
        std::ofstream out(std::string("bulk") + createTime + ".log");
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