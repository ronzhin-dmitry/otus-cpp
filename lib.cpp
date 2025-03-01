/**
@file lib.cpp
*/
#include "lib.h"
#include "version.h"

/**
 * @brief function to produce release version in github workflows
 * @return current release version (incremented on git push)
 */
int version()
{
	return PROJECT_VERSION_PATCH;
}

Application::Application(size_t N_)
{
	curState = IStatePtr{new StaticState()};
	reader = IReaderPtr{new CINReader()};
	N = N_;
}

Application::Application(size_t N_, IReaderPtr ir)
{
	curState = IStatePtr{new StaticState()};
	reader = std::move(ir);
	N = N_;
}

bool CINReader::read(std::string& str)
{
	getline(std::cin, str);
	if (std::cin.bad() || std::cin.eof())
	{
		return false;
	}
	return true;
}

bool StringReader::read(std::string& str)
{
	getline(std::cin, str);
	return true;
}

void StaticState::processInput(Application *app)
{
	if(!app->read_res)
	{
		app->flushLogs();
		app->terminate();
		return;
	}
	if (app->str == "{")
	{
		app->flushLogs();
		app->setCurrentState(IStatePtr{new DynamicState()});
	}
	else
	{
		ICommandPtr newComm(new DumbCommand(app->str));
		app->staticPush(newComm);
	}
	return;
}

void DynamicState::processInput(Application *app)
{
	if(!app->read_res)
	{
		app->terminate();
		return;
	}
	if (app->str == "{")
		openCounter += 1;
	else if (app->str == "}")
		openCounter -= 1;
	else
	{
		ICommandPtr newComm(new DumbCommand(app->str));
		app->dynamicPush(newComm);
	}
	if(openCounter == 0)
	{
		app->flushLogs();
		app->setCurrentState(IStatePtr{new StaticState()});
	}
	return;
}