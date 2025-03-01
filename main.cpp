/**
@file main.cpp
*/
#include "async.h"

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
int main(int argc, char *argv[])
{
    // Just some command line arguments parsing
    int N = 3; // default
    if (argc > 1)
    {
        std::string arg = argv[1];
        try
        {
            std::size_t pos;
            N = std::stoi(arg, &pos);
            if (pos < arg.size())
            {
                std::cerr << "Trailing characters after number: " << arg << '\n';
            }
            else if (N < 1)
            {
                std::cerr << "Negative input N " << arg << " setting to default value " << 3 << '\n';
                N = 3;
            }
        }
        catch (std::invalid_argument const &ex)
        {
            std::cerr << "Invalid number: " << arg << '\n';
        }
        catch (std::out_of_range const &ex)
        {
            std::cerr << "Number out of range: " << arg << '\n';
        }
    }

    //Command parser - entry points. Loggers should be created and subscribed to parser
    ILoggerPtr cLogger(new ConsoleLogger());
    ILoggerPtr fLogger(new FileLogger());
    Application app(N);
    app.subscribeLogger(cLogger);
    app.subscribeLogger(fLogger);
    app.runApp();
    return 0;
}
/*
int main(int argc, char *argv[])
{
    std::size_t bulk = 5;
    auto h = async::connect(bulk);
    auto h2 = async::connect(bulk);
    async::receive(h, "1", 1);
    async::receive(h2, "1\n", 2);
    async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
    async::receive(h, "b\nc\nd\n}\n89\n", 11);
    async::disconnect(h);
    async::disconnect(h2);

    return 0;
}*/
