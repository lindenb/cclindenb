#ifndef _LINDENB_BERKELEYDB_H
#define _LINDENB_BERKELEYDB_H
#include <db.h>
#include "lindenb/io/binding.h"

namespace lindenb
{

namespace bdb
{

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
			err << "A BerkekeyDB error occurred: "
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


}//bdb
}//lindenb
#endif
