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
	reader = IReaderPtr{new BasicReader()};
	N = N_;
}

Application::Application(size_t N_, IReaderPtr ir)
{
	curState = IStatePtr{new StaticState()};
	reader = std::move(ir);
	N = N_;
}

ReadState BasicReader::read(std::string& str)
{
	getline(*stream, str);
	if (stream->bad() || stream->eof())
	{
		return ReadState::DONE;
	}
	return ReadState::READY;
}

ReadState YieldReader::read(std::string& str)
{
	getline(*stream, str);
	if (stream->bad() || stream->eof())
	{
		return ReadState::WAIT;
	}
	return ReadState::READY; //will do that until terminate - we can wait for new data 
}

void StaticState::processInput(Application *app)
{
	if(app->read_res == ReadState::DONE)
	{
		app->flushLogs();
		return;
	}
	else if(app->read_res == ReadState::WAIT)
	{
		buf = app->str;
		return;
	}
	app->str = buf + app->str;
	buf = "";
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
	if(app->read_res == ReadState::DONE)
	{
		return;
	}
	else if(app->read_res == ReadState::WAIT)
	{
		buf = app->str;
		return;
	}
	app->str = buf + app->str;
	buf = "";
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