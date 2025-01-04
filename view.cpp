#include "view.h"
#include <iostream>

void DumbView::onClick(CoordinatePoint p)
{
	std::cout << "Dumb View -- use clicked coordinate (" << p.getX() << "," << p.getY() << ")" << std::endl;
	//Some logic here - i.e. call for onSave, onNew or onOpen in case if coordinate corresponds to specified buttons
}