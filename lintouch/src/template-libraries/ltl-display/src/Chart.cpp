// $Id: Chart.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Chart.cpp --
// AUTHOR: Hynek Petrak <hynek@swac.cz>
//   DATE: 24 November 2004
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

#include <qdatetime.h>
#include <limits.h>

#include "Chart.hh"
#include "Chart.xpm"

#include "config.h"

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

class _SingleValue
{
    public:
    _SingleValue()
    {
        valid = false;
    }
    int value;
    QTime timestamp;
    bool valid;
};

typedef struct Chart::ChartPrivate
{
    ChartPrivate(): values(NULL)
    {
        resetValues();
    };
    void resetValues()
    {
        valid = false;
        size = 0;
        if(values)
            delete []values;
        current = 0;
        min_value = 0;
        max_value = 0;
        auto_min = false;
        auto_max = false;
        real_time = false;
        current_min = INT_MAX;
        current_max = INT_MIN;
    }

    bool valid;
    QBrush bg_brush;
    QPen border_pen;
    QPen line_pen;
    QPen overflow_pen;
    _SingleValue *values;
    int current;
    long size;
    bool real_time;
    int min_value;
    int max_value;
    bool auto_min;
    bool auto_max;
    int current_min;
    int current_max;
    int grid_x_step;
    int grid_y_step;
    QPen grid_pen;
    // time_slice
    IOPin *input;
};

Chart::Chart( LoggerPtr logger ) :
    Template( "Chart", logger ),
    m_localizator( Localizator::nullPtr() ),
    d(new ChartPrivate)
{

    //register info
    Info i;
    i.setAuthor( "Hynek Petrak <hynek@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Chart", "ltl-display" ) );
    i.setLongDescription( QObject::tr( "Chart", "ltl-display" ) );
    i.setIcon(QImage(Chart_xpm));

    registerInfo( i );

    QString group = QObject::tr( "Appearance",
            "ltl-display" );

    registerProperty(group, "size",
        new Property(QObject::tr("History", "ltl-display"),
        "integer", "10", logger));

    registerProperty(group, "min_value",
        new Property(QObject::tr("Minimum", "ltl-display"),
        "integer", "0", logger));

    registerProperty(group, "max_value",
        new Property(QObject::tr("Maximum", "ltl-display"),
        "integer", "99", logger));

    group = QObject::tr( "Background",
            "ltl-display" );

    registerProperty(group, "bg_brush",
        new Property(QObject::tr("Background", "ltl-display"),
        "brush", "", logger));

    registerProperty(group, "border_pen",
        new Property(QObject::tr("Border", "ltl-display"),
        "pen", "", logger));

    group = QObject::tr( "Grid",
            "ltl-display" );

    registerProperty(group, "grid_x_step",
        new Property(QObject::tr("X Step", "ltl-display"),
        "integer", "10", logger));

    registerProperty(group, "grid_y_step",
        new Property(QObject::tr("Y Step", "ltl-display"),
        "integer", "10", logger));

    registerProperty(group, "grid_pen",
        new Property(QObject::tr("Pen", "ltl-display"),
        "pen", "#777777;1;DOTLINE", logger));

    group = QObject::tr( "Curve",
            "ltl-display" );

    registerProperty(group, "line_pen",
        new Property(QObject::tr("Style", "ltl-display"),
        "pen", "", logger));

    // create properties of this template
    //registerProperty( "link", Property( "string", "", logger ) );
    //registerProperty( "border_on", Property( "pen", "", logger ) );
    //registerProperty( "fill_on", Property( "brush", "", logger ) );
    //registerProperty( "text_on", Property( "string", "", logger ) );
    //registerProperty( "text_font_on", Property( "font", "ARIAL;10;NORMAL",
    //            logger ) );
    //registerProperty( "text_valign",
    //        Property( "choice(top,center,bottom)", "center", logger ) );

    // create iopins of this template
    registerIOPin( QObject::tr("Inputs", "ltl-display") , "input",
            d->input = new IOPin( this,
                QObject::tr("Input", "ltl-display" ), "number" ) );
}

Chart::~Chart()
{
    delete d;
}

void Chart::drawBackground( QPainter & p, const QRect& r )
{
    p.setPen(d->border_pen);
    p.setBrush(d->bg_brush);
    p.drawRect(r);
}

void Chart::drawGrid( QPainter & p, const QRect& r )
{
    int y, x, yy;

    int min = MIN(d->current_min, d->current_max);
    int max = MAX(d->current_min, d->current_max);

    p.setPen(d->grid_pen);

    if(d->grid_y_step > 1) {
        y = min  - min%d->grid_y_step;

        if(y <= min) y+= d->grid_y_step;

        for(; y<max; y+=d->grid_y_step) {

            yy = (r.bottom() - r.top()) * (y-min) /
                (max - min);

            p.drawLine(r.left(), yy+r.top(), r.right(), yy+r.top());
        }
    }

    if(d->grid_x_step > 1) {
        for(x=0; x<d->size; x+=d->grid_x_step) {
            int xx = (r.right() - r.left()) * (x) / (d->size);
            p.drawLine(r.left()+xx, r.top(), r.left()+xx, r.bottom());
        }
    }
}

void Chart::drawCurve( QPainter & p, const QRect& r )
{
    p.setPen(d->line_pen);

    QPoint cp, pp;
    int hh = abs(d->current_max - d->current_min);
    bool normal = d->current_min < d->current_max;
    bool pp_set = false;
    float x_step = 1.0*r.width()/d->size;
    float y_step = 1.0*r.height()/hh;
    int min = MIN(d->current_min, d->current_max);
    int max = MAX(d->current_min, d->current_max);

    // make our own current; it is the d->current minus one, because the
    // d->current points to the 1. free item
    long c = d->current - 1;

    for(long i = 0; i < d->size; ++i, --c) {

        // going backwards, watch for the bottom
        if(c < 0) {
            c = d->size - 1;
        }

        // only valid values are interesting to us
        if(!d->values[c].valid) {
            pp_set = false;
            continue;
        }

        cp.setX((int)(r.right() - (i * x_step)));

        if(d->values[c].value <= min) {
            if(normal) {
                cp.setY(r.bottom());
            } else {
                cp.setY(r.top());
            }
        } else if(d->values[c].value >= max) {
            if(normal) {
                cp.setY(r.top());
            } else {
                cp.setY(r.bottom());
            }
        } else {
            if(normal) {
                cp.setY((int)(r.bottom() -
                            (y_step * (d->values[c].value - min))));
            } else {
                cp.setY((int)(r.top() +
                            (y_step * (d->values[c].value - min))));
            }
        }

        // if(r.contains(cp)) {
            if(pp_set) {
                p.drawLine(pp, cp);
            }
            pp = cp;
            pp_set = true;
        // } else {
        //     pp_set = false;
        // }
    }
}

void Chart::drawShape( QPainter & p )
{
    const QRect r( rect() );

    p.save();
#ifdef USE_CLIP_RECT
    p.setClipRect(r, QPainter::CoordPainter);
#endif
    p.setBrush( NoBrush );
    QPen pen(QColor(0, 0, 0), 1, SolidLine);
    p.setPen(pen);

    p.drawRect(r);

    drawBackground(p, r);


    if(d->valid) {
        drawGrid(p, r);
        drawCurve(p, r);
    } else {
        p.drawText(r.topLeft(), QString("invalid"));
    }

    p.restore();
}

void Chart::mousePressed( const QPoint & /* e */ )
{
    //qWarning( "mousePressed at %d, %d", e.x(), e.y() );
}

void Chart::mouseReleased( const QPoint & /* e */ )
{
    //qWarning( "mouseReleased at %d, %d", e.x(), e.y() );
}

void Chart::mouseMoved( const QPoint & /* e */ )
{
    //qWarning( "mouseMoved at %d, %d", e.x(), e.y() );
}

void Chart::mouseDoubleClicked( const QPoint & /* e */ )
{
    //qWarning( "mouseDoubleClicked at %d, %d", e.x(), e.y() );
}

void Chart::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // shedule repaint
    update();
}

void Chart::propertiesChanged()
{
    long old_size = d->size;
    _SingleValue *old_values = d->values;

    const PropertyDict &pm = properties();

    d->bg_brush = pm["bg_brush"]->asBrush();
    d->border_pen = pm["border_pen"]->asPen();
    d->line_pen = pm["line_pen"]->asPen();
    d->grid_pen = pm["grid_pen"]->asPen();
    d->grid_x_step = pm["grid_x_step"]->asInteger();
    d->grid_y_step = pm["grid_y_step"]->asInteger();
    d->size = pm["size"]->asInteger();
    if(!d->auto_min)
        d->current_min = pm["min_value"]->asInteger();
    if(!d->auto_max)
        d->current_max = pm["max_value"]->asInteger();

    if(d->size <= 1) {
        d->resetValues();
    } else {
        if(d->size != old_size) {
            d->values = new _SingleValue[d->size];
            Q_CHECK_PTR(d->values);

            if(old_values != NULL) {
                if(d->size > old_size) { // grow
                    for(int i=0; i<old_size; ++i) {
                        d->values[i] = old_values[i];
                    }
                } else if(d->size < old_size) { // shrink
                    for(int j=d->size-1; j>=0; --j) {
                        d->values[j] = old_values[d->current];
                        --d->current;
                        if(d->current<0) {
                            d->current = old_size-1;
                        }
                    }
                    d->current = d->size - 1;
                }
            }
            delete[] old_values;
            d->valid = true;
        }
    }

    // shedule repaint
    update();
    // iopinsChanged();
}

void Chart::iopinsChanged()
{
    if(d->valid) {
        _SingleValue *cur_val = &d->values[d->current];
        cur_val->timestamp = QTime::currentTime();
        cur_val->valid = true;
        cur_val->value = d->input->realNumberValue();
        d->current++;
        d->current %= d->size;
        if(d->auto_min && (cur_val->value < d->current_min))
            d->current_min = cur_val->value;
        if(d->auto_max && (cur_val->value < d->current_max))
            d->current_max = cur_val->value;
    }
    // shedule repaint
    update();
}


QPointArray Chart::areaPoints() const
{

    return Template::areaPoints();

}

QRegion Chart::collisionRegion() const
{

    return Template::collisionRegion();

}

void Chart::setVisible( bool visible )
{
    Template::setVisible( visible );
}

void Chart::setCanvas( QCanvas * c )
{
    Template::setCanvas( c );
}

void Chart::advance( int /* phase */ )
{
}

void Chart::localize( LocalizatorPtr loc )
{
    if( ! loc.isNull() ) {
        m_localizator = loc;
    }
}

Template * Chart::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Chart( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Chart.cpp 1168 2005-12-12 14:48:03Z modesto $
