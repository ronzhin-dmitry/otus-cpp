/**
@file lib.cpp
*/
#include "lib.h"
#include "version.h"

/**
 * @brief function to produce release version in github workflows
 * @return current release version (incremented on git push)
 */
int version()
{
	return PROJECT_VERSION_PATCH;
}