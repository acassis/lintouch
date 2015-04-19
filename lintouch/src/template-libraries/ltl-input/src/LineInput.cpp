// $Id: LineInput.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: LineInput.cpp --
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 06 October 2004
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

#include "LineInput.hh"
#include "LineInput.xpm"

#include "config.h"

struct LineInput::LineInputPrivate{

    LineInputPrivate()
    {
        focused = false;
    }

    ~LineInputPrivate()
    {
    }

    int             m_font_orig_size;
    QFont           m_font_input;
    QPen            m_pen_border;
    QPen            m_pen_input;

    QString         m_input;
    QRect           m_input_pos;

    bool focused;
};

LineInput::LineInput( LoggerPtr logger ) :
    Template( "LineInput", logger ), d( new LineInputPrivate() )
{

    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Shivaji Basu <shivaji@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Line Input", "ltl-input" ) );
    i.setLongDescription( QObject::tr( "Line Input", "ltl-input" ) );
    i.setIcon( QImage( (const char **) LineInput_xpm ) );

    registerInfo( i );

    QString general = QObject::tr("Appearance", "ltl-input");

    registerProperty( general, "border",
            new Property( QObject::tr( "Border", "ltl-input" ), "pen",
                "#000000;1;SOLIDLINE", logger ) );

    registerProperty( general, "font",
            new Property( QObject::tr( "Font", "ltl-input" ),
                "font", "", logger ) );

    registerProperty( general, "foreground",
            new Property( QObject::tr( "Foreground", "ltl-input" ),
                "color", "#000000",
                logger ) );

    QString inp = QObject::tr( "Outputs", "ltl-input" );

    registerIOPin( inp, "output",
            new IOPin( this,
                QObject::tr( "Output", "ltl-input" ), "string" ) );

}

LineInput::~LineInput()
{
    delete d;
}

void LineInput::drawShape( QPainter & p )
{
    Q_ASSERT( d );
    p.save();

    p.setPen( d->m_pen_border );
    p.drawRect( rect() );

    if( ! d->m_input.isEmpty() ) {
        p.setPen( d->m_pen_input );
        p.setFont( d->m_font_input );
        p.drawText( d->m_input_pos,
                Qt::AlignLeft | Qt::AlignVCenter, d->m_input );
    }

    p.restore();

    if(d->focused)
        drawFocusRect(p);
}

void LineInput::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    // compute new font sizes
    recalculateFontSizes();

    update();
}

void LineInput::propertiesChanged()
{
    // shedule repaint
    Q_ASSERT( d );

    d->m_pen_border = properties() [ "border" ]->asPen();
    d->m_pen_input = properties() [ "foreground" ]->asColor();
    d->m_font_input = properties() [ "font" ]->asFont();

    if( d->m_font_input.pixelSize() != -1 )
        d->m_font_orig_size = d->m_font_input.pixelSize();
    else d->m_font_orig_size = d->m_font_input.pointSize();

    recalculateFontSizes();

    reconstructPointArrayAndRegion();

    iopinsChanged();

    update();
}

void LineInput::iopinsChanged()
{
    update();
}

QWidget::FocusPolicy LineInput::focusPolicy() const
{
    return QWidget::StrongFocus;
}

void LineInput::keyPressEvent( QKeyEvent * e )
{
    Q_ASSERT( d );
    //qWarning( "Button::keyPressEvent: %d", e->key() );
    switch( e->key() ){
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Insert:
    case Qt::Key_Home:
    case Qt::Key_End:
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Prior:
    case Qt::Key_Next:
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Delete:
        e->ignore();
        break;
    case Qt::Key_Backspace:
        d->m_input.truncate( d->m_input.length() - 1 );
        iopins()["output"]->setRequestedStringValue( d->m_input );
        syncIOPinsRequestedValues();
        e->accept();
        update();
        break;
    default:
        d->m_input += e->text();
        iopins()["output"]->setRequestedStringValue( d->m_input );
        syncIOPinsRequestedValues();
        //qWarning( "added %1", e->text() );
        e->accept();
        update();
        break;
    }
}

void LineInput::reconstructPointArrayAndRegion()
{
    Q_ASSERT( d );

    // make text bounding rectangle equal to our rectangle
    d->m_input_pos = rect();
    int w = d->m_pen_input.width();
    // when we are big enough, clip text inside
    if( d->m_input_pos.width() >( w + 4 ) * 2 &&
            d->m_input_pos.height() >( w + 4 ) * 2 ) {
        QPoint p( w + 4, w + 4 );
        d->m_input_pos.setTopLeft( d->m_input_pos.topLeft() + p );
        d->m_input_pos.setBottomRight( d->m_input_pos.bottomRight() - p );
    }
}

void LineInput::recalculateFontSizes()
{
    Q_ASSERT( d );
    double scale =
        (this->rect().height() / (float)virtualRect().height());

    int newsize = (int)(d->m_font_orig_size * scale);

    // manage real font size either thru pixel size or point size
    if( d->m_font_input.pixelSize() != -1 ) {
        if( newsize > 0 ) d->m_font_input.setPixelSize( newsize );
    } else {
        if( newsize > 0 ) d->m_font_input.setPointSize( newsize );
    }
}

Template * LineInput::clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new LineInput( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

void LineInput::focusInEvent ( QFocusEvent* e )
{
    Template::focusInEvent(e);

    Q_ASSERT(d);
    d->focused = true;
    update();
}

void LineInput::focusOutEvent ( QFocusEvent* e )
{
    Template::focusOutEvent(e);

    Q_ASSERT(d);
    d->focused = false;
    update();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: LineInput.cpp 1168 2005-12-12 14:48:03Z modesto $

