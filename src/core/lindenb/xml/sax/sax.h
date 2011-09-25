/*
 * sax.h
 *
 *  Created on: Sep 25, 2011
 *      Author: lindenb
 */

#ifndef SAX_H_
#define SAX_H_
#include <iostream>
namespace lindenb { namespace xml { namespace sax {

class Attributes;

class Attribute
    {
    public:
	typedef unsigned char xml_char_t;
    private:
	const Attributes* atts;
	const int index;
    public:
	Attribute(const Attributes* ,int index);
	const xml_char_t* localname() const;
	const xml_char_t* prefix() const;
	const xml_char_t* namespaceuri() const;
	const xml_char_t* value() const;
	bool is_default() const;
    };


class Attributes
    {
    public:
	typedef Attribute::xml_char_t xml_char_t;
	typedef std::size_t size_type;
    private:
	const xml_char_t** data;
	int nb_attributes;
	int nb_defaulted;
    public:
	Attributes(const xml_char_t** data,int nb_attributes,int nb_defaulted);
	size_type size() const;
	const xml_char_t* localname(size_type index) const;
	const xml_char_t* prefix(size_type index) const;
	const xml_char_t* namespaceuri(size_type index) const;
	const xml_char_t* value(size_type index) const;
	bool is_default(size_type index) const;
	Attribute at(size_type index) const;
	Attribute operator[](size_type index) const;
    };

class Namespaces;

class Namespace
    {
    public:
	typedef Attribute::xml_char_t xml_char_t;
    private:
	const Namespaces* ns;
	const int index;
    public:
	Namespace(const Namespaces* ,int index);
	const xml_char_t* prefix() const;
	const xml_char_t* namespaceuri() const;
    };

class Namespaces
    {
    public:
	typedef Namespace::xml_char_t xml_char_t;
	typedef std::size_t size_type;
    private:
	const xml_char_t** data;
	int nb_namespaces;
    public:
	Namespaces(const xml_char_t** data,int nb_namespaces);
	size_type size() const;
	const xml_char_t* prefix(size_type index) const;
	const xml_char_t* namespaceuri(size_type index) const;
	Namespace at(size_type index) const;
	Namespace operator[](size_type index) const;
    };


class DocumentHandler
    {
	private:
	    void* handler;
	public:
	    typedef Attributes::xml_char_t xml_char_t;
	    DocumentHandler();
	    virtual ~DocumentHandler();

	    virtual void endDocument();

	    virtual void startElement(
			const xml_char_t * localname,
			const xml_char_t * prefix,
			const xml_char_t * URI,
			int nb_namespaces,
			const xml_char_t ** namespaces,
			const Attributes& attributes
			);
	    virtual void endElement(
		 const xml_char_t * localname,
		 const xml_char_t * prefix,
		 const xml_char_t * URI);
	    virtual void characters(const xml_char_t* s,int len);
	    virtual void notationDecl(const xml_char_t * name, const xml_char_t * publicId,  const xmlChar * systemId);
	    int parseFile(const char* filename);
	    int parse(const char * buffer,int size);
	    int parse(std::istream& in);
    };

} } }

#endif /* SAX_H_ */
