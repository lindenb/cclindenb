/*
 * tabixcc.h
 *
 *  Created on: Sep 24, 2011
 *      Author: lindenb
 */

#ifndef TABIXCC_H_
#define TABIXCC_H_
#include <string>
#include <memory>
#include <vector>

namespace lindenb { namespace bio { namespace tabix {
/**
 * C++ wrapper for TABIX
 */
class Tabix
    {
    public:
	   class Cursor
		{
		private:
		    void* _in;
		    void* _iter;
		    Cursor(void* in,void* iter);
		public:
		    ~Cursor();
		    void close();
		    const char* next(int* len=NULL);
		friend class Tabix;
		};
    private:
	void* _in;
	int findTidByName(const char* chrom);
	std::auto_ptr<Cursor > cursor(int tid,int start,int end);
    public:
	Tabix(const char* filename);
	virtual ~Tabix();
	virtual void close();
	std::auto_ptr<std::vector<std::string> > chromosomes();
	std::auto_ptr<Cursor> cursor(const char* chrom,int start,int end);


    };


    } } }
#endif /* TABIXCC_H_ */
