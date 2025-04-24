/**
@file test_version.cpp
*/
#define BOOST_TEST_MODULE test_version

#include "lib.h"
#include <sstream>

#include <boost/test/unit_test.hpp>
#include <cmath>

#include <boost/asio.hpp>
#include <thread>
#include <future>

#include <chrono>
#include <array>
using namespace std::chrono_literals;
using namespace boost::asio;
using namespace boost::asio::ip;


BOOST_AUTO_TEST_SUITE(test_version)

BOOST_AUTO_TEST_CASE(test_valid_version)
{
	BOOST_CHECK(version() > 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_expression_parser)

BOOST_AUTO_TEST_CASE(test_simple_expression) {
    expression_parser::ExpressionParser ep("3 + 4 * 2");
    BOOST_CHECK_CLOSE(ep.evaluate(0), 11.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(test_variable_and_constants) {
    expression_parser::ExpressionParser ep("x + pi - e");
    double x_val = 1.0;
    double expected = x_val + M_PI - M_E;
    BOOST_CHECK_CLOSE(ep.evaluate(x_val), expected, 1e-6);
}

BOOST_AUTO_TEST_CASE(test_function_evaluation) {
    expression_parser::ExpressionParser ep("sin(pi/2) + log(e, e^3)");
    BOOST_CHECK_CLOSE(ep.evaluate(0), 1.0 + 3.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(test_complex_expression) {
    expression_parser::ExpressionParser ep("2*(3 + x^2) - ln(e^5)");
    double x_val = 2.0;
    double expected = 2*(3 + pow(x_val, 2)) - 5;
    BOOST_CHECK_CLOSE(ep.evaluate(x_val), expected, 1e-6);
}

BOOST_AUTO_TEST_CASE(test_invalid_expression) {
    // Некорректная функция: исключение при создании парсера
    BOOST_CHECK_THROW(expression_parser::ExpressionParser ep("invalid_func(x)"), std::invalid_argument);
    
    // Синтаксическая ошибка: исключение при вычислении
    expression_parser::ExpressionParser ep("3 + * x");
    BOOST_CHECK_THROW(ep.evaluate(0), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_integrator)

BOOST_AUTO_TEST_CASE(test_sin_integration) {
    monte_carlo_multithread::Integrator integrator;
    auto result_str = integrator.execute([](double x) { return std::sin(x); }, 0.0, M_PI, 1'000'000, 4);
    double result = std::stod(result_str);
    BOOST_CHECK_CLOSE(result, 2.0, 1.0); // 1% tolerance
}

BOOST_AUTO_TEST_CASE(test_square_integration) {
    monte_carlo_multithread::Integrator integrator;
    auto result_str = integrator.execute([](double x) { return x*x; }, 0.0, 1.0, 1'000'000, 4);
    double result = std::stod(result_str);
    BOOST_CHECK_CLOSE(result, 1.0/3.0, 2.0); // 2% tolerance
}

BOOST_AUTO_TEST_CASE(test_variance_based_calculation) {
    // Увеличиваем количество точек и допуск
    std::string query = "1;x^2;0;1;0.00001;4"; // Более строгая дисперсия
    monte_carlo_multithread::Integrator integrator;
    std::string result_str = integrator.execute(query);
    double result = std::stod(result_str);
    BOOST_CHECK_CLOSE(result, 1.0/3.0, 10.0); // 5% допуск
}

BOOST_AUTO_TEST_CASE(test_improper_integral_simple) {
    monte_carlo_multithread::Integrator integrator;
    // ∫₀^∞ e^(-x) dx = 1
    auto result_str = integrator.execute("0;exp(-x);0;inf;1000000;4");
    double result = std::stod(result_str);
    BOOST_CHECK_CLOSE(result, 1.0, 5.0); // 5% допуск
}

BOOST_AUTO_TEST_CASE(test_improper_integral_negative_infinite) {
    monte_carlo_multithread::Integrator integrator;
    // ∫_-∞^0 e^x dx = 1
    auto result_str = integrator.execute("0;exp(x);-inf;0;1000000;4");
    double result = std::stod(result_str);
    BOOST_CHECK_CLOSE(result, 1.0, 5.0);
}

BOOST_AUTO_TEST_CASE(test_improper_integral_power_function) {
    monte_carlo_multithread::Integrator integrator;
    // ∫₁^∞ 1/x² dx = 1
    auto result_str = integrator.execute("0;1/(x^2);1;inf;500000;4");
    double result = std::stod(result_str);
    BOOST_CHECK_CLOSE(result, 1.0, 8.0);
}

BOOST_AUTO_TEST_CASE(test_double_infinite_error) {
    monte_carlo_multithread::Integrator integrator;
    // Попытка интегрирования с двумя бесконечностями
    auto result_str = integrator.execute("0;exp(-x^2);-inf;inf;1000000;4");
    BOOST_TEST(result_str.find("Double infinite limits") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_server)

std::string execute_server_test(const std::string& query, short port) {
    io_service io_context;
    tcp::socket socket(io_context);
    
    std::promise<std::string> connected;
    auto future = connected.get_future();
    
    std::thread client_thread([&]{
        socket.connect(tcp::endpoint(address::from_string("127.0.0.1"), port));        
        write(socket, buffer(query + "\n"));
        
        streambuf response_buf;
        read_until(socket, response_buf, '\n');
        std::istream response_stream(&response_buf);
        std::string response;
        std::getline(response_stream, response);
        connected.set_value(response); // Передать результат
    });

    future.wait();
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for server
    
    // Cleanup
    client_thread.join();
    return future.get(); // Вернуть результат
}

BOOST_AUTO_TEST_CASE(test_server_connection_and_response) {
    const short test_port = 6666;
    
    io_service io_context;
    monte_carlo_server::Server server(io_context, test_port);
    
    std::thread server_thread([&io_context]{
        io_context.run();
    });
    
    std::string response = execute_server_test("0;x^2;0;1;1000000;4", test_port);

    double result = std::stod(response);
    BOOST_CHECK_CLOSE(result, 0.333, 5.0);
    
    io_context.stop();
    server_thread.join();
}

BOOST_AUTO_TEST_SUITE_END()