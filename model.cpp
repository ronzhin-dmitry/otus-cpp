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