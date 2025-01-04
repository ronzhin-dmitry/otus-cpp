#pragma once
#include <functional>
/**
 * @brief simple struct for 2D screen coordinates
 * possibly more complex sub-structures can be derived
 */
struct CoordinatePoint
{
protected:
    CoordinatePoint(){};
    float x;
    float y;
public:
    float getX(){return x;};
    float getY(){return y;};
    CoordinatePoint(float _x, float _y): x(_x), y(_y){};
};

/**
 * @brief interface view class to derive from
 * contains description of basic methods
 */
class IView
{
protected:
std::function<void()> saveFunc {};
std::function<void()> openFunc {};
std::function<void()> newFunc {};

virtual void clickSave(){saveFunc();};
virtual void clickOpen(){openFunc();};
virtual void clickNew(){newFunc();}
public:
/**
 * @brief by default view has a specified number on functions, which are empty
 * to actually define proper behaviour of a view, controller inits them
 */
void init(std::function<void()> _saveFunc, std::function<void()> _openFunc, std::function<void()> _newFunc)
{
    saveFunc = _saveFunc;
    openFunc = _openFunc;
    newFunc = _newFunc;
}
virtual ~IView(){};

/**
 * @brief basic method for response on user clicking on a screen
 * should be properly defined by any custom view class
 */
virtual void onClick(CoordinatePoint p) = 0;
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
~DumbView(){};
};