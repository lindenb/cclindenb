/*
 * BamFile.h
 *
 *  Created on: Sep 24, 2011
 *      Author: lindenb
 */

#ifndef LINDENB_BIO_BAM_H_
#define LINDENB_BIO_BAM_H_
#include <stdint.h>
namespace lindenb { namespace bio { namespace bam {

class BamFile;

class Read
    {
    private:
	void* _bam1;
	void* _in;
	void* _index;
    public:
	Read();
	~Read();
	int32_t tid() const;
	int32_t position() const;
	const char* chromosome() const;
    friend class BamFile;
    };

class BamFile
    {
    private:
	void* _in;
	void* _index;
    public:
	BamFile(const char* filename,bool load_index=true);
	virtual ~BamFile();
	virtual void close();
	bool read(Read* read);
	const char* chromosome(int tid);
    };

} } }

#endif /* BAMFILE_H_ */
