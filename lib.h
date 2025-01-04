#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <tuple>
#include <algorithm>
#include <type_traits>

/**
 * @brief simple function to return version of the release
 */
int version();

/**
 * @brief simple struct for 2D screen coordinates
 * possibly more complex sub-structures can be derived
 */
struct CoordinatePoint
{
protected:
    float x;
    float y;
public:
    float getX() const{return x;};
    float getY() const{return y;};
    CoordinatePoint(){};
    CoordinatePoint(float _x, float _y): x(_x), y(_y){};
};

/**
 * @brief basic abstract class for figure that will be shown in view
 * all the derived classes should contain methods with enough information for view to position on screen
 */
class Figure
{
protected:
std::string figureName;
Figure(){};
public:
/**
 * @brief method to get each specific figure name for every derived class
 */
virtual std::string getFigureName() const{return figureName;};

/**
 * @brief each derived class should have a unique way to identify figure
 */
virtual std::string getFigureID() const = 0;
};

class Rectangle:Figure
{
protected:
CoordinatePoint leftTop;
CoordinatePoint rightBottom;
Rectangle(){};
public:
Rectangle(CoordinatePoint leftTop_, CoordinatePoint rightBottom_):leftTop(leftTop_), rightBottom(rightBottom_)
{
    figureName = "Rectangle";
};
virtual std::string getFigureID() const override
{
    return std::to_string(leftTop.getX()) + ' ' + std::to_string(leftTop.getY()) +' ' + std::to_string(rightBottom.getX()) +' ' + std::to_string(rightBottom.getY());
};
};

class Circle:Figure
{
protected:
CoordinatePoint center;
float radius;
Circle(){};
public:
Circle(CoordinatePoint center_, float radius_):center(center_), radius(radius_)
{
    figureName = "Cirlce";
};
virtual std::string getFigureID() const override
{
    return std::to_string(center.getX()) + ' ' + std::to_string(center.getY()) +' ' + std::to_string(radius);
};
};