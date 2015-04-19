// $Id: Property.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Property.cpp -- 
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

#include "lt/templates/Property.hh"
using namespace lt;

#include <qregexp.h>

/*
 * Pen styles
 */
struct _PenStyles
{
    Qt::PenStyle style;
    char * representation;
};

static const _PenStyles _penStyles [] = {
    { Qt::NoPen , "NOPEN" },
    { Qt::DashLine , "DASHLINE" },
    { Qt::DotLine , "DOTLINE" },
    { Qt::DashDotLine , "DASHDOTLINE" },
    { Qt::DashDotDotLine , "DASHDOTDOTLINE" },
    { Qt::MPenStyle , "MPENSTYLE" },
    { Qt::SolidLine , "SOLIDLINE" },
};

/*
 * Brush styles
 */
struct _BrushStyles
{
    Qt::BrushStyle style;
    char * representation;
};

static const _BrushStyles _brushStyles [] = {
    { Qt::SolidPattern , "SOLIDPATTERN" },
    { Qt::Dense1Pattern , "DENSE1PATTERN" },
    { Qt::Dense2Pattern , "DENSE2PATTERN" },
    { Qt::Dense3Pattern , "DENSE3PATTERN" },
    { Qt::Dense4Pattern , "DENSE4PATTERN" },
    { Qt::Dense5Pattern , "DENSE5PATTERN" },
    { Qt::Dense6Pattern , "DENSE6PATTERN" },
    { Qt::Dense7Pattern , "DENSE7PATTERN" },
    { Qt::HorPattern , "HORPATTERN" },
    { Qt::VerPattern , "VERPATTERN" },
    { Qt::CrossPattern , "CROSSPATTERN" },
    { Qt::BDiagPattern , "BDIAGPATTERN" },
    { Qt::FDiagPattern , "FDIAGPATTERN" },
    { Qt::DiagCrossPattern , "DIAGCROSSPATTERN" },
    { Qt::CustomPattern , "CUSTOMPATTERN" },
    { Qt::NoBrush , "NOBRUSH" },
};

/*
 * Font weights
 */
struct _FontWeights
{
    QFont::Weight weight;
    char * representation;
};

static const _FontWeights _fontWeights [] = {
    { QFont::Light , "LIGHT" },
    { QFont::DemiBold , "DEMIBOLD" },
    { QFont::Bold , "BOLD" },
    { QFont::Black , "BLACK" },
    { QFont::Normal , "NORMAL" },
};

struct _Types
{
    Property::Type type;
    char * representation;
};

static const _Types _types [] = {
    // we go through the list sequentially, so be careful not to put first
    // entries that can match using QString::find before the later ones
    { Property::IntegerType, "integer" },
    { Property::FloatingPointType, "float" },
    { Property::StringType, "string" },
    { Property::ChoiceType, "choice" },
    { Property::PointType, "point" },
    { Property::RectType, "rect" },
    { Property::FontType, "font" },
    { Property::ColorType, "color" },
    { Property::PenType, "pen" },
    { Property::BrushType, "brush" },
    { Property::ResourceType, "resource" },
    { Property::LinkType, "link" },
    { Property::InvalidType, "" },
};

struct Property::PropertyPrivate
{

    PropertyPrivate() :
        type( Property::InvalidType ), isEnabled( true ), isDefault( true ),
        integerValue( 0 ), floatValue( 0.0 ), source( NULL ) {}

    QString label;
    Property::Type type;

    bool isEnabled;

    bool isDefault;
    QString defaultValue;

    long integerValue;
    double floatValue;

    QString stringValue;
    QStringList choices;

    QPoint pointValue;
    QRect rectValue;

    QFont fontValue;
    QPen penValue;
    QBrush brushValue;

    // when non-null, we are shadow and take the value from d->source as long
    // as we are isDefault
    const Property * source;

};

Property::Property( const QString & label, const QString & type,
        const QString & defValue,
        LoggerPtr logger /* = Logger::nullPtr() */ ) :
    d( new PropertyPrivate() )
{

    if( logger != Logger::nullPtr() ) {
        enableLogging( logger );
    }

    d->label = label;

    // try to parse type
    d->type = Property::decodeType( type, logger );

    // special case is choice property, remove leading choice( and
    // trailing ) and split into strings
    if( d->type == Property::ChoiceType ) {
        QString ch = type.mid( 7 ).remove( ")" );
        d->choices = QStringList::split( ",", ch );
        if( d->choices.count() > 0 ) {
            d->defaultValue = *d->choices.begin();
            d->stringValue = d->defaultValue;
        }
    }

    // try to parse default value according to given type
    if( ! defValue.isEmpty() ) {
        decodeValue( defValue );
    }

    // now encode the default back according to type/value parsing result
    d->defaultValue = encodeValue();
    // and set that we are at default value
    d->isDefault = true;
}

Property::Property( const QString & label, const Property * source,
        LoggerPtr logger /* = Logger::nullPtr() */ ) :
    d( new PropertyPrivate() )
{

    if( logger != Logger::nullPtr() ) {
        enableLogging( logger );
    }

    // when shadow property has been requested but no property has been
    // given, we pretend we are invalid
    if( source == NULL ) {
        d->type = Property::InvalidType;
    } else {
        // remember all values from d->source
        *d = *(source->d);

        // set the correct source
        d->source = source;

        // and set that we are at default value
        d->isDefault = true;
    }

    d->label = label;
}

Property::~Property()
{
    delete d;
}

Property::Type Property::decodeType(
    const QString & type, LoggerPtr logger )
{

    QString t = type.lower();
    for( unsigned i = 0;
            _types [ i ].type != Property::InvalidType; i ++ ) {
        // test from left in order to support choice(aaa,bbb,ccc) syntax
        // instead of simply using ==
        if( t.find( _types [ i ].representation ) == 0 ) {
            return _types [ i ].type;
        }
    }

    if( logger->isErrorEnabled() ) {
        logger->error(
                QString( "Invalid type of property specified: '%1'" )
                .arg( type ) );

        logger->error( QString( "Supported property types are: %1" )
                .arg( supportedTypes().join( ", " ) ) );
    }

    return Property::InvalidType;
}

QStringList Property::supportedTypes()
{
    static QStringList supportedTypes;
    static bool initialized = 0;

    if( ! initialized ) {
        initialized = 1;
        for( unsigned i = 0; 
                _types [ i ].type != Property::InvalidType; i ++ ) {
            supportedTypes.append( _types [ i ].representation );
        }
    }

    return supportedTypes;
}

bool Property::isValid() const
{
    return d->type != Property::InvalidType;
}

bool Property::isShadow() const
{
    return d->source != NULL;
}

bool Property::isEnabled() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->isEnabled();
    }
    return d->isEnabled;
}

void Property::setEnabled( bool enabled )
{
    d->isEnabled = enabled;
}

QString Property::label() const
{
    return d->label;
}

Property::Type Property::type() const
{
    return d->type;
}

QString Property::encodeType() const
{

    for( unsigned i = 0; 
            _types [ i ].type != Property::InvalidType; i ++ ) {
        if( d->type == _types [ i ].type ) {
            //special case is choice property
            if( _types [ i ].type == Property::ChoiceType ) {
                QString result = _types [ i ].representation;
                result += "(" + d->choices.join( "," ) + ")";
                return result;
            }
            return _types [ i ].representation;
        }
    }

    return QString::null;
}

bool Property::decodeValue( const QString & value )
{

    bool converted_ok = false;

    d->isDefault = false;

    switch( d->type ) {

        case Property::IntegerType:
            {
                long tmp = value.toLong( &converted_ok );
                if( converted_ok ) {
                    d->integerValue = tmp;
                    return true;
                } else if( getLogger().isErrorEnabled() ) {
                    getLogger().error( QString(
                                "Invalid value of integer property specified:"
                                " '%1'" ).arg( value ) );
                }
                return false;
            }

        case Property::FloatingPointType:
            {
                double tmp = value.toDouble( &converted_ok );
                if( converted_ok ) {
                    d->floatValue = tmp;
                    return true;
                } else if( getLogger().isErrorEnabled() ) {
                    getLogger().error( QString(
                                "Invalid value of float property specified:"
                                " '%1'" ).arg( value ) );
                }
                return false;
            }

        case Property::StringType:
        case Property::ResourceType:
        case Property::LinkType:
            d->stringValue = value;
            return true;

        case Property::ChoiceType:
            if( d->choices.contains( value ) ) {
                d->stringValue = value;
                return true;
            } else if( getLogger().isErrorEnabled() ) {
                getLogger().error( QString(
                            "Invalid value of choice property specified:"
                            " '%1'" ).arg( value ) );
                getLogger().error( QString(
                            "Supported choices are:"
                            " %1" ).arg( d->choices.join( ", " ) ) );
            }
            return false;

        case Property::PointType:
            {
                QPoint tmp = pointFromString(value, &converted_ok );
                if( converted_ok ) {
                    d->pointValue = tmp;
                    return true;
                } else if( getLogger().isErrorEnabled() ) {
                    getLogger().error( QString(
                                "Invalid value of point property specified:"
                                " '%1'" ).arg( value ) );
                }
                return false;
            }

        case Property::RectType:
            {
                QRect tmp = rectFromString( value, &converted_ok );
                if( converted_ok ) {
                    d->rectValue = tmp;
                    return true;
                } else if( getLogger().isErrorEnabled() ) {
                    getLogger().error( QString(
                                "Invalid value of rect property specified:"
                                " '%1'" ).arg( value ) );
                }
                return false;
            }

        case Property::ColorType:
            {
                QPen tmp = QPen( colorFromString( value, &converted_ok ) );
                if( converted_ok ) {
                    d->penValue = tmp;
                    return true;
                } else if( getLogger().isErrorEnabled() ) {
                    getLogger().error( QString(
                                "Invalid value of color property specified:"
                                " '%1'" ).arg( value ) );
                }
                return false;
            }

        case Property::PenType:
            {
                QPen tmp = penFromString( value, &converted_ok );
                if( converted_ok ) {
                    d->penValue = tmp;
                    return true;
                } else if( getLogger().isErrorEnabled() ) {
                    getLogger().error( QString(
                                "Invalid value of pen property specified:"
                                " '%1'" ).arg( value ) );
                }
                return false;
            }

        case Property::BrushType:
            {
                QBrush tmp = brushFromString( value, &converted_ok );
                if( converted_ok ) {
                    d->brushValue = tmp;
                    return true;
                } else if( getLogger().isErrorEnabled() ) {
                    getLogger().error( QString(
                                "Invalid value of brush property specified:"
                                " '%1'" ).arg( value ) );
                }
                return false;
            }

        case Property::FontType:
            {
                QFont tmp = fontFromString( value, &converted_ok );
                if( converted_ok ) {
                    d->fontValue = tmp;
                    return true;
                } else if( getLogger().isErrorEnabled() ) {
                    getLogger().error( QString(
                                "Invalid value of font property specified:"
                                " '%1'" ).arg( value ) );
                }
                return false;
            }

        default:
            // for invalid type we stay all the time default
            d->isDefault = true;
            break;
    }

    return false;
}

QString Property::encodeValue() const
{

    // if we are shadow property and we stay on default, we encode itself as
    // the empty string
    if( d->source != NULL && d->isDefault ) {
        return QString::null;
    }

    switch( d->type ) {

        case Property::IntegerType:
            return QString( "%1" ).arg( d->integerValue );

        case Property::FloatingPointType:
            return QString( "%1" ).arg( d->floatValue );

        case Property::StringType:
        case Property::ResourceType:
        case Property::LinkType:
            return QString( "%1" ).arg( d->stringValue );

        case Property::ChoiceType:
            return QString( "%1" ).arg( d->stringValue );

        case Property::PointType:
            return pointToString( d->pointValue );

        case Property::RectType:
            return rectToString( d->rectValue );

        case Property::PenType:
            return penToString( d->penValue );

        case Property::ColorType:
            return colorToString( d->penValue.color() );

        case Property::BrushType:
            return brushToString( d->brushValue );

        case Property::FontType:
            return fontToString( d->fontValue );

        default:
            break;
    }

    return QString::null;
}

QString Property::encodeDefaultValue() const
{
    if( d->source != NULL) {
        return d->source->d->defaultValue;
    }
    return d->defaultValue;
}

void Property::resetToDefault()
{
    if( d->source == NULL ) {
        decodeValue( d->defaultValue );
    }
    d->isDefault = true;
}

bool Property::isDefault() const
{
    return d->isDefault;
}

long Property::asInteger() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asInteger();
    }
    return d->integerValue;
}

double Property::asFloatingPoint() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asFloatingPoint();
    }
    return d->floatValue;
}

QString Property::asString() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asString();
    }
    return d->stringValue;
}

QStringList Property::asChoice() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asChoice();
    }
    return d->choices;
}

QPoint Property::asPoint() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asPoint();
    }
    return d->pointValue;
}

QRect Property::asRect() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asRect();
    }
    return d->rectValue;
}

QFont Property::asFont() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asFont();
    }
    return d->fontValue;
}

QColor Property::asColor() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asColor();
    }
    return d->penValue.color();
}

QPen Property::asPen() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asPen();
    }
    return d->penValue;
}

QBrush Property::asBrush() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asBrush();
    }
    return d->brushValue;
}

QString Property::asResource() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asResource();
    }
    return d->stringValue;
}

QString Property::asLink() const
{
    if( d->isDefault && d->source != NULL) {
        return d->source->asLink();
    }
    return d->stringValue;
}

void Property::setIntegerValue( int val )
{
    d->isDefault = false;
    d->integerValue = val;
}

void Property::setFloatingPointValue( double val )
{
    d->isDefault = false;
    d->floatValue = val;
}

void Property::setStringValue( const QString& val )
{
    d->isDefault = false;
    d->stringValue = val;
}

void Property::setChoiceValue( const QString& val )
{
    d->isDefault = false;
    if( d->choices.contains( val ) ) {
        d->stringValue = val;
    }
}

void Property::setPointValue( const QPoint& val )
{
    d->isDefault = false;
    d->pointValue = val;
}

void Property::setRectValue( const QRect& val )
{
    d->isDefault = false;
    d->rectValue = val;
}

void Property::setFontValue( const QFont& val )
{
    d->isDefault = false;
    d->fontValue = val;
}

void Property::setColorValue( const QColor& val )
{
    d->isDefault = false;
    d->penValue = QPen( val );
}

void Property::setPenValue( const QPen& val )
{
    d->isDefault = false;
    d->penValue = val;
}

void Property::setBrushValue( const QBrush& val )
{
    d->isDefault = false;
    d->brushValue = val;
}

void Property::setResourceValue( const QString& val )
{
    d->isDefault = false;
    d->stringValue = val;
}

void Property::setLinkValue( const QString& val )
{
    d->isDefault = false;
    d->stringValue = val;
}

// static public helper methods
QColor Property::colorFromString( const QString& value, bool * ok )
{

    if( value.isNull() || value.isEmpty() ) {
        if( ok ) *ok = true;
        return QColor( Qt::black );
    }

    static QRegExp penReg( "^(#[a-fA-F0-9]{6})$" );

    if( ! penReg.exactMatch( value.upper() ) ) {
        if( ok ) *ok = false;
        return QColor( Qt::black );
    }

    if( ok ) *ok = true;
    return QColor( penReg.cap(1) );
}

QString Property::colorToString( const QColor& value )
{
    return QString("%1")
        .arg( value.name().upper() );
}

QPen Property::penFromString( const QString& value, bool * ok )
{

    if( value.isNull() || value.isEmpty() ) {
        if( ok ) *ok = true;
        return QPen( Qt::SolidLine );
    }

    static QRegExp penReg( "^(#[a-fA-F0-9]{6});(\\d+);(\\w+)$" );

    if( ! penReg.exactMatch( value.upper() ) ) {
        if( ok ) *ok = false;
        return QPen( Qt::SolidLine );
    }

    int i = 0;
    for( ; _penStyles [i].style != Qt::SolidLine &&
        _penStyles [i].representation != penReg.cap(3); ++i ) {}

    //FIXME: check toInt conversion
    if( ok ) *ok = true;
    return QPen( QColor( penReg.cap(1) ), penReg.cap(2).toInt(),
            _penStyles [i].style );
}

QString Property::penToString( const QPen& value )
{

    int i = 0;
    for( ; _penStyles [i].style != Qt::SolidLine &&
            _penStyles [i].style != value.style(); ++i ) {}

    return QString("%1;%2;%3")
        .arg( value.color().name().upper() )
        .arg( value.width() )
        .arg( _penStyles [i].representation );
}

QBrush Property::brushFromString( const QString& value, bool * ok )
{

    if( value.isNull() || value.isEmpty() ) {
        if( ok ) *ok = true;
        return QBrush( Qt::NoBrush );
    }

    static QRegExp brushReg( "^(#[a-fA-F0-9]{6});(\\w+)$" );

    if( ! brushReg.exactMatch( value.upper() ) ) {
        if( ok ) *ok = false;
        return QBrush( Qt::NoBrush );
    }

    int i = 0;
    for( ; _brushStyles [i].style != Qt::NoBrush &&
        _brushStyles [i].representation != brushReg.cap(2); ++i ) {}

    if( ok ) *ok = true;
    return QBrush( QColor( brushReg.cap(1) ), _brushStyles [i].style );
}

QString Property::brushToString( const QBrush& value )
{

    int i = 0;
    for( ; _brushStyles [i].style != Qt::NoBrush &&
            _brushStyles [i].style != value.style(); ++i ) {}

    return QString("%1;%2")
        .arg( value.color().name().upper() )
        .arg( _brushStyles [i].representation );
}

QFont Property::fontFromString( const QString& value, bool * ok )
{

    if( value.isNull() || value.isEmpty() ) {
        if( ok ) *ok = true;
        return QFont();
    }

    // short/full: Verdana;12;Bold;Italic
    // short: Verdana;12;Bold;
    static QRegExp fontReg( "^([^;]+);(\\d+);(\\w+);?(\\w+)?$" );

    if( ! fontReg.exactMatch( value.upper() ) ) {
        if( ok ) *ok = false;
        return QFont();
    }

    int i = 0;
    for( ; _fontWeights [i].weight != QFont::Normal &&
        _fontWeights [i].representation != fontReg.cap(3); ++i ) {}

    //FIXME: check toInt conversion
    QFont f( fontReg.cap(1) );

    f.setPointSize( fontReg.cap(2).toInt() );

    f.setWeight( _fontWeights [i].weight );

    if( fontReg.cap(4) == "ITALIC" ) {
        f.setItalic( true );
    }

    if( ok ) *ok = true;
    return f;
}

QString Property::fontToString( const QFont& value )
{

    // always return long and precise description
    int i = 0;
    for( ; _fontWeights [i].weight != QFont::Normal &&
        _fontWeights [i].weight != value.weight(); ++i ) {}

    // Qte needs pointSize
    return QString("%1;%2;%3%4")
        .arg(value.family())
        .arg(value.pointSize())
        .arg(_fontWeights[i].representation)
        .arg((value.italic()?";ITALIC":""));
}

QRect Property::rectFromString( const QString& value, bool * ok )
{

    if( value.isNull() || value.isEmpty() ) {
        if( ok ) *ok = true;
        return QRect();
    }

    static QRegExp 
        rectReg( "^([+-]?\\d+);([+-]?\\d+);?([+-]?\\d+)?;?([+-]?\\d+)?$" );

    if( ! rectReg.exactMatch( value ) ) {
        if( ok ) *ok = false;
        return QRect();
    }

    if( !rectReg.cap(3).isEmpty() ) {
        //FIXME: check toInt conversion(see pointFromString)
        if( ok ) *ok = true;
        return QRect( QPoint( rectReg.cap(1).toInt(), rectReg.cap(2).toInt() ),
            QSize( rectReg.cap(3).toInt(), rectReg.cap(4).toInt() ) );
    } else if( !rectReg.cap(1).isEmpty() ) {
        //FIXME: check toInt conversion(see pointFromString)
        if( ok ) *ok = true;
        return QRect( QPoint( 0, 0 ),
            QSize( rectReg.cap(1).toInt(), rectReg.cap(2).toInt() ) );
    } else {
        if( ok ) *ok = false;
        return QRect();
    }
}

QString Property::rectToString( const QRect& value )
{

    return QString( "%1;%2;%3;%4" )
        .arg( value.left() ).arg( value.top() )
        .arg( value.width() ).arg( value.height() );
}

QPoint Property::pointFromString( const QString& value, bool * ok )
{

    if( value.isNull() || value.isEmpty() ) {
        if( ok ) *ok = true;
        return QPoint();
    }

    static QRegExp pointReg( "^([+-]?\\d+);([+-]?\\d+)$" );

    if( ! pointReg.exactMatch( value ) ) {
        if( ok ) *ok = false;
        return QPoint();
    }

    bool xok = false, yok = false;
    int x = pointReg.cap(1).toInt( &xok );
    int y = pointReg.cap(2).toInt( &yok );
    if( ok ) *ok = xok && yok;
    return QPoint( x, y );
}

QString Property::pointToString( const QPoint& value )
{

    return QString("%1;%2").arg(value.x()).arg(value.y());
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Property.cpp 1168 2005-12-12 14:48:03Z modesto $
