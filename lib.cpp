#include "lib.h"
#include "version.h"

int version()
{
	return PROJECT_VERSION_PATCH;
}

unsigned int iter_factorial(unsigned int n)
{
    unsigned int ret = 1;
    for(unsigned int i = 1; i <= n; ++i)
        ret *= i;
    return ret;
}