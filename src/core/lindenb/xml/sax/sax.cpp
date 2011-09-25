/*
 * sax.cpp
 *
 *  Created on: Sep 25, 2011
 *      Author: lindenb
 */
#include <cstring>
#include <cstdlib>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "lindenb/xml/sax/sax.h"
#include "lindenb/xstdlib/xstdlib.h"
#include "lindenb/lang/throw.h"

#define LOCALNS lindenb::xml::sax
#define SAX_HANDLER   ((xmlSAXHandlerPtr)(this->handler))
#define CAST_CTX static_cast<LOCALNS::DocumentHandler*>(ctx)
#define CAST_CHAR

static void _notationDeclSAXFunc(
	 void * ctx,
	 const xmlChar * name,
	 const xmlChar * publicId,
	 const xmlChar * systemId)
    {
    CAST_CTX->notationDecl(name, publicId,systemId);
    }



static void _startElementNsSAX2Func(
	void * ctx,
	const xmlChar * localname,
	const xmlChar * prefix,
	const xmlChar * URI,
	int nb_namespaces,
	const xmlChar ** namespaces,
	int nb_attributes,
	int nb_defaulted,
	const xmlChar ** attributes)
    {
    const LOCALNS::Attributes atts(attributes,nb_attributes,nb_defaulted);
    CAST_CTX->startElement(
	    localname,
	    prefix,
	    URI,
	    nb_namespaces,
	    namespaces,
	    atts
	    );
    }
static void _endDocumentSAXFunc(void * ctx)
    {
    CAST_CTX->endDocument();
    }

static void _endElementNsSAX2Func(void * ctx,
     const xmlChar * localname,
     const xmlChar * prefix,
     const xmlChar * URI)
    {
    CAST_CTX->endElement(localname,prefix,URI);
    }

static void _handleCharacters(void * ctx, const xmlChar * ch, int len)
    {
    CAST_CTX->characters(ch,len);
    }


LOCALNS::Attribute::Attribute(const Attributes* atts,int index):atts(atts),index(index)
    {
    }

const xmlChar* LOCALNS::Attribute::localname() const
    {
    return atts->localname((LOCALNS::Attributes::size_type)index);
    }

const xmlChar* LOCALNS::Attribute::prefix() const
    {
    return atts->prefix((LOCALNS::Attributes::size_type)index);
    }

const xmlChar* LOCALNS::Attribute::namespaceuri() const
    {
    return atts->namespaceuri((LOCALNS::Attributes::size_type)index);
    }

const xmlChar* LOCALNS::Attribute::value() const
    {
    return atts->value((LOCALNS::Attributes::size_type)index);
    }

bool LOCALNS::Attribute::is_default() const
    {
    return atts->is_default((LOCALNS::Attributes::size_type)index);
    }


LOCALNS::Attributes::Attributes(
	const xml_char_t** data,
	int nb_attributes,
	int nb_defaulted):
	    data(data),
	    nb_attributes(nb_attributes),
	    nb_defaulted(nb_defaulted)
    {
    }
LOCALNS::Attributes::size_type
LOCALNS::Attributes::size() const
    {
    return (size_type)nb_attributes;
    }

#define CHECK_INDEX
//if(index>=size()) THROW("index out of bound 0<="<< index<< "<"<< size() )
const xmlChar* LOCALNS::Attributes::localname(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return data[5*index+0];
    }
const xmlChar* LOCALNS::Attributes::prefix(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return data[5*index+1];
    }
const xmlChar* LOCALNS::Attributes::namespaceuri(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return data[5*index+2];
    }
const xmlChar* LOCALNS::Attributes::value(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return data[5*index+3];
    }

bool LOCALNS::Attributes::is_default(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return index>=  (size()-(size_type)this->nb_defaulted);
    }

LOCALNS::Attribute
LOCALNS::Attributes::at(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return Attribute(this,index);
    }

LOCALNS::Attribute
LOCALNS::Attributes::operator[](LOCALNS::Attributes::size_type index) const
    {
    return at(index);
    }
/*************************************************************************/
LOCALNS::Namespace::Namespace(const Namespaces* ns,int index):ns(ns),index(index)
    {
    }

const xmlChar* LOCALNS::Namespace::prefix() const
    {
    return ns->prefix((LOCALNS::Attributes::size_type)index);
    }

const xmlChar* LOCALNS::Namespace::namespaceuri() const
    {
    return ns->namespaceuri((LOCALNS::Attributes::size_type)index);
    }


/*************************************************************************/

LOCALNS::Namespaces::Namespaces(
	const xml_char_t** data,
	int nb_namespaces):
	    data(data),
	    nb_namespaces(nb_namespaces)
    {
    }
LOCALNS::Namespaces::size_type
LOCALNS::Namespaces::size() const
    {
    return (size_type)nb_namespaces;
    }

const xmlChar* LOCALNS::Namespaces::prefix(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return data[5*index+1];
    }
const xmlChar* LOCALNS::Namespaces::namespaceuri(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return data[5*index+2];
    }


LOCALNS::Namespace
LOCALNS::Namespaces::at(LOCALNS::Attributes::size_type index) const
    {
    CHECK_INDEX;
    return Namespace(this,index);
    }

LOCALNS::Namespace
LOCALNS::Namespaces::operator[](LOCALNS::Attributes::size_type index) const
    {
    return at(index);
    }



LOCALNS::DocumentHandler::DocumentHandler()
     {
     handler=(void*)::safeMalloc(sizeof(xmlSAXHandler));
     std::memset(handler,0,sizeof(xmlSAXHandler));
     SAX_HANDLER->notationDecl=_notationDeclSAXFunc;
     SAX_HANDLER->startElementNs=_startElementNsSAX2Func;
     SAX_HANDLER->endElementNs=_endElementNsSAX2Func;
     SAX_HANDLER->endDocument=_endDocumentSAXFunc;
     SAX_HANDLER->characters=_handleCharacters;

     SAX_HANDLER->initialized = XML_SAX2_MAGIC;
     }

LOCALNS::DocumentHandler::~DocumentHandler()
     {
     std::free(handler);
     }

 void  LOCALNS::DocumentHandler::notationDecl(const xml_char_t * name, const xml_char_t * publicId,  const xmlChar * systemId)
     {
     }
 void LOCALNS::DocumentHandler::startElement(
		const xml_char_t * localname,
		const xml_char_t * prefix,
		const xml_char_t * URI,
		int nb_namespaces,
		const xml_char_t ** namespaces,
		const Attributes& atts
		)
     {
     }

 void LOCALNS::DocumentHandler::endElement(
     const xml_char_t * localname,
     const xml_char_t * prefix,
     const xml_char_t * URI)
     {
     }

 void LOCALNS::DocumentHandler::characters(const xml_char_t * ch, int len)
     {

     }

 void LOCALNS::DocumentHandler::endDocument()
     {
     }


 int LOCALNS::DocumentHandler::parseFile(const char* filename)
     {
     return ::xmlSAXUserParseFile(SAX_HANDLER,this,filename);
     }

 int LOCALNS::DocumentHandler::parse(const char * buffer,int size)
     {
     return ::xmlSAXUserParseMemory(SAX_HANDLER,this,buffer,size);
     }


 static int _xmlInputReadCallback(
	 void * context,
	 char * buffer,
 	 int len)
	 {
	 std::istream* in=static_cast<std::istream*>(context);
	 in->read(buffer,len);
	 return (int)in->gcount();
	 }
 static int _xmlInputCloseCallback(void * context)
     {
     //void
     return 0;
     }
 int LOCALNS::DocumentHandler::parse(std::istream& in)
     {
     xmlParserCtxtPtr ctx;
     // Création du contexte
     if ((ctx = xmlCreateIOParserCtxt(
	     SAX_HANDLER,
	     (void*)this,
	     _xmlInputReadCallback,
	     _xmlInputCloseCallback,
	     (void*)&in, XML_CHAR_ENCODING_NONE
	     )) == NULL)
	     {
	     THROW("xmlCreateIOParserCtxt failed.");
	     }
     // Parsing du document
     int ret=xmlParseDocument(ctx);
     // Libération de la mémoire
     xmlFreeParserCtxt(ctx);
     return ret;
     }
