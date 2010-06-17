#include <fstream>
#include "lindenb/bio/das/dna_streambuf.h"
#include "lindenb/net/curl_streambuf.h"

static const char *url1="http://genome.ucsc.edu/cgi-bin/das/hg19/dna?segment=chr1:100000,100100";

static void test0001()
	{
	lindenb::net::curl_streambuf curl(url1);
	std::istream in(&curl);
	for(;;)
		{
		int c=in.get();
		if(c==-1) break;
		std::cout << (char)c;
		}
	
	}

static void test0002()
	{
	lindenb::net::curl_streambuf curl(url1);
	std::istream in_net(&curl);
	lindenb::bio::das::dna_streambuf dasdna(in_net);
	std::istream in_das(&dasdna);
	for(;;)
		{
		int c=in_das.get();
		if(c==-1) break;
		std::cout << (char)c;
		}
	
	}


int main(int argc,char** argv)
	{
	test0002();
	std::cout << "\n===============================\n";
	test0001();
	
	return 0;
	}