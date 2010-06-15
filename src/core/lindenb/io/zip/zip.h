#ifndef _LINDENB_IO_ZIP_H
#define _LINDENB_IO_ZIP_H
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
#define WHERE std::cerr << __FILE__ <<":" << __LINE__ <<":" << __FUNCTION__<< std::endl

class unzip_streambuf : public std::basic_streambuf<char>
	{
	protected:
		void _throw(int ret)
			{
			if(ret==0) return;
			WHERE;
			switch (ret)
				{
				case Z_ERRNO:
					throw std::runtime_error("I/O error");
					break;
				case Z_STREAM_ERROR:
					throw std::runtime_error("invalid compression level");
					break;
				case Z_DATA_ERROR:
					throw std::runtime_error("invalid or incomplete deflate data");
					break;
				case Z_MEM_ERROR:
					throw std::runtime_error("out of memory");
					break;
				case Z_VERSION_ERROR:
					throw std::runtime_error("zlib version mismatch!");
					break;
				default:
					WHERE;
					throw std::runtime_error("zlib error");
					break;
				}
			WHERE;
			}
	
	private:
		z_stream* strm;
		unsigned int buff_size;
		Bytef* compressed;
		Bytef* uncompressed;
		std::FILE* source;
		int state;
		
		void _init(unsigned int buff_size)
			{
			assert(buff_size>0);
			this->state=0;
			this->buff_size=buff_size;
			this->compressed=new Bytef[buff_size];
			this->uncompressed=new Bytef[buff_size];
		
			setg(	(char*)&this->uncompressed[0],
				(char*)&this->uncompressed[this->buff_size],
				(char*)&this->uncompressed[this->buff_size]
				);
			
			/* allocate inflate state */
			strm = new z_stream;
			std::memset(strm,0,sizeof(z_stream));
			strm->zalloc = Z_NULL;
			strm->zfree = Z_NULL;
			strm->opaque = Z_NULL;
			strm->avail_in = 0;
			strm->next_in = Z_NULL;
			
			int ret = ::inflateInit(strm);
    			_throw(ret);
			}
		
	
	
	public:
		unzip_streambuf(FILE* input,unsigned int buff_size):source(input)
			{
			_init(buff_size);
			}
		unzip_streambuf(FILE* input):source(input)
			{
			_init(BUFSIZ);
			}
			
		virtual ~unzip_streambuf()
			{
			if(strm!=NULL)
				{
				::inflateEnd(strm);
				delete strm;
				}
			
			if(this->compressed!=NULL)
				{
				delete [] this->compressed;
				}
		
			if(this->uncompressed!=NULL)
				{
				delete [] this->uncompressed;
				}
			}
		
		virtual int underflow ( )
			{
			int ret=0;
			
			switch(this->state)
				{
				case -1: return EOF;
				case 0:
					{
					this->strm->avail_in = std::fread(
						(char*)this->compressed,
						1,
						this->buff_size,
						this->source
						);
					if(std::ferror(this->source))
						{
						WHERE;
						_throw(Z_ERRNO);
						}
					this->state=1;
					strm->next_in = this->compressed;
					if (strm->avail_in == 0)
						{
						this->state=-1;
						return EOF;
						}
					//threw
					}
				default:
					{
					WHERE;
					strm->avail_out = this->buff_size;
					strm->next_out =  this->uncompressed;
					WHERE;
					ret = ::inflate(strm, Z_NO_FLUSH);
					if(ret!=Z_OK )
						{
						WHERE;
						std::cerr << ret << "/" <<  strm->avail_out << std::endl;
						if(strm->msg!=NULL) std::cerr << strm->msg << std::endl;
						_throw(ret);
						}
					WHERE;
					unsigned int have = this->buff_size - strm->avail_out;
					
					setg(	(char*)this->uncompressed,
						(char*)&this->uncompressed[1],
						(char*)&this->uncompressed[have+1]
						);
					if(strm->avail_out==0)
						{
						state=0;
						}
					return have==0?EOF:this->uncompressed[0];
					break;
					}
				}
			}
	};

}

#endif