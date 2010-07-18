/**
 * Author:
 *	Pierre Lindenbaum PhD plindenbaum@yahoo.fr
 * About:
 *	Interfaces for the GD library
 * References:
 *	http://www.libgd.org/Main_Page
 */
#ifndef _LINDENB_GD_H_
#define _LINDENB_GD_H_
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <gd.h>
#include <iostream>
namespace lindenb { namespace gd {

class ImageFactory;
class Image;

typedef int Color;

std::ostream& operator<< (std::ostream& out,const gdPoint& o)
	{
	out << "{'x':" << o.x << ",'y':" << o.y << "}";
	return out;
	}

class Image
	{
	protected:
		gdImagePtr img;
		
		
		Image(gdImagePtr image):img(image)
			{
			if(image==NULL)
				{
                                throw std::runtime_error("Cannot create image");
				}
			}
	public:
		Image(int sx, int sy):img(NULL)
			{
			img=::gdImageCreate(sx, sy);
			if(img==NULL)
				{
				std::ostringstream os;
                                os << "failed to create an image (" << sx <<" x "<< sy<< ")";
                                throw std::runtime_error(os.str());
				}
			}
		virtual int width() const
			{
			return gdImageSX(img);
			}
		
		virtual int height() const
			{
			return gdImageSY(img);
			}
		
		virtual Color color(int r, int g, int b)
			{
			return ::gdImageColorAllocate(img,r,g,b);
			}
		
		virtual Color color(int r, int g, int b,int a)
			{
			return ::gdImageColorAllocateAlpha(img,r,g,b,a);
			}
		//drawing functions
		virtual void setPixel(int x, int y, Color color) 
			{
			::gdImageSetPixel(img,x,y,color);
			}
		
		virtual Color getPixel(int x, int y)
			{
			return ::gdImageGetPixel(img, x,y);
			}
		
		virtual void drawLine(int x1, int y1, int x2, int y2, Color color)
			{
			::gdImageLine(img,x1,y1,x2,y2,color);
			}
			
		virtual void drawRect(int x, int y, int width, int height, Color color)
			{
			::gdImageRectangle(img,x,y,x+width,y+height,color);
			}
		
		virtual void fillRect(int x, int y, int width, int height, Color color)
			{
			::gdImageFilledRectangle(img,x,y,x+width,y+height,color);
			}
		
		virtual void drawPolygon(std::vector<gdPoint>& points, Color color)
			{
			drawPolygon(&(*points.begin()), points.size(), color);
			}
			
		virtual void drawPolygon( gdPoint* points,int nPoints, Color color)
			{
			::gdImagePolygon(img, points, nPoints, color);
			}
			
		virtual void fillPolygon(std::vector<gdPoint>& points, Color color)
			{
			fillPolygon(&(*points.begin()), points.size(), color);
			}
			
		virtual void fillPolygon( gdPoint* points,int nPoints, Color color)
			{
			::gdImageFilledPolygon(img, points, nPoints, color);
			}
		/**
		 * @param w width
		 * @param h height
		 * @param s start degree
		 * @param e end degree
		 */
		virtual void drawArc( int cx, int cy, int w, int h, int s, int e, Color color)
			{
			::gdImageArc(img, cx,  cy,  w,  h,  s,  e, color);
			}
		
		virtual void drawOval( int cx, int cy, int w, int h , Color color)
			{
			drawArc( cx,  cy,  w,h, 0 ,360, color);
			}
		
		virtual void drawCircle( int cx, int cy, int r , Color color)
			{
			r*=2;
			drawOval(cx, cy,r,r, color);
			}
		
		virtual ~Image()
			{
			if(img!=NULL)   ::gdImageDestroy(img);
			}
	private:
		FILE* _save(const char*filename)
			{
			FILE* io=std::fopen(filename,"wb");
			if(io==NULL)
				{
				std::ostringstream os;
                                os << "failed to open file \"" << filename << "\": "
                                	<< std::strerror(errno);
                                throw std::runtime_error(os.str());
				}
			return io;
			} 
	public:
		virtual void saveAsJpeg(const char *filename)
			{
			FILE* io=_save(filename);
			::gdImageJpeg(img, io, -1);
			std::fclose(io);
			}
		virtual void saveAsPng(const char *filename)
			{
			FILE* io=_save(filename);
			::gdImagePng(img, io);
			std::fclose(io);
			}
	friend class ImageFactory;
	};

class ImageFactory
	{
	protected:
		virtual Image* createImage(gdImagePtr img)
			{
			return new Image(img);
			}
	public:
		Image* createImage(int sx, int sy)
			{
			return createImage(::gdImageCreate(sx, sy));
			}
		Image* createImageTrueColor(int sx, int sy)
			{
			return createImage(::gdImageCreateTrueColor(sx,sy));
			}
	};

}}//namespaces
#endif