#ifndef LINDENB_BIO_GENETIC_CODE_H
#define LINDENB_BIO_GENETIC_CODE_H

namespace lindenb { namespace bio {

/** Abstract Genetic Code */
class GeneticCode
	{
	protected:
		GeneticCode()
			{
			}
	public:
		virtual ~GeneticCode()
			{
			}
		
		virtual bool isStandard() const
			{
			return false;
			}
		virtual const char* name() const=0;
		virtual char translate(char c1,char c2, char c3) const=0;
		char operator()(char c1,char c2, char c3) const
			{
			return translate(c1,c2,c3);
			}
	};

/** Abstract Genetic Code */
class NBCIGeneticCode:public GeneticCode
	{
	protected:
		const std::string genetic_code;
		const std::string _name;
		
		static int base2index(char c)
			{
			switch(tolower(c))
				{
				case 't': return 0;
				case 'c': return 1;
				case 'a': return 2;
				case 'g': return 3;
				default: return -1;
				}
			}  
	public:
		NBCIGeneticCode(const char* code,const char*name):genetic_code(code),_name(name)
			{
			}

		virtual ~NBCIGeneticCode()
			{
			}
		
		virtual const char* name() const
			{
			return _name.c_str();
			}	
			
		virtual char translate(char a,char b, char c) const
			{
			int base1= base2index(a);
			int base2= base2index(b);
			int base3= base2index(c);
			if(base1==-1 || base2==-1 || base3==-1)
				{
				return '?';
				}
			else
				{
				return genetic_code[base1*16+base2*4+base3];
				}
			}
		
		virtual bool isStandard() const
			{
			return true;
			}
	};

/** Standard Genetic Code */
class StandardGeneticCode:public NBCIGeneticCode
	{
	public:
		StandardGeneticCode():NBCIGeneticCode(
			"FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
			"Standard"
			)
			{
			}
		
		virtual ~StandardGeneticCode()
			{
			}
	};

} } //namespaces


#endif
