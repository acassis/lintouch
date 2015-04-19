// $Id: DefaultConfiguration.hh,v 1.2 2003/11/15 18:14:40 hynek Exp $
//
//   FILE: DefaultConfiguration.hh -- 
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

#ifndef _DEFAULTCONFIGURATION_HH
#define _DEFAULTCONFIGURATION_HH

#include "lt/configuration/AbstractConfiguration.hh"

namespace lt {

        class DefaultConfiguration;

        /** 
         * This is the default <code>Configuration</code> implementation.
         *
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class DefaultConfiguration : public AbstractConfiguration
        {

            public:

            /**
             * Construct a configuration with given name, location and
             * namespace.
             */
            DefaultConfiguration( const QString & name, 
                    const QString & location = "",
                    const QString & ns = "", const QString & prefix = "" );

            /**
             * Destructor.
             */
            virtual ~DefaultConfiguration();

            /**
             * Copy Constructor.
             */
            DefaultConfiguration (
                    const DefaultConfiguration & other );

            /**
             * Assignment operator.
             */
            DefaultConfiguration & operator = (
                    const DefaultConfiguration & other );

            /**
             * Return the name of the node.
             *
             * @return name of the <code>Configuration</code> node.
             */
            virtual QString getName() const;

            /**
             * Return a string describing location of Configuration.
             * Location can be different for different mediums (ie "file:line"
             * for normal XML files or "table:primary-key" for DB based
             * configurations);
             *
             * @return a string describing location of Configuration
             */
            virtual QString getLocation() const;

            /**
             * Returns a string indicating which namespace this Configuration
             * node belongs to.
             *
             * <p>
             * What this returns is dependent on the configuration file and
             * the Configuration builder. If the Configuration builder does
             * not support namespaces, this method will return a blank string.
             * </p>
             * <p>In the case of {@link DefaultConfigurationBuilder}, the
             * namespace will be the URI associated with the XML element.
             * Eg.,:</p>
             * <pre>
             * &lt;foo xmlns:x="http://blah.com"&gt;
             *   &lt;x:bar/&gt;
             * &lt;/foo&gt;
             * </pre>
             * <p>The namespace of <code>foo</code> will be "", and the
             * namespace of <code>bar</code> will be "http://blah.com".</p>
             *
             * @return a String identifying the namespace of this
             * Configuration.
             */
            virtual QString getNamespace() const;

            /**
             * Return the prefix assigned to this node's namespace.
             */
            virtual QString getPrefix() const;

            /**
             * Return an <code>Array</code> of <code>Configuration</code>
             * elements containing all node children with the specified name.
             * The array order will reflect the order in the source config
             * file.
             *
             * @param name The name of the children to get.
             * @return The child nodes with name <code>name</code>
             * @pre name != null
             * @post getConfigurations() != null
             *
             */
            virtual ConfigurationList getChildren( 
                    const QString & name = "" ) const;

            /**
             * Return a <code>Configuration</code> instance encapsulating the
             * specified child node.
             *
             * @param child The name of the child node.
             * @param createNew If <code>true</code>, a new
             * <code>Configuration</code> will be created and returned.
             * If <code>false</code> (default),
             * <code>null</code> will be returned when the specified
             * child doesn't exist.
             * @return Configuration
             */
            virtual const ConfigurationPtr getChild( const QString & child, 
                    bool createNew = false) const;

            /**
             * Return an array of all attribute names.  <p> <em>The order of
             * attributes in this array can not be relied on.</em> As with
             * XML, a <code>Configuration</code>'s attributes are an
             * <em>unordered</em> set. If your code relies on order, eg
             * <tt>conf.getAttributeNames()[0]</tt>, then it is liable to
             * break if a different XML parser is used.
             * </p>
             * @return a <code>String[]</code> value
             */
            virtual const QStringList getAttributeNames() const;

            /**
             * Returns the value of the configuration element as a
             * <code>String</code>.
             * If the configuration value is not set, the default value will
             * be used.
             *
             * @param defaultValue The default value desired.
             * @return String value of the <code>Configuration</code>, or
             * default if none specified.
             */
            virtual QString getValue( const QString & defaultValue = "" ) const;

            /**
             * Returns the value of the attribute specified by its name as a
             * <code>String</code>, or the default value if no attribute by
             * that name exists or is empty.
             *
             * @param name The name of the attribute you ask the value of.
             * @param defaultValue The default value desired.
             * @return String value of attribute. It will return the default
             *         value if the named attribute does not exist, or if
             *         the value is not set.
             */
            virtual QString getAttribute( const QString & name, 
                    const QString & defaultValue = "" ) const;

            /**
             * Make this configuration read only.
             */
            virtual void makeReadOnly();

            /**
             * Am I writable, e.g., non-readonly Configuration ?
             */
            virtual bool isWriteable() const;

            /**
             * Add a child <code>Configuration</code> to this configuration
             * element.
             * @param configuration a <code>Configuration</code> value
             */
            virtual bool addChild( ConfigurationPtr configuration );

            /**
             * Remove a child <code>Configuration</code> from this 
             * configuration element.
             * @param configuration a <code>Configuration</code> value
             */
            virtual bool removeChild( const ConfigurationPtr configuration );

            /**
             * Set the value of the specified attribute to 
             * the specified string.
             *
             * @param name name of the attribute to set
             * @param value a <code>String</code> value
             */
            virtual bool setAttribute( const QString & name, 
                    const QString & value );

            /**
             * Set the value of this <code>Configuration</code> object 
             * to the specified string.
             *
             * @param value a <code>String</code> value
             */
            virtual bool setValue( const QString & value );
 
            /**
             * set the location of this Configuration, returns false if not
             * writable.
             */
            virtual bool setLocation ( const QString & location );

            /**
             * set the location of this Configuration, returns false if not
             * writable.
             */
            virtual bool setPrefix ( const QString & prefix );

            /**
             * set the namespace of this Configuration, returns false if not
             * writable.
             */
            virtual bool setNamespace ( const QString & ns );

            /**
             * set the name of this Configuration, returns false if not
             * writable.
             */
            virtual bool setName ( const QString & name );

            private:

            struct DefaultConfigurationPrivate;
            DefaultConfigurationPrivate *d;

        };

} // namespace lt

#endif /* _DEFAULTCONFIGURATION_HH */

// vim: set et ts=4 sw=4:
// $Id: DefaultConfiguration.hh,v 1.2 2003/11/15 18:14:40 hynek Exp $
