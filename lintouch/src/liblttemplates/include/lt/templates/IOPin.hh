// $Id: IOPin.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: IOPin.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 03 October 2003
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

#ifndef _IOPIN_HH
#define _IOPIN_HH

#include <qdict.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include <lt/variables/io.h>

#include "lt/LTDict.hh"

namespace lt {

    class IOPin;

    typedef LTDict <IOPin> IOPinDict;

    typedef QPtrList <IOPin> IOPinList;
    typedef QPtrListIterator <IOPin> IOPinListIterator;

    class Template;
    class Variable;

    /**
     * Template IOPin.
     *
     * IOPin represents point of interaction of a Template with the
     * outer world. the familiar analogy should be a pin of electrical
     * integrated circuit.
     *
     * IOPin always has two values, Requested Value and Real Value.
     *
     * Template typically sets Requested Value and forwards the change
     * by calling syncRequestedValue to other IOPins to which this one
     * has been bound.
     *
     * When Real value is changed and forwarded by calling
     * syncRealValue the change propagates to other pins that has been
     * bound to this IOPin.
     *
     * This class is pointer based.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class IOPin
    {

        public:

            /**
             * IOPin type.
             */
            enum Type {
                NullType        = LT_IO_TYPE_NULL,
                BitType         = LT_IO_TYPE_BIT,
                NumberType      = LT_IO_TYPE_NUMBER,
                StringType      = LT_IO_TYPE_STRING,
                SequenceType    = LT_IO_TYPE_SEQUENCE,

                InvalidType     = -1,
            };

            typedef QValueList <Type> TypeList;

        public:

            /**
             * Constructor. Create IOPin for given parent. The IOPin can be
             * bound to Variable of given type. The label should be
             * localized string describing this iopin.
             */
            IOPin( Template * parent, const QString & label, Type type );

            /**
             * Constructor. Create IOPin for given parent. The IOPin can be
             * bound to Variable of given types. The label should be
             * localized string describing this iopin.
             */
            IOPin( Template * parent, const QString & label,
                    const QString & types );

            /**
             * Destructor.
             */
            virtual ~IOPin();

        private:

            /**
             * Disable unintentional copying.
             */
            IOPin( const IOPin & p );
            IOPin & operator =( const IOPin & p );

        public:

            /**
             * return our parent template.
             */
            Template * parent() const;

        public:

            /**
             * Try to convert string representation of a type to the enum
             * code. Returns InvalidType in case of error.
             */
            static Type decodeType( const QString & type );

            /**
             * Try to convert comma separated list of allowed types into the
             * list of codes. Returns empty list in case of syntax error.
             * All types will be returned when an empty string
             * representation is passed in as the types argument.
             */
            static TypeList decodeTypes(
                    const QString & types );

            /**
             * encode given type into string representation.
             */
            static QString encodeTypes( TypeList types );

            /**
             * return the list of(encoded) supported iopin types.
             */
            static QStringList supportedTypesAsStringList();

            /**
             * return the list of all supported iopin types.
             */
            static TypeList supportedTypesAsTypeList();

        public:

            /**
             * Are the given types of this iopin valid?
             */
            bool isValid() const;

            /**
             * Return the supported types of this IOPin.
             */
            TypeList types() const;

            /**
             * Return the localized label describing this template.
             */
            QString label() const;

        public:

            /**
             * Real value setter.
             *
             * Real value can be either BIT, NUMBER or STRING.
             * by using one of setRealBitValue, setRealNumberValue or
             * setRealStringValue you can change the real value of this
             * iopin. the realvalue will be marked as dirty when new
             * value differs from the former one. The dirty flag will be
             * cleared and real value will propagate to the iopins which
             * were bound to this one by calling syncRealValue.
             */
            void setRealBitValue( bool bit );
            /**
             * @see setRealValue
             */
            void setRealNumberValue( int num );
            /**
             * @see setRealValue
             */
            void setRealStringValue( const QString & str );
            /**
             * @see setRealValue
             */
            void setRealValue( const lt_io_value_t * val );

            /**
             * Return the type of currently set real value.
             */
            Type realValueType() const;

            /**
             * @see setRealValue
             */
            bool realBitValue() const;
            /**
             * @see setRealValue
             */
            int realNumberValue() const;
            /**
             * @see setRealValue
             */
            QString realStringValue() const;

            /**
             * Is the real value dirty ?
             *
             * @see setRealBitValue
             */
            bool isRealValueDirty() const;

            /**
             * sync real value with IOPins that has been bound to this
             * one. also clears the dirty real flag.
             *
             * @see setRealBitValue
             */
            void syncRealValue();

            /**
             * Requested value setter.
             *
             * Requested value can be either BIT, NUMBER or STRING.  by
             * using one of setRequestedBitValue,
             * setRequestedNumberValue or setRequestedStringValue you
             * can change the requested value of this iopin. the
             * requested value will be marked as dirty when new value
             * differs from the former one. The dirty flag will be
             * cleared and requested value will propagate to the iopin
             * to which this iopin has been bound by calling
             * syncRequestedValue.
             */
            void setRequestedBitValue( bool bit );
            /**
             * @see setRequestedValue
             */
            void setRequestedNumberValue( int num );
            /**
             * @see setRequestedValue
             */
            void setRequestedStringValue( const QString & str );
            /**
             * @see setRequestedValue
             */
            void setRequestedValue( const lt_io_value_t * val );

            /**
             * Return the type of currently set requested value.
             */
            Type requestedValueType() const;

            /**
             * @see setRequestedValue
             */
            bool requestedBitValue() const;
            /**
             * @see setRequestedValue
             */
            int requestedNumberValue() const;
            /**
             * @see setRequestedValue
             */
            QString requestedStringValue() const;

            /**
             * Is the requested value dirty ?
             *
             * @see setRequestedValue
             */
            bool isRequestedValueDirty() const;

            /**
             * sync requested value ot this IOPin to the IOPin or Variable
             * to which this IOPin has been bound. also clears the dirty
             * requested flag.
             *
             * @see setRequestedValue
             */
            void syncRequestedValue();

            /**
             * Bind this IOPin to some Variable. This IOPin will act as
             * a target for the Variable, i.e., real value of the Variable
             * will be synced to this IOPin and vice versa
             * for requested value.
             *
             * Note: unbinds any previously made bindings.
             */
            void bindToVariable( Variable * other );

            /**
             * Remove the binding between this IOPin and another
             * Variable as created by calling bindToVariable.
             */
            void unbind();

            /**
             * Return the variable we are bound to
             */
            Variable * boundTo() const;

        private:

            typedef struct IOPinPrivate;
            IOPinPrivate * d;

    };

} // namespace lt

#endif /* _IOPIN_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: IOPin.hh 1168 2005-12-12 14:48:03Z modesto $
