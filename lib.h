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

#define VARIANCE_ESTIMATE_SAMPLE_SIZE 1000

namespace expression_parser{

using namespace std;

    class ExpressionParser {
    public:
        ExpressionParser(const string& expr) {
            tokens = tokenize(expr);
            rpn = shuntingYard(tokens);
        }

        void set_expression(const string& expr) {
            tokens = tokenize(expr);
            rpn = shuntingYard(tokens);
        }

        double evaluate(double x) {
            stack<double> evalStack;
            for (const auto& token : rpn) {
                switch (token.type) {
                    case Token::NUMBER:
                        evalStack.push(stod(token.value));
                        break;
                    case Token::VARIABLE:
                        evalStack.push(x);
                        break;
                    case Token::CONSTANT:
                        if (token.value == "e") {
                            evalStack.push(M_E);
                        } else if (token.value == "pi") {
                            evalStack.push(M_PI);
                        } else {
                            throw invalid_argument("Unknown constant: " + token.value);
                        }
                        break;
                    case Token::OPERATOR:
                        handleOperator(token.value, evalStack);
                        break;
                    case Token::FUNCTION:
                        handleFunction(token.value, evalStack);
                        break;
                    default:
                        break;
                }
            }
            if (evalStack.size() != 1) {
                throw invalid_argument("Invalid expression");
            }
            return evalStack.top();
        }

    private:
        struct Token {
            enum Type {
                NUMBER, VARIABLE, CONSTANT,
                OPERATOR, FUNCTION, LPAREN, RPAREN,
                ARG_SEPARATOR
            } type;
            string value;

            Token(Type t, const string& v) : type(t), value(v) {}
        };

        vector<Token> tokens;
        vector<Token> rpn;

        vector<Token> tokenize(const string& expr) {
            vector<Token> tokens;
            size_t i = 0;
            while (i < expr.size()) {
                if (isspace(expr[i])) {
                    ++i;
                    continue;
                }

                if (isalpha(expr[i])) {
                    string name;
                    while (i < expr.size() && isalpha(expr[i])) {
                        name += tolower(expr[i++]);
                    }
                    if (name == "x") {
                        tokens.emplace_back(Token::VARIABLE, "x");
                    } else if (name == "e") {
                        tokens.emplace_back(Token::CONSTANT, "e");
                    } else if (name == "pi") {
                        tokens.emplace_back(Token::CONSTANT, "pi");
                    } else if (isFunction(name)) {
                        tokens.emplace_back(Token::FUNCTION, name);
                    } else {
                        throw invalid_argument("Unknown identifier: " + name);
                    }
                } else if (isdigit(expr[i]) || expr[i] == '.') {
                    string num;
                    while (i < expr.size() && (isdigit(expr[i]) || expr[i] == '.' || tolower(expr[i]) == 'e')) {
                        num += expr[i++];
                    }
                    try {
                        stod(num);
                    } catch (...) {
                        throw invalid_argument("Invalid number: " + num);
                    }
                    tokens.emplace_back(Token::NUMBER, num);
                } else if (expr[i] == '(') {
                    tokens.emplace_back(Token::LPAREN, "(");
                    ++i;
                } else if (expr[i] == ')') {
                    tokens.emplace_back(Token::RPAREN, ")");
                    ++i;
                } else if (expr[i] == ',') {
                    tokens.emplace_back(Token::ARG_SEPARATOR, ",");
                    ++i;
                } else if (expr[i] == '+' || expr[i] == '*' || expr[i] == '/' || expr[i] == '^') {
                    tokens.emplace_back(Token::OPERATOR, string(1, expr[i]));
                    ++i;
                } else if (expr[i] == '-') {
                    bool isUnary = tokens.empty() ||
                                    tokens.back().type == Token::LPAREN ||
                                    tokens.back().type == Token::OPERATOR;
                    tokens.emplace_back(Token::OPERATOR, isUnary ? "neg" : "-");
                    ++i;
                } else {
                    throw invalid_argument("Invalid character: " + string(1, expr[i]));
                }
            }
            return tokens;
        }

        vector<Token> shuntingYard(const vector<Token>& tokens) {
            vector<Token> output;
            stack<Token> opStack;

            for (const auto& token : tokens) {
                switch (token.type) {
                    case Token::NUMBER:
                    case Token::VARIABLE:
                    case Token::CONSTANT:
                        output.push_back(token);
                        break;
                    case Token::FUNCTION:
                        opStack.push(token);
                        break;
                    case Token::LPAREN:
                        opStack.push(token);
                        break;
                    case Token::RPAREN: {
                        while (!opStack.empty() && opStack.top().type != Token::LPAREN) {
                            output.push_back(opStack.top());
                            opStack.pop();
                        }
                        if (opStack.empty()) {
                            throw invalid_argument("Mismatched parentheses");
                        }
                        opStack.pop();
                        if (!opStack.empty() && opStack.top().type == Token::FUNCTION) {
                            output.push_back(opStack.top());
                            opStack.pop();
                        }
                        break;
                    }
                    case Token::ARG_SEPARATOR: {
                        while (!opStack.empty() && opStack.top().type != Token::LPAREN) {
                            output.push_back(opStack.top());
                            opStack.pop();
                        }
                        if (opStack.empty()) {
                            throw invalid_argument("Mismatched separator");
                        }
                        break;
                    }
                    case Token::OPERATOR: {
                        while (!opStack.empty() && opStack.top().type != Token::LPAREN &&
                            (getPriority(opStack.top().value) > getPriority(token.value) ||
                            (getPriority(opStack.top().value) == getPriority(token.value) &&
                                isLeftAssociative(token.value)))) {
                            output.push_back(opStack.top());
                            opStack.pop();
                        }
                        opStack.push(token);
                        break;
                    }
                    default:
                        break;
                }
            }

            while (!opStack.empty()) {
                if (opStack.top().type == Token::LPAREN) {
                    throw invalid_argument("Mismatched parentheses");
                }
                output.push_back(opStack.top());
                opStack.pop();
            }

            return output;
        }

        void handleOperator(const string& op, stack<double>& evalStack) {
            if (op == "neg") {
                if (evalStack.empty()) throw invalid_argument("Not enough operands");
                double a = evalStack.top(); evalStack.pop();
                evalStack.push(-a);
            } else {
                if (evalStack.size() < 2) throw invalid_argument("Not enough operands");
                double b = evalStack.top(); evalStack.pop();
                double a = evalStack.top(); evalStack.pop();
                if (op == "+") evalStack.push(a + b);
                else if (op == "-") evalStack.push(a - b);
                else if (op == "*") evalStack.push(a * b);
                else if (op == "/") {
                    if (b == 0) throw invalid_argument("Division by zero");
                    evalStack.push(a / b);
                }
                else if (op == "^") evalStack.push(pow(a, b));
                else throw invalid_argument("Unknown operator: " + op);
            }
        }

        void handleFunction(const string& func, stack<double>& evalStack) {
            size_t argCount = getFunctionArgCount(func);
            if (evalStack.size() < argCount) {
                throw invalid_argument("Not enough arguments for function " + func);
            }
            
            vector<double> args;
            for (size_t i = 0; i < argCount; ++i) {
                args.insert(args.begin(), evalStack.top());
                evalStack.pop();
            }

            if (func == "sin") evalStack.push(sin(args[0]));
            else if (func == "cos") evalStack.push(cos(args[0]));
            else if (func == "tan") evalStack.push(tan(args[0]));
            else if (func == "ctg") {
                if (tan(args[0]) == 0) throw invalid_argument("Cotangent of zero");
                evalStack.push(1.0 / tan(args[0]));
            }
            else if (func == "atan") evalStack.push(atan(args[0]));
            else if (func == "actg") evalStack.push(M_PI/2 - atan(args[0]));
            else if (func == "exp") evalStack.push(exp(args[0]));
            else if (func == "ln") {
                if (args[0] <= 0) throw invalid_argument("Log argument must be positive");
                evalStack.push(log(args[0]));
            }
            else if (func == "log") {
                if (args[0] <= 0 || args[1] <= 0) throw invalid_argument("Log arguments must be positive");
                evalStack.push(log(args[1]) / log(args[0]));
            }
            else throw invalid_argument("Unknown function: " + func);
        }

        bool isFunction(const string& name) {
            static const vector<string> funcs = {"sin", "cos", "tan", "ctg", 
                                            "atan", "actg", "exp", "ln", "log"};
            return find(funcs.begin(), funcs.end(), name) != funcs.end();
        }

        size_t getFunctionArgCount(const string& func) {
            static const map<string, int> argCounts = {
                {"sin", 1}, {"cos", 1}, {"tan", 1}, {"ctg", 1},
                {"atan", 1}, {"actg", 1}, {"exp", 1}, {"ln", 1}, {"log", 2}
            };
            auto it = argCounts.find(func);
            if (it != argCounts.end()) return it->second;
            throw invalid_argument("Unknown function: " + func);
        }

        int getPriority(const string& op) {
            if (op == "^") return 5;
            if (op == "neg") return 4;
            if (op == "*" || op == "/") return 3;
            if (op == "+" || op == "-") return 2;
            return 0;
        }

        bool isLeftAssociative(const string& op) {
            return op != "^" && op != "neg";
        }
    };
 
} //expression_parser

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
            double result; 
            std::mutex result_mutex; 
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
                        std::istream is(&buffer_);
                        std::string message;
                        std::getline(is, message);
                        
                        if (!message.empty() && message.back() == '\r') {
                            message.erase(message.end()-1);
                        }
                        
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
                                    // reading next message until we can
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