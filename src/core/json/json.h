#ifndef _LINDENB_JSON_H
#define _LINDENB_JSON_H
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "io/escape.h"
namespace lindenb
{
namespace json
	{	
	enum node_type
		{
		nil,
		boolean,
		integer,
		floating,
		string,
		array,
		object
		};
	
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
			virtual std::ostream& write(std::ostream& out) const=0;
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
			virtual std::ostream& write(std::ostream& out) const
				{
				char tt=(char)type();
				out.write((const char*)&tt,sizeof(char));
				out.write((const char*)&value(),sizeof(T));
				return out;
				}
		};
	
	
	class NilNode:public Node
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
			virtual std::ostream& write(std::ostream& out) const
				{
				char tt=(char)type();
				out.write((const char*)&tt,sizeof(char));
				return out;
				}
		};
	
	class BoolNode:public GenericNode<bool>
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
			
		};
	
	
	
	class StringNode:public GenericNode<std::string>
		{
		public:
			StringNode(const std::string& value):GenericNode<std::string>(value) {}
			virtual ~StringNode() {}
			virtual node_type type() const { return string; };
			virtual std::ostream& print(std::ostream& out) const
				{
				out << "\"";
				out << lindenb::escapeC(value());
				out << "\"";
				return out;
				}
			virtual std::ostream& write(std::ostream& out) const
				{
				char tt=(char)type();
				out.write((const char*)&tt,sizeof(char));
				std::string::size_type n=value().size();
				out.write((const char*)&n,sizeof(std::string::size_type));
				out.write(value().c_str(),n);
				return out;
				}
		};
	
	
	
	
	class IntegerNode:public GenericNode<Node::json_integer_type>
		{
		public:
			IntegerNode(Node::json_integer_type value):GenericNode<Node::json_integer_type>(value) {}
			virtual ~IntegerNode() {}
			virtual node_type type() const { return integer; };
		};
	
	
	class DoubleNode:public GenericNode<Node::json_floating_type>
		{
		public:
			DoubleNode(Node::json_floating_type value):GenericNode<Node::json_floating_type>(value) {}
			virtual ~DoubleNode() {}
			virtual node_type type() const { return floating; };
		};
	
	class ArrayNode:public Node
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
			virtual std::ostream& write(std::ostream& out) const
				{
				char tt=(char)type();
				out.write((const char*)&tt,sizeof(char));
				size_type n=size();
				out.write((const char*)&n,sizeof(std::string::size_type));
				for(size_type i=0;i< size();++i)
					{
					vector().at(i)->write(out);
					}
				return out;
				}
		};
	
	class ObjectNode:public Node
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
				out << "[";
				for(std::map<std::string,NodePtr>::const_iterator r=children.begin();
					r!=children.end();
					++r)
					{
					if(!first) out <<",";
					first=false;
					out << "\""<< lindenb::escapeC(r->first)<< "\":";
					r->second->print(out);
					}
				out << "]";
				return out;
				}
			virtual std::ostream& write(std::ostream& out) const
				{
				char tt=(char)type();
				out.write((const char*)&tt,sizeof(char));
				size_type n=size();
				out.write((const char*)&n,sizeof(std::string::size_type));
				for(std::map<std::string,NodePtr>::const_iterator r=children.begin();
					r!=children.end();
					++r)
					{
					std::string::size_type n=r->first.size();
					out.write((const char*)&n,sizeof(std::string::size_type));
					out.write(r->first.c_str(),n);
					r->second->write(out);
					}
				return out;
				}
		};
	
	
	}
	}	
#endif
