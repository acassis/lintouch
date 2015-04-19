// $Id: Connection.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Connection.hh --
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

#ifndef _CONNECTION_HH
#define _CONNECTION_HH

#include <qmap.h>
#include <qstring.h>

#include <lt/variables/variables.h>

#include "lt/templates/Template.hh"
#include "lt/templates/Variable.hh"

#include <lt/LTDict.hh>

namespace lt {

    class Connection;

    typedef LTDict<Connection> ConnectionDict;

    class IOPin;

    /**
     * Connection.
     *
     * Conection contains one or more variables and is described by its
     * type and several properties in form key=value.
     *
     * You can add variables to a connection by specifying such connection
     * as a parent when creating the variables.
     *
     * This class is pointer based.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class Connection
    {

        public:

            /**
             * The meaningful constructor. Creates the connection of given
             * type and properties.
             */
            Connection( const QString & type,
                    const PropertyDict & properties );

            /**
             * Destructor.
             * Deletes the given properties and all variables within this
             * connection.
             */
            virtual ~Connection();

        private:

            /**
             * Disable unintentional copying.
             */
            Connection( const Connection & p );
            Connection & operator=( const Connection & p );

        public:

            /**
             * Return the type of this Connection.
             */
            QString type() const;

            /**
             * Return the Properties defined by this Connection.
             */
            const PropertyDict& properties() const;

            /**
             * Return the Properties defined by this Connection.
             */
            PropertyDict& properties();

            /**
             * Return the var_set_t contained in this Connection.
             */
            lt_var_set_t * var_set_t() const;

        public:

            /**
             * Add Variable to this connection. Takes the ownership of the
             * variable and deletes it along when this connection is
             * destroyed.
             */
            void addVariable( const QString & name, Variable * var );

            /**
             * Remove Variable from this connection. Leaves the ownership of
             * the variable.
             */
            void removeVariable( const QString & name );

            /**
             * Return the Variables contained in this Connection. You should
             * not delete any of the variables, this connection is owning
             * them.
             */
            const VariableDict& variables() const;

            /**
             * Return the Variables contained in this Connection. You should
             * not delete any of the variables, this connection is owning
             * them.
             */
            VariableDict& variables();

            /**
             * Return the Variable contained in this Connection according to
             * the given name. You should not delete the variable, this
             * connection is owning it.
             */
            Variable * variable( const QString & name ) const;

            /**
             * Should the connection be available within editor?
             */
            bool isAvailable() const;

            /**
             * Set the availability flag.
             */
            void setAvailable( bool available );

        public:

            /**
             * Synchronize all dirty real values of our variables with the
             * connected iopins and notify all affected templates.
             */
            void syncRealValues();

            /**
             * Synchronize all dirty requested values of our variables with
             * the connected var_set_t. Our var_set_t is usually connected
             * with the var_set_t from the communication protocol library.
             * This method enqueues all requested value changes so that they
             * can be sent over the network to the Lintouch Server.
             */
            void syncRequestedValues();

        private:

            typedef struct ConnectionPrivate;
            ConnectionPrivate * d;

    };

} // namespace lt

#endif /* _CONNECTION_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Connection.hh 1168 2005-12-12 14:48:03Z modesto $
