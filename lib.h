#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <ctime>

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
    public:
        virtual std::string serialize() = 0;
        virtual void exec() {};
};

class DumbCommand:ICommand
{
    private:
        std::string body;
        time_t creation_time;
        DumbCommand(){};
    public:
        std::string serialize() override
        {
            return body;
        }

        DumbCommand(std::string cmd):body(cmd), creation_time(time(0)){};
};

class ILogger
{
    public:
    virtual void update(const std::list<ICommand*> &) = 0;
};

class ConsoleLogger:ILogger
{
    public:
        void update(const std::list<ICommand*> &)
        {
            //TODO
            return;
        }
};

class FileLogger:ILogger
{
    void update(const std::list<ICommand*> &)
        {
            //TODO
            return;
        }
};

/**
 * @brief Virtual class for current state of parser (we model it with finite automaton)
 * Each state implemented via singletone pattern
 */
class IState
{
public:
    virtual IState& getInstance() = 0;
    virtual void processInput(Application *) = 0;
};

class StaticState:IState
{
public:
    IState& getInstance()
    {
        static StaticState instance;
        return instance;
    }

    void processInput(Application *)
    {
        //TODO: process input stream line
        return;
    }
};

class DynamicState:IState
{
public:
    IState& getInstance()
    {
        static DynamicState instance;
        return instance;
    }

    void processInput(Application *)
    {
        //TODO: process input stream line
        return;
    }
};

/**
 * @brief Base class for context of the automaton, allows for context switching based on states and inputs
 */
class Application
{
    private:
    IState* curState;
    Application(){};
    std::list<ICommand*> coms;
    int N; //max commands in static mode
    public:
    void setCurrentState(IState* newState) {
            curState = newState;
    }

    Application(int N_, IState* initialState)
    {
        curState = initialState;
        N = N_;
    }

    void runApp()
    {
        std::cout << "app started" << std::endl;
        while(curState != nullptr)
        {
            curState->processInput(this);
        }
        std::cout << "app finished" << std::endl;
    }

    void terminate()
    {
        curState = nullptr;
        //TODO - consider additional logic
    }
};