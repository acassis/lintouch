// $Id: check_bindingshelper.cpp 1558 2006-04-06 12:21:31Z modesto $
//
//   FILE: check_editor.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 12 July 2004
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


// custom tests
#include <cppunit/extensions/HelperMacros.h>

#include <qdir.h>

#include <lt/logger/ConsoleLogger.hh>

#include <lt/templates/Connection.hh>
#include <lt/templates/IOPin.hh>
#include <lt/templates/Template.hh>
#include <lt/templates/Variable.hh>
#include <lt/project/Panel.hh>
#include <lt/project/View.hh>
#include <lt/project/TemplateManagerBuilder.hh>

#include "PluginManager.hh"
#include "Binding.hh"
#include "BindingsHelper.hh"
#include "Bindings.hh"
#include "EditorProjectBuilder.hh"
#include "EditorProject.hh"

using namespace lt;

extern QString srcdir;
extern QString builddir;

class TestBindingsHelper : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestBindingsHelper );
    CPPUNIT_TEST( testBindingsHelper1 );

    CPPUNIT_TEST_SUITE_END();

    public:

    void setUp()
    {
    }

    void tearDown()
    {
    }
    void testBindingsHelper1()
    {
        LoggerPtr logger = LoggerPtr(new ConsoleLogger);
        ConfigurationPtr es;

        TemplateManager manager =  TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();

        PluginManager pluginManager(logger);
        CPPUNIT_ASSERT(pluginManager.plugins().count() > 1);

        ServerLoggingManager slManager(logger);
        CPPUNIT_ASSERT(pluginManager.plugins().count() > 1);

        EditorProjectBuilder pbe(logger, "en_US");
        EditorProject p = pbe.buildEditorProject(
                QDir(srcdir + "/tests").absPath() + "/testproject.zip",
                manager, pluginManager, slManager, es);
        CPPUNIT_ASSERT(p.isValid());

        const BindingsList& b1 = p.bindings().
            bindingsByConnection("thermometer");
        CPPUNIT_ASSERT(b1.count() == 2);

        ///
        const BindingsList b2 = BindingsHelper::saveVariableBindingsAndUnbind(
                &p, "thermometer");
        CPPUNIT_ASSERT(b2.count() == 2);
        CPPUNIT_ASSERT(b1.count() == 0);

        BindingsHelper::restoreBindingsAndBind(&p, b2);
        CPPUNIT_ASSERT(b1.count() == 2);

        ///
        const BindingsList b3 = BindingsHelper::saveVariableBindingsAndUnbind(
                &p, "thermometer", "level");
        CPPUNIT_ASSERT(b3.count() == 1);
        CPPUNIT_ASSERT(b1.count() == 1);

        BindingsHelper::restoreBindingsAndBind(&p, b3);
        CPPUNIT_ASSERT(b1.count() == 2);

        ///
        CPPUNIT_ASSERT(p.bindings().
                bindingsByPanel("chimney", "smokeMonitor").count() == 1);
        const BindingsList b4 = BindingsHelper::saveTemplatesBindingAndUnbind(
                &p, "chimney", "smokeMonitor",
                p.views()["chimney"].panels()["smokeMonitor"].templates());
        CPPUNIT_ASSERT(b4.count() == 1);
        CPPUNIT_ASSERT(b1.count() == 2);
        CPPUNIT_ASSERT(p.bindings().
                bindingsByPanel("chimney", "smokeMonitor").count() == 0);

        BindingsHelper::restoreBindingsAndBind(&p, b4);
        CPPUNIT_ASSERT(b1.count() == 2);
        CPPUNIT_ASSERT(p.bindings().
                bindingsByPanel("chimney", "smokeMonitor").count() == 1);

        ///
        const BindingsList b5 = BindingsHelper::saveTemplatesBindingAndUnbind(
                &p, "chimney", "temperatureMonitor", p.views()["chimney"].
                panels()["temperatureMonitor"].templates());
        CPPUNIT_ASSERT(b5.count() == 2);
        CPPUNIT_ASSERT(b1.count() == 0);

        BindingsHelper::restoreBindingsAndBind(&p, b5);
        CPPUNIT_ASSERT(b1.count() == 2);

        ///
        QString key = p.views()["chimney"].
            panels()["temperatureMonitor"].templates().currentKey();
        Template* t = p.views()["chimney"].
            panels()["temperatureMonitor"].templates().current();
        CPPUNIT_ASSERT(t);

        TemplateDict td;
        td.insert(key, t);
        CPPUNIT_ASSERT(td.count() == 1);

        const BindingsList b6 = BindingsHelper::saveTemplatesBindingAndUnbind(
                &p, "chimney", "temperatureMonitor", td);
        CPPUNIT_ASSERT(b6.count() == 1);
        CPPUNIT_ASSERT(b1.count() == 1);

        BindingsHelper::restoreBindingsAndBind(&p, b6);
        CPPUNIT_ASSERT(b1.count() == 2);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION ( TestBindingsHelper );
// end of custom tests

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_bindingshelper.cpp 1558 2006-04-06 12:21:31Z modesto $
