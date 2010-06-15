#ifndef _LINDENB_BERKELEYDB_H
#define _LINDENB_BERKELEYDB_H
#include <db.h>

namespace lindenb
{

namespace bdb
{

/**
 * TupleBinding
 */
template<typename T>
class TupleBinding
	{
	public:
		virtual void objectToEntry(const T& object,DBT* entry)=0;
		virtual void free(DBT* entry)=0;
		virtual std::auto_ptr<T> entryToObject(const DBT* entry)=0;
	};

/**
 * CopyBinding
 */
template<typename T>
class CopyBinding:public TupleBinding<T>
	{
	public:
		virtual void objectToEntry(const T& object,DBT* entry)
			{
			std::memset((void*)entry,0,sizeof(DBT));
			entry->data=(char*)&object;
			entry->size=sizeof(T);
			}
		virtual void free(DBT* entry)
			{

			}
		virtual std::auto_ptr<T> entryToObject(const DBT* entry)
			{
			T *ptr=new T;
			std::memcpy(ptr,entry->data,sizeof(T));
			std::free(entry->data);
			return std::auto_ptr<T>(ptr);
			}
	};

/**
 * StringBinding
 */
class StringBinding:public TupleBinding<std::string>
	{
	public:
		virtual void objectToEntry(const std::string& object,DBT* entry)
			{
			std::memset((void*)entry,0,sizeof(DBT));
			std::string::size_type len=object.size();
			entry->size=sizeof(std::string::size_type)+len;
			char* ptr=new char[entry->size];
			std::memcpy(&ptr[0],&len,sizeof(std::string::size_type));
			object.copy(&ptr[sizeof(std::string::size_type)],len);
			entry->data=ptr;
			}
		virtual void free(DBT* entry)
			{
			delete (char*)(entry->data);
			}
		virtual std::auto_ptr<std::string> entryToObject(const DBT* entry)
			{
			std::string::size_type len;
			const char* ptr=(const char*)entry->data;
			std::memcpy(&len,&ptr[0],sizeof(std::string::size_type));
			std::string *s=new std::string(
				&(ptr[sizeof(std::string::size_type)]),
				len
				);
			return std::auto_ptr<std::string>(s);
			}
	};

class IntBinding:public CopyBinding<int>
	{
	public:
	};

class RecnoBinding:public CopyBinding<db_recno_t>
	{
	public:
	};


template<typename K,typename V>
class Database
	{
	private:
		DB *dbp;
		TupleBinding<K> *keyBinding;
		TupleBinding<V> *dataBinding;

		void _priv_open(
				DB_ENV* environment,
				const char* filename,
				const char* db_name,
				DBTYPE type,
				u_int32_t flags
				)
			{
			int ret = ::db_create(&dbp, environment, 0);

			checkError(ret);

			ret = dbp->open(dbp,        /* DB structure pointer */
				NULL,       /* Transaction pointer */
				filename, /* On-disk file that holds the database. */
				db_name,       /* Optional logical database name */
				type,   /* Database access method */
				flags,      /* Open flags */
				0);         /* File mode (using defaults) */
			checkError(ret);
			}

		void _priv_close()
			{
			if(dbp!=NULL)
				{
				dbp->close(dbp, 0);
				}
			dbp=NULL;
			}

	protected:

		void checkError(int ret)
			{
			if(ret==0) return;
			std::ostringstream err;
			err << "An error occurred: "
				<< ::db_strerror(ret)
				;
			throw std::runtime_error(err.str());
			}

		DB* db()
			{
			if(dbp==NULL) throw std::runtime_error("Database was closed");
			return dbp;
			}

		TupleBinding<K>* getKeyBinding()
				{
				return keyBinding;
				}
		TupleBinding<V>* getDataBinding()
				{
				return dataBinding;
				}


		std::auto_ptr<V> _get(DBT* key)
			{
			DBT data;
			std::memset(&data, 0, sizeof(DBT));
			int ret=db()->get(db(), txn(), key, &data, 0);
			checkError(ret);
			//TODO check value was found
			return getDataBinding()->entryToObject(&data);
			}

		/** returns the transaction, always NULL */
		DB_TXN* txn()
			{
			return NULL;
			}



	public:
		/** opens a new wrapped BerkeleyDB database */
		Database(DB_ENV* environment,
				const char* filename,
				const char* db_name,
				DBTYPE type,
				u_int32_t flags,
			TupleBinding<K> *kBinding,
			TupleBinding<V> *vBinding
			):keyBinding(kBinding),dataBinding(vBinding)
			{
			_priv_open(environment,filename,db_name,type,flags);
			}

		/** destructor */
		virtual ~Database()
			{
			_priv_close();
			}

		/** close the database */
		virtual void close()
			{
			_priv_close();
			}


		/** put(key,value) database */
		int put(const K& key,const V& value,uint32_t flag)
			{
			DBT key_entry;
			DBT value_entry;
			getKeyBinding()->objectToEntry(key,&key_entry);
			getDataBinding()->objectToEntry(value,&value_entry);
			int ret= db()->put(db(), txn(), &key_entry, &value_entry, flag);
			checkError(ret);
			getKeyBinding()->free(&key_entry);
			getDataBinding()->free(&value_entry);
			return ret;
			}


		bool delelete(const K& key,uint32_t flags)
			{
			DBT entry;
			getKeyBinding()->objectToEntry(key,&entry);
			int ret =db()->del(db(),txn(), entry,flags);
			checkError(ret);
			getKeyBinding()->free(&entry);
			return ret==0;
			}

		bool contains(const K& key)
			{
			DBT entry;
			getKeyBinding()->objectToEntry(key,&entry);
			std::auto_ptr<V> content = _get( entry);
			getKeyBinding()->free(&entry);
			return content.get()!=NULL;
			}

		std::auto_ptr<V> get(const K& key)
			{
			DBT entry;
			DBT data;
			std::memset(&data, 0, sizeof(DBT));

			getKeyBinding()->objectToEntry(key,&entry);
			int ret=db()->get(db(), txn(), key, &data, 0);
			checkError(ret);
			//TODO check value was found
			getKeyBinding()->free(&entry);
			return false;//TODO change this
			}
		std::auto_ptr<V> operator[](const K& key)
			{
			return get(key);
			}
	};

/**
 * RecnoDatabase
 */
template<typename V>
class RecnoDatabase:public Database<db_recno_t,V>
	{
	public:
		RecnoDatabase()
			{

			}
	};
}//bdb
}//lindenb
#endif
