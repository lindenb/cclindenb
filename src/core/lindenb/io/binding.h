#ifndef _LINDENB_IO_BINDING_H
#define _LINDENB_IO_BINDING_H
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>
#include "lindenb/util/debug.h"

namespace lindenb { namespace io {

/**
 * Abstract Interface used to serialize/unserialize data
 * 
 */
template<typename T>
class TupleBinding
	{
	protected:
		
		void read(std::istream& in, char* s,std::size_t n)
			{
			
			in.read(s,n);
			if((std::size_t)in.gcount()!=n)
				{
				std::ostringstream os;
				os << "failed to read " << n << " bytes (read "
					<< in.gcount() << ").";
				throw std::runtime_error(os.str());
				}
			}
		void write(std::ostream& out, const char* s,std::size_t n)
			{
			out.write(s,n);
			if(out.bad()){
				std::ostringstream os;
				os << "failed to write " << n << " bytes";
				throw std::runtime_error(os.str());
				}
			}
	public:
		/** TupleBinding */
		TupleBinding()
			{
			}
		
		virtual ~TupleBinding()
			{
			}
		/** abstract ; read input stream and creates a new object of type T */
		virtual  std::auto_ptr<T> readObject(std::istream& in)=0;
		/** abstract; write object to output stream */
		virtual void writeObject(std::ostream& out,const T* object)=0;
		
		
		virtual void writeLong(std::ostream& out, long value)
			{
			write(out,(const char*)&value,sizeof(long));
			}
		
		virtual long readLong(std::istream& in)
			{
			long value;
			read(in,(char*)&value,sizeof(long));
			return value;
			}		
		
		virtual void writeInt(std::ostream& out, int value)
			{
			write(out,(const char*)&value,sizeof(int));
			}
		
		virtual int readInt(std::istream& in)
			{
			int value;
			read(in,(char*)&value,sizeof(int));
			return value;
			}
		
		virtual void writeShort(std::ostream& out, short value)
			{
			write(out,(const char*)&value,sizeof(short));
			}
		
		virtual short readShort(std::istream& in)
			{
			short value;
			read(in,(char*)&value,sizeof(short));
			return value;
			}
			
		virtual void writeChar(std::ostream& out, char value)
			{
			write(out,(const char*)&value,sizeof(char));
			}
		
		virtual char readChar(std::istream& in)
			{
			char value;
			read(in,(char*)&value,sizeof(char));
			return value;
			}
		
		virtual void writeBool(std::ostream& out, bool value)
			{
			write(out,(const char*)&value,sizeof(bool));
			}
		
		virtual bool readBool(std::istream& in)
			{
			bool value;
			read(in,(char*)&value,sizeof(bool));
			return value;
			}
		
		virtual void writeFloat(std::ostream& out,const float value)
			{
			write(out,(const char*)&value,sizeof(float));
			}
		
		virtual float readFloat(std::istream& in)
			{
			float value;
			read(in,(char*)&value,sizeof(float));
			return value;
			}
		
		virtual void writeDouble(std::ostream& out,const double value)
			{
			write(out,(const char*)&value,sizeof(double));
			}
		
		virtual double readDouble(std::istream& in)
			{
			double value;
			read(in,(char*)&value,sizeof(double));
			return value;
			}
		
			
		virtual void writeString(std::ostream& out,const std::string* s)
			{
			std::string::size_type n=s->size();
			write(out,(char*)&n,sizeof(std::string::size_type));
			write(out,s->data(),n);
			}
			
		virtual std::auto_ptr<std::string> readString(std::istream& in,std::string* s)
			{
			std::string::size_type old_size=s->size();
			std::string::size_type n;

			read(in,(char*)&n,sizeof(std::string::size_type));
			//http://stackoverflow.com/questions/1816319
			s->resize(old_size+n,'\0');
			read(in,(char*)&(s->data()[old_size]),n);
			return std::auto_ptr<std::string>(s);
			}
		virtual std::auto_ptr<std::string> readString(std::istream& in)
			{
			std::string* s=new std::string;
			return readString(in,s);
			}
	};



template<typename T>
class CopyBinding:public TupleBinding<T>
	{
	public:
		CopyBinding() {}
		virtual ~CopyBinding() {}
		virtual T* newInstance() { return new T;}
		/** abstract ; read input stream and creates a new object of type T */
		virtual  std::auto_ptr<T> readObject(std::istream& in)
			{
			T* object=newInstance();
			this->read(in,(char*)object,sizeof(T));
			return std::auto_ptr<T>(object);
			}
		/** abstract; write object to output stream */
		virtual void writeObject(std::ostream& out,const T* object)
			{
			this->write(out,(const char*)object,sizeof(T));
			}
	};


class StringBinding:public TupleBinding<std::string>
	{
	public:
		StringBinding() {}
		virtual ~StringBinding() {}
		
		/** read input stream and creates a new object of type T */
		virtual  std::auto_ptr<std::string> readObject(std::istream& in)
			{
			return std::auto_ptr<std::string>(this->readString(in));
			}
		/** write object to output stream */
		virtual void writeObject(std::ostream& out,const std::string* object)
			{
			this->writeString(out,object);
			}
	};

} } //namespaces

#endif
