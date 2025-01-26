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
	N = N_;
}

void StaticState::processInput(Application *app)
{
	std::string str;
	getline(std::cin, str);
	if (std::cin.bad() || std::cin.eof())
	{
		app->flushLogs();
		app->terminate();
	}
	else
	{
		if (str == "{")
		{
			app->flushLogs();
			app->setCurrentState(IStatePtr{new DynamicState()});
		}
		else
		{
			ICommandPtr newComm(new DumbCommand(str));
			app->staticPush(newComm);
		}
	}
	return;
}

void DynamicState::processInput(Application *app)
{
	std::string str;
	getline(std::cin, str);
	if (std::cin.bad() || std::cin.eof())
	{
		app->terminate();
	}
	else
	{
		if (str == "{")
			openCounter += 1;
		else if (str == "}")
			openCounter -= 1;
		else
		{
			ICommandPtr newComm(new DumbCommand(str));
			app->dynamicPush(newComm);
		}
		if(openCounter == 0)
		{
			app->flushLogs();
			app->setCurrentState(IStatePtr{new StaticState()});
		}
	}
	return;
}