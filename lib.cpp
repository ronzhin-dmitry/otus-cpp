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

namespace monte_carlo_multithread
{
    using namespace std;
    struct ParseResult {
        bool use_num_points;
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
        if (tokens.size() != 6) {
            throw std::invalid_argument("Invalid number of tokens. Expected 6, got " + std::to_string(tokens.size()) + "\n");
        }

        ParseResult result;

        // Parse first token (0/1)
        if (tokens[0] == "0") {
            result.use_num_points = true;
        } else if (tokens[0] == "1") {
            result.use_num_points = false;
        } else {
            throw std::invalid_argument("First token must be '0' or '1'\n");
        }

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
        } catch (const std::exception& e) {
            return string(e.what()) + "\n";
        }
        
        expression_parser::ExpressionParser ep(p_res.function);
        auto f_q = [&ep](double x) { return ep.evaluate(x); };
        
        TransformedIntegral transformed;
        try {
            transformed = transform_integral(f_q, p_res.a, p_res.b);
        } catch (const std::exception& e) {
            return string(e.what()) + "\n";
        }
        
        if (!p_res.use_num_points) {
            try {
                double variance = estimate_variance(transformed.transformed_f, transformed.new_a, transformed.new_b);
                p_res.num_points = static_cast<size_t>(variance / p_res.dispersion);
            } catch (const std::exception& e) {
                return string(e.what()) + "\n";
            }
        }
        
        try {
            return execute_impl(transformed.transformed_f, transformed.new_a, transformed.new_b, p_res.num_points, p_res.num_workers);
        } catch (const std::exception& e) {
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
}