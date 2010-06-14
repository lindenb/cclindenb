#ifndef _LINDENB_XML_ESCAPE_H
#define _LINDENB_XML_ESCAPE_H
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
		const std::string& s;
	public:
		friend std::ostream& operator<< (std::ostream& o,escape  const& object);
		escape(const std::string& s):s(s) {}
	};

std::ostream& operator<< (std::ostream& out, escapeC const& object)
	{
	for(std::string::size_type i=0;i< object.s.size();++i)
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