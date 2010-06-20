#include <fstream>
#include <sstream>
#include <cassert>
#include "lindenb/bio/das/dna_streambuf.h"
#include "lindenb/bio/genetic_code.h"
#include "lindenb/net/curl_streambuf.h"
#include "lindenb/json/json.h"

static const char *url1="http://genome.ucsc.edu/cgi-bin/das/hg19/dna?segment=chr1:100000,100100";


static void test0006()
	{
	lindenb::io::StringBinding sb;
	}

static void test0005()
	{
	std::string s;
	for(int i=0;i< 10;++i) s+='\0';
	assert(s.size()==10);
	assert(std::strlen(s.c_str())==0);
	}

static void test0004()
	{
	lindenb::bio::StandardGeneticCode code;
	std::cout << code.translate('A','A','a') << code('A','T','G')<< std::endl;
	}

static void test0003()
	{
	std::string json="[\"hello'\t\n\",null,false,true,{},{'id':false,'_azd':-998,'id2':28.11E14,'id3':0}] " ;
	//std::string json="{'id':122,'id2':null}";
	std::istringstream in(json);
	lindenb::json::Parser parser(in);
	lindenb::json::NodePtr node=parser.parse();
	parser.eof();
	std::cout << (*node) << std::endl;
	lindenb::json::NodePtr node2=node->clone();
	std::cout << (*node2) << std::endl;
	
	assert(node->equals(node2));
	
	delete node2;
	
	std::ostringstream os;
	lindenb::json::JSONBinding binding;
	binding.writeObject(os,node);
	std::string s=os.str();
	std::istringstream is(s);
	node2= binding.readObject(is).release();
	std::cout << (*node2) << std::endl;
	delete node2;
	delete node;
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
		test0006();
		test0005();
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