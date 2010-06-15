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
namespace lindenb
{


class stdio_streambuf : public std::basic_streambuf<char>
	{
	private:
		char* buffer;
		unsigned int buffer_size;
		unsigned int buffer_capacity;
		std::FILE* source;
		
		void _init(unsigned int buffer_size)
			{
			assert(buffer_size>0);
			this->buffer_size=0;
			this->buffer_capacity=buffer_size;
			this->buffer=new char[this->buffer_capacity];
	
			setg(	&this->buffer[0],
				&this->buffer[this->buffer_capacity],
				&this->buffer[this->buffer_capacity]
				);
			}
		void _close()
			{
			
			source=NULL;
			}
	
	public:
		stdio_streambuf(FILE* input,unsigned int buff_size):source(input)
			{
			_init(buff_size);
			}
		stdio_streambuf(FILE* input):source(input)
			{
			_init(BUFSIZ);
			}
			
		~stdio_streambuf()
			{
			if(this->buffer!=NULL)
				{
				delete [] this->buffer;
				this->buffer=NULL;
				}
			}
		
		
		virtual int underflow ( )
			{
			if(source==NULL) return EOF;
			this->buffer_size=std::fread(this->buffer,1,this->buffer_capacity,this->source);
			if(this->buffer_size==0)
				{
				_close();
				return EOF;
				}
			setg(	(char*)this->buffer,
				(char*)&this->buffer[1],
				(char*)&this->buffer[this->buffer_size]
				);
			return this->buffer_size==0?EOF:this->buffer[0];
			}
	};

}

#endif