#include "lib.h"
#include "model.h"
#include "view.h"
#include "controller.h"


/**
 * @brief simple init function to pass pointers from each other
 */
void appInit(IModel* model, IView* view, IController* controller)
{
    model->addView(view);
    view->addContoller(controller);
    controller->addModel(model);
}


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
    auto model = (IModel*)(new DumbModel);
    auto view = (IView*)(new DumbView);
    auto controller = (IController*)(new DumbController);
    appInit(model, view, controller);
    controller->runApp();
    return 0;
}
