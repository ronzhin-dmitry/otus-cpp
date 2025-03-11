#include "async.h"

namespace async {

AsyncFileLoggerPull::AsyncFileLoggerPull(size_t N_workers)
{
    stop_flag = false;
    for(size_t i = 0 ; i < N_workers ; i++)
        workers.emplace_back(&AsyncFileLoggerPull::worker_thread, this);
}

AsyncFileLoggerPull::~AsyncFileLoggerPull() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop_flag = true;
    }
    cv.notify_all();
    
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void AsyncFileLoggerPull::log(std::list<ICommandPtr> comms)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        message_queue.push(comms);
    }
    cv.notify_one();
}

void AsyncFileLoggerPull::worker_thread()
{
    while (true) {
        std::list<ICommandPtr> message;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
                
            cv.wait(lock, [this]() { 
                return !message_queue.empty() || stop_flag; 
            });

            if (stop_flag && message_queue.empty()) {
                break;
            }

            if (!message_queue.empty()) {
                message = std::move(message_queue.front());
                message_queue.pop();
            }
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 1000000);
        int randomNum = distrib(gen); //hotfix to make different filenames
        std::string random_id = std::to_string(randomNum);
        time_t tt = 0;
        if(!message.empty())
            tt = (*message.begin())->getCreationTime();
        std::string createTime = std::to_string((long long)tt);
        auto tid = std::this_thread::get_id();
        std::stringstream ss;
        ss << tid;
        std::string tid_string = ss.str();
        std::string filename = std::string("bulk") + createTime + "_" + random_id + "_" + tid_string + ".log";
        std::ofstream out(filename);
            
        if (!out.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            return;
        }

        if (!message.empty()) {
            for (auto it = message.begin(); it != message.end(); it++)
            {
                out << (*it)->serialize();
                if(*it != message.back())
                    out << ",";
            }
            if (out.fail()) {
                std::cerr << "Write error in file: " << filename << std::endl;
                break;
            }
        }
        out.close();
    }
}


AsyncConsoleLogger::AsyncConsoleLogger()
{
    stop_flag = false;
    worker = std::thread(&AsyncConsoleLogger::worker_thread, this);
}

AsyncConsoleLogger::~AsyncConsoleLogger() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop_flag = true;
    }
    cv.notify_one();
    if (worker.joinable()) {
        worker.join();
    }
}

void AsyncConsoleLogger::log(std::list<ICommandPtr> comms)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        message_queue.push(comms);
    }
    cv.notify_one();
}

void AsyncConsoleLogger::worker_thread()
{
    while (true) {
        std::list<ICommandPtr> message;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
                
            cv.wait(lock, [this]() { 
                return !message_queue.empty() || stop_flag; 
            });

            if (stop_flag && message_queue.empty()) {
                break;
            }

            if (!message_queue.empty()) {
                message = std::move(message_queue.front());
                message_queue.pop();
            }
        }
        if(message.size() == 0)
            return;
        for (auto it = message.begin(); it != message.end(); it++)
        {
            std::cout << (*it)->serialize();
            if(*it != message.back())
                std::cout << ",";
        }
        std::cout << std::endl;
    }
}

handle_t connect(std::size_t bulk) {
    handle_t res = las_unused;
    if(contexts.find(res) != contexts.end())
        return -1; // some internal error, no more free handles
    las_unused++;
    IReaderPtr ip = IReaderPtr{new YieldReader()};
    contexts[res] = std::unique_ptr<Application>(new Application(bulk, std::move(ip)));
    contexts[res]->subscribeLogger(acLogger); //subscribe async console logger
    contexts[res]->subscribeLogger(afpull); //subscribe async file logger
    return res;
}

void receive(handle_t h, const char * data, std::size_t sz) {
    if (!data || sz == 0) 
        return;
    if(contexts.find(h) == contexts.end())
        return;
    std::string message(static_cast<const char*>(data), sz);
    contexts[h]->pushStringToReader(message);
}

void disconnect(handle_t h) {
    if(contexts.find(h) != contexts.end())
    {
        contexts[h]->terminate();
        contexts.erase(h);
    }
}

}


namespace asio_server
{
void SlaveStaticState::processInput(Application *app)
{
    //std::cout << "In slaveStatic, read state = " << int(app->read_res) << std::endl;
    //std::cout << "cur app->str " << app->str << std::endl;
	if (app->str == "{")
	{
        //std::cout << "changing state " << std::endl;
		app->setCurrentState(IStatePtr{new SlaveDynamicState()});
	}
	else
	{
		ICommandPtr newComm(new DumbCommand(app->str));
        auto ma = app->getMaster(); //the only major difference
        if(ma != nullptr)
		    ma->asyncStaticPush(newComm);
	}
	return;
}

void SlaveDynamicState::processInput(Application *app)
{
    //std::cout << "In dynamic state " << std::endl;
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
		app->setCurrentState(IStatePtr{new SlaveStaticState()});
	}
	return;
}
}