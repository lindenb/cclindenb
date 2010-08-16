#ifndef _LINDENB_XML_ESCAPE_H
#define _LINDENB_XML_ESCAPE_H
#include <cstring>
#include <string>
#include <iostream>
namespace lindenb
{
namespace xml
{


/**
    Motivation:
	escapes a XML string
    Usage:
    	cout << lindenb::xml::escape("<Hello/>\n");
 */
class escape
	{
	private:
		const char* s;
		std::string::size_type len;
	public:
		friend std::ostream& operator<< (std::ostream& o,escape  const& object);
		escape(const std::string& str):s(str.c_str()),len(str.size()) {}
		escape(const char*str):s(str),len(str==NULL?0:std::strlen(str)) {}
		escape(const char*str,std::size_t len):s(str),len(str==NULL?0:len) {}
	};

std::ostream& operator<< (std::ostream& out, const escape& object)
	{
	for(std::size_t i=0;i< object.len;++i)
		{
		switch(object.s[i])
			{
			case '&': out << "&amp;"; break;
			case '<': out << "&lt;"; break;
			case '>': out << "&gt;"; break;
			case '\'': out << "&apos;"; break;
			case '\"': out << "&quot;"; break;
			default: out << object.s[i]; break;
			}
		}
	return out;
	}
}
}

#endif