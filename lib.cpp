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
    string Integrator::execute(string query)
    {
        string res = "";
        //TODO
        return query;
    }
    string Integrator::execute(function<double(double)> fquery, double a, double b, size_t num_points, size_t num_workers) {
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
        return to_string(res);
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