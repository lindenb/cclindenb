#ifndef _LINDENB_IO_ISTREAMBUF_H
#define _LINDENB_IO_ISTREAMBUF_H

#include <streambuf>



namespace lindenb { namespace io {

/**
 * an abstract buffered input from std::basic_streambuf&lt;char&gt;
 */
class istreambuf : public lindenb::io::istreambuf
	{
	protected:
		/** buffer */
		char* buffer;
		/** current size of the buffer */
		unsigned int buffer_size;
		/** buffer capacity */
		unsigned int buffer_capacity;
		
		void _init(unsigned  int capacity)
			{
			if(capacity<=0) capacity=1;
			this->buffer_size=0;
			this->buffer_capacity=capacity;
			this->buffer=new char[this->buffer_capacity];
	
			setg(	&this->buffer[0],
				&this->buffer[this->buffer_capacity],
				&this->buffer[this->buffer_capacity]
				);
			}
	

		istreambuf()
			{
			_init(BUFSIZ);
			}
		
		istreambuf(unsigned int buffer_size)
			{
			_init(buffer_size);
			}
		
	public:	
		virtual ~istreambuf()
			{
			if(this->buffer!=NULL)
				{
				delete [] this->buffer;
				this->buffer=NULL;
				}
			}
	};

}}//namespaces

#endif