// $Id: DefaultConfigurationSerializer.cpp,v 1.3 2004/05/18 11:03:29 mman Exp $
//
//   FILE: DefaultConfigurationSerializer.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 19 May 2002
//
// Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
//
// This application is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License, or (at
// your option) any later version.
//
// This application is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this application; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <qfile.h>
#include <qmap.h>
#include <qptrvector.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include "lt/configuration/DefaultConfigurationSerializer.hh"

using namespace lt;

struct DefaultConfigurationSerializer::DefaultConfigurationSerializerPrivate {
    DefaultConfigurationSerializerPrivate ( unsigned i ) : indent ( i ) {};
    unsigned indent;
    QMap <QString, bool> declaredPrefixes;
};

DefaultConfigurationSerializer::DefaultConfigurationSerializer( 
        bool /* enableNamespaces = false */,
        unsigned indent /* = 4 */ ) {
    d = new DefaultConfigurationSerializerPrivate ( indent );
    Q_CHECK_PTR ( d );
}

DefaultConfigurationSerializer::~DefaultConfigurationSerializer () {
    delete d;
}

DefaultConfigurationSerializer::DefaultConfigurationSerializer (
        const DefaultConfigurationSerializer & other ) {
    d = new DefaultConfigurationSerializerPrivate ( 0 );
    Q_CHECK_PTR ( d );
    *d = *other.d;
}

DefaultConfigurationSerializer & DefaultConfigurationSerializer::operator = (
        const DefaultConfigurationSerializer & other ) {
    *d = *other.d;
    return *this;
}

bool DefaultConfigurationSerializer::serializeToFile( const QString & filename, 
        const ConfigurationPtr configuration ) {
    QFile f ( filename );
    return serializeToFile ( &f, configuration );
}

bool DefaultConfigurationSerializer::serializeToFile( QIODevice *dev, 
        const ConfigurationPtr configuration ) {

    if ( dev == NULL ) {
        return false;
    }

    // if not open, try to open in write mode
    if ( ! dev->isOpen () ) {
        dev->open ( IO_Truncate | IO_WriteOnly );
    }

    // if not writable give up
    if ( ! dev->isWritable () ) {
        return false;
    }

    QTextStream ts ( dev );

    if ( ! serializeToStream ( &ts, configuration ) ) {
        return false;
    }

    dev->close ();

    return true;
}

bool DefaultConfigurationSerializer::serializeToStream( QTextStream *stream, 
        const ConfigurationPtr configuration ) {

    if ( stream == NULL ) {
        return false;
    }

    stream->setEncoding ( QTextStream::UnicodeUTF8 );

    if ( ! serializePreambule ( stream ) ) {
        return false;
    }
    if ( ! serializeNode ( stream, configuration, 0 ) ) {
        return false;
    }

    return true;
}

bool DefaultConfigurationSerializer::serializePreambule ( QTextStream *st ) {
    *st << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n\n";
    return true;
}

bool DefaultConfigurationSerializer::serializeNode ( QTextStream *st, 
        const ConfigurationPtr node, unsigned indent ) {
    if ( node.isNull() ) return false;

    QStringList attrs = node->getAttributeNames ();
    const ConfigurationList children = node->getChildren ();
    QString name = node->getName ();
    QString ns = node->getNamespace ();
    QString prefix = node->getPrefix ();

    for ( unsigned k = 0; k < indent; k ++ ) *st << " ";
    *st << "<";

    if ( ! ns.isEmpty () && ! prefix.isEmpty () ) {
        *st << prefix << ":";
    }

    *st << name;

    if ( ! ns.isEmpty () && ! prefix.isEmpty () && 
            ! d->declaredPrefixes.contains ( prefix ) ) {
        *st << " " << "xmlns:" << prefix << "=\"" << ns << "\"";
        d->declaredPrefixes [ prefix ] = true;
    }

    QStringList::const_iterator i = const_cast<QStringList&>(attrs).begin ();
    for (; i != const_cast<QStringList&>(attrs).end (); ++ i ) {
        *st << " " << *i << "=\"";
        *st << escape ( node->getAttribute ( *i ) );
        *st << "\"";
    }

    bool ret = true;
    if ( children.count () > 0 ) {
        *st << ">\n";
        ConfigurationList::const_iterator it;
        for ( it = const_cast<ConfigurationList&>(children).begin();
                it != const_cast<ConfigurationList&>(children).end ();
                it++ )
        {
            if ( !serializeNode ( st, *it, indent + d->indent ) ) {
                ret = false;
                break;
            }
        }
        for ( unsigned k1 = 0; k1 < indent; k1 ++ ) *st << " ";
        *st << "</"; 
        if ( ! ns.isEmpty () && ! prefix.isEmpty () ) {
            *st << prefix << ":";
        }
        *st << name << ">\n";
    } else if ( node->getValue ().isEmpty () ){
        *st << "/>\n";
    } else {
        *st << ">" << escape ( node->getValue () );
        *st << "</"; 
        if ( ! ns.isEmpty () && ! prefix.isEmpty () ) {
            *st << prefix << ":";
        }
        *st << name << ">\n";
    }
    if ( ! ns.isEmpty () && ! prefix.isEmpty () ) {
        d->declaredPrefixes.remove ( prefix );
    }
    return ret;

}

QString DefaultConfigurationSerializer::escape ( const QString & str ) {
    QString res;
    for ( unsigned i = 0; i < str.length (); i ++ ) {
        if ( str.at( i ) == '<' ) {
            res += "&lt;";
        } else if ( str.at( i ) == '>' ) {
            res += "&gt;";
        } else if ( str.at( i ) == '&' ) {
            res += "&amp;";
        } else if ( str.at( i ) == '"' ) {
            res += "&quot;";
        } else if ( str.at( i ) == '\'' ) {
            res += "&apos;";
        } else {
            res += str.at( i );
        }
    }
    return res;
}

// vim: set et ts=4 sw=4:
// $Id: DefaultConfigurationSerializer.cpp,v 1.3 2004/05/18 11:03:29 mman Exp $
