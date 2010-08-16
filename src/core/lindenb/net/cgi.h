#ifndef _LINDENB_NET_CURLSTREAMBUF_H
#define _LINDENB_NET_CURLSTREAMBUF_H
#include <string>
#include <iostream>
#include <stdexcept>
#include <streambuf>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include <set>
#include <sstream>
namespace lindenb
{
namespace net
{

/**
 * name:
 * 	CGI
 *
 * author:
 *	Pierre Lindenbaum PhD. (2010)
 *	http://plindenbaum.blogspot.com
 *	plindenbaum@yahoo.fr
 *
 * usage:
 *	TODO
 */
class CGI 
	{
	public:
		/** abstract parameter */
		class Parameter
			{
			private:
				std::string _key;
			public:
				Parameter(std::string key):_key(key)
					{
					}
				virtual ~Parameter()
					{
					}
				
				const char* name() const
					{
					return _key.c_str();
					}
				virtual const char* value() const=0;
				virtual bool isSimpleParameter()=0;
			};
		
		/** simple parameter */
		class Param:public Parameter
			{
			private:
				std::string _value;
			public:
				Param(std::string key,std::string value):Parameter(key),_value(value)
					{
					}
				
				virtual const char* value() const
					{
					return _value.c_str();
					}
				
				virtual ~Param()
					{
					}
				
				virtual bool isSimpleParameter()
					{
					return true;
					}
			};
			
	protected:
		std::size_t max_input_size() const
			{
			return 10000000;
			}
	
	private:
		std::vector<Param*> parameters;
	
		void parseGET()
			{
			const char* query_string=::getenv("QUERY_STRING");
			if(query_string==NULL) throw std::runtime_error("QUERY_STRING is NULL");
			int contentLength=std::strlen(query_string);
			if(contentLength> max_input_size() ) throw std::runtime_error("Content too large");
			std::string query(query_string);
			std::istringstream in(query);
			if(contentLength>0)
				{
				parseQueryString(in,contentLength);
				}
			}
		void parsePOST()
			{
			char* contentLengthStr= getenv("CONTENT_LENGTH");
			if(contentLengthStr==NULL) throw std::runtime_error("CONTENT_LENGTH missing");
			int contentLength=atoi(contentLengthStr);
			if(contentLength<0) throw std::runtime_error("Bad content Length ");
			if(contentLength> max_input_size() ) throw std::runtime_error("Content too large");
			
			if(!isMultipart())
				{
				if(contentLength>0)
					{
					parseQueryString(std::cin,contentLength);
					}
				}
			else
				{
				throw std::runtime_error("Cannot parse multipart actions");
				}
			}
			
		void parseQueryString(std::istream& in,int maxCharRead)
			{
			Param* curr=NULL;
			int c;
			std::string key;
			std::string value;
			int count=0;
			bool in_key=true;
			while((c=in.get())!=EOF && count < maxCharRead)
				{
				++count;
				if(c=='+') c=' ';
				if(c=='&')
					{
					if(!key.empty())
						{
						parameters.push_back(new Param(key,value));
						}
					in_key=true;	
					key.clear();
					value.clear();
					}
				else if(c=='=')
					{
					in_key=false;
					value.clear();
					}
				else
					{
					if(c=='%' && count+2<=maxCharRead)
						{
						int c2= in.get();
						if(c2==EOF) throw std::runtime_error("Bad Input");
						int c3= in.get();
						if(c3==EOF) throw std::runtime_error("Bad Input");	
						c=x2c(c2,c3);
						count+=2;
						}
					if(!in_key)
						{
						value+=(char)c;
						}
					else
						{
						key+=(char)c;
						}
					}
				}
			if(!key.empty())
				{
				parameters.push_back(new Param(key,value));
				}
			}
		
		static int x2c(int c1,int c2)
			{
			char buff[3]={c1,c2,0};
			return std::strtol(buff,NULL,16);
			}
		bool isMultipart()
			{
			char* contentType= getenv("CONTENT_TYPE");
			if(contentType==NULL) return false;
			return std::strstr(contentType,"multipart/form-data")!=NULL;
			}
	public:
		CGI()
			{
			}
			
		virtual ~CGI()
			{
			std::vector<Param*>::iterator r=parameters.begin();
			while(r!=parameters.end())
				{
				delete (*r);
				++r;
				}
			}
		
		const char* requestMethod() const
			{
			return ::getenv("REQUEST_METHOD");
			}
		
		void parse()
			{
			const char* method= requestMethod();
			if(method==NULL) throw std::runtime_error("Cannot find REQUEST_METHOD");
			if(std::strcmp(method,"POST")==0)
				{
				parsePOST();
				}
			else if(std::strcmp(method,"GET")==0)
				{
				parseGET();
				}
			else
				{
				throw std::runtime_error("Unknown REQUEST_METHOD");
				}
			}
		
		bool hasParameter(std::string key) const
			{
			std::vector<Param*>::const_iterator r=parameters.begin();
			while(r!=parameters.end())
				{
				if(key.compare((*r)->name())==0)
					{
					return true;
					}
				++r;
				}
			return false;
			}
		
		const char* getParameter(std::string key) const
			{
			std::vector<Param*>::const_iterator r=parameters.begin();
			while(r!=parameters.end())
				{
				if(key.compare((*r)->name())==0)
					{
					return (*r)->value();
					}
				++r;
				}
			return NULL;
			}
			
		std::set<std::string> getParameters(std::string key) const
			{
			std::set<std::string> vals;
			std::vector<Param*>::const_iterator r=parameters.begin();
			while(r!=parameters.end())
				{
				if(key.compare((*r)->name())==0)
					{
					vals.insert(std::string( (*r)->value() ));
					}
				++r;
				}
			return vals;
			}	
			
		
		std::set<std::string> getParameterNames() const
			{
			std::set<std::string> keys;
			std::vector<Param*>::const_iterator r=parameters.begin();
			while(r!=parameters.end())
				{
				keys.insert(std::string((*r)->name()));
				++r;
				}
			return keys;
			}
		
	private:
		 void _var(std::ostream& out,const char* s)
			{
			char* p=::getenv(s);
			out << s << ":" << (p==NULL?"null":p) << "\n";
			}
	public:
		
		std::ostream& dump(std::ostream& out)
			{
			std::set<std::string> keys;
			_var(out,"DOCUMENT_ROOT");
			_var(out,"HTTP_REFERER");
			_var(out,"HTTP_USER_AGENT");
			_var(out,"PATH_INFO");
			_var(out,"PATH_TRANSLATED");
			_var(out,"QUERY_STRING");
			_var(out,"REMOTE_ADDR");
			_var(out,"REQUEST_METHOD");
			_var(out,"SCRIPT_NAME");
			_var(out,"SERVER_NAME");
			_var(out,"SERVER_PORT");
			
			std::vector<Param*>::iterator r=parameters.begin();
			while(r!=parameters.end())
				{
				out	<< (*r)->name()
					<< " : "
					<< (*r)->value()
					<< std::endl;
				++r;
				}
			return out;
			}
		
	};

}
}

#endif