#include <fstream>
#include <sstream>
#include <cassert>
#include "lindenb/bio/das/dna_streambuf.h"
#include "lindenb/bio/genetic_code.h"
#include "lindenb/net/curl_streambuf.h"
#include "lindenb/json/json.h"
#include "lindenb/util/split.h"
#include "lindenb/math/geometry.h"

static const char *url1="http://genome.ucsc.edu/cgi-bin/das/hg19/dna?segment=chr1:100000,100100";

static void test0007()
	{
	lindenb::math::geom::Line L1(-1,0,1,0);
	lindenb::math::geom::Line L2(0,-1,0,1);
	lindenb::math::geom::Line L3(0,-1,0,1);
	lindenb::math::geom::Line L4(1,-1,1,1);
	lindenb::math::geom::Line L5(10,13,20,22);
	lindenb::math::geom::Line L6(5,-13,10,21);
	lindenb::math::geom::Point p;
	std::cout << L1.intersection(L2,&p)<< std::endl;
	std::cout << p << std::endl;
	std::cout << L2.intersection(L3,&p)<< std::endl;
	std::cout << L2.intersection(L4,&p)<< std::endl;
	std::cout << L1.intersection(L5,&p)<< std::endl;
	std::cout << p << std::endl;
	std::cout << L6.intersection(L5,&p)<< std::endl;
	std::cout << p << std::endl;
	
	lindenb::math::geom::Point p1;
	lindenb::math::geom::Point p2;
	lindenb::math::geom::Circle C1(0,0,1);
	lindenb::math::geom::Circle C2(0,0,1);
	const lindenb::math::geom::Circle C3(0.5,0,1.3);
	lindenb::math::geom::Circle C4(100.1,100,1);
	std::cout << C1 << " " << C2 <<":" <<  C1.intersects(C2) << std::endl;
	std::cout << C1 << " " << C4 <<":" <<  C1.intersects(C4) << std::endl;
	std::cout << C1 << " " << C3 <<":" <<  C1.intersection(C3,&p1,&p2) << "=" << p1 << "/" << p2 << std::endl;
	}
static void test0006()
	{
	lindenb::util::Split spliter(',');
	std::vector<std::string> tokens=spliter.split("A,B,C,D,,,,");
	for(int i=0;i< tokens.size();++i) std::cout << tokens[i] << std::endl;
	assert(tokens.size()==4);
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
	std::cout << *(node2->find("[5]/id2")) << std::endl;
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
		test0007();
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