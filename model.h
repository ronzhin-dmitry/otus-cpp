#pragma once
#include <string>
#include "lib.h"
#include "view.h"
#include <memory>
class IView;
/**
 * @brief interface model class to derive from
 * contains description of basic methods
 */
class IModel
{
protected:
std::string curFile;
std::unique_ptr<IView> curView;
std::vector<const Figure*> figuresAdded;
IModel(){};
public:
void addView(IView* view)
{
    curView = std::unique_ptr<IView>(view);
};

/**
 * @brief each model should precisely define procedure for import from file
 */
virtual bool openFile(std::string filename) = 0;

/**
 * @brief each model should precisely define procedure for export to file
 */
virtual bool saveToFile(std::string filename) = 0;

/**
 * @brief each model should precisely define procedure to add new figure to model
 */
virtual bool addFigure(const Figure*) = 0;

/**
 * @brief each model should precisely define procedure to remove figure from model
 */
virtual bool removeFigure(const Figure*) = 0;

virtual ~IModel(){};
};

/**
 * @brief simple implementation example for model
 * used only for demonstration
 */
class DumbModel:IModel
{
private:

public:
/**
 * @brief dumb file opener - demonstration
 */
bool openFile(std::string filename);
/**
 * @brief dumb file saver - demonstration
 */
bool saveToFile(std::string filename);
/**
 * @brief dumb figure adding to model - demonstration
 */
virtual bool addFigure(const Figure*) override;

/**
 * @brief dumb figure removing from model - demonstration
 */
virtual bool removeFigure(const Figure*) override;
DumbModel(){};
~DumbModel(){};
};