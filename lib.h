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
    virtual void log(const std::list<ICommandPtr> &) = 0;
    virtual ~ILogger() {};
    void set_id(std::string& i) { id = i;};
};

class IReader {
protected:
    std::istream* stream;
    void assign_stream(std::istream* s) { stream = s; }
    
public:
    IReader(std::istream* s) : stream(s) {}
    virtual int read(std::string& str) = 0;
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

    int read(std::string& str) override;

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

    int read(std::string& str) override;

    void recieveString(std::string& str) override {
        iss.str(str);
        iss.clear();
    }
};

using ILoggerPtr = std::unique_ptr<ILogger>;
using IReaderPtr = std::unique_ptr<IReader>;
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
public:
    std::string str;
    int read_res;
    void setCurrentState(IStatePtr newState)
    {
        if (newState != curState)
        {
            coms.clear();
        }
        curState = std::move(newState);
    }

    Application(size_t N_);
    Application(size_t N_, IReaderPtr ir);
    void runApp()
    {
        read_res = 0;
        while (curState != nullptr && read_res == 0)
        {
            read_res = reader->read(str);
            curState->processInput(this);
            if(read_res > 1)
                break;
        }
    }

    void terminate()
    {
        read_res = 1;
        curState->processInput(this);
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

    void pushStringToReader(std::string& str)
    {
        //for the basic reader it will start and finish
        //for yield reader it will wait for new string or manual terminate
        reader->recieveString(str);
        read_res = 0;
        this->runApp();
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
        std::ofstream out(std::string("bulk") + createTime + "_" + id +".log");
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