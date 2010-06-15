#ifndef _LINDENB_NET_CURLSTREAMBUF_H
#define _LINDENB_NET_CURLSTREAMBUF_H
#include <string>
#include <iostream>
#include <stdexcept>
#include <zlib.h>
#include <streambuf>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <curl/curl.h>
namespace lindenb
{
namespace net
{

#define WHERE std::cerr << __FILE__ <<":" << __LINE__ <<":" << __FUNCTION__<< std::endl

		
static size_t xxx(void *ptr, size_t size, size_t nmemb, void *userp)
	{
	WHERE;
	return size*nmemb;
	}		
		
class curl_streambuf : public std::basic_streambuf<char>
	{
	private:
		int state;
		CURL *curl_handle;
		CURLM *multi_handle;
		char* buffer;
		unsigned int buffer_size;
		int still_running;
		
		size_t call(void *ptr, size_t size, size_t nmemb)
			{
			WHERE;
			this->buffer_size = size*nmemb;
			char *array=(char*)std::realloc(this->buffer,this->buffer_size);
			if(array==NULL) throw std::runtime_error("out of memory");
			this->buffer=array;
			std::memcpy((void*)this->buffer, ptr, this->buffer_size);
			setg(	(char*)this->buffer,
				(char*)&this->buffer[1],//1 not 0, 0 will be returned in underflow
				(char*)&this->buffer[this->buffer_size]
				);
			return this->buffer_size;
			}
		
		
		static size_t callback(void *ptr, size_t size, size_t nmemb, void *userp)
			{
			WHERE;
			return ((curl_streambuf *)userp)->call(ptr,  size, nmemb);
			}
		
		static int curl_debug_callback(CURL *, curl_infotype, char *, size_t, void *)
			{
			WHERE;
			return 0;
			}
		
		void _error(CURLcode code)
			{
			if(code!=CURLE_OK)
				{
				std::cerr << "boum"<< ::curl_easy_strerror(code) << std::endl;
				throw std::runtime_error(::curl_easy_strerror(code));
				}
			}
		void _error(CURLMcode code)
			{
			if(code!=CURLM_OK)
				{
				std::cerr << "boum"<<code << std::endl;
				}
			}
		
		void _init(const char* uri)
			{
			assert(uri!=NULL);
			std::cerr << "uri is "<< uri << std::endl;
			WHERE;
			this->state=0;
			this->buffer_size=0;
			this->still_running=0;
			
			this->curl_handle=::curl_easy_init( );
			if(this->curl_handle==NULL)
				{
				throw std::runtime_error("curl_easy_init failed");
				}
			
			WHERE;
	
			CURLcode ret=::curl_easy_setopt(this->curl_handle, CURLOPT_URL, uri);
			_error(ret);
			ret=::curl_easy_setopt(this->curl_handle, CURLOPT_WRITEFUNCTION,xxx); 
			_error(ret);
			ret=::curl_easy_setopt(this->curl_handle, CURLOPT_VERBOSE, 1); 
			_error(ret);
			ret=::curl_easy_setopt(this->curl_handle, CURLOPT_WRITEDATA, this); 
			_error(ret);
			ret=::curl_easy_setopt(this->curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
			_error(ret);
			
			this->multi_handle=::curl_multi_init();
			if(this->multi_handle==NULL)
				{
				WHERE;
				:: curl_easy_cleanup(this->curl_handle);
				this->curl_handle=NULL;
				throw std::runtime_error("curl_multi_init failed");
				}
			CURLMcode ret2=::curl_multi_add_handle(this->multi_handle, this->curl_handle);
			_error(ret2);
			WHERE;
			
			
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

	
	public:
		curl_streambuf(const char* url)
			{
			_init(url);
			}
	
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
			WHERE;
			switch(this->state)
				{
				case -1: return EOF;break;
				default:
					{
					WHERE;
					CURLMcode ret= ::curl_multi_perform(this->multi_handle,&( this->still_running));
					
					std::cerr << "ret=" << ret << "n="<< still_running << std::endl;
					if(ret==CURLM_CALL_MULTI_PERFORM)
						{WHERE;
						if( this->buffer_size==0)
							{WHERE;
							this->state=-1;
							return EOF;
							}
						return this->buffer[0];
						}
					WHERE;
					this->state=-1;
					return EOF;
					break;
					}
				}
			
			

			
				
			
			
			}
	};

}
}

#endif