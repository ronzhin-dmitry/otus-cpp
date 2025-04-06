/**
@file main.cpp
*/
#include "lib.h"

/**
 * @brief Entry point
 *
 * Execution of the program
 * starts here.
 *
 * @param argc Number of arguments
 * @param argv List of arguments
 *
 * @return Program exit status
 */
int main(int argc, char* argv[]) {
    using namespace std;
    using namespace monte_carlo_server;
    using namespace monte_carlo_multithread;

    Integrator integrator;
    // Интеграл sin(x) от 0 до π (ожидаемый результат: 2.0)
    auto result = integrator.execute(
        [](double x) { return std::sin(x); },
        0.0, 
        M_PI,
        10'000'000
    );

    std::cout << "Result 1: " << result << " expected around " << 2.0 << std::endl;

    // Интеграл x² от 0 до 1 (ожидаемый результат: ~0.333)
    auto result2 = integrator.execute(
        [](double x) { return x*x; },
        0.0,
        1.0,
        10'000'000
    );

    std::cout << "Result 2: " << result2 << " expected around " << 0.333 << std::endl;

    exit(0);
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
                io_context.stop(); // Остановка цикла обработки событий
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