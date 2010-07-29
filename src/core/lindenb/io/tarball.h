/*
 * tarball.h
 *
 *  Created on: Jul 28, 2010
 *      Author: Pierre Lindenbaum PhD
 *              plindenbaum@yahoo.fr
 *              http://plindenbaum.blogspot.com
 *              
 */

#ifndef LINDENB_IO_TARBALL_H_
#define LINDENB_IO_TARBALL_H_
#include <cstdio>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <ctime>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace lindenb { namespace io {

/**
 * Creates a Tar Archive
 */
class Tar
    {
    protected:
	std::ostream& out;

	struct PosixTarHeader
		{
		char name[100];
		char mode[8];
		char uid[8];
		char gid[8];
		char size[12];
		char mtime[12];
		char checksum[8];
		char typeflag[1];
		char linkname[100];
		char magic[6];
		char version[2];
		char uname[32];
		char gname[32];
		char devmajor[8];
		char devminor[8];
		char prefix[155];
		char pad[12];
		};


	void _init(PosixTarHeader* header)
	    {
	    std::memset(header,0,sizeof(PosixTarHeader));
	    std::sprintf(header->magic,"ustar  ");
	    std::sprintf(header->mtime,"%011lo",time(NULL));
	    std::sprintf(header->mode,"%07o",0644);
	    char * s = ::getlogin();
	    if(s!=NULL)  std::snprintf(header->uname,32,"%s",s);
	    std::sprintf(header->gname,"%s","users");
	    }

	void _checksum(PosixTarHeader* header)
	    {
	    unsigned int sum = 0;
	    char *p = (char *) header;
	    char *q = p + sizeof(PosixTarHeader);
            while (p < header->checksum) sum += *p++ & 0xff;
	    for (int i = 0; i < 8; ++i)  {
	          sum += ' ';
	          ++p;
	        }
	    while (p < q) sum += *p++ & 0xff;

	    std::sprintf(header->checksum,"%06o",sum);
	    }

	void _size(PosixTarHeader* header,unsigned long fileSize)
	    {
	    std::sprintf(header->size,"%011llo",(long long unsigned int)fileSize);
	    }

	void _filename(PosixTarHeader* header,const char* filename)
	    {
	    if(filename==NULL || filename[0]==0 || std::strlen(filename)>=100)
		{
		std::ostringstream os;
	    	os << "invalid archive name \"" << filename << "\"";
	    	throw std::runtime_error(os.str());
		}
	    std::snprintf(header->name,100,"%s",filename);
	    }

	void _endRecord(std::size_t len)
	    {
	    char c='\0';
	    while((len%sizeof(PosixTarHeader))!=0)
		    {
		    out.write(&c,sizeof(char));
		    ++len;
		    }
	    }
    public:

	Tar(std::ostream& out):out(out)
	    {
	    assert(sizeof(PosixTarHeader)==512);
	    }

	virtual ~Tar()
	    {
	    }

	/** writes 2 empty blocks. Should be always called before closing the Tar file */
	void finish()
	    {
	    //The end of the archive is indicated by two blocks filled with binary zeros
	    PosixTarHeader header;
	    std::memset((void*)&header,0,sizeof(PosixTarHeader));
	    out.write((const char*)&header,sizeof(PosixTarHeader));
	    out.write((const char*)&header,sizeof(PosixTarHeader));
	    out.flush();
	    }

	void put(const char* filename,const std::string& s)
	    {
	    put(filename,s.c_str(),s.size());
	    }
	void put(const char* filename,const char* content)
	    {
	    put(filename,content,std::strlen(content));
	    }

	void put(const char* filename,const char* content,std::size_t len)
	    {
	    PosixTarHeader header;
	    _init(&header);
	    _filename(&header,filename);
	    header.typeflag[0]=0;
	    _size(&header,len);
	    _checksum(&header);
	    out.write((const char*)&header,sizeof(PosixTarHeader));
	    out.write(content,len);
	    _endRecord(len);
	    }

	void putFile(const char* filename,const char* nameInArchive)
	    {
	    char buff[BUFSIZ];
	    std::FILE* in=std::fopen(filename,"rb");
	    if(in==NULL)
		{
		std::ostringstream os;
		os << "Cannot open " << filename << " "<< std::strerror(errno);
		throw std::runtime_error(os.str());
		}
	    std::fseek(in, 0L, SEEK_END);
	    long int len= std::ftell(in);
	    std::fseek(in,0L,SEEK_SET);

	    PosixTarHeader header;
	    _init(&header);
	    _filename(&header,nameInArchive);
	    header.typeflag[0]=0;
	    _size(&header,len);
	    _checksum(&header);
	    out.write((const char*)&header,sizeof(PosixTarHeader));


	    std::size_t nRead=0;
	    while((nRead=std::fread(buff,sizeof(char),BUFSIZ,in))>0)
		{
		out.write(buff,nRead);
		}
	    std::fclose(in);

	    _endRecord(len);
	    }
    };


}}

#endif /* LINDENB_IO_TARBALL_H_ */
