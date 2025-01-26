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

void DynamicState::processInput(Application * app)
{
	std::cout << "In Dyn " << std::endl;
	std::string str;
	getline(std::cin, str);
	if (std::cin.bad() || std::cin.eof())
	{
		app->flushLogs();
		app->terminate();
	}
	return;
}