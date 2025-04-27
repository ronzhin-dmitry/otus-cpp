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
 * I leave it here for good memories about the first example on the course =)
 */
int version();

#include <vector>
#include <string>
#include <stack>
#include <cmath>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <map>

#define VARIANCE_ESTIMATE_SAMPLE_SIZE 10000

namespace expression_parser{

using namespace std;

    /*!
	\brief Класс для синтаксического парсинга текстовой строки.
    Объект expressionParser один раз читает текстовую строку.
    Тестовая строка превращается в вектор токенов в обратной польской записи (RPN).
    Метод evaluate через чтение RPN вычисляет значение выражения в точке.
    Чтение RPN может выполняться в параллель множеством потоков (evaluate потокобезопасен).
    */
    class ExpressionParser {
    public:
        /// @brief Инициализирует парсер с заданным выражением
        ExpressionParser(const string& expr) {
            tokens = tokenize(expr);
            rpn = shuntingYard(tokens);
        }
        /// @brief Обновляет текущее выражение для парсинга
        void set_expression(const string& expr) {
            tokens = tokenize(expr);
            rpn = shuntingYard(tokens);
        }
        /// @brief Вычисляет значение выражения в точке x
        double evaluate(double x);

    private:
        /// @brief Токен для представления элементов выражения
        struct Token {
            enum Type {
                NUMBER, VARIABLE, CONSTANT,
                OPERATOR, FUNCTION, LPAREN, RPAREN,
                ARG_SEPARATOR
            } type;
            string value;

            Token(Type t, const string& v);
        };

        vector<Token> tokens;
        vector<Token> rpn;

        /// @brief Разбивает строку на токены
        vector<Token> tokenize(const string& expr);

        /// @brief Преобразует токены в RPN с помощью алгоритма Shunting Yard
        vector<Token> shuntingYard(const vector<Token>& tokens);

        /// @brief Обрабатывает операторы при вычислении RPN
        void handleOperator(const string& op, stack<double>& evalStack);

        /// @brief Обрабатывает функции при вычислении RPN
        void handleFunction(const string& func, stack<double>& evalStack);

        /// @brief Проверяет, является ли строка именем функции
        bool isFunction(const string& name);

        /// @brief Возвращает количество аргументов для функции
        size_t getFunctionArgCount(const string& func);

        /// @brief Возвращает приоритет оператора
        int getPriority(const string& op);

        /// @brief Проверяет левую ассоциативность оператора
        bool isLeftAssociative(const string& op);
    };
 
} //expression_parser

namespace monte_carlo_multithread
{
    using namespace std;
    using namespace boost::asio;
    /// @brief Задача для параллельного вычисления методом Монте-Карло
    class MonteCarloTask {
        public:
            MonteCarloTask(double a, double b, size_t total_points, size_t num_workers)
                : a(a), b(b), points_per_worker(total_points / num_workers),
                  remaining_workers(num_workers), result(0.0) {}
        
            double a;
            double b;
            size_t points_per_worker;
            std::atomic<size_t> remaining_workers;
            double result; 
            std::mutex result_mutex; 
        };
    
    /*!
	\brief Класс для численного интегрирования методом Монте-Карло.
    Работает с использованием boost thread pool.
    Вычисляются частичные суммы и аккумулируется ответ через метод execute.
    В случае несобственного интеграла с одним бесконечным пределом производится замена переменных.
    */
    class Integrator
    {
        private:
            thread_pool tpool_;
            size_t max_nworkers_;
            double calculate_partial_sum(double a, double b, size_t points, function<double(double)> fquery);
            struct TransformedIntegral {
                std::function<double(double)> transformed_f;
                double new_a;
                double new_b;
                TransformedIntegral();
            };
        
            TransformedIntegral transform_integral(const std::function<double(double)>& f, double a, double b);

            std::string execute_impl(const std::function<double(double)>& fquery, double a, double b, size_t num_points, size_t num_workers);

        public:
            /// @brief Инициализирует интегратор с указанным числом потоков
            explicit Integrator(size_t nworkers = 0) 
                : tpool_(nworkers ? nworkers : std::thread::hardware_concurrency()),
                max_nworkers_(nworkers ? nworkers : std::thread::hardware_concurrency()) 
            {}

            Integrator(const Integrator&) = delete;
            Integrator& operator=(const Integrator&) = delete;

            /// @brief Обрабатывает строковый запрос на интегрирование
            string execute(string query);
            /// @brief Выполняет интегрирование функции f на интервале [a,b]
            string execute(function<double(double)> fquery, double a, double b, size_t num_points, size_t num_workers = 0);
    };
}

namespace monte_carlo_server
{
    using boost::asio::ip::tcp;
    using namespace monte_carlo_multithread;
    /*!
	\brief Класс для сессии асинхронного сервера
    Основная работа в read_message
    */
    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket, Integrator& mci)
            : socket_(std::move(socket)), mci_(mci){}
        
        void start() {
                // Убедиться, что сокет открыт
                if (!socket_.is_open()) return;
                
                boost::asio::post(socket_.get_executor(),
                    [self = shared_from_this()]() {
                        self->read_message();
                    }
                );
        }

    private:
        /// @brief Асинхронно читает сообщение от клиента
        void read_message();

        tcp::socket socket_;
        boost::asio::streambuf buffer_;
        Integrator &mci_;
    };

    /*!
	\brief Класс асинхронного сервера. Поддерживает множество сессий.
    */
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
                    std::make_shared<Session>(std::move(socket), mci_)->start();
                }
                accept_connection(); // Важно: продолжать принимать соединения
            }
        );
    }

        tcp::acceptor acceptor_;
    };
} // namespace monte_carlo_server