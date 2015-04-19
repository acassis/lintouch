// $Id: QWTActiveText.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTActiveText.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 27 April 2005
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
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

#include <qcolor.h>
#include <qevent.h>
#include <qfont.h>
#include <qregexp.h>

#include "QWTActiveText.hh"
#include "QWTActiveText.xpm"

#include "config.h"

#define QDEBUG_RECT(r, t) qDebug(QString("%1::"#r": x:%2 y:%3 w:%4 h:%5") \
        .arg((int)t,0,16).arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

static int _StringToFlags( const QString &ha, const QString &va ) {

    int flags;

    // update text align flags
    if( ha == "right" ) {
        flags = Qt::AlignRight;
    } else if( ha == "hcenter" ) {
        flags = Qt::AlignHCenter;
    } else if( ha == "justify" ) {
        flags = Qt::AlignJustify;
    } else {
        flags = Qt::AlignLeft;
    }

    if( va == "bottom" ) {
        flags |= Qt::AlignBottom;
    } else if( va == "vcenter" ) {
        flags |= Qt::AlignVCenter;
    } else {
        flags |= Qt::AlignTop;
    }

#ifdef Q_WS_QWS
    flags |= Qt::WordBreak | Qt::NoAccel;
#else
    flags |= Qt::BreakAnywhere | Qt::NoAccel;
#endif

    return flags;
}

struct QWTActiveText::QWTActiveTextPrivate
{
    QWTActiveTextPrivate() :
        localizator(LocalizatorPtr(new Localizator))
    {
        for(int i=0; i<10; ++i) {
            values[i].io = NULL;
        }
    }

    void readProps(int i, const PropertyDict &pm)
    {
        Q_ASSERT(i >= 0);
        Q_ASSERT(i < 10);

        int ii = i+1;

        values[i].len = pm[QString("i%1_length").arg(ii)]->asInteger();

        QString fmt = pm[QString("i%1_type").arg(ii)]->asString();
        if(fmt == "Bit") {
            values[i].t = IOPin::BitType;
        } else if(fmt == "String") {
            values[i].t = IOPin::StringType;
        } else { // Number
            values[i].t = IOPin::NumberType;
        }

        fmt = pm[QString("i%1_base").arg(ii)]->asString();
        if(fmt == "2") {
            values[i].base = 2;
        } else if(fmt == "8") {
            values[i].base = 8;
        } else if(fmt == "16") {
            values[i].base = 16;
        } else { //10
            values[i].base = 10;
        }
    }

    LocalizatorPtr localizator;

    QPointArray points;

    QFont f_orig, f;
    QString txt;

    struct value_t {
        IOPin* io;
        IOPin::Type t;
        int base;
        int len;
    } values[10];
};

Info QWTActiveText::makeInfo()
{
    Info i;
    i.setAuthor("Patrik Modesto <modesto@swac.cz>");
    i.setVersion(VERSION);
    i.setDate(QDate::fromString(TODAY, Qt::ISODate));
    i.setShortDescription (QObject::tr("ActiveText", "ltl-qwt"));
    i.setLongDescription (
            QObject::tr("ActiveText Widget", "ltl-qwt"));
    i.setIcon(QImage(QWTActiveText_xpm));
    return i;
}

const QString QWTActiveText::TYPE = "ActiveText";

QWTActiveText::QWTActiveText(LoggerPtr logger /*= Logger::nullPtr()*/) :
    QwtPlainText("", QFont(), QColor()),
    Template(TYPE, logger),
    d(new QWTActiveTextPrivate)
{
    Q_CHECK_PTR(d);

    //register info
    registerInfo(makeInfo());

    QString g = QObject::tr("Appearance","ltl-qwt");
    registerProperty(g, "text",
            new Property(QObject::tr("Text", "ltl-qwt"),
                "string", "At 'i1' is '%1'", logger));

    registerProperty(g, "font",
            new Property(QObject::tr("Font", "ltl-qwt"),
                "font", "", logger));

    registerProperty(g, "color",
            new Property(QObject::tr("Color", "ltl-qwt"),
                "color", "", logger));

    // text align flags
    registerProperty( g, "halign",
            new Property(QObject::tr("Horizontal Alignment", "ltl-qwt"),
                "choice(left,right,hcenter,justify)", "hcenter", logger ) );

    registerProperty( g, "valign",
            new Property( QObject::tr("Vertical Alignment", "ltl-qwt"),
                "choice(top,bottom,vcenter)", "vcenter", logger ) );


    QString io_g = QObject::tr("Inputs", "ltl-qwt");
    int ii;

    for(int i=0; i<10; ++i) {
        ii = i + 1;
        g = QObject::tr("Placeholder %1","ltl-qwt").arg(ii);

        registerProperty(g, QString("i%1_type").arg(ii),
                new Property(QObject::tr("Type", "ltl-qwt"),
                    "choice(Bit,Number,String)", "Number", logger));

        registerProperty(g, QString("i%1_base").arg(ii),
                new Property(QObject::tr("Base", "ltl-qwt"),
                    "choice(2,8,10,16)", "10", logger));

        registerProperty(g, QString("i%1_length").arg(ii),
                new Property(QObject::tr("Length", "ltl-qwt"),
                    "integer", "0", logger));


        // [-- IOPins --]
        registerIOPin(io_g, QString("i%1").arg(ii),
                d->values[i].io = new IOPin(this,
                    QObject::tr("Position %1", "ltl-qwt").arg(ii),
                    "bit,number,string"));
        Q_CHECK_PTR(d->values[i].io);
    }

}

QWTActiveText::~QWTActiveText()
{
    delete d;
}

void QWTActiveText::localize(LocalizatorPtr localizator)
{
    Q_CHECK_PTR(d);
    d->localizator = localizator;
}

void QWTActiveText::drawShape(QPainter &p)
{
    p.save();

    const QPoint o = Template::rect().topLeft();
    p.translate(o.x(), o.y());
    const QRect rr(0, 0, Template::rect().width(), Template::rect().height());

    QwtPlainText::draw(&p, rr);

    p.restore();
}

void QWTActiveText::propertiesChanged()
{
    Q_ASSERT(d);

    const PropertyDict &pm = (const PropertyDict&)properties();

    // font
    d->f_orig = pm["font"]->asFont();
    recalculateFontSizes();

    // color
    QwtPlainText::setColor(pm["color"]->asColor());

    // Alignment
    QwtPlainText::setAlignment(_StringToFlags(
            pm["halign"]->asString(), pm["valign"]->asString()));

    // text
    d->txt = pm["text"]->asString();
    QwtPlainText::setText(d->txt);

    for(int i=0; i<10; ++i) {
        d->readProps(i, pm);
    }

    // update and schedule repaint
    Template::update();
}

void QWTActiveText::setRect(const QRect & rect)
{
    Template::setRect(rect);

    d->points = rect;

    recalculateFontSizes();

    // and schedule repaint
    Template::update();
}

void QWTActiveText::iopinsChanged()
{
    Q_ASSERT(d);

    QString t = d->txt;
    static QRegExp r("%\\d");

    for(int i=0; i<10; ++i) {
        if(t.find(r) == -1) {
            break;
        }
        Q_ASSERT(d->values[i].io);
        switch(d->values[i].t) {
            case IOPin::BitType:
                t = t.arg(d->values[i].io->realBitValue());
                break;

            case IOPin::NumberType:
                t = t.arg(d->values[i].io->realNumberValue(),
                        d->values[i].len, d->values[i].base);
                break;

            case IOPin::StringType:
                t = t.arg(d->values[i].io->realStringValue(),
                        d->values[i].len);
                break;

            default:
                // default added because of gcc warning
                break;
        }
    }

    QwtPlainText::setText(t);

    Template::update();
}

void QWTActiveText::recalculateFontSizes()
{
    d->f = makeFont(d->f_orig);
    QwtPlainText::setFont(d->f);
}

QPointArray QWTActiveText::areaPoints() const
{
    return d->points;
}


Template * QWTActiveText::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */) const
{
    Template * result = new QWTActiveText(logger);
    setupAfterClone(result, loc, tm);
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTActiveText.cpp 844 2005-06-21 12:40:50Z mman $
