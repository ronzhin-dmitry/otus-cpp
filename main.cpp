/**
@file main.cpp
*/
#include "lib.h"
#define N_DEFAULT 3

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
    // Command parser - entry points. Main only calls for app init.
    int N = N_DEFAULT; // default
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
                std::cerr << "Negative input N " << arg << " setting to default value " << N_DEFAULT << '\n';
                N = N_DEFAULT;
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

    ILoggerPtr cLogger(new ConsoleLogger());
    ILoggerPtr fLogger(new FileLogger());
    Application app(N); // TODO - put valid state
    app.subscribeLogger(cLogger);
    app.subscribeLogger(fLogger);
    app.runApp();
    return 0;
}
