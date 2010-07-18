/**
 * Author:
 *	Pierre Lindenbaum PhD plindenbaum@yahoo.fr
 * About:
 *	Interfaces for Math
 * References:
 *	
 */
#ifndef _LINDENB_MATH_H_
#define _LINDENB_MATH_H_
#include <cmath>
namespace lindenb { namespace math {
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

static inline double deg2rad(double deg)
	{
	return (deg)*(M_PI/180.0);
	}

static inline double rad2deg(double rad)
	{
	return (rad)*(180.0/M_PI);
	}
/** Euclidean distance function sqrt(x*x+y*y) */
static inline double hypoth(double x,double y)
	{
	return std::sqrt(x*x+y*y);
	}

template<typename T>
inline int sign(T val)
	{
	return (val>=0?1:-1);
	}

}}//namespaces
#endif