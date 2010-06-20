/**
 * Author:
 *	Pierre Lindenbaum PhD plindenbaum@yahoo.fr
 * About:
 *	Interfaces for the GNU gdbm library
 * References:
 *	http://www.vivtek.com/gdbm/api.html
 */
#ifndef _LINDENB_GDBM_H_
#define _LINDENB_GDBM_H_
#include <sstream>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include "lindenb/io/binding.h"
#include <gdbm.h>

namespace lindenb { namespace gdbm {
	


template<typename K,typename V>
class Database
	{
	public:
		/** read-write access for everybody */
		static const int DEFAULT_MODE=0666;
		enum read_write
			{
			reader=GDBM_READER,//to open the file as a reader 
			writer=GDBM_WRITER,//to open the file as a writer 
			wrcreat=GDBM_WRCREAT,//to open as a writer and create the file if it doesn't exist 
			newdb=GDBM_NEWDB//to open as a writer and overwrite any existing file 
			};
		static const read_write DEFAULT_READWRITE=wrcreat;
		
		
		
	private:
		GDBM_FILE dbf;
		lindenb::io::TupleBinding<K> *keyBinding;
		lindenb::io::TupleBinding<V> *dataBinding;
		read_write read_write_mode;
		
		class inbuff:public std::streambuf
			{
			private:
				const datum entry;
				int nRead;
				
			public:
				inbuff(const datum entry):std::streambuf(),entry(entry),nRead(0)
					{
					setg(	entry.dptr,
						entry.dptr,
						&(entry.dptr[entry.dsize])
						);
					}
				virtual ~inbuff() {}
				virtual int underflow()
					{
					return EOF;
					}
			};
		
	protected:
		lindenb::io::TupleBinding<K>* getKeyBinding()	
			{
			return this->keyBinding;
			}
		lindenb::io::TupleBinding<V>* getDataBinding()
			{
			return this->dataBinding;
			}
			
	public:
		
			
		bool isReadOnly()
			{
			return	this->read_write_mode==reader;
			}		
		
	private:	
		std::string serializeKey(const K* key)
			{
			std::ostringstream out;
			getKeyBinding()->writeObject(out,key);
			return out.str();
			}
		
		std::string serializeData(const V* value)
			{
			std::ostringstream out;
			getDataBinding()->writeObject(out,value);
			return out.str();
			}
		
		void _priv_open(const char* filename,int block_size,read_write read_write,int mode,void (*fatal_func)())
			{
			this->read_write_mode=read_write;
			gdbm_errno=0;
			dbf= ::gdbm_open((char*)filename,
				block_size,
				(int)read_write,
				mode,
				fatal_func
				);
			if(dbf==NULL)
				{
				std::ostringstream err;
				err << "Cannot open \""
				    << filename
				    << "\" : "
				    << gdbm_strerror(gdbm_errno)
				    ;
				throw std::runtime_error(err.str());
				}
			
			}
		
		void _priv_close()
			{
			if(dbf!=NULL)
				{
				if(!isReadOnly())
				   {
				   ::gdbm_reorganize(dbf);
				   }
				::gdbm_close(dbf);
				}
			dbf=NULL;
			}
		
		GDBM_FILE db()
			{
			if(dbf==NULL) throw std::runtime_error("Database was closed");
			return dbf;
			}
	
		std::auto_ptr<V> _get(datum key)
			{
			datum content = ::gdbm_fetch(db(), key);
			if(content.dptr==NULL) return std::auto_ptr<V>(NULL);
			inbuff buff(key);
			std::istream in(&buff);
			std::auto_ptr<V> v= getDataBinding()->readObject(in);
			std::free(content.dptr);
			return v;
			}
		
		//If successful, the function returns true
		bool store(const K& key,const V& value,int flag)
			{
			datum key_entry;
			datum value_entry;
			std::string SK= serializeKey(&key);
			key_entry.dptr = (char*)SK.data();
			key_entry.dsize = (int)SK.size();
			std::string SV= serializeData(&value);
			value_entry.dptr = (char*)SV.data();
			value_entry.dsize = (int)SV.size();
			bool  success = ::gdbm_store(db(), key_entry, value_entry, flag)==0;
			return success;
			}
	
	public:		
		Database(const char* filename,int block_size,read_write rw,int mode,void (*fatal_func)(),
			lindenb::io::TupleBinding<K> *kBinding,
			lindenb::io::TupleBinding<V> *vBinding
			):keyBinding(kBinding),dataBinding(vBinding)
			{
			_priv_open(filename,block_size,rw,mode,fatal_func);
			}
		
		Database(const char* filename,
			lindenb::io::TupleBinding<K> *kBinding,
			lindenb::io::TupleBinding<V> *vBinding
			)
			{
			_priv_open(filename,0,DEFAULT_READWRITE,DEFAULT_MODE,NULL);
			}
			
		
		
		/** try to shrink the database */
		bool reorganize()
			{
			return ::gdbm_reorganize(db())==0;
			}
		
		virtual ~Database()
			{
			_priv_close();
			}
		
		/** close the databse */
		virtual void close()
			{
			_priv_close();
			}
		
		
		/** insert the following key/value. Replace previous data if it exists */
		bool put(const K& key,const V& value)
			{
			return store(key,value,GDBM_REPLACE);
			}
		
		/** insert the following key/value. Do nothing if key exists.
		    returns true if key was inserted
		 */
		bool putNoReplace(const K& key,const V& value)
			{
			return store(key,value,GDBM_INSERT);
			}
		
		/** removes the given key, returns true if key existed and was deleted */
		bool remove(const K& key)
			{
			datum entry;
			std::string SK= serializeKey(key);
			entry.dptr = (char*)SK.data();
			entry.dsize = (int)SK.size();
			int ret = ::gdbm_delete(db(), entry)==0;
			return ret;
			}
		/** returns true if the database contains the given key */
		bool contains(const K& key)
			{
			datum entry;
			std::string SK= serializeKey(&key);
			entry.dptr = (char*)SK.data();
			entry.dsize = (int)SK.size();
			bool exists = ::gdbm_exists(db(), entry) == 1;//returns 1 if key exists
			return exists;
			}
		/** returns the value for the given key or NULL */
		std::auto_ptr<V> get(const K& key)
			{
			datum entry;
			std::string SK= serializeKey(&key);
			entry.dptr = (char*)SK.data();
			entry.dsize = (int)SK.size();
			std::auto_ptr<V> val = _get( entry);
			return val;
			}
		
		std::auto_ptr<V> operator[](const K& key)
			{
			return get(key);
			}
		//http://www.delorie.com/gnu/docs/gdbm/gdbm_9.html
		class Cursor
			{
			private:
				datum key;
				Database<K,V>* owner;
				bool is_first;
				bool eof;
			public:
				Cursor(Database<K,V>* owner):owner(owner),is_first(true),eof(false)
					{
					}
				Cursor(const Cursor& cp):key(cp.key),owner(cp.owner),is_first(cp.is_first),eof(cp.eof)
					{
					}
				~Cursor()
					{
					std::free(key.dptr);//free last value
					}
				bool next()
					{
					if(eof) return false;
					if(is_first)
						{
						is_first=false;
						key = ::gdbm_firstkey ( owner->db() );
						}
					else
						{
						datum nextkey = ::gdbm_nextkey ( owner->db(), key);
						std::free(key.dptr);//free previous value
						key=nextkey;
						}
					eof=  key.dptr==NULL;
					return !eof;
					}
				std::auto_ptr<K> first()
					{
					if(key.dptr==NULL) return std::auto_ptr<K>();
					
					inbuff buff(key);
					std::istream in(&buff);
					return std::auto_ptr<K>(owner->getDataBinding()->readObject(in));
					}
				
				std::auto_ptr<V> second()
					{
					if(key.dptr==NULL) return std::auto_ptr<V>();
					return std::auto_ptr<V>(owner->_get(key));
					}
			};
		
		Cursor cursor()
			{
			return Cursor(this);
			}
		
	};
	
	
	
	}//namespace gdbm
	
	}//namespace lindenb
#endif