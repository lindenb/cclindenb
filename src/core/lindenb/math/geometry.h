/**
 * Author:
 *      Pierre Lindenbaum PhD plindenbaum@yahoo.fr
 * About:
 *     Utilities for geometry
 * References:
 *
 */
#ifndef _LINDENB_GEOMETRY_H_
#define _LINDENB_GEOMETRY_H_
#include <iostream>
#include <algorithm>
#include <math.h>
#include "lindenb/math/math.h"
namespace lindenb { namespace math {  namespace geom {
	
	class Point
		{
		public:
			double x;
			double y;
			Point():x(0),y(0)
				{
				}
			Point(double x,double y):x(x),y(y)
				{
				}
			Point(const Point& cp):x(cp.x),y(cp.y)
				{
				}
			
			Point& translate(double dx,double dy)
				{
				x+=dx;
				y+=dy;
				return (*this);
				}
			
			double distanceSq(double x1,double y1) const
				{
				return (x-x1)*(x-x1)+(y-y1)*(y-y1);
				}
			
			double distance(double x1,double y1) const
				{
				return hypot(x-x1,y-y1);
				}
			
			double distanceSq(const Point& other) const
				{
				return (x-other.x)*(x-other.x)+(y-other.y)*(y-other.y);
				}
				
			double distance(const Point& other) const
				{
				return hypot(x-other.x,y-other.y);
				}
			
			
			
			std::ostream& print(std::ostream &out) const
				{
				out << "{'x':" << x << ",'y':" << y <<"}";
				return out;
				}
		};
	std::ostream& operator<<(std::ostream &out, const Point& p)
		{
		return p.print(out);
		}
	
	
	class Line
		{
		public:
			enum Intersection { NO_INTERESECTION=0, INTERESECTION=1, PARALLEL=2, COINCIDENT=3, };
			
			Point p1;
			Point p2;
			
			Line(double x1,double y1,double x2,double y2):p1(x1,y1),p2(x2,y2)
				{
				}
			Line(const Point& p1,const Point& p2):p1(p1),p2(p2)
				{
				}
			
			Line(const Line& cp):p1(cp.p1),p2(cp.p2)
				{
				}
			
			Line()
				{
				}
			
			double midX() const
				{
				return (p2.x + p1.x)/2.0;
				}
			
			double midY() const
				{
				return (p2.y + p1.y)/2.0;
				}
			
			double dx() const
				{
				return p2.x - p1.x;
				}
			
			double dy() const
				{
				return p2.y - p1.y;
				}
		
			Line& translate(double dx,double dy)
				{
				p1.translate(dx,dy);
				p2.translate(dx,dy);
				return (*this);
				}
		
			Intersection intersection(const Line& other, Point* result) const
    				{
        			double denom =  other.dy()*dx() - other.dx()*dy();
        			double nume_a = (other.dx()*(p1.y - other.p1.y)) - (other.dy()*(p1.x - other.p1.x));
        			double nume_b = (dx()*(p1.y - other.p1.y)) - dy()*(p1.x - other.p1.x);

				if(denom == 0.0)
					{
					if(nume_a == 0.0 && nume_b == 0.0)
						{
						return COINCIDENT;
						}
					return PARALLEL;
					}

				double ua = nume_a / denom;
			
				if(result!=NULL)
						{
						result->x = p1.x + ua*dx();
						result->y = p1.y + ua*dy();
						}
				return INTERESECTION;
				/* for segments 
				double ub = nume_b / denom;
				if(ua >= 0.0 && ua <= 1.0 && ub >= 0.0 && ub <= 1.0)
					{
					if(result!=NULL)
						{
						result->x = p1.x + ua*dx();
						result->y = p1.y + ua*dy();
						}
					return INTERESECTION;
					}
				
				return NO_INTERESECTION;*/
				}

			
			std::ostream& print(std::ostream &out) const
				{
				out << "{'x1':" << p1.x << ",'y1':" << p1.y<<
					",'x2':"<< p2.x << ",'y2':" << p2.y <<"}";
				return out;
				}
		};
	
	std::ostream& operator<<(std::ostream &out, const Line& L)
		{
		return L.print(out);
		}
		
	class Rectangle
		{
		public:
			double x;
			double y;
			double width;
			double height;
			Rectangle():x(0),y(0),width(0),height(0)
				{
				}
			Rectangle(const Rectangle& cp):x(cp.x),y(cp.y),width(cp.width),height(cp.height)
				{
				}
			
			Rectangle(double x, double y,double width, double height):x(x),y(y),width(width),height(height)
				{
				}
			
			Rectangle(const Point& p1,const Point& p2):
						x(std::min(p1.x,p2.x)),
						y(std::min(p1.y,p2.y)),
						width(std::fabs(p1.x-p2.x)),
						height(std::fabs(p1.y-p2.y)),
				{
				}
			
			~Rectangle()
				{
				}
			
			std::ostream& print(std::ostream &out) const
				{
				out << "{'x':" << x << ",'y':" << y<<
					",'width':"<< width << ",'height':" << height <<"}";
				return out;
				}
		};
	
	std::ostream& operator<<(std::ostream &out, const Rectangle& R)
		{
		return L.print(out);
		}
	
	
	class Circle
		{
		public:
			Point center;
			double r;
			Circle():center(0,0),r(0)
				{
				}
			
			Circle(double cx,double cy,double r):center(cx,cy),r(r)
				{	
				}
			
			Circle(const Point& c,double r):center(c),r(r)
				{	
				}
				
			Circle(const Circle& cp):center(cp.center),r(cp.r)
				{
				}
			
			double diameter() const
				{
				return r*2.0;
				}
			
			/* see http://local.wasp.uwa.edu.au/~pbourke/geometry/2circle/ */
			bool intersection(const Circle& other,Point* p1,Point* p2) const
				{
				/* dx and dy are the vertical and horizontal distances between
				* the circle centers.
				*/
				double dx = other.center.x - center.x;
				double dy = other.center.y - center.y;
				
				/* Determine the straight-line distance between the centers. */
				double d = lindenb::math::hypoth(dx,dy); 
				
				/* Check for solvability. */
				if (d > (r + other.r))
					{
					/* no solution. circles do not intersect. */
					return false;
					}
					
				if (d < std::fabs(r - other.r))
					{
					/* no solution. one circle is contained in the other */
					return false;
					}
				if(r==other.r && dx==0 && dy==0)
					{
					return false;
					}
				
				/* 'point 2' is the point where the line through the circle
				* intersection points crosses the line between the circle
				* centers.  
				*/
				
				/* Determine the distance from point this.center to point 2. 
				a^2+h^2=this.r^2
				b^2+h^2=other.r^2
				
				*/
				double a = ((r*r) - (other.r*other.r) + (d*d)) / (2.0 * d) ;
				
				/* Determine the coordinates of point 2. */
				double x2 = center.x + (dx * a/d);
				double y2 = center.y + (dy * a/d);
				
				/* Determine the distance from point 2 to either of the
				* intersection points.
				*/
				double h = std::sqrt(r*r-a*a);
				
				/* Now determine the offsets of the intersection points from
				* point 2.
				*/
				double rx = -dy * (h/d);
				double ry = dx * (h/d);
				/* Determine the absolute intersection points. */
				if(p1!=NULL)
					{
					p1->x = x2 + rx;
					p1->y = y2 + ry;
					}
				if(p2!=NULL)
					{
					p2->x = x2 - rx;
					p2->y = y2 - ry;
					}
				return true;
				}
			
			 bool intersects(const Circle& cp) const
				{
				return intersection(cp,NULL,NULL);
				}
			
			std::ostream& print(std::ostream &out) const
				{
				out << "{'cx':" << center.x << ",'cy':" << center.y<< ",'r':"<< r <<"}";
				return out;
				}		
		};
	
	std::ostream& operator<<(std::ostream &out, const Circle& L)
		{
		return L.print(out);
		}
	
}}}//namespaces
#endif
