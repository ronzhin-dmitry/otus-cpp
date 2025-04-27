/**
@file lib.cpp
*/
#include "lib.h"
#include "version.h"
#include <random>

/**
 * @brief function to produce release version in github workflows
 * @return current release version (incremented on git push)
 */
int version()
{
    return PROJECT_VERSION_PATCH;
}

namespace expression_parser{
    using namespace std;
    double ExpressionParser::evaluate(double x) {
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
                        throw invalid_argument("Unknown constant: " + token.value + "\n");
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
            throw invalid_argument("Invalid expression\n");
        }
        return evalStack.top();
    }

    void ExpressionParser::handleOperator(const string& op, stack<double>& evalStack) {
        if (op == "neg") {
            if (evalStack.empty()) throw invalid_argument("Not enough operands\n");
            double a = evalStack.top(); evalStack.pop();
            evalStack.push(-a);
        } else {
            if (evalStack.size() < 2) throw invalid_argument("Not enough operands\n");
            double b = evalStack.top(); evalStack.pop();
            double a = evalStack.top(); evalStack.pop();
            if (op == "+") evalStack.push(a + b);
            else if (op == "-") evalStack.push(a - b);
            else if (op == "*") evalStack.push(a * b);
            else if (op == "/") {
                if (b == 0) throw invalid_argument("Division by zero\n");
                evalStack.push(a / b);
            }
            else if (op == "^") evalStack.push(pow(a, b));
            else throw invalid_argument("Unknown operator: " + op + "\n");
        }
    }

    void ExpressionParser::handleFunction(const string& func, stack<double>& evalStack) {
        size_t argCount = getFunctionArgCount(func);
        if (evalStack.size() < argCount) {
            throw invalid_argument("Not enough arguments for function " + func + "\n");
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
            if (tan(args[0]) == 0) throw invalid_argument("Cotangent of zero\n");
            evalStack.push(1.0 / tan(args[0]));
        }
        else if (func == "atan") evalStack.push(atan(args[0]));
        else if (func == "actg") evalStack.push(M_PI/2 - atan(args[0]));
        else if (func == "exp") evalStack.push(exp(args[0]));
        else if (func == "ln") {
            if (args[0] <= 0) throw invalid_argument("Log argument must be positive\n");
            evalStack.push(log(args[0]));
        }
        else if (func == "log") {
            if (args[0] <= 0 || args[1] <= 0) throw invalid_argument("Log arguments must be positive\n");
            evalStack.push(log(args[1]) / log(args[0]));
        }
        else throw invalid_argument("Unknown function: " + func + "\n");
    }

    bool ExpressionParser::isFunction(const string& name) {
        static const vector<string> funcs = {"sin", "cos", "tan", "ctg", 
                                        "atan", "actg", "exp", "ln", "log"};
        return find(funcs.begin(), funcs.end(), name) != funcs.end();
    }

    size_t ExpressionParser::getFunctionArgCount(const string& func) {
        static const map<string, int> argCounts = {
            {"sin", 1}, {"cos", 1}, {"tan", 1}, {"ctg", 1},
            {"atan", 1}, {"actg", 1}, {"exp", 1}, {"ln", 1}, {"log", 2}
        };
        auto it = argCounts.find(func);
        if (it != argCounts.end()) return it->second;
        throw invalid_argument("Unknown function: " + func + "\n");
    }

    int ExpressionParser::getPriority(const string& op) {
        if (op == "^") return 5;
        if (op == "neg") return 4;
        if (op == "*" || op == "/") return 3;
        if (op == "+" || op == "-") return 2;
        return 0;
    }

    bool ExpressionParser::isLeftAssociative(const string& op) {
        return op != "^" && op != "neg";
    }

    ExpressionParser::Token::Token(Type t, const string& v) : type(t), value(v) {}

    vector<ExpressionParser::Token> ExpressionParser::tokenize(const string& expr) {
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
                    throw invalid_argument("Unknown identifier: " + name + "\n");
                }
            } else if (isdigit(expr[i]) || expr[i] == '.') {
                string num;
                while (i < expr.size() && (isdigit(expr[i]) || expr[i] == '.' || tolower(expr[i]) == 'e')) {
                    num += expr[i++];
                }
                try {
                    stod(num);
                } catch (...) {
                    throw invalid_argument("Invalid number: " + num + "\n");
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
                throw invalid_argument("Invalid character: " + string(1, expr[i]) + "\n");
            }
        }
        return tokens;
    }

    vector<ExpressionParser::Token> ExpressionParser::shuntingYard(const vector<ExpressionParser::Token>& tokens) {
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
                        throw invalid_argument("Mismatched parentheses\n");
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
                        throw invalid_argument("Mismatched separator\n");
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
                throw invalid_argument("Mismatched parentheses\n");
            }
            output.push_back(opStack.top());
            opStack.pop();
        }

        return output;
    }
}

namespace monte_carlo_multithread
{
    using namespace std;
    struct ParseResult {
        bool use_num_points;
        bool just_evaluate_expression;
        std::string function;
        double a;
        double b;
        size_t num_points;
        double dispersion;
        size_t num_workers;
    };

    std::vector<std::string> split_tokens(const std::string& input) {
        std::vector<std::string> tokens;
        std::istringstream tokenStream(input);
        std::string token;
        
        while (std::getline(tokenStream, token, ';')) {
            size_t start = token.find_first_not_of(" \t");
            size_t end = token.find_last_not_of(" \t");
            
            if (start != std::string::npos)
                tokens.push_back(token.substr(start, end - start + 1));
            else
                tokens.push_back("");
        }
        
        return tokens;
    }


    ParseResult parse_input_string(const std::string& input) {
        auto tokens = split_tokens(input);
        ParseResult result;
        if(tokens.size() == 2)
        {
            //Simple-calculator mode. Inputs are expression and variable
            result.just_evaluate_expression = true;
            result.function = tokens[0];
            // Parse a and b
            try {
                    result.a = std::stod(tokens[1]);
                } catch (const std::exception&) {
                    throw std::invalid_argument("Simple calculator mode - invalid double for evaluation of function\n");
                }
        }
        else
        {
            if (tokens.size() != 6) {
                throw std::invalid_argument("Invalid number of tokens. Expected 6 or 2, got " + std::to_string(tokens.size()) + "\n");
            }

            // Parse first token (0/1)
            if (tokens[0] == "0") {
                result.use_num_points = true;
            } else if (tokens[0] == "1") {
                result.use_num_points = false;
            } else {
                throw std::invalid_argument("First token must be '0' or '1'\n");
            }
            result.just_evaluate_expression = false;
            // Parse function
            result.function = tokens[1];

            // Parse a and b
            try {
                if (tokens[2] == "inf") {
                    result.a = INFINITY;
                } else if (tokens[2] == "-inf") {
                    result.a = -INFINITY;
                } else {
                    result.a = std::stod(tokens[2]);
                }

                if (tokens[3] == "inf") {
                    result.b = INFINITY;
                } else if (tokens[3] == "-inf") {
                    result.b = -INFINITY;
                } else {
                    result.b = std::stod(tokens[3]);
                }
            } catch (const std::exception&) {
                throw std::invalid_argument("Invalid double value in a/b parameters\n");
            }

            // Parse fifth parameter
            if (!tokens[4].empty() && tokens[4].find('-') != std::string::npos) {
                throw std::invalid_argument("num points and dispersion cannot be negative\n");
            }
            if (result.use_num_points) {
                try {
                    result.num_points = std::stoull(tokens[4]);
                } catch (const std::exception&) {
                    throw std::invalid_argument("Invalid num_points value\n");
                }
            } else {
                try {
                    result.dispersion = std::stod(tokens[4]);
                } catch (const std::exception&) {
                    throw std::invalid_argument("Invalid dispersion value\n");
                }
            }

            // Parse num_workers
            if (!tokens[5].empty() && tokens[5].find('-') != std::string::npos) {
                throw std::invalid_argument("num workers can't be negative\n");
            }
            try {
                result.num_workers = std::stoull(tokens[5]);
            } catch (const std::exception&) {
                throw std::invalid_argument("Invalid num_workers value\n");
            }
        }
        return result;

    }

    double estimate_variance(function<double(double)> fquery, double a, double b, size_t pilot_samples = VARIANCE_ESTIMATE_SAMPLE_SIZE) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(a, b);
        
        double sum = 0.0;
        double sum_sq = 0.0;
        
        for (size_t i = 0; i < pilot_samples; ++i) {
            double x = dist(gen);
            double fx = fquery(x); 
            sum += fx;
            sum_sq += fx * fx;
        }
        
        double mean = sum / pilot_samples;
        return (sum_sq / pilot_samples - mean * mean) * (b - a) * (b - a);
    }

    string Integrator::execute_impl(const std::function<double(double)>& fquery, double a, double b, size_t num_points, size_t num_workers)
    {
        if (num_workers == 0 || num_workers > max_nworkers_) {
            num_workers = max_nworkers_;
        }
        auto task = std::make_shared<MonteCarloTask>(a, b, num_points, num_workers);
        
        std::shared_ptr<std::atomic<size_t>> remaining_workers = std::make_shared<std::atomic<size_t>>(num_workers);
        std::shared_ptr<std::mutex> mtx = std::make_shared<std::mutex>();
        std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
        
        for (size_t i = 0; i < num_workers; ++i) {
            post(tpool_, [this, task, fquery, remaining_workers, mtx, cv]() {
                double partial_sum = calculate_partial_sum(task->a, task->b, task->points_per_worker, fquery);
                {
                    std::lock_guard<std::mutex> lock(task->result_mutex);
                    task->result += partial_sum;
                }
                if (--(*remaining_workers) == 0) {
                    std::lock_guard<std::mutex> lock(*mtx);
                    cv->notify_one();
                }
            });
        }
        
        std::unique_lock<std::mutex> lock(*mtx);
        cv->wait(lock, [remaining_workers]() { return *remaining_workers == 0; });
        
        double res = task->result * (b - a) / num_points;
        return std::to_string(res) + "\n";
    }

    string Integrator::execute(string query)
    {
        using namespace expression_parser;
        //this is the main function to process query logic
        //the protocol is following:
        // One of 2 different sets of arguments should be provided:
        //0   ; f(x)  ; a     ;   b   ;num_points;num_workers
        //bool; string; double; double;  size_t  ;  size_t
        //OR:
        //1   ; f(x)  ; a     ;   b   ;dispersion;num_workers
        //bool; string; double; double;  double  ;  size_t
        ////
        //num workers can be set to 0 to use default
        ParseResult p_res;
        try {
            p_res = parse_input_string(query);
            expression_parser::ExpressionParser ep(p_res.function);
            auto f_q = [&ep](double x) { return ep.evaluate(x); };
            if(p_res.just_evaluate_expression)
            {
                return std::to_string(f_q(p_res.a)) + "\n"; // simple calulator mode
            }
            TransformedIntegral transformed;
            transformed = transform_integral(f_q, p_res.a, p_res.b);
            
            if (!p_res.use_num_points) {
                    double variance = estimate_variance(transformed.transformed_f, transformed.new_a, transformed.new_b);
                    p_res.num_points = static_cast<size_t>(variance / p_res.dispersion);
            }  
            return execute_impl(transformed.transformed_f, transformed.new_a, transformed.new_b, p_res.num_points, p_res.num_workers);
        }
        catch (const std::exception& e) {
            return string(e.what()) + "\n";
        }
    }

    string Integrator::execute(function<double(double)> fquery, double a, double b, size_t num_points, size_t num_workers) {
        TransformedIntegral transformed;
        try {
            transformed = transform_integral(fquery, a, b);
        } catch (const std::exception& e) {
            return string(e.what()) + "\n";
        }
        return execute_impl(transformed.transformed_f, transformed.new_a, transformed.new_b, num_points, num_workers);
    }

    double Integrator::calculate_partial_sum(double a, double b, size_t points, function<double(double)> fquery) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> x_dist(a, b);
        
        double partial_sum = 0;
        for (size_t i = 0; i < points; ++i) {
                double x = x_dist(gen);
                double y = fquery(x); 
                partial_sum += y;
            }
        return partial_sum;
    }

    Integrator::TransformedIntegral::TransformedIntegral(){};

    Integrator::TransformedIntegral Integrator::transform_integral(const std::function<double(double)>& f, double a, double b) {
        TransformedIntegral result;
        if (std::isinf(a) || std::isinf(b)) {
            if (std::isinf(a) && std::isinf(b)) {
                throw std::invalid_argument("Double infinite limits are not supported\n");
            } else if (std::isinf(a) && a < 0) {
                // integrate from -inf to b
                result.new_a = 0.0;
                result.new_b = 1.0;
                result.transformed_f = [f, b](double t) {
                    if (t <= 0.0 || t >= 1.0) return 0.0;
                    double x = b - (1.0 - t)/t;
                    double jacobian = 1.0 / (t * t);
                    return f(x) * jacobian;
                };
            } else if (std::isinf(b) && b > 0) {
                // from a to +inf
                result.new_a = 0.0;
                result.new_b = 1.0;
                result.transformed_f = [f, a](double t) {
                    if (t <= 0.0 || t >= 1.0) return 0.0;
                    double x = a + (1.0 - t)/t;
                    double jacobian = 1.0 / (t * t);
                    return f(x) * jacobian;
                };
            } else {
                throw std::invalid_argument("Unsupported infinite limits\n");
            }
        } else {
            result.new_a = a;
            result.new_b = b;
            result.transformed_f = f;
        }
        return result;
    }

} //namespace monre_carlo_multithread

namespace monte_carlo_server
{
    void Session::read_message() {
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
}//namespace monte_carlo_server