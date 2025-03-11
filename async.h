#pragma once

#include <cstddef>
#include "lib.h"
#include <unordered_map>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <chrono>
#include <pthread.h>
#include <boost/asio.hpp>

namespace async {
//Multithread wrappers for async calls
using handle_t = size_t;
inline std::unordered_map<handle_t, std::unique_ptr<Application>> contexts; //global variable for context swtiching
inline handle_t las_unused = 0;

class AsyncFileLoggerPull : public ILogger
{
private:
    std::queue<std::list<ICommandPtr>> message_queue;
    std::mutex queue_mutex;
    std::condition_variable cv;
    std::vector<std::thread> workers;
    bool stop_flag;
    void worker_thread();
public:
    void log(std::list<ICommandPtr> comms);
    AsyncFileLoggerPull(size_t N_workers = 2);
    ~AsyncFileLoggerPull();
};

class AsyncConsoleLogger : public ILogger
{
private:
    std::queue<std::list<ICommandPtr>> message_queue;
    std::mutex queue_mutex;
    std::condition_variable cv;
    std::thread worker;
    bool stop_flag;
    void worker_thread();
public:
    void log(std::list<ICommandPtr> comms);
    AsyncConsoleLogger();
    ~AsyncConsoleLogger();
};

inline ILoggerPtr afpull = ILoggerPtr{new AsyncFileLoggerPull()};
inline ILoggerPtr acLogger = ILoggerPtr{new AsyncConsoleLogger()};


//interfaces from the task statement
handle_t connect(std::size_t bulk);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

}

namespace asio_server
{
    using boost::asio::ip::tcp;

    class SlaveStaticState : public IState
    {
    public:
        void processInput(Application *);
        ~SlaveStaticState() {};
    };

    class SlaveDynamicState : public IState
    {
        int openCounter = 1;
    public:
        void processInput(Application *);
        ~SlaveDynamicState() {};
    };

    class Session: public std::enable_shared_from_this<Session>
    {
        private:
            tcp::socket socket_;
            boost::asio::streambuf buffer_;
            std::unique_ptr<Application> slave_app;
            int N_;
            void read_message() {
                auto self(shared_from_this());
                boost::asio::async_read_until(socket_, buffer_, '\n',
                    [this, self](boost::system::error_code ec, std::size_t length) {
                        if (!ec) {
                            // Извлекаем данные из буфера
                            std::istream is(&buffer_);
                            std::string message;
                            std::getline(is, message);
                            
                            // Удаляем возможный \r в конце (для совместимости с клиентами)
                            if (!message.empty() && message.back() == '\r') {
                                message.erase(message.end()-1);
                            }
                            //process with SlaveApp
                            //std::cout << "Recieved " << message << std::endl;
                            slave_app->pushStringToReader(message);

                            if(length != 0) // just to make compiler happy
                                read_message();
                            else
                                read_message();
                        }
                    });
            }
        public:
        Session(tcp::socket socket, int N, IAppPtr ma)
         : socket_(std::move(socket)), N_(N) {
            slave_app = std::unique_ptr<Application>(new Application(N_,std::unique_ptr<IState>(new SlaveStaticState()), std::unique_ptr<IReader>(new BasicReader()), ma));
            slave_app->subscribeLogger(async::acLogger);
            slave_app->subscribeLogger(async::afpull);
         }
     
        void start() {
            read_message();
        }
    };

    class Server
    {
        private:
        boost::asio::ip::tcp::acceptor acceptor_;
        IAppPtr master_application;
        int N_;
        void accept_connection()
        {
            acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket){
                    if(!ec)
                    {
                        std::make_shared<Session>(std::move(socket), N_, master_application)->start();
                    }
                    accept_connection();
                }
            );
        };

        public:
            Server(boost::asio::io_context& io_context, short port, int N):
                acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), N_(N)
                {
                    //in master application reader is not working so we don't bother
                    //it is just a sync-queue to push messages into from slaves, and run logger
                    master_application = std::shared_ptr<Application>(new Application(N_));
                    master_application->subscribeLogger(async::acLogger);
                    master_application->subscribeLogger(async::afpull);
                    accept_connection();
                }
            ~Server()
            {
                master_application->terminate();
            }

    };
} // namespace asio_server
