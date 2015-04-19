// $Id: Parameters.hh,v 1.1 2003/11/11 15:32:38 mman Exp $
//
//   FILE: Parameters.hh -- 
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

#ifndef _PARAMETERS_HH
#define _PARAMETERS_HH

#include <qstringlist.h>
#include <qvaluelist.h>
#include "lt/SharedPtr.hh"

#include "lt/configuration/Configuration.hh"

namespace lt {

        class Parameters;
        /**
         * predefined type of SharedPtr to Parameters
         */
        typedef SharedPtr<Parameters> ParametersPtr;

        /**
         * predefined type to hold list of Parameters
         */
        typedef QValueList <ParametersPtr> ParametersList;

        /**
         * The <code>Parameters</code> class represents a set of key-value
         * pairs.
         * <p>
         * The <code>Parameters</code> object provides a mechanism to obtain
         * values based on a <code>String</code> name.  There are convenience
         * methods that allow you to use defaults if the value does not exist,
         * as well as obtain the value in any of the same formats that are in
         * the {@link Configuration} interface.
         * </p><p>
         * While there are similarities between the <code>Parameters</code>
         * object and the java.util.Properties object, there are some
         * important semantic differences.  First, <code>Parameters</code> are
         * <i>read-only</i>.  Second, <code>Parameters</code> are easily
         * derived from {@link Configuration} objects.  Lastly, the
         * <code>Parameters</code> object is derived from XML fragments that
         * look like this:
         * <pre><code>
         *  &lt;parameter name="param-name" value="param-value" /&gt;
         * </code></pre>
         * </p><p>
         * <strong>Note: this class is not thread safe by default.</strong> If
         * you require thread safety please synchronize write access to this
         * class to prevent potential data corruption.  </p>
         *
         * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo
         * Fumagalli</a>
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         * @version 1.0
         */
        class Parameters
        {
            public:
                /**
                 * Construct empty parameters.
                 */
                Parameters ();

                /**
                 * Destructor.
                 */
                ~Parameters ();

                /**
                 * Set the <code>String</code> value of a specified parameter.
                 * <p />
                 * If the specified value is <b>null</b> the parameter is
                 * removed.
                 *
                 * @param name a <code>String</code> value
                 * @param value a <code>String</code> value
                 * @return The previous value of the parameter or <b>null</b>.
                 */
                const QString setParameter( const QString & name, 
                        const QString & value );

                /**
                 * Remove a parameter from the parameters object
                 * @param name a <code>String</code> value
                 */
                void removeParameter( const QString & name );

                /**
                 * Retrieve an array of all parameter names.
                 *
                 * @return the parameters names
                 */
                const QStringList getParameterNames() const;

                /**
                 * Test if the specified parameter can be retrieved.
                 *
                 * @param name the parameter name
                 * @return true if parameter is a name
                 */
                bool isParameter( const QString & name ) const;

                /**
                 * Retrieve the <code>String</code> value of the specified
                 * parameter.
                 * <p />
                 * If the specified parameter cannot be found,
                 * <code>defaultValue</code> is returned.
                 *
                 * @param name the name of parameter
                 * @param defaultValue the default value, returned if
                 * parameter does not exist or parameter's name is null
                 * @return the value of parameter
                 */
                const QString getParameter( const QString & name, 
                        const QString & defaultValue = "" ) const;

                /**
                 * Retrieve the <code>int</code> value of the specified
                 * parameter.
                 * <p />
                 * If the specified parameter cannot be found,
                 * <code>defaultValue</code> is returned.
                 *
                 * Hexadecimal numbers begin with 0x, Octal numbers begin with
                 * 0o and binary numbers begin with 0b, all other values are
                 * assumed to be decimal.
                 *
                 * @param name the name of parameter
                 * @param defaultValue value returned if parameter does not
                 * exist or is of wrong type
                 * @return the integer parameter type
                 */
                int getParameterAsInt( const QString & name, 
                        int defaultValue = 0 ) const;

                /**
                 * @see Parameters::getParameterAsInt
                 */
                unsigned int getParameterAsUInt( const QString & name, 
                        unsigned int defaultValue = 0) const;

                /**
                 * Retrieve the <code>long</code> value of the specified
                 * parameter.
                 * <p />
                 * If the specified parameter cannot be found,
                 * <code>defaultValue</code> is returned.
                 *
                 * Hexadecimal numbers begin with 0x, Octal numbers begin with
                 * 0o and binary numbers begin with 0b, all other values are
                 * assumed to be decimal.
                 *
                 * @param name the name of parameter
                 * @param defaultValue value returned if parameter does not
                 * exist or is of wrong type
                 * @return the long parameter type
                 */
                long getParameterAsLong( const QString & name, 
                        long defaultValue = 0) const;

                /**
                 * @see Parameters::getParameterAsLong
                 */
                unsigned long getParameterAsULong( const QString & name, 
                        unsigned long defaultValue = 0) const;

                /**
                 * Retrieve the <code>float</code> value of the specified
                 * parameter.
                 * <p />
                 * If the specified parameter cannot be found,
                 * <code>defaultValue</code> is returned.
                 *
                 * @param name the parameter name
                 * @param defaultValue the default value if parameter does not
                 * exist or is of wrong type
                 * @return the value
                 */
                float getParameterAsFloat( const QString & name, 
                        float defaultValue = 0.0) const;
 
                /**
                 * Retrieve the <code>boolean</code> value of the specified
                 * parameter.
                 * <p />
                 * If the specified parameter cannot be found,
                 * <code>defaultValue</code> is returned.
                 *
                 * @param name the parameter name
                 * @param defaultValue the default value if parameter does not
                 * exist or is of wrong type
                 * @return the value
                 */
                bool getParameterAsBool( const QString & name, 
                        bool defaultValue = false ) const;

                /**
                 * Merge parameters from another <code>Parameters</code>
                 * instance into this.
                 *
                 * @param other the other Parameters
                 */
                void merge( const ParametersPtr other );

                /**
                 * Create a <code>Parameters</code> object from a
                 * <code>Configuration</code> object using the supplied
                 * element name.
                 *
                 * @param configuration the Configuration
                 * @param elementName   the element name for the parameters
                 * @return This <code>Parameters</code> instance.
                 */
                static ParametersPtr fromConfiguration( 
                        const ConfigurationPtr configuration,
                        const QString & elementName = "parameter");


                /** 
                 * Make this Parameters read-only.
                 */
                void makeReadOnly();

            private:
                /**
                 * Checks if this <code>Parameters</code> object is writeable.
                 */
                bool isWriteable() const;

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
                 * Copy Constructor.
                 */
                Parameters (
                        const Parameters & other );

                /**
                 * Assignment operator.
                 */
                Parameters & operator = (
                        const Parameters & other );

            private:

                struct ParametersPrivate;
                ParametersPrivate *d;
        };

} // namespace lt

#endif /* _PARAMETERS_HH */

// vim: set et ts=4 sw=4:
// $Id: Parameters.hh,v 1.1 2003/11/11 15:32:38 mman Exp $
