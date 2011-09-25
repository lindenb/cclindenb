#include "lindenb/lang/throw.h"
#include "tabixcc.h"
#include "bgzf.h"
#include "tabix.h"

using namespace std;


#define LOCALNS lindenb::bio::tabix
#define TABIX_PTR (static_cast<tabix_t*>(this->_in))
#define ITER (static_cast<ti_iter_t>(this->_iter))

LOCALNS::Tabix::Tabix(const char* filename):_in(::ti_open(filename,0))
    {
    if(_in==NULL) THROW("Cannot open tabix file: "<< filename);
    if(::ti_lazy_index_load(TABIX_PTR)<0)
	{
	::ti_close(TABIX_PTR);
	THROW("cannot load index for "<< filename);
	}

    }

void LOCALNS::Tabix::close()
    {
    if(TABIX_PTR!=NULL) ::ti_close(TABIX_PTR);
    this->_in=NULL;
    }

LOCALNS::Tabix::~Tabix()
    {
    close();
    }


auto_ptr<vector<string> > LOCALNS::Tabix::chromosomes()
    {
    int n;
    vector<string>* chroms=new vector<string>;
    const char **names = ::ti_seqname(TABIX_PTR->idx, &n);
    chroms->reserve(n);
    for(int i=0;i< n;++i)
		{
		chroms->push_back(names[i]);
		}
    free(names);
    return auto_ptr< vector<string> >(chroms);
    }


int LOCALNS::Tabix::findTidByName(const char* chrom)
    {
    return chrom==NULL? -1:
	    ::ti_get_tid(TABIX_PTR->idx, chrom );
    }

std::auto_ptr<LOCALNS::Tabix::Cursor>
LOCALNS::Tabix::cursor(int tid,int chromStart,int chromEnd)
    {
    ti_iter_t iter= ::ti_queryi(TABIX_PTR,tid,chromStart,chromEnd);
    return auto_ptr<Cursor>(new Cursor(this->_in,(void*)iter));
    }

std::auto_ptr<LOCALNS::Tabix::Cursor>
LOCALNS::Tabix::cursor(const char* chrom,int chromStart,int chromEnd)
    {
    return cursor(findTidByName(chrom),chromStart,chromEnd);
    }


const char* LOCALNS::Tabix::Cursor::next(int* len)
    {
    if(_iter==NULL) return NULL;
    const char* s;
    int L=0;
    if((s=::ti_read(TABIX_PTR,ITER, &L))==0)
	{
	close();
	return NULL;
	}
    *len=(len!=NULL?L:-1);
    return s;
    }

void LOCALNS::Tabix::Cursor::close()
    {
    if(ITER!=NULL) ::ti_iter_destroy(ITER);
    this->_iter=NULL;
    }
LOCALNS::Tabix::Cursor::Cursor(void* in,void* iter):_in(in),_iter(iter)
    {

    }
LOCALNS::Tabix::Cursor::~Cursor()
    {
    close();
    }
