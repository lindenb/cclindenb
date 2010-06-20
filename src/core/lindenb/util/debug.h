/**
 * useful macros for debugging
 *
 */
#ifndef LINDENB_UTIL_DEBUG_H
#define LINDENB_UTIL_DEBUG_H

#define WHERE "\""<< __FILE__<<":"<<__FUNCTION__<<"\"[" << __LINE__ << "]"

#ifndef NDEBUG

#include <iostream>
#define DEBUG(a) std::cerr << WHERE << ":" << a << std::endl

#else

#define DEBUG(a) do {} while(false)

#endif



#endif
