#pragma once
#include "model.h"
#include "lib.h"
#include <memory>
#include <iostream>
class IModel;
/**
 * @brief interface controller class to derive from
 * contains description of basic methods
 * each controller is expected to have unique model to work with
 */
class IController
{
protected:
std::unique_ptr<IModel> curModel;
IController(){};
public:
void addModel(IModel* model)
{
    curModel = std::unique_ptr<IModel>(model);
};

/**
 * @brief virtual destructor
 */

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

/**
 * @brief pure virtual method for drawing some figure on a screen 
 * (may be initiated differently for different figures, may require different amount of clicks)
 */
virtual void onDraw(const Figure*) = 0;
};

/**
 * @brief simple implementation example for controller
 * used only for demonstration
 */
class DumbController:IController
{
private:

public:
DumbController(){};
~DumbController(){};
void runApp() override;
void onSave() override;
void onOpen() override;
void onNew() override;
void onDraw(const Figure*) override;
};