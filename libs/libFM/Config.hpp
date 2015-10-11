#ifndef __Config_hpp__
#define __Config_hpp__

#ifdef NDEBUG
#define DBOUT(x)
#else
#include <iostream>
#define DBOUT(x)  x
#endif

#endif
