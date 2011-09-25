/*
 * throw.h
 *
 *  Created on: Sep 24, 2011
 *      Author: lindenb
 */

#ifndef LINDENB_LANG_THROW_H_
#define LINDENB_LANG_THROW_H_

#include <sstream>
#include <string>
#include <stdexcept>

#ifdef __GNUC__

#include <execinfo.h> /* compile with -rdynamic */
#define THROW_BACKTRACE_SIZ 30
#define THROW(msg)  do {\
    std::ostringstream __os;\
    __os << __FILE__<< ":" << __FUNCTION__ << ":" \
	<< __LINE__ << ":" << msg << std::endl;\
    void    *array[THROW_BACKTRACE_SIZ];\
    std::size_t  size = ::backtrace(array, THROW_BACKTRACE_SIZ);\
    char    **_trace_strings = ::backtrace_symbols(array, size);\
    if(_trace_strings!=NULL)\
	{\
	for (std::size_t i = 0; i < size; ++i)\
	       {\
	      __os << "\t" << _trace_strings[i] << std::endl;\
	       }\
	std::free(_trace_strings);\
	}\
    throw std::runtime_error(__os.str());\
    } while(0)

#else

#define THROW(msg)  do {\
    std::ostringstream __os;\
    __os << __FILE__<< ":" << __FUNCTION__ << ":" \
	<< __LINE__ << ":" << msg << std::endl;\
    throw std::runtime_error(__os.str());\
    } while(0)
#endif

#endif /* THROW_H_ */
