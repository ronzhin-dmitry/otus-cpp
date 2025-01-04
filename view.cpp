#include "view.h"
#include <iostream>

void DumbView::onClick(CoordinatePoint p)
{
	std::cout << "Dumb View -- use clicked coordinate (" << p.getX() << "," << p.getY() << ")" << std::endl;
	//Some logic here - i.e. call for onSave, onNew or onOpen in case if coordinate corresponds to specified buttons
	//for example, let's call drawing a circle:
	curController->onDraw((Figure*)(new Circle({1.,1.},1.)));
}

bool DumbView::drawFigure(const Figure * fig)
{
	std::cout << "Dumb View -- drawed new figure on the view: " << fig->getFigureName() << ", figure ID:" << fig->getFigureID() << std::endl;
	figuresDrawn.push_back(fig);
	//Some additional logic here, i.e. passing some response to controller after monitoring for user actions
	return true;
}

bool DumbView::eraseFigure(const Figure * fig)
{
	std::cout << "Dumb View -- erased figure from the view: " << fig->getFigureName() << ", figure ID:" << fig->getFigureID() << std::endl;
	figuresDrawn.erase(std::find(figuresDrawn.begin(), figuresDrawn.end(), fig));
	//Some additional logic here, i.e. passing some response to controller after monitoring for user actions
	return true;
}