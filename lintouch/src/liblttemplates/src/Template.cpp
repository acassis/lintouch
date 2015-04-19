// $Id: Template.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: Template.cpp --
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

#include <qapplication.h>

#include "lt/templates/Template.hh"
#include "lt/templates/TemplateManager.hh"
using namespace lt;

struct Template::TemplatePrivate
{

    TemplatePrivate() :
        rect( 0, 0, 10, 10 ), virtualRect( 0, 0, 10, 10 )
    {
        nestedTemplates.setAutoDelete( true );
    }

    ~TemplatePrivate()
    {
    }

    // our type
    QString type;

    // our library
    QString library;

    // info about us
    Info info;

    // our property groups
    LTMap <PropertyDict> propertyGroups;

    // all of our properties
    PropertyDict properties;

    // dummy properties that are returned when requested property group does
    // not exist
    PropertyDict dummyPG;

    // our iopin groups
    LTMap <IOPinDict> iopinGroups;

    // all of our iopins
    IOPinDict iopins;

    // dummy iopins that are returned when requested iopin group does not
    // exist
    IOPinDict dummyIG;

    // template types requested by constructor by calling
    // requestNestedTemplate
    QMap <QString, QString> requestedNestedTemplates;
    QMap <QString, QString> requestedNestedTemplateLibs;
    // really acquired nested templates.
    TemplateDict nestedTemplates;

    // localizator we use to access localized resources
    LocalizatorPtr localizator;

    // viewAdaptor we use to control our parent view
    ViewAdaptor * viewAdaptor;

    // our rect as set by setRect
    QRect rect;

    // our rect as set by setVirtualRect
    QRect virtualRect;

    // our point area
    QPointArray points;

    // the region where we respond to mouse events
    QRegion collisionRegion;

    // our shortcut
    QKeySequence shortcut;

    // for focusable template, focusing rectangle
    QRect focusRect;
};

Template::Template( const QString & type,
        LoggerPtr logger /* = Logger::nullPtr() */ ) :
    QCanvasPolygonalItem( NULL ), LogEnabled(), d( new TemplatePrivate() )
{

    d->type = type;

    if( logger != Logger::nullPtr() ) {
        enableLogging( logger );
    }

    if( getLogger().isDebugEnabled() ) {
        getLogger().debug( QString( "Template of type %1 created" )
                .arg( d->type ) );
    }
}

Template::~Template()
{
    // now set autodelete on iopins and properties and clear both
    // dictionaries to ensure all properties and iopins are destroyed

    d->properties.setAutoDelete( true );
    d->properties.clear();

    d->iopins.setAutoDelete( true );
    d->iopins.clear();

    if( getLogger().isDebugEnabled() ) {
        getLogger().debug( QString( "Template of type %1 destroyed" )
                .arg( d->type ) );
    }
    delete d;
}

QString Template::type() const
{
    return d->type;
}

QString Template::library() const
{
    return d->library;
}

void Template::setLibrary( const QString & lib )
{
    d->library = lib;
}

const Info & Template::info() const
{
    return d->info;
}

const LTMap <PropertyDict> & Template::propertyGroups() const
{
    return d->propertyGroups;
}

LTMap <PropertyDict> & Template::propertyGroups()
{
    return d->propertyGroups;
}

const PropertyDict & Template::properties(
        const QString & group /* = QString::null */ ) const
{
    if( group == QString::null ) {
        return d->properties;
    }
    if( d->propertyGroups.contains( group ) ) {
        return d->propertyGroups[ group ];
    }
    return d->dummyPG;
}

PropertyDict & Template::properties(
        const QString & group /* = QString::null */ )
{
    if( group == QString::null ) {
        return d->properties;
    }
    if( d->propertyGroups.contains( group ) ) {
        return d->propertyGroups[ group ];
    }
    return d->dummyPG;
}

const LTMap <IOPinDict> & Template::iopinGroups() const
{
    return d->iopinGroups;
}

LTMap <IOPinDict> & Template::iopinGroups()
{
    return d->iopinGroups;
}

const IOPinDict & Template::iopins(
        const QString & group /* = QString::null */ ) const
{
    if( group == QString::null ) {
        return d->iopins;
    }
    if( d->iopinGroups.contains( group ) ) {
        return d->iopinGroups[ group ];
    }
    return d->dummyIG;
}

IOPinDict & Template::iopins(
        const QString & group /* = QString::null */ )
{
    if( group == QString::null ) {
        return d->iopins;
    }
    if( d->iopinGroups.contains( group ) ) {
        return d->iopinGroups[ group ];
    }
    return d->dummyIG;
}

bool Template::registerInfo( const Info & info )
{
    d->info = info;
    return true;
}

bool Template::registerProperty( const QString & group, const QString & name,
        Property * prop )
{
    //first test whether we already have the given group, if not, add it to
    //our data structures
    if( ! d->propertyGroups.contains( group ) ) {
        d->propertyGroups.insert( group, PropertyDict() );
    }

    // we are inserting here the property without checking the key, the
    // reason is that d->properties will take the ownership of given pointer
    // and delete everything appropriately even when empty or already used
    // key was specified
    d->properties.insert( name, prop );

    // and now update the group cache
    d->propertyGroups[ group ].insert( name, prop );

    return true;
}

void Template::setPropertyGroupEnabled( const QString & group, bool enabled )
{
    if( d->propertyGroups.contains( group ) ) {
        PropertyDict props = d->propertyGroups[ group ];
        for( unsigned i = 0; i < props.count(); i ++ ) {
            props[ i ]->setEnabled( enabled );
        }
    }
}

bool Template::registerIOPin( const QString & group, const QString & name,
        IOPin * pin )
{
    //first test whether we already have the given group, if not, add it to
    //our data structures
    if( ! d->iopinGroups.contains( group ) ) {
        d->iopinGroups.insert( group, IOPinDict() );
    }

    // we are inserting here the iopin without checking the key, the
    // reason is that d->iopins will take the ownership of given pointer
    // and delete everything appropriately even when empty or already used
    // key was specified
    d->iopins.insert( name, pin );

    // and now update the group cache
    d->iopinGroups[ group ].insert( name, pin );

    return true;
}

bool Template::requestNestedTemplate( const QString & name,
        const QString & type, const QString & lib /* = QString::null */ )
{

    if( name.isEmpty() || d->requestedNestedTemplates.contains( name ) ) {
        return false;
    }

    d->requestedNestedTemplates.insert( name, type );
    d->requestedNestedTemplateLibs.insert( name, lib );

    return true;
}

bool Template::registerNestedTemplate( const QString & name,
        Template * tmpl )
{

    if( name.isEmpty() || d->nestedTemplates.contains( name ) ) {
        return false;
    }

    if( tmpl == NULL ) {
        return false;
    }

    d->nestedTemplates.insert( name, tmpl );

    return true;
}

void Template::localize( LocalizatorPtr localizator )
{
    d->localizator = localizator;

    unsigned count = d->nestedTemplates.count();

    for( unsigned i = 0 ; i < count ; i ++ ) {
        d->nestedTemplates[ i ]->localize( localizator );
    }
}

void Template::compose( const TemplateManager & tm )
{

    QMapConstIterator <QString, QString> i =
        const_cast<QMap<QString, QString> &>(
                d->requestedNestedTemplates).begin();

    for( ; i != const_cast<QMap<QString, QString> &>(
                d->requestedNestedTemplates).end(); ++i ) {

        //try to acquire template of a given type
        Template * t = tm.instantiate( i.data(), getLoggerPtr(),
                d->requestedNestedTemplateLibs[ i.key() ] );

        if( t != NULL ) {
            // recursively compose even our nested templates
            t->compose( tm );
            // and put our template to our internal hash
            registerNestedTemplate( i.key(), t );
        }
    }
}

Template * Template::nestedTemplate( const QString & name )
{
    return d->nestedTemplates[ name ];
}

void Template::setViewAdaptor( ViewAdaptor * va )
{
    d->viewAdaptor = va;
}

ViewAdaptor * Template::viewAdaptor() const
{
    return d->viewAdaptor;
}

QRect Template::rect() const
{
    return d->rect;
}

void Template::setRect( const QRect & rect )
{
    //qWarning( "Template::setRect: %d, %d, %d x %d",
    //        rect.left(), rect.top(), rect.width(), rect.height() );

    // polygonal item is special, it uses areaPoints to determine bounding
    // rectangle, and thus has to be invalidated when bounding rectangle
    // changes when the item is visible
    invalidate();

    d->rect = rect;
    d->points = QPointArray(
            QRect(rect.left() - 1, rect.top() - 1,
                rect.width() + 2, rect.height() + 2), TRUE );
    d->collisionRegion = QRegion( d->points );

    // move the item BUT after our boundingrect is setup because the move
    // uses the boundingrect to setup our chunks on a canvas!
    move( d->rect.x(), d->rect.y() );

    // focusRect
    d->focusRect = rect;
    d->focusRect.moveBy( 1, 1 );
    d->focusRect.setSize( rect.size() - QSize( 2, 2 ) );
}

QRect Template::virtualRect() const
{
    return d->virtualRect;
}

void Template::setVirtualRect( const QRect & rect )
{
    //qWarning( "Template::setVirtualRect: %d, %d, %d x %d",
    //        rect.left(), rect.top(), rect.width(), rect.height() );

    d->virtualRect = rect;
}

QPointArray Template::areaPoints() const
{
    return d->points;
}

QRegion Template::collisionRegion() const
{
    return d->collisionRegion;
}

bool Template::collidesWith( const QPoint & p ) const
{
    return d->collisionRegion.contains( p );
}

void Template::draw( QPainter & p )
{
    // exit immediatelly when we are not visible
    if( ! isVisible() ) return;

    //qWarning( "Template[%d]::draw: visible %d, canvas %d",
    //       (unsigned)this, isVisible(),(unsigned)canvas() );

    //qWarning( "Template[%d]::draw: my rect is %d, %d %dx%d",
    //       (unsigned)this, r.left(), r.top(), r.width(), r.height() );
    //qWarning( "enab=%d, sel=%d, act=%d", isEnabled(), isSelected(),
    //        isActive() );

    p.save();

    // paint us
    drawShape( p );

    p.restore();
}

void Template::drawFocusRect( QPainter& p)
{
    p.save();
    p.setBrush( QBrush::NoBrush );
    p.setPen( QPen( QColor( 100, 100, 100 ), 2, QPen::DotLine ) );
    p.drawRect( d->focusRect );
    p.restore();
}

void Template::focusInEvent ( QFocusEvent * )
{
    update();
}

void Template::focusOutEvent ( QFocusEvent * )
{
    update();
}

void Template::setVisible( bool visible )
{
    QCanvasPolygonalItem::setVisible( visible );
    move( d->rect.x(), d->rect.y() );
}

void Template::setShortcut( const QKeySequence & sq )
{
    d->shortcut = sq;
}

const QKeySequence & Template::shortcut() const
{
    return d->shortcut;
}

void Template::drawShape( QPainter & p )
{
    //qWarning( "Template::drawShape: not reimplemented ???" );
    p.setBrush( QBrush( QColor( 255, 255, 255 ) ) );
    p.setPen( QPen( QColor( 0, 0, 0 ), 0, QPen::SolidLine ) );
    p.drawRect( d->rect );
}

void Template::syncIOPinsRequestedValues()
{
    bool any_dirty = 0;
    unsigned count = d->iopins.count();
    for( unsigned i = 0 ; i < count ; i++ ) {
        if( d->iopins[ i ]->isRequestedValueDirty() ) {
            d->iopins[ i ]->syncRequestedValue();
            any_dirty = 1;
        }
    }
    if( any_dirty ) {
        qApp->postEvent( qApp,
                new QCustomEvent( IOPinRequestedValuesChangedEvent ) );
    }
}

void Template::syncIOPinsRealValues()
{
    bool any_dirty = 0;
    unsigned count = d->iopins.count();
    for( unsigned i = 0 ; i < count ; i++ ) {
        if( d->iopins[ i ]->isRealValueDirty() ) {
            d->iopins[ i ]->syncRealValue();
            any_dirty = 1;
        }
    }
    if( any_dirty ) {
        qApp->postEvent( qApp,
                new QCustomEvent( IOPinRealValuesChangedEvent ) );
    }
}

int Template::rtti() const
{
    return Template::RTTI;
}

Template * Template::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Template( "default", logger );
    setupAfterClone( result, loc, tm );
    return result;
}

void Template::setupAfterClone( Template * newTemplate,
        const LocalizatorPtr & loc, const TemplateManager & tm ) const
{
    Q_ASSERT( d != NULL );
    Q_ASSERT( newTemplate != NULL );

    // check that the templates have the same type
    Q_ASSERT( d->type == newTemplate->d->type );

    // pass in template manager
    newTemplate->compose( tm );

    // what to do with localizator and viewAdaptor
    newTemplate->localize( loc );

    // transfer the virtual and current rectangle
    newTemplate->setVirtualRect( virtualRect() );
    newTemplate->setRect( rect() );
    newTemplate->setZ( z() );

    // clone the locked state
    newTemplate->setEnabled(isEnabled());

    // remember from where we came
    newTemplate->setLibrary( library() );

    // transfer the values of properties that are non-default
    for( unsigned i = 0; i < d->properties.count(); i ++ ) {

        // we don't need to transfer the default (unchanged values)
        if( d->properties[ i ]->isDefault() ) continue;

        // set the newTemplate's property value to our value
        newTemplate->d->properties[ i ]->decodeValue(
                d->properties[ i ]->encodeValue() );
    }

    newTemplate->propertiesChanged();

    // FIXME: we should probably clone the variable bindings, but I'm not
    // sure yet so I'm leaving it open for now
}

QFont Template::makeFont( const QFont & original,
        const QString & /* text */ /* = "MMMMMMMMMM" */ ) const
{
    Q_ASSERT( d != NULL );

    QFont result = original;

    int original_size = original.pixelSize();
    if( original_size == -1 ) original_size = original.pointSize();

    // compute scaling coeficient
    double scale =
        (this->rect().height() / (float)virtualRect().height());

#if BROKEN_ALGORITHM
    // compute width of dummy text for original font
    int original_w = QFontMetrics( original ).width( text );

    // compute fitting coeficient
    double original_fit = (float)original_w / (float)original_size;
#endif

    // compute proposed new size.
    int result_size = (int)(original_size * scale + 0.5);

    // stick to minimum font size 2 (to have space for decreasing)
    if( result_size < 2 ) result_size = 2;

    if( original.pixelSize() == -1 ) {
        result.setPointSize( result_size );
    } else {
        result.setPixelSize( result_size );
    }

#if BROKEN_ALGORITHM
    // compute width of dummy text for new font
    int result_w = QFontMetrics( result ).width( text );

    // compute fitting coeficient
    double result_fit = (float)result_w / (float)result_size;

    // if the new text will be logically wider, try to make the font smaller
    if( result_fit >= original_fit ) {
        qWarning( "Template::makeFont: need to decrease font by one" );
        if( original.pixelSize() == -1 ) {
            result.setPointSize( result_size - 1 );
        } else {
            result.setPixelSize( result_size - 1 );
        }
    }

    qWarning(
            "height %d -> height %d, scale = %f\n"
            "original_size = %d, original_twidth = %d, original_fit=%f\n"
            "result_size = %d, result_twidth = %d, result_fit=%f\n",
            virtualRect().height(), rect().height(), scale,
            original_size, original_w, original_fit,
            result_size, result_w, result_fit );
#endif

    return result;
}

QKeySequence Template::keyEvent2Sequence( const QKeyEvent * e )
{
    QKeySequence result;
    if( e == NULL ) return result;

    if( e->key() == Key_Control || e->key() == Key_Meta ||
            e->key() == Key_Shift || e->key() == Key_Alt ||
            e->key() == Key_Menu ) {
        return result;
    }

    int keyCombination = e->key();

    if( e->state() & Qt::ShiftButton ) {
        keyCombination += SHIFT;
    }

    if( e->state() & Qt::ControlButton ) {
        keyCombination += CTRL;
    }

    if( e->state() & Qt::AltButton ) {
        keyCombination += ALT;
    }

    if( e->state() & Qt::MetaButton ) {
        keyCombination += META;
    }

    result = QKeySequence( keyCombination );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Template.cpp 1774 2006-05-31 13:17:16Z hynek $
