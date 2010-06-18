#include <fstream>
#include <sstream>
#include "lindenb/bio/das/dna_streambuf.h"
#include "lindenb/bio/genetic_code.h"
#include "lindenb/net/curl_streambuf.h"
#include "lindenb/json/json.h"

static const char *url1="http://genome.ucsc.edu/cgi-bin/das/hg19/dna?segment=chr1:100000,100100";

static void test0004()
	{
	lindenb::bio::StandardGeneticCode code;
	std::cout << code.translate('A','A','a') << code('A','T','G')<< std::endl;
	}

static void test0003()
	{
	std::string json="[\"hello'\t\n\",null,false,true,{},{'id':false,'_azd':-998,'id2':28.11E14,'id3':0}] " ;
	std::istringstream in(json);
	lindenb::json::Parser parser(in);
	lindenb::json::NodePtr node=parser.parse();
	parser.eof();
	std::cout << (*node) << std::endl;
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
static void test0001()
	{
	lindenb::net::curl_streambuf curl(url1);
	std::istream in(&curl);
	std::string line;
	while(std::getline(in,line,'\n'))
		{
		std::cout << line << std::endl;
		}
	}

static const char* g_delim="==============================================";
int main(int argc,char** argv)
	{
	try
		{
		test0004();
		std::cout << g_delim  << std::endl;
		test0003();
		std::cout << g_delim  << std::endl;
		test0002();
		std::cout << g_delim  << std::endl;
		test0001();
		}
	catch(std::runtime_error& err)
		{
		std::cerr << "Error2:" << err.what() << std::endl;
		}
	catch(std::exception& err)
		{
		std::cerr << "Error:" << err.what() << std::endl;
		}
	catch(...)
		{
		std::cerr << "Undefined Error" << std::endl;
		}
	return 0;
	}