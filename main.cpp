/**
@file main.cpp
*/
#include "lib.h"

/**
 * @brief Entry point of the Monte-Carlo Integration project
 *
 * Execution of the program starts here. 
 * Main file shows how to start the MCI server on specified port (port specification via argv).
 * Under commented sections one can fing examples of execution of syntax-parser module and intgrator module.
 *
 * @param argc Number of arguments
 * @param argv List of arguments (expecting port number as a single CLI argument)
 *
 * @return Program exit status
 */
int main(int argc, char* argv[]) {
    using namespace std;
    using namespace monte_carlo_server;
    using namespace monte_carlo_multithread;
    using namespace expression_parser;

    /* 
    //Examples of usage for simple primitives:
    Integrator integrator;
    // Integral of sin(x) from 0 to π (expected: 2.0)
    auto result = integrator.execute(
        [](double x) { return std::sin(x); },
        0.0, 
        M_PI,
        10'000'000
    );

    std::cout << "Result 1: " << result << " expected around " << 2.0 << std::endl;

    // Integral of x² from 0 to 1 (expected: ~0.333)
    auto result2 = integrator.execute(
        [](double x) { return x*x; },
        0.0,
        1.0,
        10'000'000
    );

    std::cout << "Result 2: " << result2 << " expected around " << 0.333 << std::endl;

    auto ep = ExpressionParser("sin(x)^2 + log(pi, x)");
    std::cout << "sin(x)^2 + log(pi, x) at x = Pi is " << ep.evaluate(M_PI) << std::endl;

    auto ep2 = ExpressionParser("3*x + x^2 - ln(x) + 15.3");
    std::cout << "3*x + x^2 - ln(x) + 15.3 at x = 1 is "  << ep2.evaluate(1) << std::endl;
    std::cout << "3*x + x^2 - ln(x) + 15.3 at x = 2 is "  << ep2.evaluate(2) << std::endl;
    */
    try {
        if (argc != 2) {
            cerr << "Need two arguments" << endl;
            return 1;
        }

        int port_tmp = stoi(argv[1]);
        if (port_tmp < SHRT_MIN || port_tmp > SHRT_MAX) {
            throw out_of_range("Not short value - please specify port number to listen");
        }
        short port = static_cast<short>(port_tmp);

        //Creating server
        boost::asio::io_context io_context;

        //(Ctrl+C)
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait(
            [&io_context](const boost::system::error_code&, int) {
                cout << "\nShutting down server..." << endl;
                io_context.stop(); 
            });
        Server serv(io_context, port);
        io_context.run();

    } catch (const invalid_argument& e) {
        cerr << "has to be int" << endl;
        return 1;
    } catch (const out_of_range& e) {
        cerr << "error: " << e.what() << endl;
        return 1;
    }
}