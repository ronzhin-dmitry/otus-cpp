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
int main()
{
    //Command parser - entry points. Main only calls for app init.
    Application app(nullptr); //TODO - put valid state
    app.runApp();
    return 0;
}
