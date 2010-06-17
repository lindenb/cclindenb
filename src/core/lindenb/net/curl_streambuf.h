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
 * 	curl_streambuf
 *
 * author:
 *	Pierre Lindenbaum PhD. (2010)
 *	http://plindenbaum.blogspot.com
 *	plindenbaum@yahoo.fr
 *
 * usage:
 *	curl_streambuf buf("http://anywhere.org");
 * 	std::istream in(&buf);
 */
class curl_streambuf : public std::basic_streambuf<char>
	{
	private:
		/** url */
		std::string url;
		/** current state: 0 init, 1 fetch, -1 EOF */
		int state;
		/** CURL ptr */
		CURL *curl_handle;
		/** CURLM ptr */
		CURLM *multi_handle;
		/** the buffer for streambug */
		char* buffer;
		/** size of the buffer for streambug */
		unsigned int buffer_size;
		/**  number of CURL running */
		int still_running;
		/** check callback was called once */
		bool callback_was_called;
		
		/** called by the CURL  callback */
		size_t call(void *ptr, size_t size, size_t nmemb)
			{
			this->callback_was_called=true;
			this->buffer_size = size*nmemb;
			char *array=(char*)std::realloc(this->buffer,this->buffer_size);
			if(array==NULL) throw std::runtime_error("out of memory");
			this->buffer=array;
			std::memcpy((void*)this->buffer, ptr, this->buffer_size);
			setg(	(char*)this->buffer,
				(char*)&this->buffer[0],
				(char*)&this->buffer[this->buffer_size]
				);
			return this->buffer_size;
			}
		
		/** call back called by libCURL */
		static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userp)
			{
			return ((curl_streambuf *)userp)->call(ptr,  size, nmemb);
			}
		
		void check_error(CURLcode code)
			{
			if(code!=CURLE_OK)
				{
				throw std::runtime_error(::curl_easy_strerror(code));
				}
			}
		void check_error(CURLMcode code)
			{
			if(code!=CURLM_OK)
				{
				throw std::runtime_error(::curl_multi_strerror(code));
				}
			}
		
		
	public:
		/** constructor from a valid url */
		curl_streambuf(const char* url):url(url),
				state(0),
				curl_handle(NULL),
				buffer(NULL),
				buffer_size(0)
			{
			
			
			/* alloc the buffer */
			this->buffer=(char*)std::malloc(BUFSIZ);
			if(this->buffer==NULL)
				{
				throw std::runtime_error("out of memory");
				}
	

			this->buffer_size=0;
			setg(	&this->buffer[0],
				&this->buffer[BUFSIZ],
				&this->buffer[BUFSIZ]
				);
			}
		
		/** destructor */
		virtual ~curl_streambuf()
			{
			if(this->multi_handle!=NULL)
				{
				if(curl_handle!=NULL)
					{
					::curl_multi_remove_handle(
						this->multi_handle,
						this->curl_handle);
					}
				::curl_multi_cleanup(this->multi_handle);
				}
			if(curl_handle!=NULL)
				{
				:: curl_easy_cleanup(this->curl_handle);
				}
			if(this->buffer!=NULL)
				{
				std::free(this->buffer);
				}
			}
		
		
		virtual int underflow ( )
			{
			CURLcode ret;
			CURLMcode ret2;
			switch(this->state)
				{
				case -1: return EOF;break;
				case 0:
					{
					this->state=1;
					this->buffer_size=0;
					this->still_running=0;
					
					/* init curl */
					this->curl_handle=::curl_easy_init( );
					if(this->curl_handle==NULL)
						{
						throw std::runtime_error("::curl_easy_init failed");
						}
					
					ret=::curl_easy_setopt(this->curl_handle, CURLOPT_URL, this->url.c_str());
					check_error(ret);
					
					
					ret=::curl_easy_setopt(this->curl_handle, CURLOPT_VERBOSE, 0); 
					check_error(ret);
					ret=::curl_easy_setopt(this->curl_handle, CURLOPT_WRITEDATA, this); 
					check_error(ret);
					ret=::curl_easy_setopt(this->curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
					check_error(ret);
					ret=::curl_easy_setopt(this->curl_handle, CURLOPT_WRITEFUNCTION,
						curl_streambuf::write_callback
						); 
					check_error(ret);
					
					
					this->multi_handle=::curl_multi_init();
					if(this->multi_handle==NULL)
						{
						:: curl_easy_cleanup(this->curl_handle);
						this->curl_handle=NULL;
						throw std::runtime_error("curl_multi_init failed");
						}
					
					ret2=::curl_multi_add_handle(
						this->multi_handle,
						this->curl_handle
						);
					check_error(ret2);
					this->still_running=1;
					//threw
					}
				default:
					{
					this->buffer_size=0;
					
					do
						{
						this->callback_was_called=false;
						ret2= ::curl_multi_perform(this->multi_handle,&( this->still_running));
						} while( this->still_running!=0 && this->callback_was_called==false);
					
					
					if( this->buffer_size==0)
						{
						this->state=-1;
						return EOF;
						}
					return this->buffer[0];
					}
				}
			}
	};

}
}

#endif