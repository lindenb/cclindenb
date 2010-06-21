#ifndef LINDENB_UTIL_SPLIT_H
#define LINDENB_UTIL_SPLIT_H
#include <vector>
#include <string>
#include <cstring>
namespace lindenb {  namespace util {
/**
 * splits a string into token
 */
class Split
	{
	private:
		char delim;
	public:
		Split(char delim):delim(delim)
			{
			}
		Split():delim('\t')
			{
			}
		virtual ~Split()
			{
			}
		
		std::vector<std::string> split(const char* s,std::size_t len)
			{
			std::vector<std::string> tokens;
			//skip the last items
			while(len>0 && s[len-1]==delim)
				{
				len--;
				}
			std::size_t last=0;
			std::size_t i=0;
			while(i<=len)
				{
				if(i==len || s[i]== delim)
					{
					tokens.push_back(std::string(&s[last],i-last));
					if(i==len) break;
					last=i+1;
					}
				++i;
				}
			return tokens;
			}
		
		std::vector<std::string> split(const char* s)
			{
			return split(s,std::strlen(s));
			}
		
		std::vector<std::string> split(const std::string& s)
			{
			return split(s.data(),s.size());
			}
		
	};


 }}//namespaces
#endif
