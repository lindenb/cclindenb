/**
 * Author:
 *	Pierre Lindenbaum PhD plindenbaum@yahoo.fr
 * About:
 *	Memory Allocator
 */


#ifndef _LINDENB_LANG_ALLOCATOR_
#define _LINDENB_LANG_ALLOCATOR_

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <new>

namespace lindenb
{

typedef class MemoryAllocator
    {
    public:
        MemoryAllocator() {}
        virtual ~MemoryAllocator() {}
        /** alloc some memory */
        virtual void* alloc(std::size_t n) throw(std::bad_alloc) =0;
       /** Allocates a block of memory for an array of num elements, each of them size bytes long, and initializes all its bits to zero. */
        virtual void* calloc(std::size_t num_elements, std::size_t size) throw(std::bad_alloc)
        	{
        	void *ptr=this->alloc(num_elements*size);
        	std::memset(ptr,0,num_elements*size);
        	return ptr;
        	}
        /** release some memory */
        virtual void release(void* ptr)=0;
    protected:
        void _throw(std::size_t n) throw(std::bad_alloc)
            {
            std::ostringstream msg;
            msg << "Out of Memory: cannot allocate "<< n << " bytes.";
            throw std::bad_alloc(msg.str());
            }
    }MemoryAllocatorPtr;

/**
 * Allocator implementation using malloc/free
 *
 */
class CMemoryAllocator:public MemoryAllocator
    {
    virtual void* alloc(std::size_t n) throw(std::bad_alloc)
        {
        void* p=std::malloc(n);
        if(p==null) _throw(n);
        return (void*)p;
        }
     
    virtual void release(void* ptr)
        {
        std::free(ptr);
        }
    };
/**
 * Allocator implementation using C++ new/delete
 *
 */
class CCMemoryAllocator:public MemoryAllocator
    {
    virtual void* alloc(std::size_t n) throw(std::bad_alloc)
        {
        char* p=new (std::nothrow) char[n];
        if(p==null) _throw(n);
        return (void*)p;
        }
    
    
    virtual void release(void* ptr)
        {
        if(ptr!=NULL) delete[] ptr;
        }
    };
}//lindenb
#endif