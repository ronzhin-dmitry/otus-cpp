#include "model.h"
#include <iostream>


bool DumbModel::openFile(std::string filename)
{
	curFile = filename;
	std::cout << "Dumb Model -- open file " << filename << std::endl;
	return true;
}

bool DumbModel::saveToFile(std::string filename)
{
	std::cout << "Dumb Model -- save to file " << filename << std::endl;
	return true;
}

bool DumbModel::addFigure(const Figure* fig)
{
	std::cout << "Dumb Model -- added new figure to model. Figure: " << fig->getFigureName() << ", figure ID: " << fig->getFigureID() << std::endl;
	figuresAdded.push_back(fig);
	curView->drawFigure(fig);
	//some aux logic here
	return true;
}

bool DumbModel::removeFigure(const Figure* fig)
{
	std::cout << "Dumb Model -- removed figure from model. Figure: " << fig->getFigureName() << ", figure ID: " << fig->getFigureID() << std::endl;
	figuresAdded.erase(std::find(figuresAdded.begin(), figuresAdded.end(), fig));
	curView->eraseFigure(fig);
	//some aux logic here
	return true;
}