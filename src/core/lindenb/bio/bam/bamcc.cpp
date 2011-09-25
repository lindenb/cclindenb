/*
 * BamFile.cpp
 *
 *  Created on: Sep 24, 2011
 *      Author: lindenb
 */
#include <cerrno>
#include "lindenb/lang/throw.h"
#include "lindenb/bio/bam/bamcc.h"
#define BAMFILE_PTR (static_cast<samfile_t*>(this->_in))
#define READ_PTR (static_cast<samfile_t*>(this->_bam1))
#define LOCALNS lindenb::bio::bam

LOCALNS::Read::Read():_bam((void*)bam_init1()),
    _in(NULL),_index(NULL)
    {
    if(_bam==NULL) THROW("Cannot init bam");
    }

LOCALNS::Read::~Read()
    {
    bam_destroy1(READ_PTR);
    }

int
LOCALNS::Read::tid()
    {
    return READ_PTR->core.tid;
    }

const char*
LOCALNS::Read::chromosome()
    {
    return BAMFILE_PTR->header->target_name[tid()];
    }

int LOCALNS::Read::position()
    {
    return BAMFILE_PTR->core.pos;
    }

LOCALNS::BamFile::BamFile(const char* filename,bool load_index):_in(0)
    {
    errno=0;
    _in=(void*)samopen(filename, "rb", 0);
    if(_in==NULL)
	{
	THROW("Cannot open bam file \"" << filename << "\" "<< strerror(errno));
	}
    if(load_index)
	{
	errno=0;
	_index=(void*)::bam_index_load(filename);
	if(_index==NULL)
	    {
	    ::samclose(BAMFILE_PTR);
	    THROW("Cannot open index for bam file \""
		<< filename << "\" " << strerror(errno)
		);
	    }
	}
    }

void LOCALNS::BamFile::close()
    {
    if(_in!=NULL)
	   {
	    ::samclose(BAMFILE_PTR);
	    _in=NULL;
	   }
    }

LOCALNS::BamFile::~BamFile()
    {
    close();
    }

bool LOCALNS::BamFile::read(Read* read)
    {
    if(::samread(BAMFILE_PTR, read->_bam1)>0)
	{
	read->_in=this->_in;
	read->_index=this->_index;
	return true;
	}

    read->_in=NULL;
    read->_index=NULL;
    return false;
    }

const char* LOCALNS::BamFile::chromosome(int tid)
    {
    return BAMFILE_PTR->header->target_name[tid]
    }
