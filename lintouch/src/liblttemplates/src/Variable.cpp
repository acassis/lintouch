// $Id: Variable.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Variable.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 19 November 2003
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

#include <lt/variables/variables.h>

#include "lt/templates/Variable.hh"
#include "lt/templates/Connection.hh"
using namespace lt;

struct Variable::VariablePrivate
{

    VariablePrivate() :
        type( IOPin::InvalidType )
    {
    }

    virtual ~VariablePrivate()
    {
    }

    // type of this variable
    IOPin::Type type;

    // properties of this variable
    PropertyDict properties;

};

Variable::Variable( const QString & type,
    const PropertyDict & properties) :
    m_var( NULL ), d ( new VariablePrivate )
{
    d->type = IOPin::decodeType( type );
    d->properties = properties;

    // create m_var
    lt_io_value_type_e tp = LT_IO_TYPE_NULL;
    if( type == "bit" ) {
        tp = LT_IO_TYPE_BIT;
    } else if( type == "number" ) {
        tp = LT_IO_TYPE_NUMBER;
    } else if( type == "string" ) {
        tp = LT_IO_TYPE_STRING;
    }

    m_var = lt_var_create( tp, 1, NULL );
}

Variable::~Variable()
{
    //unbind all iopins connected to this variable
    IOPinList iopinsCopy = m_connectedIOPins;
    for (IOPin *it = iopinsCopy.first(); it;
        it = iopinsCopy.next())
        it->unbind();

    delete d;

    // destroy the pool out of which m_var was allocated
    apr_pool_destroy( lt_var_pool_get( m_var ) );
}

IOPin::Type Variable::type() const
{
    return d->type;
}

const PropertyDict& Variable::properties () const
{
    return d->properties;
}

PropertyDict& Variable::properties ()
{
    return d->properties;
}

const IOPinList & Variable::connectedIOPins() const
{
    return m_connectedIOPins;
}

IOPinList & Variable::connectedIOPins()
{
    return m_connectedIOPins;
}

lt_var_t * Variable::var_t() const
{
    return m_var;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Variable.cpp 1168 2005-12-12 14:48:03Z modesto $
