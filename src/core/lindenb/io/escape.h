#ifndef _LINDENB_IO_ESCAPE_H
#define _LINDENB_IO_ESCAPE_H
#include <string>
#include <iostream>
namespace lindenb { namespace io {
/**
    Motivation:
	escapes a C string
    Usage:
    	cout << lindenb:escapeC("Hello\n");
 */
class escapeC
	{
	private:
		const std::string& s;
	public:
		friend std::ostream& operator<< (std::ostream& o,escapeC  const& object);
		escapeC(const std::string& s):s(s) {}
	};

std::ostream& operator<< (std::ostream& out, escapeC const& object)
	{
	for(std::string::size_type i=0;i< object.s.size();++i)
		{
		switch(object.s[i])
			{
			case '\\': out << "\\\\"; break;
			case '\"': out << "\\\""; break;
			case '\'': out << "\\\'"; break;
			case '\n': out << "\\n"; break;
			case '\r': out << "\\r"; break;
			case '\t': out << "\\t"; break;
			default: out << object.s[i]; break;
			}
		}
	return out;
	}

}}

#endif