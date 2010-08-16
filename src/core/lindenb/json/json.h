#ifndef _LINDENB_JSON_H
#define _LINDENB_JSON_H
#include <iostream>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <climits>
#include <cerrno>
#include <cmath>
#include "lindenb/io/lexer.h"
#include "lindenb/io/binding.h"
#include "lindenb/io/escape.h"
#include "lindenb/util/debug.h"

namespace lindenb
{
namespace json
	{	
	enum node_type
		{
		nil=258,//starting from 258 for lex/flex tool
		boolean=259,
		integer=260,
		floating=261,
		string=262,
		array=263,
		object=265
		};
	
	//forward declaration
	class NilNode;
	class BoolNode;
	class StringNode;
	class IntegerNode;
	class DoubleNode;
	class ArrayNode;
	class ObjectNode;

	/**
	 * An Abstract JSON node
	 *
	 */
	typedef class Node
		{
		protected:
			Node() {}
			
		public:
			typedef long json_integer_type;
			typedef double json_floating_type;
			
			virtual ~Node() {}
			virtual node_type type() const=0;
			virtual std::ostream& print(std::ostream& out) const=0;
			bool isNil() const { return type()==nil;}
			bool isBool() const { return type()==boolean;}
			bool isFloating() const { return type()==floating;}
			bool isDouble() const { return isFloating();}
			bool isInteger() const { return type()==integer;}
			bool isNumber() const { return isInteger() || isFloating();}
			bool isString() const { return type()==string;}
			bool isArray() const { return type()==array;}
			bool isObject() const { return type()==object;}
			virtual Node* clone() const=0;
			virtual bool equals(const Node* object) const=0;
		protected:
			void cktype(node_type t) const
				{
				if(t!=type())
					{
					std::ostringstream os;
					os << "JSON node is type=" << type()
				           << "but was casted as " << t;
				        throw new std::runtime_error(os.str());
					}
				}
		public:
			virtual NilNode* asNil() { cktype(nil); return (NilNode*)this;};
			virtual BoolNode* asBool() { cktype(boolean); return (BoolNode*)this;};
			virtual StringNode* asString() { cktype(string); return (StringNode*)this;};
			virtual IntegerNode* asInteger() { cktype(integer); return (IntegerNode*)this;};
			virtual DoubleNode* asDouble() { cktype(floating); return (DoubleNode*)this;};
			virtual ArrayNode* asArray() { cktype(array); return (ArrayNode*)this;};
			virtual ObjectNode* asObject() { cktype(object); return (ObjectNode*)this;};
			
			virtual const NilNode* asNil() const { cktype(nil); return (const NilNode*)this;};
			virtual const BoolNode* asBool() const { cktype(boolean); return (const BoolNode*)this;};
			virtual const StringNode* asString() const { cktype(string); return (const StringNode*)this;};
			virtual const IntegerNode* asInteger() const { cktype(integer); return (const IntegerNode*)this;};
			virtual const DoubleNode* asDouble() const { cktype(floating); return (const DoubleNode*)this;};
			virtual const ArrayNode* asArray() const { cktype(array); return (const ArrayNode*)this;};
			virtual const ObjectNode* asObject() const { cktype(object); return (const ObjectNode*)this;};
			virtual const Node* find(const char* path) const
				{
				return NULL;
				}
			
		friend std::ostream& operator<< (std::ostream& o,Node  const& object);
		}*NodePtr;
	
	std::ostream& operator<< (std::ostream& o,Node  const& object)
	 	{
	 	return object.print(o);
	 	}
	
	/**
	 *
	 *  LeafNode
	 *
	 */
	class LeafNode:public Node
		{
		protected:
			LeafNode() {}
		public:
			virtual ~LeafNode() {}
			virtual node_type type() const=0;
			virtual std::ostream& print(std::ostream& out) const=0;
			virtual bool equals(const Node* object) const=0;
			virtual Node* clone() const=0;
		};
	
	/**
	 *
	 * NilNode
	 *
	 */
	typedef class NilNode:public LeafNode
		{
		public:
			NilNode() {}
			virtual ~NilNode() {}
			virtual node_type type() const { return nil;};
			virtual std::ostream& print(std::ostream& out) const
				{
				out << "null";
				return out;
				}
			virtual Node* clone() const
				{
				return new NilNode();
				}
			virtual bool equals(const Node* object) const
				{
				if(object==this) return true;
				if(object==NULL  || object->type()!=this->type()) return false;
				return true;
				}
		}*NilNodePtr;
	
	/**
	 *
	 * BoolNode
	 *
	 */
	typedef class BoolNode:public LeafNode
		{
		private:
			bool _value;
		public:
			BoolNode(bool value):LeafNode(value) {}
			virtual ~BoolNode() {}
			virtual node_type type() const { return boolean; };
			virtual bool value() const { return _value;}
			virtual std::ostream& print(std::ostream& out) const
				{
				out << (value()?"true":"false");
				return out;
				}
			virtual Node* clone() const
				{
				return new BoolNode(value());
				}
			virtual bool equals(const Node* object) const
				{
				if(object==this) return true;
				if(object==NULL  || object->type()!=this->type()) return false;
				return value()==((BoolNode*)object)->value();
				}
		}*BoolNodePtr;
	
	
	/**
	 *
	 *  StringNode
	 *
	 */
	typedef class StringNode:public LeafNode
		{
		private:
			std::string _value;
		public:
			StringNode(const std::string& value):StringNode(value) {}
			StringNode(const char* value):StringNode(value) {}
			const std::string& value() const { return _value;}
			virtual ~StringNode() {}
			virtual node_type type() const { return string; };
			virtual std::ostream& print(std::ostream& out) const
				{
				out << "\"";
				out << lindenb::io::escapeC(value());
				out << "\"";
				return out;
				}
			virtual Node* clone() const
				{
				return new StringNode(value());
				}
			virtual bool equals(const Node* object) const
				{
				if(object==this) return true;
				if(object==NULL  || object->type()!=this->type()) return false;
				return value().compare(((StringNode*)object)->value())==0;
				}
			char at(std::string::size_type i) const
				{
				return _value.at(i);
				}
			char operator[](std::string::size_type i) const
				{
				return at(i);
				}
			std::string::size_type size() const
				{
				return _value.size();
				}
		}*StringNodePtr;
	
	
	
	/**
	 *
	 * IntegerNode
	 *
	 */
	typedef class IntegerNode:public LeafNode
		{
		private:
			Node::json_integer_type _value;
		public:
			IntegerNode(Node::json_integer_type value):_value(value) {}
			virtual ~IntegerNode() {}
			virtual node_type type() const { return integer; };
			Node::json_integer_type value() const { return _value;}
			virtual Node* clone() const
				{
				return new IntegerNode(value());
				}
			virtual bool equals(const Node* object) const
				{
				if(object==this) return true;
				if(object==NULL  || object->type()!=this->type()) return false;
				return value()==((IntegerNode*)object)->value();
				}
		}*IntegerNodePtr;
	
	/**
	 *
	 * DoubleNode
	 *
	 */
	typedef class DoubleNode:public LeafNode
		{
		private:
			Node::json_floating_type _value;
		public:
			DoubleNode(Node::json_floating_type value):_value(value) {}
			virtual ~DoubleNode() {}
			Node::json_floating_type value() const { return _value;}
			virtual node_type type() const { return floating; };
			virtual Node* clone() const
				{
				return new DoubleNode(value());
				}
			virtual bool equals(const Node* object) const
				{
				if(object==this) return true;
				if(object==NULL  || object->type()!=this->type()) return false;
				return value()==((DoubleNode*)object)->value();
				}
		}*DoubleNodePtr;
	
	
	/**
	 *
	 *  ComplexNode
	 *
	 */
	class ComplexNode:public Node
		{
		protected:
			ComplexNode() {}
		public:
			virtual ~ComplexNode() {}
			virtual node_type type() const=0;
			virtual std::ostream& print(std::ostream& out) const=0;
			virtual bool equals(const Node* object) const=0;
			virtual Node* clone() const=0;
		};	
	
	/**
	 *
	 * ArrayNode
	 *
	 */
	typedef class ArrayNode:public ComplexNode
		{
		private:
			std::vector<NodePtr> children;
		public:
			typedef std::vector<NodePtr>::size_type size_type;
			ArrayNode():ComplexNode() {}
			virtual ~ArrayNode()
				{
				while(!children.empty())
					{
					NodePtr c=children.back();
					children.pop_back();
					if(c!=NULL) delete c;
					}
				}
			
			virtual node_type type() const { return array; };
			
			
			virtual NodePtr at(size_type i)
				{
				return children.at(i);
				}
			virtual NodePtr operator[](size_type i)
				{
				return at(i);
				}
			
			virtual const NodePtr at(size_type i) const
				{
				return children.at(i);
				}
			
			virtual const NodePtr operator[](size_type i) const
				{
				return at(i);
				}
			
			virtual size_type size() const { return vector().size(); };
			
			virtual bool empty() const
				{
				return children.empty()==0;
				}
				
			virtual std::ostream& print(std::ostream& out) const
				{
				out << "[";
				for(size_type i=0;i< size();++i)
					{
					if(i!=0) out << ",";
					at(i)->print(out);
					}
				out << "]";
				return out;
				}
			
			virtual Node* clone() const
				{
				ArrayNode* node= new ArrayNode();
				node->children.reserve(size());
				for(size_type i=0;i< size();++i)
					{
					node->children.push_back(
						this->vector().at(i)->clone()
						);
					}
				return node;
				}
			virtual bool equals(const Node* object) const
				{
				if(object==this) return true;
				if(object==NULL  || object->type()!=this->type()) return false;
				ArrayNode* other=((ArrayNode*)object);
				if(size()!=other->size()) return false;
				for(size_type i=0;i< size();++i)
					{
					if( vector().at(i)->equals( other->vector().at(i) )==false)
						{
						return false;
						}
					}
				return true;
				}
			virtual const Node* find(const char* path) const
				{
				char* p=(char*)path;
				if(*p!='[') return NULL;
				++p;
				char* p2=p;
				while(*p2!=']' && *p2!=0) ++p2;
				if(*p2!=']') return NULL;
				
				std::string num(p,p2-p);
				errno=0;
				unsigned long int index=std::strtoul(num.c_str(),NULL,10);
				
				if(index>=size() || errno!=0) return NULL;
				const NodePtr c=at(index);
				if(*(p2+1)==0) return c; 
				return c->find(p2+1);
				}
		}*ArrayNodePtr;
	
	/**
	 *
	 * ObjectNode
	 *
	 */
	typedef class ObjectNode:public ComplexNode
		{
		private:
			std::map<std::string,NodePtr> children;
		public:
			typedef std::map<std::string,NodePtr>::const_iterator const_iterator;
			typedef std::map<std::string,NodePtr>::iterator iterator;
			typedef std::map<std::string,NodePtr>::size_type size_type;
			ObjectNode():ComplexNode() {}
			virtual ~ObjectNode()
				{
				for(std::map<std::string,NodePtr>::iterator r=children.begin();
					r!=children.end();
					++r)
					{
					delete r->second;
					}
				children.clear();
				}
			std::set<std::string> keys() const
				{
				std::set<std::string> _set;
				for(std::map<std::string,NodePtr>::const_iterator r=children.begin();
					r!=children.end();
					++r)
					{
					_set.insert(r->first);
					}
				return _set;
				}
			
			
			virtual node_type type() const { return object; };
			
			std::map<std::string,NodePtr>& map()
				{
				return children;
				}
			
			
			virtual bool containsKey(const char* s) const
				{
				return map().find(s)!=map().end();
				}
			
			virtual const NodePtr get(const char* key) const
				{
				std::map<std::string,NodePtr>::const_iterator r=map().find(key);
				if(r==map().end()) return NULL;
				return r->second;
				}
			
			
			const std::map<std::string,NodePtr>& map() const
				{
				return children;
				} 
			virtual size_type size() const { return map().size(); };
			
			virtual std::ostream& print(std::ostream& out) const
				{
				bool first=true;
				out << "{";
				for(std::map<std::string,NodePtr>::const_iterator r=children.begin();
					r!=children.end();
					++r)
					{
					if(!first) out <<",";
					first=false;
					out << "\""<< lindenb::io::escapeC(r->first)<< "\":";
					r->second->print(out);
					}
				out << "}";
				return out;
				}
				
			virtual iterator begin()
				{
				return map().begin();
				}
			virtual const_iterator begin() const
				{
				return map().begin();
				}
			
			virtual iterator end()
				{
				return map().end();
				}
			
			virtual const_iterator end() const
				{
				return map().end();
				}	
			
			virtual Node* clone() const
				{
				ObjectNode* node= new ObjectNode();
				for(std::map<std::string,NodePtr>::const_iterator r=children.begin();
					r!=children.end();
					++r)
					{
					node->children.insert(std::pair<std::string,NodePtr>(r->first,r->second->clone()));
					}
				return node;
				}
			virtual bool equals(const Node* object) const
				{
				if(object==this) return true;
				if(object==NULL  || object->type()!=this->type()) return false;
				ObjectNode* other=((ObjectNode*)object);
				if(size()!=other->size()) return false;
				std::map<std::string,NodePtr>::const_iterator r=children.begin();
				std::map<std::string,NodePtr>::const_iterator r_end=children.end();
				std::map<std::string,NodePtr>::const_iterator r2=other->children.begin();
				while(r!=r_end)
					{
					if( r->first.compare(r2->first)!=0) return false;
					if( r->second->equals(r2->second)==false) return false;
					++r;
					++r2;
					}
				return true;
				}
		}*ObjectNodePtr;

/** A parser for JSON */
class	Parser: public lindenb::io::Lexer
	{
	protected:
	
		void _int(std::string& content)
			{
			int c=skipWhitespaces();
			if(!std::isdigit(c))
				{
				std::ostringstream os;
				os << "expected a digit context: " << toString();
				throw std::runtime_error(os.str());
				}
			consumme(1);
			content+=(char)c;
			while((c=get())!=EOF)
				{
				if(!std::isdigit(c)) break;
				consumme(1);
				content+=(char)c;
				}
			}
		
		void _string(std::string& content)
			{
			skipWhitespaces();
			int openChar=get();
			if(!(openChar=='\'' || openChar=='\"'))
				{
				std::ostringstream os;
				os << "expected a simple or a double quote " << toString();
				throw std::runtime_error(os.str());
				}
			consumme(1);
			
			for(;;)
				{
				int c=get();
				if(c==EOF)
					{
					std::ostringstream os;
					os << "unclosed string " << toString();
					throw std::runtime_error(os.str());
					}
				
				consumme(1);
				if(c==openChar) break;
				if(c=='\\')
					{
					c=get();
					if(c==EOF)
						{
						std::ostringstream os;
						os << "unclosed string " << toString();
						throw std::runtime_error(os.str());
						}
					consumme(1);
					switch(c)
						{
						case '\\': content+='\\'; break;
						case '\"': content+='\"'; break;
						case '\'': content+='\''; break;
						case 'n': content+='\n'; break;
						case 'r': content+='\r'; break;
						case 't': content+='\t'; break;
						default: content+=(char)c;
						}
					continue;
					}
				 content+=(char)c;
				}
			}
	public:
		Parser(std::istream& in):Lexer(in) {}
		
		NilNodePtr parseNull()
			{
			if(this->skipWhitespaces()!='n') throw std::runtime_error("expected null");
			if(!inAvail("null"))  throw std::runtime_error("expected null");
			consumme(4);
			return new NilNode();
			}
			
		
		BoolNodePtr parseBool()
			{
			skipWhitespaces();
			if(inAvail("true"))
				{
				consumme(4);
				return new BoolNode(true);
				}
			if(inAvail("false"))
				{
				consumme(5);
				return new BoolNode(false);
				}
			throw std::runtime_error("expected null");
			}
		
		StringNodePtr parseString()
			{
			std::string str;
			_string(str);
			return new StringNode(str);
			}
			
		ArrayNodePtr parseArray()
			{
			ArrayNodePtr array=new ArrayNode();
			if(this->skipWhitespaces()!='[')
				throw std::runtime_error("expected a [");
			consumme(1);
			while(true)
				{
				int c= get();
				if(c==EOF)
					{
					std::ostringstream os;
					os << "unclosed array: " << toString();
					throw std::runtime_error(os.str());
					}
				if(c==']')
					{
					consumme(1);
					break;
					}
				
				try
					{
					NodePtr item=parse();
					array->vector().push_back(item);
					}
				catch(std::exception& error)
					{
					delete array;
					throw error;
					}
				c= get();
				if(c==']')
					{
					consumme(1);
					break;
					}
				if(c!=',')
					{
					std::ostringstream os;
					os << "expected a comma " << toString();
					throw std::runtime_error(os.str());
					}
				//consumme comma
				consumme(1);
				}
			return array;
			}
		
		ObjectNodePtr parseObject()
			{
			
			ObjectNodePtr object=new ObjectNode();
			if(this->skipWhitespaces()!='{')
				{
				std::ostringstream os;
				os << "expected a '{' context: " << toString();
				throw std::runtime_error(os.str());
				}
			consumme(1);
			
			while(true)
				{
				
				int c= get();
				if(c==EOF)
					{
					std::ostringstream os;
					os << "unclosed object: " << toString();
					throw std::runtime_error(os.str());
					}
				if(c=='}')
					{
					consumme(1);
					break;
					}
				
				try
					{
					std::string key;
					_string(key);
					
					if(object->map().find(key)!=object->map().end())
						{
						std::ostringstream os;
						os << "key "<< key << " defined twice:"  << toString();
						throw std::runtime_error(os.str());
						}
					
					if(skipWhitespaces()!=':')
						{
						std::ostringstream os;
						os << "exepected a ':' after "<< key << " :"  << toString();
						throw std::runtime_error(os.str());
						}
					consumme(1);//consumme ':'
					NodePtr item=parse();
					object->map().insert(std::pair<std::string,NodePtr>(key,item));
					}
				catch(std::exception& error)
					{
					delete object;
					throw error;
					}
				c= get();
				if(c=='}')
					{
					consumme(1);
					break;
					}
				if(c!=',')
					{
					std::ostringstream os;
					os << "expected a comma " << toString();
					throw std::runtime_error(os.str());
					}
				//consumme comma
				consumme(1);
				}
			return object;
			}		
		
		
		
		NodePtr parseNumber()
			{
			std::string content;
			int c=this->skipWhitespaces();
			
			if(c=='+' || c=='-')
				{
				content+=(c);
				consumme(1);
				}
			
			
			bool seen_E=false;
			bool seen_dot =false;
			while((c=get())!=EOF)
				{
				if(std::isdigit(c))
					{
					consumme(1);
					content+=(char)c;
					}
				else if(!seen_dot && c=='.')
					{
					consumme(1);
					//is_floating=true;
					seen_dot=true;
					content+=(char)c;
					}
				else if(!seen_E && (c=='e' || c=='E'))
					{
					consumme(1);
					//is_floating=true;
					seen_E=true;
					content+=(char)c;
					}
				else
					{
					break;
					}
				}
			char *p2=NULL;
			Node::json_integer_type as_long=std::strtol(content.c_str(),&p2,10);
			if(!(as_long==LONG_MAX || as_long==LONG_MIN || *p2!=0 ))
				{
				return  new IntegerNode(as_long);
				}
			
			Node::json_floating_type as_float=std::strtod(content.c_str(),&p2);
			if(as_float==HUGE_VAL || as_float==-HUGE_VAL || *p2!=0 )
				{
				std::ostringstream os;
				os << "bad number: " << content << toString();
				throw std::runtime_error(os.str());
				}
			
			return  new DoubleNode(as_float);
			}
		
		IntegerNodePtr parseInteger()
			{
			NodePtr node= parseNumber();
			if(node->type()==integer) return (IntegerNodePtr)node;
			
			std::ostringstream os;
			os << "cannot cast  DoubleNodePtr to integer " << (*node) << toString();
			throw std::runtime_error(os.str());
				
			}
			
		DoubleNodePtr parseDouble()
			{
			NodePtr node= parseNumber();
			if(node->type()==floating) return (DoubleNodePtr)node;
			DoubleNodePtr node2=new DoubleNode(((IntegerNodePtr)node)->value());
			delete node;
			return node2;
			}
		
		void eof()
			{
			int i=this->skipWhitespaces();
			if(i==EOF) return;
			std::ostringstream os;
			os << "expected EOF " << toString();
			throw std::runtime_error(os.str());
			}
		
		NodePtr parse()
			{
			int i=this->skipWhitespaces();
			switch(i)
				{
				case EOF:return NULL;
				case 'n': return parseNull();
				case 't': case 'f': return parseBool();
				case '[': return parseArray();
				case '{': return parseObject();
				case '\'': case '\"': return parseString();
				case '+':
				case '-':
				case '.':
				case 'E':case 'e':
				case '0':case '1':case '2':case '3':
				case '4':case '5':case '6':case '7':
				case '8':case '9': return parseNumber();
				}
			std::ostringstream os;
			os << "Bad character " << toString();
			throw std::runtime_error(os.str());
			}
	};

/**
 *
 * JSONBinding 
 * serialization of a JSON structure
 */
class JSONBinding: public lindenb::io::TupleBinding<Node>
	{
	public:
		/** JSONBinding */
		JSONBinding()
			{
			}
		/** ~JSONBinding */
		virtual ~JSONBinding()
			{
			}
		/** readObject */
		virtual  std::auto_ptr<Node> readObject(std::istream& in)
			{
			unsigned char t;
			read(in,(char*)&t,sizeof(unsigned char));
			
			switch((node_type)t)
				{
				case nil:return std::auto_ptr<Node>(new NilNode()); break;//nothing todo
				case boolean: return std::auto_ptr<Node>(new BoolNode(readBool(in)));break;
				case integer: return  std::auto_ptr<Node>(new IntegerNode(readLong(in)));break;
				case floating: return std::auto_ptr<Node>(new DoubleNode(readDouble(in)));break;
				case string:
					{
					std::auto_ptr<std::string> s=readString(in);
					std::auto_ptr<Node> s2(new StringNode(*(s.get())));
					return s2;
					break;
					}
				case array:
					{
					ArrayNodePtr array=new ArrayNode();
					ArrayNode::size_type n;
					read(in,(char*)&n,sizeof(ArrayNode::size_type));
					for(ArrayNode::size_type i=0;i< n;++i)
						{
						array->vector().push_back(readObject(in).release());
						}
					return std::auto_ptr<Node>(array);
					break;
					}
				case object:
					{
					ObjectNodePtr map= new ObjectNode();
					std::map<std::string,NodePtr>::size_type n;
					read(in,(char*)&n,sizeof(std::map<std::string,NodePtr>::size_type));
					
					for(std::map<std::string,NodePtr>::size_type i=0;i< n;++i)
						{
						std::auto_ptr<std::string> key=readString(in);
						NodePtr value=readObject(in).release();
						map->map().insert(std::pair<std::string,NodePtr>(*(key.get()),value));
						}
					return std::auto_ptr<Node>(map);
					break;
					}
				default:throw std::runtime_error("bad type");
				}
			
			throw std::runtime_error("should never happen");
			return  std::auto_ptr<Node>();
			}
		/** abstract; write object to output stream */
		virtual void writeObject(std::ostream& out,const Node* myobject)
			{
			unsigned char t=(unsigned char)myobject->type();
			write(out,(const char*)&t,sizeof(unsigned char));
			switch(myobject->type())
				{
				case nil:break;//nothing todo
				case boolean: writeBool(out, ((const BoolNodePtr)myobject)->value());break;
				case integer: writeLong(out, ((const IntegerNodePtr)myobject)->value());break;
				case floating: writeDouble(out, ((const DoubleNodePtr)myobject)->value());break;
				case string: writeString(out, (&((const StringNodePtr)myobject)->value()));break;
				case array:
					{
					const ArrayNodePtr array=(const ArrayNodePtr)myobject;
					ArrayNode::size_type n=array->size();
					write(out,(const char*)&n,sizeof(std::vector<NodePtr>::size_type));
					for(std::vector<NodePtr>::size_type i=0;i< array->size();++i)
						{
						writeObject(out,array->vector().at(i));
						}
					break;
					}
				case object:
					{
					const ObjectNodePtr map=((const ObjectNodePtr)myobject);
					ObjectNode::size_type n=map->map().size();
					write(out,(const char*)&n,sizeof(ObjectNode::size_type));
					for(ObjectNode::const_iterator r=map->begin();
						r!=map->end();
						++r)
						{
						writeString(out,&(r->first));
						writeObject(out,r->second);
						}
					break;
					}
				default:throw std::runtime_error("bad type");
				}
			}
		};

/**
 *
 * NodeComparator
 *
 */
class NodeComparator
	{
	public:
		NodeComparator()
			{
			}
		virtual ~NodeComparator()
			{
			}
		
		virtual int compare(const NodePtr a,const NodePtr b)
			{
			if(a==b) return true;
			switch(a->type())
				{
				case nil:
					{
					if(b->type()==nil) return 0;
					return a->type()-b->type();
					break;
					}
				case boolean:
					{
					const NilNode* ptrA = a->asNil();
					if(b->type()==boolean)
						{
						const NilNode* ptrB = b->asNil();
						return ((int)(ptrA->value()))-((int)(ptrB->value()));
						}
					return a->type()-b->type();
					break;
					}
				case integer:
					{
					const IntegerNode* ptrA = a->asInteger();
					switch(b->type())
						{
						case integer:
							{
							const IntegerNode* ptrB = b->asInteger();
							return (int)(ptrA->value()- ptrB->value());
							break;
							}
						case floating:
							{
							const DoubleNode* ptrB = b->asDouble();
							return (int)((double)(ptrA->value())- ptrB->value());
							break;
							}
						default:break;
						}
					return a->type()-b->type();
					break;
					}
				case floating:
					{
					const DoubleNode* ptrA = a->asDouble();
					switch(b->type())
						{
						case integer:
							{
							const IntegerNode* ptrB = b->asInteger();
							return (int)(ptrA->value()- (double)ptrB->value());
							break;
							}
						case floating:
							{
							const DoubleNode* ptrB = b->asDouble();
							return (int)(ptrA->value()- ptrB->value());
							break;
							}
						default:break;
						}
					return a->type()-b->type();
					break;
					}
				case string:
					{
					const StringNode* ptrA = a->asString();
					if(b->type()==string)
						{
						const StringNode* ptrB = b->asString();
						return ptrA->compare(ptrB);
						}
					return a->type()-b->type();
					break;
					}
				case array:
					{
					const ArrayNode* ptrA = a->asArray();
					if(b->type()==array)
						{
						const ArrayNode* ptrB = b->asArray();
						if(ptrA->size() != ptrB->size())
							{
							return ((long)ptrA->size()) - ((long)ptrB->size());
							}
						for(ArrayNode::size_type i=0;
							i < ptrA->size();
							++i)
							{
							int n= compare(ptrA->at(i),ptrB->at(i));
							if(n!=0) return n;
							}
						return 0;
						}
					return a->type()-b->type();
					break;
					}
				case object:
					{
					const ObjectNode* ptrA = a->asObject();
					
					if(b->type()==object)
						{
						const ObjectNode* ptrB = b->asObject();
						if(ptrA->size() != ptrB->size())
							{
							return ((long)ptrA->size()) - ((long)ptrB->size());
							}
						ObjectNode::const_iterator rA=ptrA->begin();
						ObjectNode::const_iterator rB=ptrB->begin();
						while(rA!=ptrA->end())
							{
							
							++rB;
							++rA;
							}
						}
					return a->type()-b->type();
					break;
					}
				}
			}
		
		bool operator()(const NodePtr a,const NodePtr b)
			{
			return lt(a,b);
			}
			
	};


}}//namespaces	
#endif
