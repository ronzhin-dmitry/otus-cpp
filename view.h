#pragma once
#include <functional>
#include "lib.h"
#include "controller.h"
#include "vector"
#include <memory>
class IController;

/**
 * @brief interface view class to derive from
 * contains description of basic methods
 */
class IView
{
protected:
std::unique_ptr<IController> curController;
std::vector<const Figure*> figuresDrawn;
public:
void addContoller(IController* controller)
{
    curController = std::unique_ptr<IController>(controller);
};
virtual ~IView(){};

/**
 * @brief basic method for response on user clicking on a screen
 * should be properly defined by any custom view class
 */
virtual void onClick(CoordinatePoint p) = 0;
/**
 * @brief basic method for adding figure on the view
 */
virtual bool drawFigure(const Figure * fig) = 0;

/**
 * @brief basic method for deleting figure from the view
 */
virtual bool eraseFigure(const Figure * fig) = 0;
};


/**
 * @brief simple implementation example for view
 * used only for demonstration
 */
class DumbView:IView
{
protected:

public:
/**
 * @brief on-Click response method
 */
void onClick(CoordinatePoint p) override;

/**
 * @brief dumb adding of a figure on view
 */
virtual bool drawFigure(const Figure * fig) override;

/**
 * @brief dumb figure removal
 */
virtual bool eraseFigure(const Figure * fig) override;
~DumbView(){};
};