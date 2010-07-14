#ifndef _LINDENB_IO_STDIOSTREAMBUF_H
#define _LINDENB_IO_STDIOSTREAMBUF_H
#include <string>
#include <iostream>
#include <stdexcept>
#include <zlib.h>
#include <streambuf>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "lindenb/io/";
namespace lindenb {namespace io {


class stdio_streambuf : public lindenb::io::istreambuf
	{
	private:
		std::FILE* source;
	public:
		stdio_streambuf(FILE* input,unsigned int buff_size):
					lindenb::io::istreambuf(buff_size),
					source(input)
			{
			}
		stdio_streambuf(FILE* input):source(input)
			{
			}
			
		virtual ~stdio_streambuf()
			{
			}
		
		
		virtual int underflow ( )
			{
			if(source==NULL) return EOF;
			this->buffer_size= std::fread(this->buffer,1,this->buffer_capacity,this->source);
			if(this->buffer_size==0)
				{
				source=NULL;
				return EOF;
				}
			setg(	(char*)this->buffer,
				(char*)&this->buffer[1],
				(char*)&this->buffer[this->buffer_size]
				);
			return this->buffer_size==0?EOF:this->buffer[0];
			}
	};

} }

#endif