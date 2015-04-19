// $Id: CSV.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: CSV.cpp --
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 05 October 2004
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

#include "CSV.hh"

static QChar delimiter = ',';
static QChar quotechar = '"';

Connection * csv2connection( const QStringList & lines )
{
    Connection * ret = NULL;
    QStringList::ConstIterator linesit = lines.begin();

    // try to find a connection
    QString connType;
    PropertyDict connProps;
    QStringList connHeader;
    QStringList varHeader;

    for( ; linesit != lines.end(); ++ linesit ) {
        const QStringList fields = CSVDecodeRecord( *linesit );

        //try to find "Connection,Type" fields
        if( fields.count() >= 2 &&
                fields[ 0 ] == "Connection" && fields[ 1 ] == "Type" ) {
            // move to the next line & remember connection props names
            connHeader = fields;
            ++ linesit;
            break;
        }
    }

    // found a connection header and have a next line
    if( connHeader.count() > 0 && linesit != lines.end() ) {
        const QStringList fields = CSVDecodeRecord( *linesit );
        connType = fields[ 1 ];
        for( unsigned j = 2; j < connHeader.count(); j ++ ) {
            connProps.insert( connHeader[ j ],
                    new Property( connHeader[ j ], "string",
                        ( ( j < fields.count() ) ? fields[ j ] : "" ) ) );
        }

        // move to the next line & create connection
        ret = new Connection( connType, connProps );
        ++ linesit;
    }

    // try to find variables
    for( ; linesit != lines.end(); ) {
        const QStringList fields = CSVDecodeRecord( *linesit );

        //try to find "Variable,Type" fields
        if( fields.count() >= 2 &&
                fields[ 0 ] == "Variable" && fields[ 1 ] == "Type" ) {

            // move to the next line & remember var props names
            varHeader = fields;
            ++ linesit;

            // iterate over the consecutive lines until we find
            // "Varibale,Type" again or we reach last line, skip empty lines
            for( ; linesit != lines.end(); ++ linesit ) {
                const QStringList vardata = CSVDecodeRecord( *linesit );

                // skip line with no records (or without at least name &
                // type
                if( vardata.count() < 2 ) continue;

                // proceed with the next section when "Variable,Type" is
                // found
                if( vardata.count() >= 2 &&
                        vardata[ 0 ] == "Variable" &&
                        vardata[ 1 ] == "Type" ) {
                    break;
                }

                // vardata now contain parsed line with variable name, type
                // and properties
                QString varName;
                QString varType;
                PropertyDict varProps;

                varName = vardata[ 0 ];
                varType = vardata[ 1 ];
                for( unsigned k = 2; k < varHeader.count(); k ++ ) {
                    varProps.insert( varHeader[ k ],
                    new Property( varHeader[ k ], "string",
                        ( ( k < vardata.count() ) ? vardata[ k ] : "" ) ) );
                }

                // create variable & add it to connection
                Variable * v = new Variable( varType, varProps );
                ret->addVariable( varName, v );

            }
        } else {
            // move to the next line
            ++ linesit;
        }
    }

    return ret;
}

QStringList connection2csv( const Connection * conn )
{
    QStringList ret;

    // safety check
    if( conn == NULL ) return ret;

    // prepare connection header & connection record
    unsigned i, j;
    QStringList connHeader;
    QStringList connRecord;

    connHeader.append( "Connection" );
    connHeader.append( "Type" );

    connRecord.append( "Unnamed" );
    connRecord.append( conn->type() );

    const PropertyDict & pd = conn->properties();
    for( i = 0; i < pd.count(); i ++ ) {
        connHeader.append( pd.keyAt( i ) );
        connRecord.append( pd[ i ]->asString() );
    }

    // append connection record to the resulting stream
    ret.append( CSVEncodeRecord( connHeader ) );
    ret.append( CSVEncodeRecord( connRecord ) );

    // empty record as a separator
    ret.append( CSVEncodeRecord( QStringList() ) );

    bool has_bitvars = false;
    QStringList varHeaderBit;
    varHeaderBit.append( "Variable" );
    varHeaderBit.append( "Type" );
    bool has_numvars = false;
    QStringList varHeaderNum;
    varHeaderNum.append( "Variable" );
    varHeaderNum.append( "Type" );
    bool has_strvars = false;
    QStringList varHeaderStr;
    varHeaderStr.append( "Variable" );
    varHeaderStr.append( "Type" );

    QStringList varRecord;
    const VariableDict & vd = conn->variables();
    const Variable * v;

    // prepare headers for variables of various types
    for( i = 0; i < vd.count(); i ++ ) {
        v = vd[ i ];
        if( ! has_bitvars && v->type() == IOPin::BitType ) {
            has_bitvars = true;
            const PropertyDict & pd1 = v->properties();
            for( j = 0; j < pd1.count(); j ++ ) {
                varHeaderBit.append( pd1.keyAt( j ) );
            }
        }
        if( ! has_numvars && v->type() == IOPin::NumberType ) {
            has_numvars = true;
            const PropertyDict & pd1 = v->properties();
            for( j = 0; j < pd1.count(); j ++ ) {
                varHeaderNum.append( pd1.keyAt( j ) );
            }
        }
        if( ! has_strvars && v->type() == IOPin::StringType ) {
            has_strvars = true;
            const PropertyDict & pd1 = v->properties();
            for( j = 0; j < pd1.count(); j ++ ) {
                varHeaderStr.append( pd1.keyAt( j ) );
            }
        }
    }

    // prepare all bit variables
    if( has_bitvars ) {

        ret.append( CSVEncodeRecord( varHeaderBit ) );

        for( i = 0; i < vd.count(); i ++ ) {
            if( vd[ i ]->type() == IOPin::BitType ) {

                varRecord.append( vd.keyAt( i ) );
                varRecord.append( "BIT" );
                const PropertyDict & pd1 = vd[ i ]->properties();
                for( j = 0; j < pd1.count(); j ++ ) {
                    varRecord.append( pd1[ j ]->asString() );
                }
                ret.append( CSVEncodeRecord( varRecord ) );
                varRecord.clear();
            }
        }

        // empty record as a separator
        ret.append( CSVEncodeRecord( QStringList() ) );
    }

    // prepare all num variables
    if( has_numvars ) {

        ret.append( CSVEncodeRecord( varHeaderNum ) );

        for( i = 0; i < vd.count(); i ++ ) {
            if( vd[ i ]->type() == IOPin::NumberType ) {

                varRecord.append( vd.keyAt( i ) );
                varRecord.append( "NUMBER" );
                const PropertyDict & pd1 = vd[ i ]->properties();
                for( j = 0; j < pd1.count(); j ++ ) {
                    varRecord.append( pd1[ j ]->asString() );
                }
                ret.append( CSVEncodeRecord( varRecord ) );
                varRecord.clear();
            }
        }

        // empty record as a separator
        ret.append( CSVEncodeRecord( QStringList() ) );
    }

    // prepare all str variables
    if( has_strvars ) {

        ret.append( CSVEncodeRecord( varHeaderStr ) );

        for( i = 0; i < vd.count(); i ++ ) {
            if( vd[ i ]->type() == IOPin::StringType ) {

                varRecord.append( vd.keyAt( i ) );
                varRecord.append( "STRING" );
                const PropertyDict & pd1 = vd[ i ]->properties();
                for( j = 0; j < pd1.count(); j ++ ) {
                    varRecord.append( pd1[ j ]->asString() );
                }
                ret.append( CSVEncodeRecord( varRecord ) );
                varRecord.clear();
            }
        }

        // empty record as a separator
        ret.append( CSVEncodeRecord( QStringList() ) );
    }

    return ret;
}

QString CSVEncodeField( const QString & src )
{
    //FIXME: this algorithm is probably not very effective

    QString ret;
    bool needs_to_be_quoted = false;
    //bool needs_to_be_quoted = true;

    for( unsigned i = 0; i < src.length(); i ++ ) {
        // delimiter within a field means we need to quote whole field
        if( src[ i ] == delimiter ) needs_to_be_quoted = true;

        // quotechar within a field needs to be doubled
        if( src[ i ] == quotechar ) {
            needs_to_be_quoted = true;
            ret += quotechar;
        }

        // append original character to the return buffer
        ret += src[ i ];
    }

    if( needs_to_be_quoted ) {
        ret = quotechar + ret + quotechar;
    }

    return ret;
}

QString CSVEncodeRecord( const QStringList & src )
{
    QString ret;
    bool delim = 0;

    QStringList::ConstIterator it = src.begin();
    for( ; it != src.end(); ++it ) {
        // prepend delimiter for all but first record
        if( delim ) ret += delimiter;

        // append field
        ret += CSVEncodeField( *it );

        // further on, we need delimiters after each field
        delim = true;
    }

    return ret + "\r\n";
}

QStringList CSVDecodeRecord( const QString & line )
{
    QStringList ret;
    bool quoted = false;
    QString field;

    for( unsigned i = 0; i < line.length(); i ++ ) {
        // ignore anything following line endings
        if( line[ i ] == '\r' || line[ i ] == '\n' ) {
            break;
        }

        // found a delimiter and not in quotes, append parsed field and
        // continue with next character
        if( line[ i ] == delimiter && ! quoted ) {
            ret.append( field );
            field = "";
            continue;
        }

        // found a quote, toggle mode
        if( line[ i ] == quotechar ) {
            quoted = !quoted;
            // two consecutive quotechars are replaced with one quote
            if( quoted && i > 1 && line[ i - 1 ] == quotechar ) {
                field.append( quotechar );
            }
            continue;
        }

        // ordinary character, append to internal buffer
        field.append( line[ i ] );
    }

    // append last processed field
    ret.append( field );

    return ret;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: CSV.cpp 1169 2005-12-12 15:22:15Z modesto $
