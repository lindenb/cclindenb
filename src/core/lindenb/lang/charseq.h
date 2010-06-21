#ifndef _LINDENB_CHARSEQ_H
#define _LINDENB_CHARSEQ_H

namespace lindenb
{

/**
 * Abstraction of a CharSequence
 * 
 */
typedef class CharSequence
	{
	protected:
		CharSequence()
			{
			}
	public:
		typedef  int size_type;
		
		virtual ~CharSequence()
			{
			}
		/** returns the index-th char in the sequence */
		virtual char at(size_type index) const=0;
		/** returns the length of the sequence */
		virtual size_type length() const=0;

		virtual bool empty() const
			{
			return length()==0;
			}

		virtual int compare(const CharSequence* other) const
			{
			size_type i=0;
			for(;;)
				{
				if(i==this->length())
					{
					if(i==other->length()) return 0;
					return -1;
					}
				if(i==other->length()) return 1;
				int d = (this->at(i) -  other->at(i) );
				if(d!=0) return d;
				}
			return 0;
			}

		virtual int compareIgnoreCase(CharSequence* other) const
			{
			size_type i=0;
			for(;;)
				{
				if(i==this->length())
					{
					if(i==other->length()) return 0;
					return -1;
					}
				if(i==other->length()) return 1;
				int d = (std::toupper(this->at(i)) -  std::toupper(other->at(i)));
				if(d!=0) return d;
				}
			return 0;
			}

		virtual std::ostream& print(std::ostream& out) const
			{
			for(size_type i=0;i< length();++i) out.put(at(i));
			return out;
			}
		virtual std::string toString() const
			{
			std::ostringstream out;
			print(out);
			return out.str();
			}

		virtual std::auto_ptr<CharSequence> substring(size_type start,size_type end) const;
		virtual std::auto_ptr<CharSequence> toUpperCase() const;
		virtual std::auto_ptr<CharSequence> toLowerCase() const;
		virtual std::auto_ptr<CharSequence> reverse() const;
	}*CharSequencePtr;

std::ostream &operator<<(std::ostream &out, const CharSequence& seq)
	{
	return seq.print(out);
	}


class DelegateCharSequence: public CharSequence
	{
	private:
		CharSequencePtr delegate;
	public:

		DelegateCharSequence(CharSequencePtr delegate):delegate(delegate)
			{
			assert(delegate!=NULL);
			}

		virtual ~DelegateCharSequence()
			{
			}

		virtual CharSequencePtr getDelegate()
			{
			return delegate;
			}

		virtual const CharSequencePtr getDelegate() const
			{
			return delegate;
			}

		virtual char at(size_type index) const
			{
			return getDelegate()->at(index);
			}

		virtual size_type length() const
			{
			return getDelegate()->length();
			}
	};

/**
 * SubCharSequence
 */
class SubCharSequence: public DelegateCharSequence
	{
	private:
		size_type start;
		size_type end;
	public:

	SubCharSequence(CharSequencePtr delegate,size_type start,size_type end):DelegateCharSequence(delegate),
			start(start),end(end)
		{
		assert(delegate!=NULL);
		}

	virtual ~SubCharSequence()
		{
		}
	

	virtual char at(size_type index) const
		{
		return getDelegate()->at(index + this->start);
		}

	virtual size_type length() const
		{
		return (this->end - this->start);
		}
	};

std::auto_ptr<CharSequence> CharSequence::substring(size_type start,size_type end) const
	{
	return std::auto_ptr<CharSequence>(new SubCharSequence(this,start,end));
	}


/**
 * ReverseCharSequence
 */
class ReverseCharSequence: public DelegateCharSequence
	{
	public:

		ReverseCharSequence(CharSequencePtr delegate):DelegateCharSequence(delegate)
			{
			}

		virtual ~ReverseCharSequence()
			{
			}
	

		virtual char at(size_type index) const
			{
			return getDelegate()->at( (length()-1)-index);
			}

	};

std::auto_ptr<CharSequence> CharSequence::reverse() const
	{
	return std::auto_ptr<CharSequence>(new ReverseCharSequence(this));
	}

/**
 * Transform
 */
class TransformCharSequence: public DelegateCharSequence
	{
	public:

		TransformCharSequence(CharSequencePtr delegate,):DelegateCharSequence(delegate)
			{
			}

		virtual ~TransformCharSequence()
			{
			}

		virtual char transform(char c) const=0;

		virtual char at(size_type index) const
			{
			return this->transform(getDelegate()->at( index));
			}
	};

/**
 * ToUpperCharSequence
 */
class ToUpperCharSequence: public DelegateCharSequence
	{
	public:

		ToUpperCharSequence(CharSequencePtr delegate,):TransformCharSequence(delegate)
			{
			}

		virtual ~ToUpperCharSequence()
			{
			}

		virtual char transform(char c) const
			{
			return std::toupper(c);
			}
	};

/**
 * ToLowerCharSequence
 */
class ToLowerCharSequence: public DelegateCharSequence
	{
	public:
		ToLowerCharSequence(CharSequencePtr delegate,):TransformCharSequence(delegate)
			{
			}

		virtual ~ToLowerCharSequence()
			{
			}

		virtual char transform(char c) const
			{
			return std::tolower(c);
			}
	};


std::auto_ptr<CharSequence> CharSequence::toUpperCase() const
	{
	return std::auto_ptr<CharSequence>(new ToUpperCharSequence());
	}

std::auto_ptr<CharSequence> CharSequence::toLowerCase() const
	{
	return std::auto_ptr<CharSequence>(new ToLowerCharSequence());
	}

typedef class MemoryCharSequence:public CharSequence
		{
		protected:
			char* _sequence;
			size_type _size;
		public:
			MemoryCharSequence(const char* sequence,size_type length):_sequence(sequence),_size(length)
				{

				}

			MemoryCharSequence(const char* sequence):_sequence(sequence),_size(std::strlen(sequence))
				{
				}

			virtual ~MemoryCharSequence()
				{
				//don't free anything
				}

			virtual std::ostream& print(std::ostream& out) const
				{
				out.write(this->_sequence,this->_size);
				return out;
				}

			virtual char at(size_type index) const
				{
				return this->_sequence[index];
				}
			virtual size_type length() const
				{
				return this->_size;
				}
			virtual std::auto_ptr<MemoryCharSequence> clone() const=0;
			virtual std::auto_ptr<CharSequence> CharSequence::substring(size_type start,size_type end)
				{
				return std::auto_ptr<CharSequence>(new MemoryCharSequence(&_sequence[start],end-start));
				}
		}*MemCharSequencePtr;


typedef class MemCharSequence:public MemoryCharSequence
	{
	public:
		MemCharSequence(const char* sequence,size_type length):MemoryCharSequence(sequence,length)
			{
			}

		MemCharSequence(const char* sequence):MemoryCharSequence(sequence)
			{
			}

		virtual ~MemCharSequence()
			{
			delete [] _sequence;
			}

		virtual std::auto_ptr<MemoryCharSequence> clone() const
			{
			char* s= new char[this->length()+1];
			std::memcpy(this->_sequence,s,sizeof(char)*this->length());
			s[this->length()]=O;
			return std::auto_ptr<MemoryCharSequence>(new MemCharSequence(s,this->length()));
			}
	}*MemCharSequencePtr;



typedef class MallocCharSequence:public MemoryCharSequence
		{
		public:
			MallocCharSequence(const char* sequence,size_type length):MemoryCharSequence(sequence,length)
				{
				}

			MallocCharSequence(const char* sequence):MemoryCharSequence(sequence)
				{
				}

			MallocCharSequence(const MallocCharSequence& cp)
				{
				this->_size=cp._size;
				this->_sequence = (char*)std::malloc(sizeof(char)*(cp.length()+1));
				if(this->_sequence==NULL) throw std::bad_alloc("out of memory");
				std::memcpy(this->_sequence,cp._sequence,sizeof(char)*cp.length());
				this->_sequence[cp.length()]=O;
				}

			virtual ~MallocCharSequence()
				{
				std::free(_sequence);
				}

			virtual std::auto_ptr<MemoryCharSequence> clone() const
				{
				return std::auto_ptr<MemoryCharSequence>(new MallocCharSequence(*this));
				}
		}*MemCharSequencePtr;

typedef class StringCharSequence:public CharSequence
	{
	private:
		const std::string& _sequence;
		const std::string::size_type start;
		const std::string::size_type end;
	public:
		StringCharSequence(const std::string& sequence):_sequence(sequence),
			start(0),end(sequence.size())
			{
			}
		
		StringCharSequence(const std::string& sequence,
			const std::string::size_type begin,
			const std::string::size_type end
			):_sequence(sequence),
			start(begin),end(end)
			{
			assert(begin>=0 && begin <=end && end<=sequence.size());
			}
		
		StringCharSequence(const StringCharSequence& cp):_sequence(cp.sequence),
			start(cp.start),end(cp.end)
			{
			}
	
		virtual ~StringCharSequence()
			{
			
			}

		virtual std::ostream& print(std::ostream& out) const
			{
			out << this->_sequence;
			return out;
			}

		virtual char at(size_type index) const
			{
			return this->_sequence[index];
			}

		virtual size_type length() const
			{
			return this->_sequence.size();
			}
	}*MemCharSequencePtr;


}


#endif
