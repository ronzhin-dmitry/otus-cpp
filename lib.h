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

namespace database
{
    using namespace std;
    class Database
    {
        private:
            map<int, string> table_A;
            map<int, string> table_B;
            shared_mutex mtx;
            string intersection();
            string symmetric_difference();
            string insert_A(int id, string val);
            string insert_B(int id, string val);
            string truncate_A();
            string truncate_B();
        public:
            string execute(string command);
    };
}

namespace join_server
{
    using boost::asio::ip::tcp;
    // Класс для обработки соединения
    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket, database::Database &db_)
            : socket_(std::move(socket)), db(db_){}
        
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
                            response = db.execute(message);
                        else
                            response = db.execute(message); //TODO: maybe later add different logic
                        
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
        database::Database &db;
    };

    class Server {
    public:
        database::Database db;
        Server(boost::asio::io_context& io_context, short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
            accept_connection();
        }

    private:
        void accept_connection() {
            acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<Session>(std::move(socket), db)->start();
                    }
                    accept_connection();
                });
        }

        tcp::acceptor acceptor_;
    };
} // namespace join_server