#ifndef _LINDENB_NET_CURLSTREAMBUF_H
#define _LINDENB_NET_CURLSTREAMBUF_H
#include <string>
#include <iostream>
#include <stdexcept>
#include <streambuf>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <curl/curl.h>
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
				virtual const char* name() const
					{
					return _key.c_str();
					}
			};
		
		/** simple parameter */
		class Param:public Parameter
			{
			private:
				std::string _value;
			public:
				Parameter(std::string key,std::string value):Parameter(key),_value(value)
					{
					}
				
				const char* value() const
					{
					return _value.c_str();
					}
				
				virtual ~Parameter()
					{
					}
				
			};
			
		
	private:
		std::vector<Param*> parameters;
		
		void priv_cleanup()
			{
			}
		void parseGET()
			{
			const char* query_string=::getenv("QUERY_STRING");
			if(query_string==NULL) throw std::runtime_error("QUERY_STRING is NULL");
			std::istringstream in( query_string );
			parseQueryString(in);
			}
		void parsePOST()
			{
			String contentLengthStr= getenv("CONTENT_LENGTH");
			if(contentLengthStr==null) throw new IOException("CONTENT_LENGTH missing");
			int contentLength=0;
			try {
				contentLength= Integer.parseInt(contentLengthStr);
			} catch (Exception e) {
				throw new IOException("Bad content Length "+contentLength);
				}
			if(contentLength<0) throw new IOException("Bad Content");
			if(contentLength>getContentMaxLength()) throw new IOException("Content too large");
			
			if(!isMultipart())
				{
				parse(new BufferedReader(
						new InputStreamReader(System.in)),
						contentLength);
				}
			else
				{
				throw new IOException("Cannot parse multipart actions");
				}
			}
			
		void parseQueryString(std::istream& in)
			{
			Param* curr=NULL;
			}
		
		
		int c;
		StringBuilder key=new StringBuilder();
		StringBuilder value=null;
		int count=0;
		while((c=in.read())!=-1 && count < maxCharRead)
			{
			++count;
			if(c=='+') c=' ';
			if(c=='&')
				{
				if(key.length()!=0)
					{
					addParameter(key,value);
					}	
				key=new StringBuilder();
				value=null;
				}
			else if(c=='=' && key.length()>0 && value==null)
				{
				value = new StringBuilder();
				}
			else
				{
				if(c=='%' && count+2<=maxCharRead)
					{
					int c2= in.read();
					if(c2==-1) throw new IOException("Bad Input");
					int c3= in.read();
					if(c3==-1) throw new IOException("Bad Input");	
					c=x2c(c2,c3);
					count+=2;
					}
				if(value!=null)
					{
					value.append((char)c);
					}
				else
					{
					key.append((char)c);
					}
				}
			}
		if(key.length()!=0)
			{
			addParameter(key,value);
			}
		}
	
	 static int x2c(int c1,int c2)
		{
		char buff[c1,c2,0];
		return stroi(buff,NULL,16);
		}
	public:
		CGI()
			{
			}
			
		virtual ~CGI()
			{
			priv_cleanup();
			}
		bool isMultipart()
			{
			String contentType= getenv("CONTENT_TYPE");
			if(contentType==null) return false;
			return contentType.indexOf("multipart/form-data")!=-1;
			}
		void parse()
			{
			char* requestMethod=::getenv("REQUEST_METHOD");
			if(requestMethod==NULL) throw std::runtime_error("Cannot find REQUEST_METHOD");
			if(std::strcmp(requestMethod,"POST")==0)
				{
				parsePOST();
				}
			else if(std::strcmp(requestMethod,"GET")==0)
				{
				parseGET();
				}
			else
				{
				throw std::runtime_error("Unknown REQUEST_METHOD");
				}
			}
		
		public std::set<std::string> getParameterNames()
			{
			std::set<std::string> keys;
			for(Param p:this.parameters)
				{
				keys.add(p.getKey());
				}
			return keys;
			}
	};

}
}

#endif