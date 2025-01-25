#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <memory>

/**
 * @brief simple function to return version of the release.
 */
int version();

/**
 * @brief general command class, self-contained. Allows serizalization and execution (possible) of a command.
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
        DumbCommand(){};
    public:
        std::string serialize() override
        {
            return body;
        }

        DumbCommand(std::string cmd):body(cmd){};
};

//TODO - add observer logger class

class Application;

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

/**
 * @brief Base class for context of the automaton, allows for context switching based on states and inputs
 */
class Application
{
    private:
    IState* curState;
    Application(){};
    public:
    void setCurrentState(IState* newState) {
            curState = newState;
         }

    Application(IState* initialState)
    {
        curState = initialState;
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