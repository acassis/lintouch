// $Id: QWTPlot.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTPlot.cpp -- 
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

#include "QWTPlot.hh"
#include "QWTPlot.xpm"

#include "config.h"

#define QDEBUG_RECT(r, t) qDebug(QString("%1::"#r": x:%2 y:%3 w:%4 h:%5") \
        .arg((int)t,0,16).arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

#define MAX_DATASOURCE_COUNT (3)

struct QWTPlot::QWTPlotPrivate
{
    enum { Cyclic, Continuous };

    QWTPlotPrivate() :
        localizator(LocalizatorPtr(new Localizator)),
        xbottom_samples(100), xbottom_c(Continuous), xbottom_data(NULL),
        xbottom_next(0), xbottom_values(0),
        xbottom_io(NULL)
    {
        for(int i=0; i<MAX_DATASOURCE_COUNT; ++i) {
            curves[i].c = -1;
            curves[i].data = NULL;
        }
    }

    ~QWTPlotPrivate()
    {
        delete[] xbottom_data;
        for(int i=0; i<MAX_DATASOURCE_COUNT; ++i) {
            delete[] curves[i].data;
        }
    }

    LocalizatorPtr localizator;

    // xbottom properties
    int xbottom_samples;
    int xbottom_c;
    double* xbottom_data;
    int xbottom_next;
    int xbottom_values;

    // curve data
    struct curves_t {
        long c;
        double* data;

        IOPin* io;
    } curves[MAX_DATASOURCE_COUNT];

    IOPin* xbottom_io;
};

Info QWTPlot::makeInfo()
{
    Info i;
    i.setAuthor("Patrik Modesto <modesto@swac.cz>");
    i.setVersion(VERSION);
    i.setDate(QDate::fromString(TODAY, Qt::ISODate));
    i.setShortDescription (QObject::tr("Plot", "ltl-qwt"));
    i.setLongDescription (QObject::tr(
                "Qwt Plot Widget", "ltl-qwt"));
    i.setIcon(QImage(QWTPlot_xpm));
    return i;
}

const QString QWTPlot::TYPE = "Plot";

QWTPlot::QWTPlot(LoggerPtr logger /*= Logger::nullPtr()*/) :
    QwtPlot(0, TYPE),
    Template(TYPE, logger),
    d(new QWTPlotPrivate)
{
    Q_CHECK_PTR(d);

    //register info
    registerInfo(makeInfo());

    QString g = QObject::tr("Caption","ltl-qwt");
    registerProperty(g, "caption",
            new Property(QObject::tr("Caption", "ltl-qwt"),
                "String", "<caption>", logger));

    registerProperty(g, "caption-font",
            new Property(QObject::tr("Font", "ltl-qwt"),
                "font", "", logger));


    // X Axe - Bottom
    g = QObject::tr("X Axis","ltl-qwt");
    registerProperty(g, "xbottom-label",
            new Property(QObject::tr("Label", "ltl-qwt"),
                "string", "time", logger));

    registerProperty(g, "xbottom-samples",
            new Property(QObject::tr("Samples", "ltl-qwt"),
                "integer", "100", logger));

    registerProperty(g, "xbottom-step-size",
            new Property(QObject::tr("Step Size", "ltl-qwt"),
                "float", "10", logger));

    registerProperty(g, "xbottom-mode",
            new Property(QObject::tr("Sampling Mode", "ltl-qwt"),
                "choice(Cyclic,Continuous)", "Continuous", logger));

    // Y Axe - Left
    g = QObject::tr("Y Axis","ltl-qwt");
    registerProperty(g, "yleft-label",
            new Property(QObject::tr("Label", "ltl-qwt"),
                "string", "<value>", logger));

    registerProperty(g, "yleft-min",
            new Property(QObject::tr("Lower Limit", "ltl-qwt"),
                "float", "0", logger));

    registerProperty(g, "yleft-max",
            new Property(QObject::tr("Upper Limit", "ltl-qwt"),
                "float", "100", logger));

    registerProperty(g, "yleft-step-size",
            new Property(QObject::tr("Step Size", "ltl-qwt"),
                "float", "25", logger));

    registerProperty(g, "yleft-autoscale",
            new Property(QObject::tr("Autoscale", "ltl-qwt"),
                "choice(Yes,No)", "No", logger));

    g = QObject::tr("Grid","ltl-qwt");
    registerProperty(g, "maj-grid-x-enabled",
            new Property(QObject::tr("Major X Grid Enabled", "ltl-qwt"),
                "choice(Yes,No)", "Yes", logger));

    registerProperty(g, "grid-x-enabled",
            new Property(QObject::tr("X Grid Enabled", "ltl-qwt"),
                "choice(Yes,No)", "No", logger));

    registerProperty(g, "maj-grid-y-enabled",
            new Property(QObject::tr("Major Y Grid Enabled", "ltl-qwt"),
                "choice(Yes,No)", "Yes", logger));

    registerProperty(g, "grid-y-enabled",
            new Property(QObject::tr("Y Grid Enabled", "ltl-qwt"),
                "choice(Yes,No)", "No", logger));

    registerProperty(g, "grid-maj-pen",
            new Property(QObject::tr("Major Grid Pen", "ltl-qwt"),
                "pen", "", logger));

    registerProperty(g, "grid-pen",
            new Property(QObject::tr("Grid Pen", "ltl-qwt"),
                "pen", "", logger));

    // Legend
    g = QObject::tr("Legend","ltl-qwt");
    registerProperty(g, "legend-enable",
            new Property(QObject::tr("Enabled", "ltl-qwt"),
                "choice(Yes,No)", "No", logger));

    registerProperty(g, "legend-pos",
            new Property(QObject::tr("Position", "ltl-qwt"),
                "choice(Left,Right,Top,Bottom)", "Right", logger));

    registerProperty(g, "legend-font",
            new Property(QObject::tr("Font", "ltl-qwt"),
                "font", "", logger));

    g = QObject::tr("Inputs", "ltl-qwt");

    for(int i=0; i<MAX_DATASOURCE_COUNT; ++i) {
        registerDataSource(i, QString("y%1").arg(i+1),
                QObject::tr("Value %1", "ltl-qwt").arg(i+1));
    }

    // IOPIN for X
    registerIOPin(g, "x",
            d->xbottom_io = new IOPin(this, QObject::tr("x", "ltl-qwt"),
                "bit,number,string"));
    Q_CHECK_PTR(d->xbottom_io);
}

QWTPlot::~QWTPlot()
{
    delete d;
}

void QWTPlot::registerDataSource(int pos, const QString& prefix,
        const QString& title)
{
    QString g = title;
    registerProperty(g, prefix,
            new Property(QObject::tr("Enabled", "ltl-qwt"),
                "choice(Yes,No)", "No", getLoggerPtr()));

    registerProperty(g, QString("%1-label").arg(prefix),
            new Property(QObject::tr("Label", "ltl-qwt"),
                "string", title, getLoggerPtr()));

    registerProperty(g, QString("%1-pen").arg(prefix),
            new Property(QObject::tr("Pen", "ltl-qwt"),
                "pen", "", getLoggerPtr()));

    registerProperty(g, QString("%1-brush").arg(prefix),
            new Property(QObject::tr("Brush", "ltl-qwt"),
                "brush", "", getLoggerPtr()));

    registerProperty(g, QString("%1-style").arg(prefix),
            new Property(QObject::tr("Style", "ltl-qwt"),
                "choice(Lines,Sticks,Steps,Dots,Spline)", "Lines",
                getLoggerPtr()));

    g = QObject::tr("Inputs", "ltl-qwt");
    registerIOPin(g, prefix,
            d->curves[pos].io = new IOPin(this, prefix, "number"));
    Q_CHECK_PTR(d->curves[pos].io);
}

void QWTPlot::curvePropertiesChanged(int pos, const QString& prefix,
        const PropertyDict &pm)
{
    QWTPlotPrivate::curves_t* t = &d->curves[pos];

    int style;

    if(t->c != -1) {
        removeCurve(t->c);
        t->c = -1;
        delete[] t->data;
        t->data = NULL;
    }
    if(pm[prefix]->asString() == "Yes") {
        t->c = QwtPlot::insertCurve(
                pm[QString("%1-label").arg(prefix)]->asString(),
                QwtPlot::xBottom, QwtPlot::yLeft);

        Q_ASSERT(t->data == NULL);
        t->data = new double[d->xbottom_samples];
        Q_CHECK_PTR(t->data);

        QwtPlot::setCurvePen(t->c, pm[QString("%1-pen").arg(prefix)]->asPen());
        QwtPlot::setCurveBrush(t->c,
                pm[QString("%1-brush").arg(prefix)]->asBrush());

        QString s = pm[QString("%1-style").arg(prefix)]->asString();
        if(s == "Sticks") {
            style = QwtCurve::Sticks;
        } else if(s == "Steps") {
            style = QwtCurve::Steps;
        } else if(s == "Dots") {
            style = QwtCurve::Dots;
        } else if(s == "Spline") {
            style = QwtCurve::Spline;
        } else {
            style = QwtCurve::Lines;
        }
        QwtPlot::setCurveStyle(t->c, style);
    }
}

void QWTPlot::localize(LocalizatorPtr localizator)
{
    Q_CHECK_PTR(d);
    d->localizator = localizator;
}

void QWTPlot::drawShape(QPainter &p)
{
    p.save();

    const QPoint o = Template::rect().topLeft();
    p.translate(o.x(), o.y());
    const QRect rr(QPoint(0, 0), Template::rect().size());

    QwtPlot::replot();
    QwtPlot::print(&p, rr);

    p.restore();
}

void QWTPlot::propertiesChanged()
{
    Q_ASSERT(d);

    const PropertyDict &pm =(const PropertyDict&)properties();

    QwtPlot::setTitle(pm["caption"]->asString());
    QwtPlot::setTitleFont(pm["caption-font"]->asFont());

    QwtPlot::setGridPen(pm["grid-pen"]->asPen());
    QwtPlot::setGridMajPen(pm["grid-maj-pen"]->asPen());

    QwtPlot::enableGridX(pm["maj-grid-x-enabled"]->asString() == "Yes");
    QwtPlot::enableGridXMin(pm["grid-x-enabled"]->asString() == "Yes");
    QwtPlot::enableGridY(pm["maj-grid-y-enabled"]->asString() == "Yes");
    QwtPlot::enableGridYMin(pm["grid-y-enabled"]->asString() == "Yes");

    ///////////////
    // X Bottom Axe
    enableXBottomAxis(true);
    d->xbottom_samples = pm["xbottom-samples"]->asInteger();
    setAxisTitle(QwtPlot::xBottom, pm["xbottom-label"]->asString());
    delete[] d->xbottom_data;
    d->xbottom_data = NULL;
    if(d->xbottom_samples < 1) {
        d->xbottom_samples = 1;
    }
    d->xbottom_data = new double[d->xbottom_samples];
    Q_CHECK_PTR(d->xbottom_data);

    if(pm["xbottom-mode"]->asString() == "Continuous") {
        setAxisScale(QwtPlot::xBottom, -d->xbottom_samples, 0.0,
                pm["xbottom-step-size"]->asFloatingPoint());
        d->xbottom_c = QWTPlotPrivate::Continuous;
        d->xbottom_next = d->xbottom_samples - 1;
        for(int i = 0; i < d->xbottom_samples; ++i) {
            d->xbottom_data[i] = i - (d->xbottom_samples - 1);
        }
    } else {
        setAxisScale(QwtPlot::xBottom, 0.0, d->xbottom_samples,
                pm["xbottom-step-size"]->asFloatingPoint());
        d->xbottom_c = QWTPlotPrivate::Cyclic;
        d->xbottom_next = 0;
        for(int i = 0; i < d->xbottom_samples; ++i) {
            d->xbottom_data[i] = i;
        }
    }

    /////////////
    // Y Left Axe
    enableYLeftAxis(true);
    setAxisScale(QwtPlot::yLeft,
            pm["yleft-min"]->asFloatingPoint(),
            pm["yleft-max"]->asFloatingPoint(),
            pm["yleft-step-size"]->asFloatingPoint());
    setAxisTitle(QwtPlot::yLeft, pm["yleft-label"]->asString());
    if(pm["yleft-autoscale"]->asString() == "Yes") {
        setAxisAutoScale(QwtPlot::yLeft);
    }

    QString s;

    //////////////
    // DataSource
    for(int i=0; i<MAX_DATASOURCE_COUNT; ++i) {
        curvePropertiesChanged(i, QString("y%1").arg(i+1), pm);
    }

    /////////
    // legend
    QString lp = pm["legend-pos"]->asString();
    if(lp == "Left") {
        QwtPlot::setLegendPosition(QwtPlot::Left);
    } else if(lp == "Top") {
        QwtPlot::setLegendPosition(QwtPlot::Top);
    } else if(lp == "Bottom") {
        QwtPlot::setLegendPosition(QwtPlot::Bottom);
    } else {
        QwtPlot::setLegendPosition(QwtPlot::Right);
    }
    QwtPlot::enableLegend((pm["legend-enable"]->asString() == "Yes"), -1);
    QwtPlot::setLegendFont(pm["legend-font"]->asFont());

    //////////////////////////////
    // update and schedule repaint
    QWTPlot::setRect(Template::rect());
}

void QWTPlot::setRect(const QRect & rect)
{
    const QSize oldSize = Template::rect().size();

    Q_ASSERT(d);

    Template::setRect(rect);

    const QRect rr(QPoint(0, 0), rect.size());
    QwtPlot::setGeometry(rr);
    {
        QResizeEvent rse(oldSize, rect.size());
        QwtPlot::resizeEvent(&rse);
    }

    QwtPlot::updateLayout();

    // and schedule repaint
    Template::update();
}

void QWTPlot::iopinsChanged()
{
    Q_ASSERT(d);
    Q_ASSERT(d->xbottom_io);
    Q_ASSERT(d->xbottom_data);

    if(d->xbottom_io->isRealValueDirty()) {

        for(int i=0; i<MAX_DATASOURCE_COUNT; ++i) {
            QWTPlotPrivate::curves_t* t = &d->curves[i];

            if(t->data == NULL) {
                continue;
            }

            if(t->c != -1) {
                Q_ASSERT(t->data);
                t->data[d->xbottom_next] = t->io->realNumberValue();
            }
        }

        if(d->xbottom_values < d->xbottom_samples) {
            ++d->xbottom_values;
        }

        ++d->xbottom_next;
        if(d->xbottom_next == d->xbottom_samples) {

            if(d->xbottom_c == QWTPlotPrivate::Cyclic) {
                d->xbottom_next = 0;
            } else {
                for(int j=0; j<MAX_DATASOURCE_COUNT; ++j) {
                    QWTPlotPrivate::curves_t* t = &d->curves[j];
                    if(t->data == NULL) continue;

                    for(int i = 1; i < d->xbottom_samples; ++i) {
                        if(t->c != -1) {
                            t->data[i - 1] = t->data[i];
                        }
                    }
                }
                --d->xbottom_next;
            }
        }

        if(d->xbottom_c == QWTPlotPrivate::Cyclic) {
            for(int j=0; j<MAX_DATASOURCE_COUNT; ++j) {
                QWTPlotPrivate::curves_t* t = &d->curves[j];
                if(t->data == NULL) continue;

                if(t->c != -1) {
                    QwtPlot::setCurveRawData(t->c, d->xbottom_data, t->data,
                            d->xbottom_values);
                }
            }
        } else {
            int f = d->xbottom_samples - d->xbottom_values;
            for(int j=0; j<MAX_DATASOURCE_COUNT; ++j) {
                QWTPlotPrivate::curves_t* t = &d->curves[j];
                if(t->data == NULL) continue;

                if(t->c != -1) {
                    QwtPlot::setCurveRawData(t->c,
                            &d->xbottom_data[f], &t->data[f],
                            d->xbottom_values);
                }
            }
        }

        Template::update();
    }

    for(int i=0; i<MAX_DATASOURCE_COUNT; ++i) {
        d->curves[i].io->syncRealValue();
    }
    d->xbottom_io->syncRealValue();
}

Template * QWTPlot::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */) const
{
    Template * result = new QWTPlot(logger);
    setupAfterClone(result, loc, tm);
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTPlot.cpp 844 2005-06-21 12:40:50Z mman $
