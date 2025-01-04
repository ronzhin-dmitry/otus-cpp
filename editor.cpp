#include "lib.h"
#include "model.h"
#include "view.h"
#include "controller.h"

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
    //MVC self-made arch example
    auto Model = (IModel*)(new DumbModel);
    auto View = (IView*)(new DumbView);
    auto Controller = (IController*)(new DumbController(View, Model));
    Controller->runApp();
    return 0;
}
