#pragma once
#include "model.h"
#include "view.h"
#include <memory>
#include <iostream>
/**
 * @brief interface controller class to derive from
 * contains description of basic methods
 * each controller is expected to have unique view and model to work with
 */
class IController
{
protected:
IController(){};
std::unique_ptr<IView> curView;
std::unique_ptr<IModel> curModel;
public:
/**
 * @brief virtual method to init controller with view and model
 * passes actions to the view (so that it knows whom to call)
 */
IController(IView* view, IModel* model);

virtual ~IController(){};
/**
 * @brief virtual method for start point of application
 */
virtual void runApp();

/**
 * @brief pure virtual method for action on clicking at "Save" button
 */
virtual void onSave() = 0;

/**
 * @brief pure virtual method for action on clicking at "Open" button
 */
virtual void onOpen() = 0;

/**
 * @brief pure virtual method for action on clicking at "New" button
 */
virtual void onNew() = 0;
};

/**
 * @brief simple implementation example for controller
 * used only for demonstration
 */
class DumbController:IController
{
private:
DumbController();
public:
DumbController(IView* view, IModel* model): IController(view, model){};
~DumbController(){};
void runApp() override;
void onSave() override;
void onOpen() override;
void onNew() override;
};