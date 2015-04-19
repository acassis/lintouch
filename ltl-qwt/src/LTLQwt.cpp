// $Id: LTLQwt.cpp 347 2005-02-23 14:56:10Z modesto $
//
//   FILE: LTLQwt.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 04 November 2003
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

#include "LTLQwt.hh"

#include "lt/templates/PluginHelper.hh"
using namespace lt;

#include "QWTAnalogClock.hh"
#include "QWTButton.hh"
#include "QWTDial.hh"
#include "QWTKnob.hh"
#include "QWTPlot.hh"
#include "QWTScale.hh"
#include "QWTActiveText.hh"
#include "QWTSlider.hh"
#include "QWTThermo.hh"
#include "QWTWheel.hh"

#include "config.h"

// library private data
struct LTLQwt::LTLQwt_private
{
    LTLQwt_private()
    {
    }

    ~LTLQwt_private()
    {
    }
};

LTLQwt::LTLQwt() :
    TemplateLibrary(),
    d( new LTLQwt_private )
{
    Q_CHECK_PTR(d);

    //register info
    Info i;
    i.setAuthor( "Martin Man <mman@swac.cz>, Patrik Modesto <modesto@swac.cz" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Qt Technical Widgets", "ltl-qwt" ) );
    i.setLongDescription( QObject::tr( "Qt Technical Widgets", "ltl-qwt" ) );

    registerInfo(i);

    // register templates into this library (in alphabetical order)
    registerTemplateInfo( QWTActiveText::TYPE,  QWTActiveText::makeInfo());
    registerTemplateInfo( QWTAnalogClock::TYPE, QWTAnalogClock::makeInfo());
    registerTemplateInfo( QWTButton::TYPE,      QWTButton::makeInfo());
    registerTemplateInfo( QWTDial::TYPE,        QWTDial::makeInfo());
    registerTemplateInfo( QWTKnob::TYPE,        QWTKnob::makeInfo());
    registerTemplateInfo( QWTPlot::TYPE,        QWTPlot::makeInfo());
    registerTemplateInfo( QWTScale::TYPE,       QWTScale::makeInfo());
    registerTemplateInfo( QWTSlider::TYPE,      QWTSlider::makeInfo());
    registerTemplateInfo( QWTThermo::TYPE,      QWTThermo::makeInfo());
    registerTemplateInfo( QWTWheel::TYPE,       QWTWheel::makeInfo());
}

LTLQwt::~LTLQwt()
{
    delete d;
}

Template * LTLQwt::instantiate(const QString & type,
        LoggerPtr logger /*= Logger::nullPtr()*/) const
{
    Q_CHECK_PTR(d);

    if(type == QWTAnalogClock::TYPE) {
        return new QWTAnalogClock(logger);

    } else if(type == QWTButton::TYPE) {
        return new QWTButton(logger);

    } else if(type == QWTDial::TYPE) {
        return new QWTDial(logger);

    } else if(type == QWTKnob::TYPE) {
        return new QWTKnob(logger);

    } else if(type == QWTPlot::TYPE) {
        return new QWTPlot(logger);

    } else if(type == QWTScale::TYPE) {
        return new QWTScale(logger);

    } else if(type == QWTActiveText::TYPE) {
        return new QWTActiveText(logger);

    } else if(type == QWTSlider::TYPE) {
        return new QWTSlider(logger);

    } else if(type == QWTThermo::TYPE) {
        return new QWTThermo(logger);

    } else if(type == QWTWheel::TYPE) {
        return new QWTWheel(logger);

    }
    return NULL;
}

// library loader macro
EXPORT_LT_PLUGIN(LTLQwt, "lt::TemplateLibrary");

// vim: set et ts=4 sw=4 tw=76:
// $Id: LTLQwt.cpp 347 2005-02-23 14:56:10Z modesto $
