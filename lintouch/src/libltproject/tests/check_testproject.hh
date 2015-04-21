// $Id: check_testproject.hh,v 1.48 2004/12/16 12:33:22 modesto Exp $
//
//   FILE: check_testproject.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 27 November 2003
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

#ifndef _CHECK_TESTPROJECT_HH
#define _CHECK_TESTPROJECT_HH

// custom tests
#include <cppunit/extensions/HelperMacros.h>

#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qlibrary.h>
#include <qthread.h>
#include <qtimer.h>

#include <lt/logger/ConsoleLogger.hh>
#include <lt/configuration/DefaultConfigurationBuilder.hh>

#include <lt/cp/http.h>

#include <lt/templates/Connection.hh>
#include <lt/templates/Localizator.hh>
#include <lt/templates/PluginTemplateLibraryLoader.hh>
#include <lt/templates/TemplateLibrary.hh>
#include <lt/templates/TemplateManager.hh>
#include <lt/templates/Variable.hh>
#include <lt/vfs/VFSZIP.hh>

#include "lt/project/InfoBuilder.hh"
#include "lt/project/TemplateBuilder.hh"
#include "lt/project/VariableBuilder.hh"
#include "lt/project/ConnectionBuilder.hh"
#include "lt/project/Project.hh"
#include "lt/project/TemplateManagerBuilder.hh"
#include "lt/project/ProjectBuilder.hh"
#include "lt/project/ProjectAsyncBuilder.hh"
#include "lt/project/Panel.hh"
#include "lt/project/PanelBuilder.hh"
#include "lt/project/View.hh"
#include "lt/project/ViewBuilder.hh"
using namespace lt;

extern QString srcdir;
extern QString builddir;

extern QApplication *g_app;

class DummyTemplate: public Template
{
    public:

        DummyTemplate( LoggerPtr logger = Logger::nullPtr() ) :
            Template( "Dummy" ) {};
        virtual ~DummyTemplate() {};

        virtual Template * clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
                LoggerPtr logger = Logger::nullPtr() ) const
        {
            Template * result = new DummyTemplate( logger );
            setupAfterClone( result, loc, tm );
            return result;
        };
};

class TestProject :
    public QObject,
    public CppUnit::TestFixture
{
    Q_OBJECT

        CPPUNIT_TEST_SUITE ( TestProject );

    CPPUNIT_TEST ( testVariableBuilder );
    CPPUNIT_TEST ( testConnectionBuilder );
    CPPUNIT_TEST ( testPanel );
    CPPUNIT_TEST ( testPanelBuilder );
    CPPUNIT_TEST ( testView );
    CPPUNIT_TEST ( testViewBuilder );
    CPPUNIT_TEST ( testProject );
    CPPUNIT_TEST ( testEProject );
    CPPUNIT_TEST ( testProjectBuilder );
    CPPUNIT_TEST ( testProjectAsyncBuilder );
    CPPUNIT_TEST ( testTemplateBuilder );
    CPPUNIT_TEST ( testInfoBuilder );

    CPPUNIT_TEST_SUITE_END ();

    QString templateLibraryPath;
    LoggerPtr logger;
    TemplateManager templateManager;

    lt_http_server_t * http;

    bool m_progress;
    int m_status;
    bool m_downloaded;
    bool m_finished;
    ProjectAsyncBuilder *m_pab;

    public:
    TestProject()
    {
        logger = LoggerPtr(new ConsoleLogger);
        templateManager =
            TemplateManagerBuilder(logger).buildFromDefaultDirs();
    }

    void setUp () {
        m_status = -1;
    }

    void tearDown () {
    }

    void testPanel () {

        Panel p1, p2;

        Template * t1 = NULL, * t2 = NULL;

        t1 = new DummyTemplate();
        t2 = new DummyTemplate();
        t1->setRect( QRect( 0, 0, 10, 10 ) );
        t2->setRect( QRect( 10, 10, 10, 10 ) );

        p1.addTemplate( "t1", t1 );
        p1.addTemplate( "t2", t2 );

        CPPUNIT_ASSERT( p1.templates().count() == 2 );
        CPPUNIT_ASSERT( p1.templates().contains( "t1" ) );
        CPPUNIT_ASSERT( p1.templates().contains( "t2" ) );
        CPPUNIT_ASSERT( p1.templates()[ "t1" ] == t1 );
        CPPUNIT_ASSERT( p1.templates()[ "t2" ] == t2 );

        p2 = p1.clone( Localizator::nullPtr(), TemplateManager() );

        CPPUNIT_ASSERT( p2.templates().count() == 2 );
        CPPUNIT_ASSERT( p2.templates().contains( "t1" ) );
        CPPUNIT_ASSERT( p2.templates().contains( "t2" ) );
        CPPUNIT_ASSERT( p2.templates()[ "t1" ] != t1 );
        CPPUNIT_ASSERT( p2.templates()[ "t2" ] != t2 );

        t1->setRect( QRect( 100, 100, 9, 9 ) );
        t2->setRect( QRect( 100, 100, 11, 11 ) );

        CPPUNIT_ASSERT( p1.templates()[ "t1" ]->rect() ==
                QRect( 100, 100, 9, 9 ) );
        CPPUNIT_ASSERT( p1.templates()[ "t2" ]->rect() ==
                QRect( 100, 100, 11, 11 ) );

        CPPUNIT_ASSERT( p2.templates()[ "t1" ]->rect() ==
                QRect( 0, 0, 10, 10 ) );
        CPPUNIT_ASSERT( p2.templates()[ "t2" ]->rect() ==
                QRect( 10, 10, 10, 10 ) );
    }

    void testPanelBuilder () {
        CPPUNIT_ASSERT(
                templateManager.templateLibraries().count());

        QString err;
        int line, col;
        ConfigurationPtr c =
            DefaultConfigurationBuilder().buildFromFile(
                    srcdir + "/tests/data/Panel.xml", err, line, col);
        CPPUNIT_ASSERT(!c.isNull());

        Panel p = PanelBuilder::buildFromConfiguration(c,
                LocalizatorPtr(),
                templateManager, logger);

        CPPUNIT_ASSERT(p.templates().count() == 2);
        CPPUNIT_ASSERT(p.templates()["overHeat"]->
                type() == "Lamp" );
        CPPUNIT_ASSERT(p.templates()["overHeat"]->
                library() == "mytemplatelibrary" );
        CPPUNIT_ASSERT(p.templates()["overHeat"]->
                shortcut() == QKeySequence( "SHIFT+F11" ) );
        CPPUNIT_ASSERT(p.templates()["overHeat"]->
                properties()["fill_off"]->encodeValue() ==
                "#FF0000;SOLIDPATTERN");
        CPPUNIT_ASSERT(p.templates()["underHeat"]->
                properties()["fill_off"]->encodeValue() ==
                "#0000FF;SOLIDPATTERN");

        c = DefaultConfigurationBuilder().buildFromFile(
                srcdir + "/tests/data/PanelEmpty.xml", err, line, col);
        CPPUNIT_ASSERT(!c.isNull());
        p = PanelBuilder::buildFromConfiguration(c,
                LocalizatorPtr(),
                templateManager, logger);
        CPPUNIT_ASSERT(p.templates().count() == 0);

        c = DefaultConfigurationBuilder().buildFromFile(
                srcdir + "/tests/data/PanelEmpty2.xml", err, line, col);
        CPPUNIT_ASSERT(!c.isNull());
        p = PanelBuilder::buildFromConfiguration(c,
                LocalizatorPtr(),
                templateManager, logger);
        CPPUNIT_ASSERT(p.templates().count() == 0);
    }


    void testVariableBuilder () {
        QString err;
        int line, col;
        ConfigurationPtr c =
            DefaultConfigurationBuilder().buildFromFile(
                    srcdir + "/tests/data/Variable.xml", err, line, col);
        CPPUNIT_ASSERT(!c.isNull());

        Variable *v = VariableBuilder::buildFromConfiguration(
                c, logger);
        CPPUNIT_ASSERT(v);
        CPPUNIT_ASSERT(v->type() == IOPin::NumberType);
        CPPUNIT_ASSERT(v->properties().contains("address"));
        CPPUNIT_ASSERT(!v->properties().contains("hamburger"));
        CPPUNIT_ASSERT(v->properties()["address"]->asString() == "1");
        delete v;
    }

    void testConnectionBuilder () {
        QFile f(srcdir + "/tests/data/Thermometer.xml");
        f.open(IO_ReadOnly);
        Connection *c = ConnectionBuilder::buildFromFile(&f, logger);

        CPPUNIT_ASSERT(c);

        CPPUNIT_ASSERT(c->properties().contains("subtype"));
        CPPUNIT_ASSERT(c->properties()["subtype"]->encodeValue()
                == "thermometer");

        CPPUNIT_ASSERT(c->variables().count() == 2);
        CPPUNIT_ASSERT(!c->variables().contains("address"));
        CPPUNIT_ASSERT(c->variables().contains("pressure"));

        CPPUNIT_ASSERT(c->variables()["pressure"]
                ->type() == IOPin::NumberType);
        CPPUNIT_ASSERT(c->variables()["pressure"]
                ->properties()["address"]->asString() == "2");

        f.close();
        f.setName(srcdir + "/tests/data/ThermometerEmpty.xml");
        f.open(IO_ReadOnly);
        delete c;
        c = ConnectionBuilder::buildFromFile(&f, logger);
        CPPUNIT_ASSERT(c);

        f.close();
        f.setName(srcdir + "/tests/data/ThermometerEmpty2.xml");
        f.open(IO_ReadOnly);
        delete c;
        c = ConnectionBuilder::buildFromFile(&f, logger);
        CPPUNIT_ASSERT(c);
        delete c;
    }

    void testView () {

        View v, v1;

        Panel p1, p2, p3;

        CPPUNIT_ASSERT ( v.panels ().count () == 0 );
        CPPUNIT_ASSERT ( v.virtualWidth () == (unsigned int) 0  );
        CPPUNIT_ASSERT ( v.virtualHeight () == (unsigned int) 0 );
        CPPUNIT_ASSERT ( v.removePanel ( "whatever" ) == false );

        v.setVirtualWidth ( 1024 );
        CPPUNIT_ASSERT ( v.virtualWidth () == 1024 );
        v.setVirtualHeight ( 768 );
        CPPUNIT_ASSERT ( v.virtualHeight () == 768 );

        CPPUNIT_ASSERT ( v.addPanel ( "p1", p1 ) == true );
        CPPUNIT_ASSERT ( v.addPanel ( "p2", p2 ) == true );
        CPPUNIT_ASSERT ( v.addPanel ( "p3", p3 ) == true );
        CPPUNIT_ASSERT ( v.addPanel ( "p3", p2 ) == false );

        CPPUNIT_ASSERT ( v.panels ().count () == 3 );
        CPPUNIT_ASSERT ( v.panels ().contains ( "p1" ) );
        CPPUNIT_ASSERT ( v.panels ().contains ( "p2" ) );
        CPPUNIT_ASSERT ( v.panels ().contains ( "p3" ) );

        v1 = v.clone( Localizator::nullPtr(), TemplateManager() );

        CPPUNIT_ASSERT ( v.removePanel ( "p1" ) == true );
        CPPUNIT_ASSERT ( v.removePanel ( "p2" ) == true );
        CPPUNIT_ASSERT ( v.removePanel ( "p3" ) == true );
        CPPUNIT_ASSERT ( v.removePanel ( "p3" ) == false );

        CPPUNIT_ASSERT ( v.panels ().count () == 0 );

        // now test the cloned instance
        CPPUNIT_ASSERT ( v1.virtualWidth () == 1024 );
        CPPUNIT_ASSERT ( v1.virtualHeight () == 768 );


        CPPUNIT_ASSERT ( v1.panels ().count () == 3 );
        CPPUNIT_ASSERT ( v1.panels ().contains ( "p1" ) );
        CPPUNIT_ASSERT ( v1.panels ().contains ( "p2" ) );
        CPPUNIT_ASSERT ( v1.panels ().contains ( "p3" ) );

    }

    void testViewBuilder () {
        CPPUNIT_ASSERT(
                templateManager.templateLibraries().count() > 0);

        QFile f(srcdir + "/tests/data/View.xml");
        f.open(IO_ReadOnly);
        ViewBuilder vb(0, templateManager, LocalizatorPtr());
        View v = vb.buildFromFile(&f, logger);

        CPPUNIT_ASSERT(v.panels().count() == 2);
        CPPUNIT_ASSERT(v.virtualWidth() == 170);
        CPPUNIT_ASSERT(v.panels()["temperatureMonitor"]
                .templates()["overHeat"]
                ->properties()["fill_off"]->encodeValue() ==
                "#FF0000;SOLIDPATTERN");

        f.close();
        f.setName(srcdir + "/tests/data/ViewEmpty.xml");
        f.open(IO_ReadOnly);
        v = vb.buildFromFile(&f);

        f.close();
        f.setName(srcdir + "/tests/data/ViewEmpty2.xml");
        f.open(IO_ReadOnly);
        v = vb.buildFromFile(&f);

        f.close();
        f.setName(srcdir + "/tests/data/ViewInvalid.xml");
        f.open(IO_ReadOnly);
        v = vb.buildFromFile(&f);
    }

    void testProject () {

        Project p;

        View v1, v2, v3;
        Connection *c1 = new Connection("string", PropertyDict());
        Connection *c2 = new Connection("string", PropertyDict());
        Connection *c3 = new Connection("string", PropertyDict());


        // set info and determine whether we are valid.

        CPPUNIT_ASSERT ( p.connections ().count () == 0 );
        CPPUNIT_ASSERT ( p.views ().count () == 0 );

        // test view handling
        CPPUNIT_ASSERT ( p.addView ( "v1", v1 ) == true );
        CPPUNIT_ASSERT ( p.addView ( "v2", v2 ) == true );
        CPPUNIT_ASSERT ( p.addView ( "v3", v3 ) == true );
        CPPUNIT_ASSERT ( p.addView ( "v3", v2 ) == false );

        CPPUNIT_ASSERT ( p.views ().count () == 3 );
        CPPUNIT_ASSERT ( p.views ().contains ( "v1" ) );
        CPPUNIT_ASSERT ( p.views ().contains ( "v2" ) );
        CPPUNIT_ASSERT ( p.views ().contains ( "v3" ) );

        CPPUNIT_ASSERT ( p.removeView ( "v1" ) == true );
        CPPUNIT_ASSERT ( p.removeView ( "v2" ) == true );
        CPPUNIT_ASSERT ( p.removeView ( "v3" ) == true );
        CPPUNIT_ASSERT ( p.removeView ( "v3" ) == false );

        CPPUNIT_ASSERT ( p.views ().count () == 0 );

        // test connection handling
        CPPUNIT_ASSERT ( p.addConnection ( "c1", c1 ) == true );
        CPPUNIT_ASSERT ( p.addConnection ( "c2", c2 ) == true );
        CPPUNIT_ASSERT ( p.addConnection ( "c3", c3 ) == true );
        CPPUNIT_ASSERT ( p.addConnection ( "c3", c2 ) == false );

        CPPUNIT_ASSERT ( p.connections ().count () == 3 );
        CPPUNIT_ASSERT ( p.connections ().contains( "c1" ) );
        CPPUNIT_ASSERT ( p.connections ().contains( "c2" ) );
        CPPUNIT_ASSERT ( p.connections ().contains( "c3" ) );

        CPPUNIT_ASSERT ( p.removeConnection ( "c1" ) == true );
        CPPUNIT_ASSERT ( p.removeConnection ( "c2" ) == true );
        CPPUNIT_ASSERT ( p.removeConnection ( "c3" ) == true );
        CPPUNIT_ASSERT ( p.removeConnection ( "c3" ) == false );

        CPPUNIT_ASSERT ( p.connections ().count () == 0 );

        // test logging handling
        // Logging* l1 = new Logging("TYPE1", PropertyDict());
        // Logging* l2 = new Logging("TYPE2", PropertyDict());
        // Logging* l3 = new Logging("TYPE3", PropertyDict());
        // CPPUNIT_ASSERT(l1);
        // CPPUNIT_ASSERT(l2);
        // CPPUNIT_ASSERT(l3);

        // CPPUNIT_ASSERT ( p.addLogging ( "l1", l1 ) == true );
        // CPPUNIT_ASSERT ( p.addLogging ( "l2", l2 ) == true );
        // CPPUNIT_ASSERT ( p.addLogging ( "l3", l3 ) == true );
        // CPPUNIT_ASSERT ( p.addLogging ( "l3", l2 ) == false );

        // CPPUNIT_ASSERT ( p.loggings ().count () == 3 );
        // CPPUNIT_ASSERT ( p.loggings ().contains( "l1" ) );
        // CPPUNIT_ASSERT ( p.loggings ().contains( "l2" ) );
        // CPPUNIT_ASSERT ( p.loggings ().contains( "l3" ) );

        // CPPUNIT_ASSERT ( p.removeLogging ( "l1" ) == true );
        // CPPUNIT_ASSERT ( p.removeLogging ( "l2" ) == true );
        // CPPUNIT_ASSERT ( p.removeLogging ( "l3" ) == true );
        // CPPUNIT_ASSERT ( p.removeLogging ( "l3" ) == false );

        // CPPUNIT_ASSERT ( p.loggings ().count () == 0 );
    }

    void testProjectBuilder () {
        ProjectBuilder pb(logger, "en_US");
        Project p = pb.buildRuntimeProject(
                QDir(srcdir + "/tests/data").absPath() +
                "/testproject.zip");
        CPPUNIT_ASSERT(p.isValid());

        CPPUNIT_ASSERT(p.info().shortDescription() == "Bacon");

        CPPUNIT_ASSERT(p.views().count() == 1);
        CPPUNIT_ASSERT(p.views().contains("chimney"));
        CPPUNIT_ASSERT(p.views()["chimney"]
                .panels()["temperatureMonitor"]
                .templates()["overHeat"]
                ->properties()["fill_off"]->encodeValue() ==
                "#FF0000;SOLIDPATTERN");

        CPPUNIT_ASSERT(p.connections().count() == 2);
        CPPUNIT_ASSERT(p.connections().contains("ukrainian"));
        CPPUNIT_ASSERT(p.connections()["ukrainian"]
                ->variables()["readiness"]
                ->properties()["phrase"]->asString() == "gotovo");

        CPPUNIT_ASSERT(p.localizator()->localizedMessage("full_only",
                    "lt")
                == "I AM FULL MESSAGE ONLY");

        CPPUNIT_ASSERT(p.views()["chimney"]
                .panels()["temperatureMonitor"]
                .templates().contains("Probe1"));

        CPPUNIT_ASSERT(p.views()["chimney"]
                .panels()["temperatureMonitor"]
                .templates()["Probe1"]->properties().contains("sensor"));

        p = pb.buildRuntimeProject(
                QDir(srcdir + "/tests/data").absPath() +
                "/empty.zip");
        CPPUNIT_ASSERT(p.isValid());

        p = pb.buildRuntimeProject(
                QDir(srcdir + "/tests/data").absPath() +
                "/empty2.zip");
        CPPUNIT_ASSERT(p.isValid());

        p = pb.buildRuntimeProject(
                QDir(srcdir + "/tests/data").absPath() +
                "/garbage.zip");
        CPPUNIT_ASSERT(!p.isValid());

        //test the embedded template library

        //duplicate the zip file for the update

        //get the full name of the library that will be inserted
        //into the test project file
#if defined(Q_OS_UNIX)

        QString libName = "/usr/local/lib/lintouch/template-libraries"
            "/libmytemplatelibrary.so";
        if (!QDir().exists(libName))
            libName = "/usr/lib/lintouch/template-libraries"
                "/libmytemplatelibrary.so";

#elif defined(Q_OS_WIN32)

        QString libName = "mytemplatelibrary";

        HMODULE handle = LoadLibraryA(libName);
        if (handle == NULL)
            qWarning("Cannot find the library " + libName);

        char filename[MAX_PATH];
        if (GetModuleFileNameA(handle, filename, MAX_PATH) == 0)
            qWarning("Cannot get the full name of the library");
        else
            libName = filename;

        if (handle != NULL)
            FreeModule(handle);

#endif

        QString origName = QDir(srcdir + "/tests/data").absPath() +
            "/selfcontained.zip";
        QString updatedName = QDir("build").absPath() +
            "/selfupdated.zip";
        QFile orig(origName);
        QFile updated(updatedName);
        if (orig.open(IO_ReadOnly) && updated.open(IO_WriteOnly))
        {
            updated.writeBlock(orig.readAll());
            orig.close();
            updated.close();
        }
        else
            qWarning("Cannot create an updated project file");

        //TESTME: here we face problems due to platform specific name handling

        //copy the current template library into the zip
        //and build the project
        QFile lib(libName);
        if (lib.open(IO_ReadOnly))
        {
            VFSPtr updatedVFS(new VFSZIP(updatedName, false, logger));
            if (updatedVFS->isValid())
            {
                QIODevice *dev = updatedVFS->openFile(
                        QLibrary("template-libraries/test").library(),
                        IO_WriteOnly);
                if (dev->isOpen())
                    dev->writeBlock(lib.readAll());
                else
                    qWarning("Unable to update the project with the "
                            "stock template library. Unable to open the"
                            " new project file.");
                delete dev;
                p = pb.buildRuntimeProject(updatedName);
            }
            else
            {
                qWarning("Unable to update the project with the "
                        "stock template library.");
                p = pb.buildRuntimeProject(origName);
            }
            lib.close();
        }
        else
        {
            qWarning("Cannot open the stock template library."
                    " Using the default (may not work)");
            p = pb.buildRuntimeProject(origName);
        }

        CPPUNIT_ASSERT(p.isValid());

        QDir().remove(updatedName);
    }

    void testEProject()
    {
        LoggerPtr logger = LoggerPtr(new ConsoleLogger);

        TemplateManager manager = TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();
        CPPUNIT_ASSERT(manager.templateLibraries().contains(
                    "mytemplatelibrary"));

        const TemplateLibrary *library = manager.templateLibraries(
                )["mytemplatelibrary"];
        CPPUNIT_ASSERT(library->isValid());
        CPPUNIT_ASSERT(library->filename().contains(
                    "mytemplatelibrary"));

        CPPUNIT_ASSERT(manager.templateLibraries().count());

        // substitute tests for the above
        CPPUNIT_ASSERT(manager.templateLibraries().current());

        TemplateLibraryDict libraries = manager.templateLibraries();
        // TemplateLibraryDictIterator it(libraries);
        CPPUNIT_ASSERT(libraries.current());
        CPPUNIT_ASSERT(libraries.current() != NULL);
    }

    private slots:

        void finished(int status)
        {
            QString msg;
            switch (status)
            {
                case ProjectAsyncBuilder::Error:
                    msg = "Finished: error";
                    break;
                case ProjectAsyncBuilder::Finished:
                    msg = "Finished: OK";
                    break;
                case ProjectAsyncBuilder::Cancelled:
                    msg = "Finished: cancelled";
                    break;
            }
            qDebug(msg);
            m_status = status;
            m_finished = true;
        }

    void downloadProgress(int downloaded, int total)
    {
        qDebug(QString("Got %1 of %2").arg(downloaded)
                .arg(total == -1 ? (QString) "unknown" :
                    QString::number(total)));
        m_progress = true;
    }

    void buildProgress(int panel, int panelsTotal)
    {
        qDebug(QString("Building panel %1 of %2").arg(panel)
                .arg(panelsTotal));
        m_progress = true;
        if (panel >= panelsTotal/2)
        {
            qDebug("Cancelling here...");
            m_pab->cancel();
        }
    }

    void downloaded()
    {
        qDebug("Project downloaded (signal).");
        m_downloaded = true;
    }


    private:


    void waitAndProcessEvents(int msec) {
        QTime t;
        t.start();
        for (;(t.elapsed() < msec) && ! m_finished;) {
            g_app->processEvents(100);
        }
    }

    void subtestProjectAsyncBuilder(const QString &location) {
        //WARNING: this is not the proper way to use ProjectAsyncBuilder
        //it is merely a hack to make tests clearer

        //DO NOT TREAT THE FOLLOWING AS AN EXAMPLE OF USING
        // ProjectAsyncBuilder!!!!!!!!!!!!!!!!!1
        ProjectAsyncBuilder *pab = new ProjectAsyncBuilder(logger);
        connect(pab, SIGNAL(finished(int)),
                this, SLOT(finished(int)));
        m_finished = false;

        pab->buildFromUrl(location);

        waitAndProcessEvents(30*1000);

        CPPUNIT_ASSERT(m_status == ProjectAsyncBuilder::Finished);

        Project p = pab->getProject();
        CPPUNIT_ASSERT(p.isValid());

        CPPUNIT_ASSERT(p.info().shortDescription() == "Bacon");

        CPPUNIT_ASSERT(p.views().count() == 1);
        CPPUNIT_ASSERT(p.views().contains("chimney"));
        CPPUNIT_ASSERT(p.views()["chimney"]
                .panels()["temperatureMonitor"]
                .templates()["overHeat"]
                ->properties()["fill_off"]->encodeValue() ==
                "#FF0000;SOLIDPATTERN");
        delete pab;
        m_status = -1;
    }

    void failtestProjectAsyncBuilder(const QString &location) {
        ProjectAsyncBuilder *pab = new ProjectAsyncBuilder(logger);
        connect(pab, SIGNAL(finished(int)),
                this, SLOT(finished(int)));
        m_finished = false;

        pab->buildFromUrl(location);

        waitAndProcessEvents(30*1000);

        Project p = pab->getProject();

        if ( m_status != ProjectAsyncBuilder::Error ) {
            CPPUNIT_ASSERT( m_status == ProjectAsyncBuilder::Finished );
            CPPUNIT_ASSERT( !p.isValid() );
        }

        CPPUNIT_ASSERT(!p.isValid());
        m_status = -1;

        //do the same thing again
        m_finished = false;

        pab->buildFromUrl(location);

        waitAndProcessEvents(30*1000);

        p = pab->getProject();

        if ( m_status != ProjectAsyncBuilder::Error ) {
            CPPUNIT_ASSERT( m_status == ProjectAsyncBuilder::Finished );
            CPPUNIT_ASSERT( !p.isValid() );
        }

        CPPUNIT_ASSERT(!p.isValid());
        delete pab;
        m_status = -1;
    }

    void startHTTPServer(const QString &filename, unsigned * port )
    {
        QCString fn = filename.utf8();
        CPPUNIT_ASSERT( lt_http_server_create( &http, fn, 0, NULL )
                == LT_HTTP_SUCCESS );
        CPPUNIT_ASSERT( lt_http_server_start( http )
                == LT_HTTP_SUCCESS );
        if( port != NULL ) {
            *port = lt_http_server_port_get( http );
        }
        m_progress = false;
    }

    void shutdownHTTPServer()
    {
        CPPUNIT_ASSERT( lt_http_server_stop( http )
                == LT_HTTP_SUCCESS );
        CPPUNIT_ASSERT( lt_http_server_destroy( http )
                == LT_HTTP_SUCCESS );
        apr_pool_destroy( lt_http_server_pool_get( http ) );
        http = NULL;
    }

    public:

    void testProjectAsyncBuilder () {
        unsigned port;
        //positive answers
        qDebug("testing local ZIP async...");
        subtestProjectAsyncBuilder(srcdir + "/tests/data/testproject.zip");

        qDebug("testing an invalid local ZIP async...");
        QString zipname = "build/nonexistent.zip";
        QDir().remove(zipname);
        VFSPtr vzip(new VFSZIP(zipname, false, logger));
        CPPUNIT_ASSERT(vzip->isValid());
        CPPUNIT_ASSERT(vzip->mkdir("anything"));
        failtestProjectAsyncBuilder(zipname);
        QDir().remove(zipname);

        qDebug("testing an inexistent local dir async...");
        failtestProjectAsyncBuilder("nonexistent");

        qDebug("testing HTTP async...");
        startHTTPServer(srcdir + "/tests/data/testproject.zip", &port);
        subtestProjectAsyncBuilder(
                QString("http://127.0.0.1:%1/testproject.zip")
                .arg( port ) );
        shutdownHTTPServer();

        //negative answers
        qDebug("testing the down-server HTTP async...");
        failtestProjectAsyncBuilder(
                "http://127.0.0.1:1122/testproject.zip");

        //FIXME: the following test is disabled because our C based HTTP
        //server does not support 404
#if 0
        qDebug("testing the file-not-found HTTP async...");
        startHTTPServer(srcdir + "/tests/data/testproject.zip", &port);
        failtestProjectAsyncBuilder(
                QString("http://127.0.0.1:%1/zip.zap").arg( port ));
        shutdownHTTPServer();
#endif

        //answers related to the progress
        Project p;
        ProjectAsyncBuilder *pab = new ProjectAsyncBuilder(logger);
        connect(pab, SIGNAL(finished(int)),
                this, SLOT(finished(int)));
        m_finished = false;

        connect(pab, SIGNAL(downloadProgress(int, int)),
                this, SLOT(downloadProgress(int, int)));

        //FIXME: the following test is disabled because it's very
        //unstable and it's very difficult to cancel the build in single
        //thread qt environment where we really don't know which code is
        //executing at what time.
#if 0
        qDebug("testing cancelling in the progress...");
        startHTTPServer(srcdir + "/tests/data/testproject.zip", &port);

        pab->buildFromUrl(
                QString("http://127.0.0.1:%1/testproject.zip").arg( port ));

        qWarning( "waiting before canceling the build..." );
        waitAndProcessEvents(100);

        //timeout elapsed, now cancel the download
        qWarning( "now canceling the build..." );
        pab->cancel();
        CPPUNIT_ASSERT(m_status == ProjectAsyncBuilder::Cancelled);

        //wait for the script to terminate
        shutdownHTTPServer();

        CPPUNIT_ASSERT(m_progress);

        p = pab->getProject();
        CPPUNIT_ASSERT(!p.isValid());
        delete pab;
#endif

        m_status = -1;
        m_downloaded = false;

        //build progress
        qDebug("testing the build progress...");
        startHTTPServer(srcdir + "/tests/data/long.zip", &port);

        m_pab = pab = new ProjectAsyncBuilder(logger);
        connect(pab, SIGNAL(finished(int)),
                this, SLOT(finished(int)));
        connect(pab, SIGNAL(buildProgress(int, int)),
                this, SLOT(buildProgress(int, int)));
        connect(pab, SIGNAL(downloaded()),
                this, SLOT(downloaded()));

        //build the project
        pab->buildFromUrl(
                QString("http://127.0.0.1:%1/long.zip").arg( port ));
        while (m_status == -1)
            g_app->processEvents();

        CPPUNIT_ASSERT(m_status == ProjectAsyncBuilder::Cancelled);

        //wait for the script to terminate
        shutdownHTTPServer();

        CPPUNIT_ASSERT(m_progress);
        CPPUNIT_ASSERT(m_downloaded);

        m_status = -1;

        p = pab->getProject();
        CPPUNIT_ASSERT(!p.isValid());

        delete pab;
    }

    void testInfoBuilder() {
        LoggerPtr logger = LoggerPtr(new ConsoleLogger);

        //build a configuration from the test file
        QString err;
        int line, col;
        ConfigurationPtr conf =
            DefaultConfigurationBuilder().buildFromFile(
                    srcdir + "/tests/data/Info.xml" , err, line, col);
        if(conf.isNull())
            logger->error(QString("Parsing error: %1:%2: %3")
                    .arg(line).arg(col).arg(err));
        Info info = InfoBuilder::buildFromConfiguration(conf, logger);

        CPPUNIT_ASSERT(info.author() == "Caius Cosades");
        CPPUNIT_ASSERT(info.version() == "0.7");
        CPPUNIT_ASSERT(info.date() == QDate(2003, 12, 11));
        CPPUNIT_ASSERT(info.shortDescription() == "Artillery");
        CPPUNIT_ASSERT(info.longDescription() ==
                "Monitoring the number of soldiers.");
    }

    void testTemplateBuilder() {

        Template * lamp = templateManager.instantiate( "Lamp", logger,
                "mytemplatelibrary" );
        CPPUNIT_ASSERT( lamp != NULL );
        lamp->compose( templateManager );

        lamp->properties()["border_off"]->decodeValue(
                "#00FF00;3;DASHDOTLINE");
        CPPUNIT_ASSERT(lamp->properties()["border_off"]->encodeValue()
                == "#00FF00;3;DASHDOTLINE");

        QString err;
        int line, col;
        ConfigurationPtr c =
            DefaultConfigurationBuilder().buildFromFile(
                    srcdir + "/tests/data/Template.xml", err, line, col);
        CPPUNIT_ASSERT(!c.isNull());

        Template * lampBuilt =
            TemplateBuilder::buildFromConfiguration(c,
                    LocalizatorPtr(),
                    templateManager, logger);
        CPPUNIT_ASSERT(lampBuilt != NULL);

        CPPUNIT_ASSERT(lampBuilt->properties()["fill_off"]
                ->encodeValue() == "#FF0000;DENSE1PATTERN");
        CPPUNIT_ASSERT(lampBuilt->properties()["shape"]->encodeValue() ==
                "triangle");
    }
};

#endif /* _CHECK_TESTPROJECT_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_testproject.hh,v 1.48 2004/12/16 12:33:22 modesto Exp $
