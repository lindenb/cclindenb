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
#include <zlib.h>
#include "lindenb/lang/throw.h"

namespace lindenb { namespace io {

/**
 * buffer reading a gz file
 */
class gzinbuf : public public lindenb::io::istreambuf
	{
	private:
		gzFile* source;
		
		void _close()
			{
			if(source!=NULL)
				{
				::gzclose(source);
				source=NULL;
				}
			}
	
	public:
		gzinbuf(const char *path,int buff_size=BUFSIZ):
			lindenb::io::istreambuf(buff_size),
			source(NULL)
			{
			std::errno=0;
			source=::gzopen(path,"rb");
			if(source==NULL)
				{
				THROW("failed to open \"" << path<< "\" ("
                                        << std::strerror(errno) << ").");
				}
			}
			
		virtual ~gzinbuf()
			{
			_close();
			}
		
		
		virtual int underflow ( )
			{
			if(source==NULL) return EOF;
			int nRead= ::gzread(this->source,(void*)this->buffer,this->buffer_capacity,);
			if(nRead<0)
				{
				 throw std::runtime_error("I/O error");
				}
			else if(nRead==0)
				{
				_close();
				return EOF;
				}
			this->buffer_size=(unsigned int)nRead;
			setg(	(char*)this->buffer,
				(char*)&this->buffer[1],
				(char*)&this->buffer[this->buffer_size]
				);
			return this->buffer_size==0?EOF:this->buffer[0];
			}
	};

}}//namespaces

#endif
