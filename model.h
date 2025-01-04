#pragma once
#include <string>
/**
 * @brief interface model class to derive from
 * contains description of basic methods
 */
class IModel
{
protected:
std::string curFile;
public:
/**
 * @brief each model should precisely define procedure for import from file
 */
virtual bool openFile(std::string filename) = 0;

/**
 * @brief each model should precisely define procedure for export to file
 */
virtual bool saveToFile(std::string filename) = 0;

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
bool openFile(std::string filename);
bool saveToFile(std::string filename);
~DumbModel(){};
};