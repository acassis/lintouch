// $Id: AbstractConfiguration.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
//
//   FILE: AbstractConfiguration.hh -- 
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

#ifndef _ABSTRACTCONFIGURATION_HH
#define _ABSTRACTCONFIGURATION_HH

#include "lt/configuration/Configuration.hh"

namespace lt {

        /**
         * This is an abstract <code>Configuration</code> implementation that
         * deals with methods that can be abstracted away from underlying
         * implementations.
         *
         * @author <a href="mailto:fede@apache.org">Federico Barbieri</a>
         * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo
         * Fumagalli</a>       
         * @author <a href="mailto:leo.sutic@inspireinfrastructure.com">Leo
         * Sutic</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class AbstractConfiguration : public Configuration 
        {
            public:
            /**
             * Returns the value of the configuration element as an
             * <code>int</code>.
             *
             * Hexadecimal numbers begin with 0x, Octal numbers begin with
             * 0o and binary numbers begin with 0b, all other values are
             * assumed to be decimal.
             *
             * @param defaultValue the default value to return if value
             * malformed or empty
             * @return the value
             */
            virtual int getValueAsInt( int defaultValue = 0 ) const;

            /**
             * Returns the value of the configuration element as an
             * <code>uint</code>.
             *
             * Hexadecimal numbers begin with 0x, Octal numbers begin with
             * 0o and binary numbers begin with 0b, all other values are
             * assumed to be decimal.
             *
             * @param defaultValue the default value to return if value
             * malformed or empty
             * @return the value
             */
            virtual unsigned int getValueAsUInt( 
                    unsigned int defaultValue = 0 ) const;

            /**
             * Returns the value of the configuration element as an
             * <code>long</code>.
             *
             * Hexadecimal numbers begin with 0x, Octal numbers begin with
             * 0o and binary numbers begin with 0b, all other values are
             * assumed to be decimal.
             *
             * @param defaultValue the default value to return if value
             * malformed or empty
             * @return the value
             */
            virtual long getValueAsLong( long defaultValue = 0 ) const;

            /**
             * Returns the value of the configuration element as an
             * <code>ulong</code>.
             *
             * Hexadecimal numbers begin with 0x, Octal numbers begin with
             * 0o and binary numbers begin with 0b, all other values are
             * assumed to be decimal.
             *
             * @param defaultValue the default value to return if value
             * malformed or empty
             * @return the value
             */
            virtual unsigned long getValueAsULong( 
                    unsigned long defaultValue = 0 ) const;

            /**
             * Returns the value of the configuration element as a
             * <code>float</code>.
             *
             * Hexadecimal numbers begin with 0x, Octal numbers begin with
             * 0o and binary numbers begin with 0b, all other values are
             * assumed to be decimal.
             *
             * @param defaultValue the default value to return if value
             * malformed or empty
             * @return the value
             */
            virtual float getValueAsFloat( float defaultValue = 0.0 ) const;

            /**
             * Returns the value of the configuration element as a
             * <code>bool</code>.
             *
             * Hexadecimal numbers begin with 0x, Octal numbers begin with
             * 0o and binary numbers begin with 0b, all other values are
             * assumed to be decimal.
             *
             * @param defaultValue the default value to return if value
             * malformed or empty
             * @return the value
             */
            virtual bool getValueAsBool( bool defaultValue = false ) const;

            /**
             * Returns the value of the attribute specified by its name as a
             * <code>int</code>, or the default value if no attribute by
             * that name exists or is empty.
             *
             * @param name The name of the attribute you ask the value of.
             * @param defaultValue The default value desired.
             * @return int value of attribute. It will return the default
             *         value if the named attribute does not exist, or if
             *         the value is not set.
             */
            virtual int getAttributeAsInt( const QString & name, 
                    int defaultValue = 0) const;

            /**
             * Returns the value of the attribute specified by its name as a
             * <code>long</code>, or the default value if no attribute by
             * that name exists or is empty.
             *
             * @param name The name of the attribute you ask the value of.
             * @param defaultValue The default value desired.
             * @return long value of attribute. It will return the default
             *          value if the named attribute does not exist, or if
             *          the value is not set.
             */
            virtual long getAttributeAsLong( const QString & name, 
                    long defaultValue = 0 ) const;

            /**
             * Returns the value of the attribute specified by its name as a
             * <code>float</code>, or the default value if no attribute by
             * that name exists or is empty.
             *
             * @param name The name of the attribute you ask the value of.
             * @param defaultValue The default value desired.
             * @return float value of attribute. It will return the default
             *          value if the named attribute does not exist, or if
             *          the value is not set.
             */
            virtual float getAttributeAsFloat( const QString & name, 
                    float defaultValue = 0.0 ) const;

            /**
             * Returns the value of the attribute specified by its name as a
             * <code>boolean</code>, or the default value if no attribute by
             * that name exists or is empty.
             *
             * @param name The name of the attribute you ask the value of.
             * @param defaultValue The default value desired.
             * @return boolean value of attribute. It will return the default
             *         value if the named attribute does not exist, or if
             *         the value is not set.
             */
            virtual bool getAttributeAsBool( const QString & name, 
                    bool defaultValue = false ) const;

            /**
             * Return a <code>Configuration</code> instance encapsulating the
             * specified child node.
             *
             * @param child The name of the child node.
             * @param createNew If <code>true</code>, a new
             * <code>Configuration</code> will be created and returned if the
             * specified child does not exist. If <code>false</code>,
             * <code>null</code> will be returned when the specified
             * child doesn't exist.
             * @return Configuration
             */
            virtual const ConfigurationPtr getChild( const QString & child, 
                    bool createNew = true) const;

            /**
             * Parses string represenation of the <code>long</code> value.
             * <p />
             * Hexadecimal numbers begin with 0x, Octal numbers begin with
             * 0o and binary
             * numbers begin with 0b, all other values are assumed to be
             * decimal.
             *
             * @param value the value to parse
             * @return the long value
             */ 
            long parseLong ( const QString & value ) const;

            /**
             * Parses string represenation of the <code>ulong</code> value.
             * <p />
             * Hexadecimal numbers begin with 0x, Octal numbers begin with
             * 0o and binary
             * numbers begin with 0b, all other values are assumed to be
             * decimal.
             *
             * @param value the value to parse
             * @return the ulong value
             */ 
            unsigned long parseULong ( const QString & value ) const;

            /**
             * Parses string represenation of the <code>bool</code> value.
             * <p />
             * "yes", "true", and "1" case insensitive are considered to be
             * true, the rest is false.
             *
             * @param value the value to parse
             * @return the bool value
             */ 
            bool parseBool ( const QString & value ) const;

            /**
             * Destructor
             */
            virtual ~AbstractConfiguration () {};
        };

} // namespace lt

#endif /* _ABSTRACTCONFIGURATION_HH */

// vim: set et ts=4 sw=4:
// $Id: AbstractConfiguration.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
