// $Id: TemplatePropertiesControls.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplatePropertiesControls.cpp -- 
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 5th September 2004
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

#include "TemplatePropertiesControls.hh"

#include <qlayout.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qtooltip.h>
#include <qfontdatabase.h>

// line styles
#include "images/DashDotDotLine.xpm"
#include "images/DashDotLine.xpm"
#include "images/DashLine.xpm"
#include "images/DotLine.xpm"
#include "images/NoPen.xpm"
#include "images/SolidLine.xpm"

// brysh styles
#include "images/BDiagPattern.xpm"
#include "images/CrossPattern.xpm"
#include "images/Dense1Pattern.xpm"
#include "images/Dense2Pattern.xpm"
#include "images/Dense3Pattern.xpm"
#include "images/Dense4Pattern.xpm"
#include "images/Dense5Pattern.xpm"
#include "images/Dense6Pattern.xpm"
#include "images/Dense7Pattern.xpm"
#include "images/DiagCrossPattern.xpm"
#include "images/FDiagPattern.xpm"
#include "images/HorPattern.xpm"
#include "images/NoBrush.xpm"
#include "images/SolidPattern.xpm"
#include "images/VerPattern.xpm"

////////////////////////////////////////////////////////////
// PEN
////////////////////////////////////////////////////////////
struct _PenStyles {
    Qt::PenStyle style;
    char * representation;
};

static const _PenStyles _penStyles[] = {
    { Qt::SolidLine, "SolidLine" },
    { Qt::DashLine, "DashLine" },
    { Qt::DotLine, "DotLine" },
    { Qt::DashDotLine, "DashDotLine" },
    { Qt::DashDotDotLine, "DashDotDotLine" },
    { Qt::NoPen, "NoPen" },
};


////////////////////////////////////////////////////////////
// BRUSH
////////////////////////////////////////////////////////////
struct _BrushStyles {
    Qt::BrushStyle style;
    char * representation;
};

static const _BrushStyles _brushStyles [] = {
    { Qt::SolidPattern , "SolidPattern" },
    { Qt::Dense1Pattern , "Dense1Pattern" },
    { Qt::Dense2Pattern , "Dense2Pattern" },
    { Qt::Dense3Pattern , "Dense3Pattern" },
    { Qt::Dense4Pattern , "Dense4Pattern" },
    { Qt::Dense5Pattern , "Dense5Pattern" },
    { Qt::Dense6Pattern , "Dense6Pattern" },
    { Qt::Dense7Pattern , "Dense7Pattern" },
    { Qt::HorPattern , "HorPattern" },
    { Qt::VerPattern , "VerPattern" },
    { Qt::CrossPattern , "CrossPattern" },
    { Qt::BDiagPattern , "BDiagPattern" },
    { Qt::FDiagPattern , "FDiagPattern" },
    { Qt::DiagCrossPattern , "DiagCrossPattern" },
    { Qt::NoBrush , "NoBrush" },
};


////////////////////////////////////////////////////////////
// PEN
////////////////////////////////////////////////////////////
struct TemplatePropertyPen::TemplatePropertyPenPrivate {

    TemplatePropertyPenPrivate( QString k, const PropertyDict& p) :
            m_ls( NULL ), m_lw( NULL ), m_cb( NULL), key(k), pd(p) {}
    /**
     * TODO
     */
    QComboBox* m_ls;

    /**
     * TODO
     */
    QSpinBox* m_lw;

    /**
     * TODO
     */
    QPushButton* m_cb;

    /**
     * TODO
     */
    QPen m_pen;

    /**
     * Property key
     */
    QString key;

    /**
     * PropertyDict
     */
    const PropertyDict& pd;
};

TemplatePropertyPen::TemplatePropertyPen(
        const QString& k, const PropertyDict& pd,
        QWidget* parent, const char* name /*= NULL*/ ) :
    QWidget( parent, name ),
    d( new TemplatePropertyPenPrivate(k,pd) )
{
    Q_CHECK_PTR( d );

    QHBoxLayout* tl = new QHBoxLayout( this );
    Q_CHECK_PTR( tl );

    d->m_ls = new QComboBox( this );
    Q_CHECK_PTR( d->m_ls );
    tl->addWidget( d->m_ls );
    QToolTip::add( d->m_ls, tr("Specify the pen\'s line style") );
    d->m_ls->setSizePolicy(
            QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    setFocusProxy( d->m_ls );

    //!! order is important! Has to be the same as in _penStyles[] struct !!

    d->m_ls->insertItem( QPixmap( (const char**)&SolidLine_xpm ) );
    d->m_ls->insertItem( QPixmap( (const char**)&DashLine_xpm ) );
    d->m_ls->insertItem( QPixmap( (const char**)&DotLine_xpm ) );
    d->m_ls->insertItem(
            QPixmap( (const char**)&DashDotLine_xpm ) );
    d->m_ls->insertItem(
            QPixmap( (const char**)&DashDotDotLine_xpm ) );
    d->m_ls->insertItem(
            QPixmap( (const char**)&NoPen_xpm ) );

    connect( d->m_ls, SIGNAL( activated( int ) ),
            this, SLOT( styleValueChanged( int ) ) );

    d->m_lw = new QSpinBox( this );
    Q_CHECK_PTR( d->m_lw );
    d->m_lw->setMaxValue( 100 );
    tl->addWidget( d->m_lw );
    QToolTip::add( d->m_lw, tr("Specify the pen\'s line width") );
    connect( d->m_lw, SIGNAL( valueChanged( int ) ),
            this, SLOT( widthValueChangedSlot( int ) ) );
    d->m_lw->setSizePolicy(
            QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    d->m_cb = new QPushButton( this );
    Q_CHECK_PTR( d->m_cb );
    tl->addWidget( d->m_cb );
    d->m_cb->setDefault( false );
    d->m_cb->setAutoDefault( false );
    QToolTip::add( d->m_cb, tr("Specify the pen\'s color") );
    connect( d->m_cb, SIGNAL( clicked() ),
            this, SLOT( colorButtonClicked() ) );
    d->m_cb->setMinimumWidth(
            QFontMetrics(d->m_cb->font()).boundingRect("M...M").width());
    d->m_cb->setSizePolicy(
            QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));

    setTabOrder( d->m_ls, d->m_lw );
    setTabOrder( d->m_lw, d->m_cb );

    updateFromPen(d->pd[d->key]->asPen());
}
TemplatePropertyPen::~TemplatePropertyPen()
{
    delete d;
}

int TemplatePropertyPen::penStyleToIndex( Qt::PenStyle s )
{
    int i;
    for( i = 0; _penStyles[i].style != Qt::NoPen &&
            _penStyles[i].style != s; ++i ) {}
    return i;
}

void TemplatePropertyPen::widthValueChangedSlot( int val )
{
    if( d->m_pen.width() != (unsigned)val ) {
        d->m_pen.setWidth( val );
        emit( penPropertyChanged( this, d->m_pen ) );
    }
}
void TemplatePropertyPen::colorButtonClicked()
{
    QColor c = QColorDialog::getColor( d->m_cb->paletteBackgroundColor(),
            this );

    if( c.isValid() && d->m_pen.color() != c ) {
        d->m_cb->setPaletteBackgroundColor( c );
        d->m_pen.setColor( c );
        emit( penPropertyChanged( this, d->m_pen ) );
    }
}

void TemplatePropertyPen::styleValueChanged( int v )
{
    if( d->m_pen.style() != _penStyles[v].style ) {
        d->m_pen.setStyle( _penStyles[ v ].style );
        emit( penPropertyChanged( this, d->m_pen ) );
    }
}

void TemplatePropertyPen::updateFromPen( const QPen& p )
{
    Q_ASSERT(d);
    Q_ASSERT(d->m_ls);
    Q_ASSERT(d->m_lw);
    Q_ASSERT(d->m_cb);

    d->m_ls->setCurrentItem( penStyleToIndex(p.style()) );
    d->m_lw->setValue( p.width() );
    d->m_cb->setPaletteBackgroundColor( p.color() );
    d->m_pen = p;
}

void TemplatePropertyPen::updateSlot( const QString& propKey )
{
    const Property* p = d->pd[d->key];
    if( propKey == d->key ) {
        Q_ASSERT(d);

        if( p->asPen() != d->m_pen ) {
            updateFromPen(p->asPen());
        }
    }

    // enable/disable property
    setEnabled(p->isEnabled());
}

const QString TemplatePropertyPen::key() const
{
    Q_ASSERT(d);
    return d->key;
}


////////////////////////////////////////////////////////////
// BRUSH
////////////////////////////////////////////////////////////
struct TemplatePropertyBrush::TemplatePropertyBrushPrivate {
    TemplatePropertyBrushPrivate( QString k, const PropertyDict& p) :
        m_bs( NULL ), m_cb( NULL ), key(k), pd(p) {}
    /**
     * TODO
     */
    QComboBox* m_bs;

    /**
     * TODO
     */
    QPushButton* m_cb;

    /**
     * TODO
     */
    QBrush m_brush;

    /**
     * Property key
     */
    QString key;

    /**
     * PropertyDict
     */
    const PropertyDict& pd;
};

TemplatePropertyBrush::TemplatePropertyBrush(
        const QString& k, const PropertyDict& pd,
        QWidget* parent, const char* name /*= NULL*/ ) :
    QWidget( parent, name ),
    d( new TemplatePropertyBrushPrivate(k,pd) )
{
    Q_CHECK_PTR( d );

    QHBoxLayout* tl = new QHBoxLayout( this );
    Q_CHECK_PTR( tl );

    d->m_bs = new QComboBox( this );
    Q_CHECK_PTR( d->m_bs );
    tl->addWidget( d->m_bs );
    QToolTip::add( d->m_bs, tr("Specify the brush\'s fill pattern") );
    d->m_bs->setSizePolicy(
            QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    setFocusProxy( d->m_bs );

    //!! order is important! Has to be the same as in _brushStyles[] struct
    //!!
    d->m_bs->insertItem(
            QPixmap( (const char**)&SolidPattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&Dense1Pattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&Dense2Pattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&Dense3Pattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&Dense4Pattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&Dense5Pattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&Dense6Pattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&Dense7Pattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&HorPattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&VerPattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&CrossPattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&BDiagPattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&FDiagPattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&DiagCrossPattern_xpm ) );
    d->m_bs->insertItem(
            QPixmap( (const char**)&NoBrush_xpm ) );

    connect( d->m_bs, SIGNAL( activated( int ) ),
            this, SLOT( styleValueChanged( int ) ) );
    d->m_cb = new QPushButton( this );
    Q_CHECK_PTR( d->m_cb );
    tl->addWidget( d->m_cb );
    d->m_cb->setDefault( false );
    d->m_cb->setAutoDefault( false );
    QToolTip::add( d->m_cb, tr("Specify the brush\'s color") );
    d->m_cb->setMinimumWidth(
            QFontMetrics(d->m_cb->font()).boundingRect("M...M").width());
    d->m_cb->setSizePolicy(
            QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));
    connect( d->m_cb, SIGNAL( clicked() ),
            this, SLOT( colorButtonClicked() ) );

    setTabOrder( d->m_bs, d->m_cb );

    updateFromBrush( d->pd[d->key]->asBrush() );
}

TemplatePropertyBrush::~TemplatePropertyBrush()
{
    delete d;

}

int TemplatePropertyBrush::brushStyleToIndex( Qt::BrushStyle s )
{
    int i;
    for( i = 0; _brushStyles[i].style != Qt::NoBrush &&
            _brushStyles[i].style != s; ++i ) {}
    return i;
}

void TemplatePropertyBrush::colorButtonClicked()
{
    QColor c = QColorDialog::getColor( d->m_cb->paletteBackgroundColor(),
            this );

    if( c.isValid() && d->m_brush.color() != c ) {
        d->m_cb->setPaletteBackgroundColor( c );
        d->m_brush.setColor( c );

        emit( brushPropertyChanged( this, d->m_brush ) );
    }
}

void TemplatePropertyBrush::styleValueChanged( int v )
{
    if( d->m_brush.style() != _brushStyles[ v ].style ) {
        d->m_brush.setStyle( _brushStyles[ v ].style );

        emit( brushPropertyChanged( this, d->m_brush ) );
    }
}

void TemplatePropertyBrush:: updateFromBrush( const QBrush& b )
{
    Q_ASSERT( d );
    Q_ASSERT( d->m_cb );
    Q_ASSERT(d->m_bs);

    d->m_bs->setCurrentItem( brushStyleToIndex( b.style() ) );
    d->m_cb->setPaletteBackgroundColor( b.color() );
    d->m_brush = b;
}

void TemplatePropertyBrush::updateSlot( const QString& propKey )
{
    const Property* p = d->pd[d->key];
    if( propKey == d->key ) {
        Q_ASSERT(d);

        if( p->asBrush() != d->m_brush ) {
            updateFromBrush(p->asBrush());
        }
    }

    // enable/disable property
    setEnabled(p->isEnabled());
}

const QString TemplatePropertyBrush::key() const
{
    Q_ASSERT(d);
    return d->key;
}


////////////////////////////////////////////////////////////
// FONT
////////////////////////////////////////////////////////////
struct TemplatePropertyFont::TemplatePropertyFontPrivate {
    TemplatePropertyFontPrivate( QString k, const PropertyDict& p) :
        m_fn( NULL ), m_cf( NULL ), key(k), pd(p) {}

    /**
     * TODO
     */
    QLineEdit* m_fn;

    /**
     * TODO
     */
    QPushButton* m_cf;

    /**
     * TODO
     */
    QFont m_font;

    /**
     * Property key
     */
    QString key;

    /**
     * PropertyDict
     */
    const PropertyDict& pd;
};

TemplatePropertyFont::TemplatePropertyFont( const QString& k,
        const PropertyDict& pd, QWidget* parent,
        const char* name /*= NULL*/ ) : QWidget( parent, name ),
    d( new TemplatePropertyFontPrivate(k,pd) )
{
    Q_CHECK_PTR( d );

    QHBoxLayout* tl = new QHBoxLayout( this );
    Q_CHECK_PTR( tl );

    d->m_fn = new QLineEdit( this );
    Q_CHECK_PTR(  d->m_fn );
    d->m_fn->setSizePolicy(
            QSizePolicy(
                QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    tl->addWidget(  d->m_fn );
    QToolTip::add(  d->m_fn, tr("Shows the current font name and size") );
    d->m_fn->setSizePolicy(
            QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    d->m_fn->setReadOnly( true );

    setFocusProxy( d->m_fn );

    d->m_cf = new QPushButton( "...", this );
    Q_CHECK_PTR( d->m_cf );
    d->m_cf->setMinimumWidth(
            QFontMetrics(d->m_cf->font()).boundingRect("M...M").width());
    d->m_cf->setSizePolicy(
            QSizePolicy(
                QSizePolicy::Maximum, QSizePolicy::Fixed));
    tl->addWidget( d->m_cf );
    d->m_cf->setDefault( false );
    d->m_cf->setAutoDefault( false );
    QToolTip::add( d->m_cf, tr("Change the font") );
    d->m_cf->setSizePolicy(
            QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    connect( d->m_cf, SIGNAL( clicked() ),
            this, SLOT( changeFont() ) );

    setTabOrder( d->m_fn, d->m_cf );

    updateFromFont( d->pd[d->key]->asFont() );
}

TemplatePropertyFont::~TemplatePropertyFont()
{
    delete d;
}

void TemplatePropertyFont::changeFont()
{
    Q_ASSERT(d);

    bool ok;

    // find the correct-case font family in QFontDatabase and set it to
    // m_font; otherwise the QFontDialog would not find it.
    QStringList fl = QFontDatabase().families()
        .grep(d->m_font.family(),false);
    if( fl.count() > 0 ) {
        d->m_font.setFamily( fl.front() );
    }

    QFont f = QFontDialog::getFont( &ok, d->m_font, this );
    if( ok && ( f != d->m_font ) ) {
        d->m_font = f;

        emit( fontPropertyChanged( this, d->m_font ) );
    }
}

void TemplatePropertyFont:: updateFromFont( const QFont& f )
{
    Q_ASSERT( d );
    Q_ASSERT( d->m_fn );

    d->m_font = f;
    d->m_fn->setText( QString("%1 %2")
            .arg(d->m_font.family())
            .arg(d->m_font.pointSize()) );

    QFont lf = f;
    if(f.pointSize() > 72) {
        lf.setPointSize(72);
    } else if(f.pointSize() < 6) {
        lf.setPointSize(6);
    }
    d->m_fn->setFont(lf);

    d->m_fn->setMinimumWidth(
        QFontMetrics(lf).boundingRect(d->m_fn->text()).width());

    updateGeometry();
}

void TemplatePropertyFont::updateSlot( const QString& propKey )
{
    const Property* p = d->pd[d->key];
    if( propKey == d->key ) {
        Q_ASSERT(d);

        if( p->asFont() != d->m_font ) {
            updateFromFont(p->asFont());
        }
    }

    // enable/disable property
    setEnabled(p->isEnabled());
}

const QString TemplatePropertyFont::key() const
{
    Q_ASSERT(d);
    return d->key;
}


////////////////////////////////////////////////////////////
// COLOR
////////////////////////////////////////////////////////////
struct TemplatePropertyColor::TemplatePropertyColorPrivate {
    TemplatePropertyColorPrivate( QString k, const PropertyDict& p) :
        m_cb( NULL ), key(k), pd(p) {}

    /**
     * TODO
     */
    QPushButton* m_cb;

    /**
     * TODO
     */
    QColor m_color;

    /**
     * Property key
     */
    QString key;

    /**
     * PropertyDict
     */
    const PropertyDict& pd;
};

TemplatePropertyColor::TemplatePropertyColor( const QString& k,
        const PropertyDict& pd, QWidget* parent,
        const char* name /*= NULL*/ ) : QWidget( parent, name ),
    d( new TemplatePropertyColorPrivate(k,pd) )
{
    Q_CHECK_PTR( d );

    QHBoxLayout* tl = new QHBoxLayout( this );
    Q_CHECK_PTR( tl );

    d->m_cb = new QPushButton( "...", this );
    Q_CHECK_PTR( d->m_cb );
    tl->addWidget( d->m_cb );
    d->m_cb->setDefault( false );
    d->m_cb->setAutoDefault( false );
    QToolTip::add( d->m_cb, tr("Change the color") );
    d->m_cb->setMinimumWidth(
            QFontMetrics(d->m_cb->font()).boundingRect("M...M").width());
    d->m_cb->setSizePolicy(
            QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    connect( d->m_cb, SIGNAL( clicked() ),
            this, SLOT( changeColor() ) );

    setFocusProxy( d->m_cb );

    updateFromColor( d->pd[d->key]->asColor() );
}

TemplatePropertyColor::~TemplatePropertyColor()
{
    delete d;
}

void TemplatePropertyColor::changeColor()
{
    QColor c = QColorDialog::getColor( d->m_cb->paletteBackgroundColor(),
            this );

    if( c.isValid() && ( c != d->m_color ) ) {
        d->m_color = c;
        d->m_cb->setPaletteBackgroundColor( d->m_color );

        emit( colorPropertyChanged( this, d->m_color ) );
    }
}

void TemplatePropertyColor:: updateFromColor( const QColor& f )
{
    Q_ASSERT( d );
    Q_ASSERT( d->m_cb );

    d->m_color = f;
    d->m_cb->setPaletteBackgroundColor( d->m_color );
}

void TemplatePropertyColor::updateSlot( const QString& propKey )
{
    const Property* p = d->pd[d->key];
    if( propKey == d->key ) {
        Q_ASSERT(d);

        if( p->asColor() != d->m_color ) {
            updateFromColor(p->asColor());
        }
    }

    // enable/disable property
    setEnabled(p->isEnabled());
}

const QString TemplatePropertyColor::key() const
{
    Q_ASSERT(d);
    return d->key;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplatePropertiesControls.cpp 1169 2005-12-12 15:22:15Z modesto $
