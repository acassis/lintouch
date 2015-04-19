// $Id: Configuration.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
//
//   FILE: Configuration.hh -- 
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

#ifndef _CONFIGURATION_HH
#define _CONFIGURATION_HH

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include "lt/SharedPtr.hh"

namespace lt {

        class Configuration;

        /**
         * predefined type of SharedPtr to Configuration
         */
        typedef SharedPtr<Configuration> ConfigurationPtr;
        /**
         * predefined type to hold list of Configurations
         */
        typedef QValueList <ConfigurationPtr> ConfigurationList;

        /** 
         * <code>Configuration</code> is a interface encapsulating a
         * configuration node used to retrieve configuration values.
         *
         * <p>
         * This is a "read only" interface preventing applications from
         * modifying their own configurations. Once it is created, the
         * information never changes.
         * </p>
         * <h3>Data Model</h3>
         * <p>
         * The data model is a subset of XML's; a single-rooted hierarchical
         * tree where each node can contain multiple <em>attributes</em>, and
         * leaf nodes can also contain a <em>value</em>. Reflecting this,
         * <code>Configuration</code>s are usually built from an XML file by
         * the {@link DefaultConfigurationBuilder} class, or directly by a SAX
         * parser using a {@link SAXConfigurationHandler} or {@link
         * NamespacedSAXConfigurationHandler} event handler.
         * </p>
         *
         * @author <a href="mailto:fede@apache.org">Federico Barbieri</a>
         * @author <a href="mailto:pier@apache.org">Pierpaolo Fumagalli</a>
         * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
         * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:jefft@apache.org">Jeff Turner</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class Configuration 
        {

            public:
            /**
             * Return the name of the node.
             *
             * @return name of the <code>Configuration</code> node.
             */
            virtual QString getName() const = 0;

            /**
             * Return a string describing location of Configuration.
             * Location can be different for different mediums (ie "file:line"
             * for normal XML files or "table:primary-key" for DB based
             * configurations);
             *
             * @return a string describing location of Configuration
             */
            virtual QString getLocation() const = 0;

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
            virtual QString getNamespace() const = 0;

            /**
             * Return a prefix assigned to this node's namespace.
             */
            virtual QString getPrefix() const = 0;

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
                    bool createNew = false) const = 0;

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
                    const QString & name = "" ) const = 0;

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
            virtual const QStringList getAttributeNames() const = 0;

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
            virtual QString getValue( 
                    const QString & defaultValue = "" ) const = 0;

            /**
             * Returns the value of the configuration element as an
             * <code>int</code>.
             * If the configuration value is not set, the default value will
             * be used.
             *
             * @param defaultValue The default value desired.
             * @return int value of the <code>Configuration</code>, or default
             * if none specified.
             */
            virtual int getValueAsInt( int defaultValue = 0 ) const = 0;

            /**
             * Returns the value of the configuration element as an
             * <code>uint</code>.
             * If the configuration value is not set, the default value will
             * be used.
             *
             * @param defaultValue The default value desired.
             * @return int value of the <code>Configuration</code>, or default
             * if none specified.
             */
            virtual unsigned int getValueAsUInt( 
                    unsigned int defaultValue = 0 ) const = 0;

            /**
             * Returns the value of the configuration element as a
             * <code>long</code>.
             * If the configuration value is not set, the default value will
             * be used.
             *
             * @param defaultValue The default value desired.
             * @return long value of the <code>Configuration</code>, or default
             *          if none specified.
             */
            virtual long getValueAsLong( long defaultValue = 0) const = 0;

            /**
             * Returns the value of the configuration element as a
             * <code>ulong</code>.
             * If the configuration value is not set, the default value will
             * be used.
             *
             * @param defaultValue The default value desired.
             * @return long value of the <code>Configuration</code>, or default
             *          if none specified.
             */
            virtual unsigned long getValueAsULong( 
                    unsigned long defaultValue = 0) const = 0;

            /**
             * Returns the value of the configuration element as a
             * <code>float</code>.
             * If the configuration value is not set, the default value will
             * be used.
             *
             * @param defaultValue The default value desired.
             * @return float value of the <code>Configuration</code>, or
             * default if none specified.
             */
            virtual float getValueAsFloat( float defaultValue = 0.0 ) const = 0;

            /**
             * Returns the value of the configuration element as a
             * <code>boolean</code>.
             * If the configuration value is not set, the default value will
             * be used.
             *
             * @param defaultValue The default value desired.
             * @return boolean value of the <code>Configuration</code>, or
             * default if none specified.
             */
            virtual bool getValueAsBool( bool defaultValue = false ) const = 0;

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
                    const QString & defaultValue = "" ) const = 0;

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
                    int defaultValue = 0) const = 0;

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
                    long defaultValue = 0 ) const = 0;

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
                    float defaultValue = 0.0 ) const = 0;

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
                    bool defaultValue = false ) const = 0;

            /**
             * Make this configuration read only.
             */
            virtual void makeReadOnly() = 0;

            /**
             * Am I writable, e.g., non-readonly Configuration ?
             */
            virtual bool isWriteable () const = 0;

            /**
             * set the location of this Configuration, returns false if not
             * writable.
             */
            virtual bool setLocation ( const QString & location ) = 0;

            /**
             * set the location of this Configuration, returns false if not
             * writable.
             */
            virtual bool setPrefix ( const QString & prefix ) = 0;

            /**
             * set the namespace of this Configuration, returns false if not
             * writable.
             */
            virtual bool setNamespace ( const QString & ns ) = 0;

            /**
             * set the name of this Configuration, returns false if not
             * writable.
             */
            virtual bool setName ( const QString & name ) = 0;

            /**
             * set the value of this Configuration, returns false if not
             * writable.
             */
            virtual bool setValue ( const QString & value ) = 0;

            /**
             * Set the value of the specified attribute to 
             * the specified string. returns false if not writable.
             *
             * @param name name of the attribute to set
             * @param value a <code>String</code> value
             */
            virtual bool setAttribute( const QString & name, 
                    const QString & value ) = 0;

            /**
             * Add a child <code>Configuration</code> to this configuration
             * element. returns false when not writtable.
             * @param configuration a <code>Configuration</code> value
             */
            virtual bool addChild( ConfigurationPtr configuration ) = 0;

            /**
             * Remove a child <code>Configuration</code> from this 
             * configuration element. returns false when not writtable.
             * @param configuration a <code>Configuration</code> value
             */
            virtual bool removeChild( const ConfigurationPtr configuration ) = 0;

            /**
             * Destructor
             */
            virtual ~Configuration () {};
        };

} // namespace lt

#endif /* _CONFIGURATION_HH */

// vim: set et ts=4 sw=4:
// $Id: Configuration.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
