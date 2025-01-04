#include "controller.h"
#include <functional>

IController::IController(IView* view, IModel* model): curView(view), curModel(model)
{
	view->init([this](){this->onSave();}, [this](){this->onOpen();}, [this](){this->onNew();});
}

void IController::runApp()
{
	std::cout << "Base controller class - some logic here" << std::endl;
	//For example - some logic to call Model->View->Controller user-reponse cycle
}

void DumbController::runApp()
{
    std::cout << "Dumb editor started" << std::endl;
    IController::runApp();
}

void DumbController::onSave()
{
    std::cout << "Dumb controller - specify file for export:" << std::endl;
	std::string output;
    std::cin >> output;
    curModel->saveToFile(output);
	//Some auxilary logic here (i.e. logging of last save time)
}

void DumbController::onOpen()
{
    std::cout << "Dumb controller - specify file for import:" << std::endl;
	std::string input;
    std::cin >> input;
    curModel->saveToFile(input);
	//Some auxilary logic here (i.e. logging of last open time)
}

void DumbController::onNew()
{
    std::cout << "Dumb controller - new document created" << std::endl;
	//Some auxilary logic here (i.e. proposing to save last edited doc)
}