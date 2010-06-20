#ifndef _LINDENB_JSON_H
#define _LINDENB_JSON_H
#include <iostream>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <climits>
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
		nil=1,
		boolean=2,
		integer=3,
		floating=4,
		string=5,
		array=6,
		object=7
		};
	

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
		friend std::ostream& operator<< (std::ostream& o,Node  const& object);
		}*NodePtr;
	
	std::ostream& operator<< (std::ostream& o,Node  const& object)
	 	{
	 	return object.print(o);
	 	}
	
	template<typename T>
	class GenericNode:public Node
		{
		protected:
			T _value;
		public:
			
	
			GenericNode(const T& var):_value(var) {}
			virtual ~GenericNode() {}
			virtual T& value() { return _value;}
			virtual const T& value() const { return _value;}
			virtual node_type type() const=0;
			virtual std::ostream& print(std::ostream& out) const
				{
				out << value();
				return out;
				}
			virtual bool equals(const Node* object) const=0;
			virtual Node* clone() const=0;
		};
	
	
	typedef class NilNode:public Node
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
	
	typedef class BoolNode:public GenericNode<bool>
		{
		public:
			BoolNode(const bool& value):GenericNode<bool>(value) {}
			virtual ~BoolNode() {}
			virtual node_type type() const { return boolean; };
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
	
	
	
	typedef class StringNode:public GenericNode<std::string>
		{
		public:
			StringNode(const std::string& value):GenericNode<std::string>(value) {}
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
		}*StringNodePtr;
	
	
	
	
	typedef class IntegerNode:public GenericNode<Node::json_integer_type>
		{
		public:
			IntegerNode(Node::json_integer_type value):GenericNode<Node::json_integer_type>(value) {}
			virtual ~IntegerNode() {}
			virtual node_type type() const { return integer; };
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
	
	
	typedef class DoubleNode:public GenericNode<Node::json_floating_type>
		{
		public:
			DoubleNode(Node::json_floating_type value):GenericNode<Node::json_floating_type>(value) {}
			virtual ~DoubleNode() {}
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
	
	typedef class ArrayNode:public Node
		{
		private:
			std::vector<NodePtr> children;
		public:
			typedef std::vector<NodePtr>::size_type size_type;
			ArrayNode():Node() {}
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
			
			std::vector<NodePtr>& vector()
				{
				return children;
				}
			
			const std::vector<NodePtr>& vector() const
				{
				return children;
				} 
			virtual size_type size() const { return vector().size(); };
			
			virtual std::ostream& print(std::ostream& out) const
				{
				out << "[";
				for(size_type i=0;i< size();++i)
					{
					if(i!=0) out << ",";
					vector().at(i)->print(out);
					}
				out << "]";
				return out;
				}
			
			virtual Node* clone() const
				{
				ArrayNode* node= new ArrayNode();
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
		}*ArrayNodePtr;
	
	typedef class ObjectNode:public Node
		{
		private:
			std::map<std::string,NodePtr> children;
		public:
			typedef std::map<std::string,NodePtr>::size_type size_type;
			ObjectNode():Node() {}
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
			
			virtual node_type type() const { return object; };
			
			std::map<std::string,NodePtr>& map()
				{
				return children;
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
				case string: return std::auto_ptr<Node>(new StringNode(*readString(in))); break;	
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
						std::string* key=readString(in);
						NodePtr value=readObject(in).release();
						
						map->map().insert(std::pair<std::string,NodePtr>(*key,value));
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
					std::map<std::string,NodePtr>::size_type n=map->map().size();
					write(out,(const char*)&n,sizeof(std::map<std::string,NodePtr>::size_type));
					for(std::map<std::string,NodePtr>::const_iterator r=map->map().begin();
						r!=map->map().end();
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

	}}//namespaces	
#endif
