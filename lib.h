#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <vector>
#include <tuple>
/**
 * @brief simple function to return version of the release.
 */
int version();

namespace monte_carlo_multithread
{
    using namespace std;
    using namespace boost::asio;
    class MonteCarloTask {
        public:
            MonteCarloTask(double a, double b, size_t total_points, size_t num_workers)
                : a(a), b(b), points_per_worker(total_points / num_workers),
                  remaining_workers(num_workers), result(0.0) {}
        
            double a;
            double b;
            size_t points_per_worker;
            std::atomic<size_t> remaining_workers;
            std::atomic<double> result;
        };

    class Integrator
    {
        private:
            thread_pool tpool_;
            size_t max_nworkers_;
            double calculate_partial_sum(double a, double b, size_t points, function<double(double)> fquery);
        public:
            explicit Integrator(size_t nworkers = 0) 
                : tpool_(nworkers ? nworkers : std::thread::hardware_concurrency()),
                max_nworkers_(nworkers ? nworkers : std::thread::hardware_concurrency()) 
            {}

            Integrator(const Integrator&) = delete;
            Integrator& operator=(const Integrator&) = delete;
            string execute(string query);
            string execute(function<double(double)> fquery, double a, double b, size_t num_points, size_t num_workers = 0);
    };
}

namespace monte_carlo_server
{
    using boost::asio::ip::tcp;
    using namespace monte_carlo_multithread;
    // Класс для обработки соединения
    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket, Integrator& mci)
            : socket_(std::move(socket)), mci_(mci){}
        
        void start() {
            read_message();
        }

    private:
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
                        
                        // Применяем функцию обработки
                        std::string response = "";
                        
                        if(length != 0)
                            response = mci_.execute(message);
                        else
                            response = mci_.execute(message); //TODO: maybe later add different logic
                        
                        // Асинхронная отправка ответа
                        boost::asio::async_write(socket_, 
                            boost::asio::buffer(response),
                            [this, self](boost::system::error_code ec, std::size_t) {
                                if (!ec) {
                                    // Читаем следующее сообщение
                                    read_message();
                                }
                            });
                            
                    }
                });
        }

        tcp::socket socket_;
        boost::asio::streambuf buffer_;
        Integrator &mci_;
    };

    class Server {
    public:
        Integrator mci_;
        Server(boost::asio::io_context& io_context, short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
            accept_connection();
        }

    private:
        void accept_connection() {
            acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<Session>(std::move(socket), std::ref(mci_))->start();
                    }
                    accept_connection();
                });
        }

        tcp::acceptor acceptor_;
    };
} // namespace monte_carlo_server