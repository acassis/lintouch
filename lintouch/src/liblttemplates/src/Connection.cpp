// $Id: Connection.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Connection.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 06 November 2003
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

#include <lt/templates/Connection.hh>
using namespace lt;

struct Connection::ConnectionPrivate
{
    ConnectionPrivate()
    {
        vset = lt_var_set_create( 0, NULL );
        Q_CHECK_PTR(vset);
    }

    virtual ~ConnectionPrivate()
    {
        apr_pool_destroy( lt_var_set_pool_get( vset ) );
    }

    // type of this connection
    QString type;

    // properties of this connection
    PropertyDict properties;

    // variables registered within this connection.
    VariableDict variables;

    // C level var_set_t used for synchronization with communication
    // protocol library.
    lt_var_set_t * vset;

    // should the connection be visible within editor?
    bool isAvailable;

};

Connection::~Connection()
{
    d->variables.setAutoDelete( true );
    d->variables.clear();

    d->properties.setAutoDelete( true );
    d->properties.clear();

    delete d;
}

Connection::Connection( const QString & type,
        const PropertyDict & properties ) :
    d ( new ConnectionPrivate )
{
    Q_CHECK_PTR(d);
    d->type = type;
    d->properties = properties;
    d->isAvailable = true;
}

QString Connection::type() const
{
    Q_ASSERT(d);
    return d->type;
}

const PropertyDict& Connection::properties() const
{
    Q_ASSERT(d);
    return d->properties;
}

PropertyDict& Connection::properties()
{
    Q_ASSERT(d);
    return d->properties;
}

lt_var_set_t * Connection::var_set_t() const
{
    Q_ASSERT(d);
    return d->vset;
}

void Connection::addVariable( const QString & name, Variable * var )
{
    Q_ASSERT(d);
    if( var == NULL ) return;
    d->variables.insert( name, var );

    // now we add var_t of the given variable to our varset
    QCString key = name.utf8();
    lt_var_set_variable_register( d->vset,
            key.data(), var->var_t() );
}

void Connection::removeVariable( const QString & name )
{
    Q_ASSERT(d);

    Variable* var = d->variables[name];
    if( var == NULL ) return;

    // now we remove var_t of the given variable to our varset
    QCString key = name.utf8();
    lt_var_set_variable_unregister( d->vset, key.data() );

    // and remove the name and Variable* from dict
    d->variables.remove( name );
}

const VariableDict& Connection::variables() const
{
    Q_ASSERT(d);
    return d->variables;
}

VariableDict& Connection::variables()
{
    Q_ASSERT(d);
    return d->variables;
}

Variable * Connection::variable( const QString & name ) const
{
    Q_ASSERT(d);
    return d->variables[ name ];
}

bool Connection::isAvailable() const
{
    return d->isAvailable;
}

void Connection::setAvailable( bool available )
{
    d->isAvailable = available;
}

void Connection::syncRealValues()
{
    Q_ASSERT(d);
    QMap <Template*,Template*> notify;

    // we will walk the variables
    for(unsigned i = 0; i < d->variables.count(); ++i ) {

        Variable * var = d->variables[i];
        lt_var_t * vart = var->var_t();

        // if the value is dirty we need to sync it to all connected iopins
        if( lt_var_real_value_is_dirty( vart ) ) {

            // so for each iopin connected to this variable
            IOPin * i = var->m_connectedIOPins.first();
            for( ; i; i = var->m_connectedIOPins.next() ) {

                // we transfer the real value to the iopin
                i->setRealValue( lt_var_real_value_get( vart ) );

                // remember that we need to notify parent of iopin i
                if( ! notify.contains( i->parent() ) ) {
                    notify.insert( i->parent(), i->parent() );
                }
            }
        }

    }

    // now notify all templates about the change
    QMapIterator <Template*, Template*> it = notify.begin();
    for( ; it != notify.end(); ++it ) {
        it.key()->iopinsChanged();
    }

    // and clear dirty flags on our varset
    lt_var_set_real_values_sync( d->vset, -1 );
}

void Connection::syncRequestedValues ()
{
    Q_ASSERT(d);
    // our varset should be filled with new values by calling one or more
    // times Template::syncIOPinsRequestedValues

    // we simply feed the changed values to communication protocol core
    lt_var_set_requested_values_sync( d->vset, -1 );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Connection.cpp 1168 2005-12-12 14:48:03Z modesto $
