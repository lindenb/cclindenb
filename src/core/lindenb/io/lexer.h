#ifndef _LINDENB_IO_LEXER_H
#define _LINDENB_IO_LEXER_H
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>

namespace lindenb { namespace io {
/**
    Motivation:
	a  simple queue-based Lexer
    Usage:
    	
 */
class Lexer
	{
	private:
		std::istream& in;
		std::vector<char> buffer;
	public:
		typedef std::vector<char>::size_type size_type; 
		Lexer(std::istream& in):in(in)
			{
			
			}
		
		int mustWhitespaces()
			{
			int c=0;
			while(!isEof() &&
				std::isspace(get()))
				{
				c++;
				consumme(1);
				}
			if(c==0)
				{
				throw std::runtime_error("Error expected whitespace before "/* +toString() */);
				}
			return c;
			}
		
		/** skip all withspaces, returns the next get() character */
		int skipWhitespaces()
			{
			int c;
			while((c=get())!=EOF &&
				std::isspace(c))
				{
				consumme(1);
				}
			return get();
			}
		
		/** return the next char without removing it from the stack */
		int get()
			{
			return get(0);
			}
		
		/** return the index-th char without removing it from the stack */
		int get(int index)
			{
			while(buffer.size()<=(size_type)index)
				{
				int c= in.get();
				if(c==EOF) return EOF;
				buffer.push_back((char)c);
				}
			return buffer[index];
			}
		
		/** pop and returns the next element in the stack */
		int pop()
			{
			if(!buffer.empty())
				{
				char c= buffer.at(0);
				buffer.erase(buffer.begin());
				return c;
				}
			return in.get();
			}
		
		bool inAvail(const char* s)
			{
			return inAvail(0,s);
			}
	
		
		bool inAvail(int index,const char* s)
			{
			std::size_t len=std::strlen(s);
			for(std::size_t i=0;i< (std::size_t)len;++i)
				{
				if(get(index+i)!=s[i]) return false;
				}
			return true;
			}
	
		
		int consumme(const int index,const int n)
			{
			int n_read=0;
			while(	(int) buffer.size()< index )
				{
				int c= in.get();
				if(c==EOF) return 0;
				buffer.push_back((char)c);
				}
			while(	n_read<n && (int)buffer.size()>index )
				{
				n_read++;
				buffer.erase(buffer.begin()+index);
				}
			while(n_read<n)
				{
				if(in.get()==-1) break;
				n_read++;
				}
			return n_read;
			}
		
		int consumme( int n)
			{
			return consumme(0,n);
			}
		
		bool isEof()
			{
			return get()==EOF;
			}
		std::string toString(int i)
			{
			std::string str;
			for(int j=0;j< i;++j)
				{
				int c=get(j);
				if(c==EOF) { str.append("<<EOF>>"); break; }
				str+=(char)c;
				}
			return str;
			}
		
		std::string toString()
			{
			return toString(20);
			}
		};


}}//namespaces

#endif