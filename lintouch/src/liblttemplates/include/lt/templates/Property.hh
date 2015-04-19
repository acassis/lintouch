// $Id: Property.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Property.hh -- 
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

#ifndef _PROPERTY_HH
#define _PROPERTY_HH

#include <qbrush.h>
#include <qdict.h>
#include <qfont.h>
#include <qobject.h>
#include <qpen.h>
#include <qstringlist.h>

#include <lt/logger/LogEnabled.hh>

#include <lt/SharedPtr.hh>

#include "lt/LTDict.hh"

namespace lt {

    class Property;

    typedef LTDict <Property> PropertyDict;

    /**
     * Property.
     *
     * Property represents customizable property of a Template. It's
     * defined by its type and some value. Type and value can be
     * decoded/encoded from/to string.
     *
     * There are at this moment the following property types
     * <ul>
     * <li>integer
     * <li>float
     * <li>string
     * <li>choice
     * <li>point
     * <li>rect
     * <li>font
     * <li>color
     * <li>pen
     * <li>brush
     * <li>resource
     * <li>link
     * </ul>
     *
     * You can create two types of properties:
     *
     * Standalone property: which is defined by its type and default value.
     * Value of the standalone property can be changed and user can examine
     * whether the value has been changed of whether the property returns
     * currently its default value.
     *
     * Shadow property: inherits its value from some other property as long
     * as the user does not change the value of shadow property directly.
     *
     * Example:
     *
     * <pre>
     * //standalone property
     * Property fill_off( "brush", "#ff0000;SOLIDPATTERN" );
     *
     * //shadow property
     * Property fill_on( fill_off );
     * </pre>
     *
     * In the previous example the user can change the value of fill_off
     * property and this value will be returned by fill_on property as long
     * as the user does not change the value of fill_on directly. As soon as
     * the user changes the value of fill_on directly, the two properties
     * become independent.
     *
     * This class is pointer based.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class Property : public LogEnabled
    {

        public:

            /**
             * Property type.
             */
            enum Type {
                IntegerType = 1,
                FloatingPointType,
                StringType,
                ChoiceType,

                PointType,
                RectType,

                FontType,
                ColorType,
                PenType,
                BrushType,

                ResourceType,
                LinkType,

                InvalidType = -1,
            };

        public:

            /**
             * Create standalone property of given type.
             *
             * Creates the property of given type and initializes it with
             * given default value.
             *
             * @param label The localized string that describes this
             * property.
             * @param type The type of this property(string encoded).
             * @param defValue The default value of this property
             * (string encoded).
             * @param logger Where to report problems.
             */
            Property( const QString & label,
                    const QString & type,
                    const QString & defValue = "",
                    LoggerPtr logger = Logger::nullPtr() );

            /**
             * Create shadow property.
             *
             * Creates the property whose type and default value is the same
             * as of the given property. This property serves as a shadow
             * for given source property as long as its value is not
             * modified.
             *
             * @param label The localized string that describes this
             * property.
             * @param source The source property.
             * @param logger Where to report problems.
             */
            Property( const QString & label, const Property * source,
                    LoggerPtr logger = Logger::nullPtr() );

            /**
             * Destructor.
             */
            virtual ~Property();

        private:

            /**
             * Disable unintentional copying.
             */
            Property( const Property & p );
            Property & operator =( const Property & p );

        public:

            /**
             * Try to parse string representation of a type into
             * appropriate enum code.
             */
            static Type decodeType( const QString & type,
                    LoggerPtr logger = Logger::nullPtr() );

            /**
             * return the list of(encoded) supported property types.
             */
            static QStringList supportedTypes();

            /**
             * return QColor decoded from given QString.
             */
            static QColor colorFromString( const QString& value,
                    bool * ok = NULL );

            /**
             * return QString representation of given QColor.
             */
            static QString colorToString( const QColor& value );

            /**
             * return QPen decoded from given QString.
             */
            static QPen penFromString( const QString& value,
                    bool * ok = NULL );

            /**
             * return QString representation of given QPen.
             */
            static QString penToString( const QPen& value );

            /**
             * return QBrush decoded from given QString.
             */
            static QBrush brushFromString( const QString& value,
                    bool * ok = NULL );

            /**
             * return QString representation of given QBrush.
             */
            static QString brushToString( const QBrush& value );

            /**
             * return QFont decoded from given QString.
             */
            static QFont fontFromString( const QString& value,
                    bool * ok = NULL );

            /**
             * return QString representation of given QFont.
             */
            static QString fontToString( const QFont& value );

            /**
             * return QRect decoded from given QString.
             */
            static QRect rectFromString( const QString& value,
                    bool * ok = NULL );

            /**
             * return QString representation of given QRect.
             */
            static QString rectToString( const QRect& value );

            /**
             * return QPoint decoded from given QString.
             */
            static QPoint pointFromString( const QString& value,
                    bool * ok = NULL );

            /**
             * return QString representation of given QPoint.
             */
            static QString pointToString( const QPoint& value );

        public:

            /**
             * Is the type of the property correct ???
             */
            bool isValid() const;

            /**
             * Is this the shadow property ???
             */
            bool isShadow() const;

            /**
             * Is this property enabled. Can be used by GUI to
             * enable/disable editing of property value. Properties are
             * enabled by default. Use setEnabled() to change the flag.
             *
             * If this is a shadow property at default value, we are enabled
             * if our source is enabled.
             */
            bool isEnabled() const;

            /**
             * Change the ``enabled`` flag of this property.
             */
            void setEnabled( bool enabled );

            /**
             * Return the localized label of this Property.
             */
            QString label() const;

            /**
             * Return the type of this Property.
             */
            Type type() const;

            /**
             * Encode the type of this property as a string.
             */
            QString encodeType() const;

            /**
             * Try to set the value of this property by parsing a given
             * string.
             */
            bool decodeValue( const QString & value );

            /**
             * Encode the value of this property as a string.
             */
            QString encodeValue() const;

            /**
             * Encode the default value of this property as a string.
             */
            QString encodeDefaultValue() const;

            /**
             * Reset the value of this property to default value.
             */
            void resetToDefault();

            /**
             * Is the value of this property the default one ???
             */
            bool isDefault() const;

            /**
             * Return the value of this property as an integer.
             */
            long asInteger() const;

            /**
             * Return the value of this property as a floating point.
             */
            double asFloatingPoint() const;

            /**
             * Return the value of this property as a string.
             */
            QString asString() const;

            /**
             * Return the value of this property as list of choices.
             * Note that the currently selected value from the list of
             * choices is available by calling asString().
             */
            QStringList asChoice() const;

            /**
             * Return the value of this property as a QPoint.
             */
            QPoint asPoint() const;

            /**
             * Return the value of this property as a QRect.
             */
            QRect asRect() const;

            /**
             * Return the value of this property as a QFont.
             */
            QFont asFont() const;

            /**
             * Return the value of this property as a QColor.
             */
            QColor asColor() const;

            /**
             * Return the value of this property as a QPen.
             */
            QPen asPen() const;

            /**
             * Return the value of this property as a QBrush.
             */
            QBrush asBrush() const;

            /**
             * Return the value of this property as a Resource Name.
             */
            QString asResource() const;

            /**
             * Return the value of this property as a Link Name.
             */
            QString asLink() const;

            /**
             * Set new value of type int.
             */
            void setIntegerValue( int val );

            /**
             * Set new value of type double.
             */
            void setFloatingPointValue( double val );

            /**
             * Set new value of type QString.
             */
            void setStringValue( const QString& val );

            /**
             * Set new value of type choice.
             */
            void setChoiceValue( const QString& val );

            /**
             * Set new value of type QPoint.
             */
            void setPointValue( const QPoint& val );

            /**
             * Set new value of type QRect.
             */
            void setRectValue( const QRect& val );

            /**
             * Set new value of type QFont.
             */
            void setFontValue( const QFont& val );

            /**
             * Set new value of type QColor.
             */
            void setColorValue( const QColor& val );

            /**
             * Set new value of type QPen.
             */
            void setPenValue( const QPen& val );

            /**
             * Set new value of type QBrush.
             */
            void setBrushValue( const QBrush& val );

            /**
             * Set new value of type Resource.
             */
            void setResourceValue( const QString& val );

            /**
             * Set new value of type Link.
             */
            void setLinkValue( const QString& val );

        private:

            typedef struct PropertyPrivate;
            PropertyPrivate * d;

    };

} // namespace lt

#endif /* _PROPERTY_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Property.hh 1168 2005-12-12 14:48:03Z modesto $
