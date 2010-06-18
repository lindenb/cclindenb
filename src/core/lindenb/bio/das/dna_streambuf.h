#ifndef _LINDENB_BIO_DAS_DNASTREAMBUF_H
#define _LINDENB_BIO_DAS_DNASTREAMBUF_H
#include <iostream>
#include <stdexcept>
#include <libxml/xmlreader.h>


namespace lindenb { namespace bio { namespace das {
/**
 * name:
 * 	dna_streambuf
 *
 * author:
 *	Pierre Lindenbaum PhD. (2010)
 *	http://plindenbaum.blogspot.com
 *	plindenbaum@yahoo.fr
 *
 * usage:
 *	lindenb::net::curl_streambuf curl("http://genome.ucsc.edu/cgi-bin/das/hg19/dna?segment=chr1:100000,100100");
 *	std::istream in_net(&curl);
 *	lindenb::bio::das::dna_streambuf dasdna(in_net);
 *	std::istream in_das(&dasdna);
 */
class dna_streambuf:public std::streambuf
	{
	private:
		std::istream& in;
		xmlTextReaderPtr reader;
		/** state 0:init 1:in dna -1:EOF */
		int state;
		/** streambuf buffer */
		xmlChar* buffer;
		
		void _priv_close()
			{
			if(buffer!=NULL) ::xmlFree(buffer);
			buffer=NULL;
			if(reader!=NULL)
				{
				/* xmlFreeTextReader calls closeCallback which calls _priv_close
				 avoid loop */
				xmlTextReaderPtr copy=reader;
				reader=NULL;
				::xmlFreeTextReader(copy);
				}
			reader=NULL;
			}
		
		int inputReadCallback(char * buffer,  int len)
			{
			in.read(buffer,len);
			return in.gcount();
			}
		
		/** Callback used in the I/O Input API to read the resource
	 	context:	an Input context
		buffer:	the buffer to store data read
		len:	the length of the buffer in bytes
		Returns:	the number of bytes read or -1 in case of error
	 	*/
		static int xmlInputReadCallback(void * context,  char * buffer,  int len)
			{
			return ((dna_streambuf*)(context))->inputReadCallback(buffer,len);
			}
		
		int closeCallback()
			{
			_priv_close(); 
			return 0;
			}
		
		static int xmlInputCloseCallback(void * context)
			{
			return ((dna_streambuf*)(context))->closeCallback();
			}
	public:
		dna_streambuf(std::istream& in):std::streambuf(),in(in),reader(NULL),state(0),buffer(NULL)
			{
			LIBXML_TEST_VERSION;
			this->buffer=::xmlCharStrdup( "");
			if(this->buffer==NULL) throw std::runtime_error("Cannot create empty buffer");
			setg(	(char*)this->buffer,
				(char*)&(this->buffer)[1],
				(char*)&(this->buffer)[1]
				);
			}
		
		virtual ~dna_streambuf()
			{
			_priv_close();
			}
		virtual int underflow()
			{
			switch(state)
				{
				case -1: return EOF;
				case 0: {
					this->state=1;
					this->reader=::xmlReaderForIO(
						dna_streambuf::xmlInputReadCallback,
						dna_streambuf::xmlInputCloseCallback,
						this,
						NULL,NULL,
						XML_PARSE_NOBLANKS
						);
					if(reader==NULL)
						{
						this->state=-1;
						throw std::runtime_error("Cannot ::xmlReaderForIO");
						}
					//loop until we get DNA tag
					
					for(;;)
					 	{
					 	int success= xmlTextReaderRead(this->reader);
						if(!success)
							{
							_priv_close();
							throw std::runtime_error("xmlTextReaderRead fails");
							}
						int nodeType=xmlTextReaderNodeType(this->reader);
						if(nodeType!=XML_READER_TYPE_ELEMENT) continue;
						const xmlChar* tagName= ::xmlTextReaderConstName(this->reader);
						if(::xmlStrcmp(tagName,BAD_CAST "DNA")==0) break;
					 	}
					//threw
					}
				default:
					{
					if(this->reader==NULL) { state=-1; return EOF;}
					int success= xmlTextReaderRead(this->reader);
					if(!success)
						{
						_priv_close();
						throw std::runtime_error("xmlTextReaderRead fails");
						}
					int nodeType=xmlTextReaderNodeType(this->reader);
					switch(nodeType)
						{
						case XML_READER_TYPE_END_ELEMENT:
							{
							_priv_close();
							return EOF;
							break;
							}
						case XML_READER_TYPE_TEXT:
						case XML_READER_TYPE_WHITESPACE:
						case XML_READER_TYPE_CDATA:
						case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
							{
							//free previous value
							if(this->buffer!=NULL) ::xmlFree(this->buffer);
							this->buffer=NULL;
							this->buffer= xmlTextReaderValue(this->reader);
							
							if(this->buffer==NULL)
								{
								_priv_close();
								throw std::runtime_error("xmlTextReaderValue fails");
								}
							int len=xmlStrlen(this->buffer);
							int newLen=0;
							for(int i=0;i< len;++i)
								{
								if(std::isspace(this->buffer[i])) continue;
								this->buffer[newLen++]=this->buffer[i];
								}
							if( newLen==0) return overflow();
							setg(	(char*)this->buffer,
								(char*)this->buffer,
								(char*)&(this->buffer)[newLen]
								);
							return this->buffer[0];
							break;
							}
						default:
							{
							_priv_close();
							throw std::runtime_error("unexpected case");
							break;
							}
						}
					//never reached
					return EOF;
					break;
					}
				}
			
			}
	};
} } } //namespaces


#endif
