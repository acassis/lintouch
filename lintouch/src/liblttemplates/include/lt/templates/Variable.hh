// $Id: Variable.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Variable.hh --
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

#ifndef _VARIABLE_HH
#define _VARIABLE_HH

#include <qdict.h>

#include <lt/variables/variables.h>

#include "lt/templates/IOPin.hh"
#include "lt/templates/Property.hh"

#include <lt/LTDict.hh>

namespace lt {

    class Variable;

    typedef LTDict <Variable> VariableDict;

    class Connection;

    /**
     * Variable.
     *
     * Variable is defined by its name (not stored in this class), data type
     * and the properties of form key=value.
     *
     * This class is pointer based.
     */
    class Variable
    {

        friend class IOPin;
        friend class Connection;

        public:

            /**
             * The meaningful constructor. Creates the variable of given
             * type and associates given properties with the variable.
             */
            Variable( const QString & type,
                    const PropertyDict & properties );

            /**
             * Destructor.
             */
            virtual ~Variable ();

        private:

            /**
             * Disable unintentional copying.
             */
            Variable( const Variable & p );
            Variable & operator=( const Variable & p );

        public:

            /**
             * Return the type of this variable.
             * Returns IOPin::InvalidType when unknown type was specified in
             * the constructor.
             */
            IOPin::Type type() const;

            /**
             * Return the Properties defined by this Variable.
             */
            const PropertyDict& properties() const;

            /**
             * Return the Properties defined by this Variable.
             */
            PropertyDict& properties();

            /**
             * Return the iopins connected to us.
             */
            const IOPinList & connectedIOPins() const;

            /**
             * Return the iopins connected to us.
             */
            IOPinList & connectedIOPins();

        public:

            /**
             * Return the underlying lt_var_t.
             */
            lt_var_t * var_t() const;

        protected:

            /**
             * The underlying var_t data structure. kept as the protected
             * member to allow direct access from friend class Connection.
             */
            lt_var_t * m_var;

            /**
             * The list of IOPins connected to us. kept as the protected
             * member to allow direct access from friend class Connection.
             */
            IOPinList m_connectedIOPins;

        private:

            typedef struct VariablePrivate;
            VariablePrivate * d;

    };

} // namespace lt

#endif /* _VARIABLE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Variable.hh 1168 2005-12-12 14:48:03Z modesto $
