// $Id: check_editor.cpp 1572 2006-04-13 12:04:52Z modesto $
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

#include <qapplication.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qdatetime.h>

#include <lt/logger/ConsoleLogger.hh>
#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationBuilder.hh>
#include <lt/configuration/DefaultConfigurationSerializer.hh>

#include <lt/ui/ui.h>
#include <lt/project/TemplateManagerBuilder.hh>
#include <lt/project/InfoBuilder.hh>
#include <lt/project/TemplateBuilder.hh>
#include <lt/project/PanelBuilder.hh>
#include <lt/project/ViewBuilder.hh>
#include <lt/project/ConnectionBuilder.hh>
#include <lt/project/VariableBuilder.hh>
#include <lt/templates/TemplateManager.hh>
#include <lt/vfs/TempFiles.hh>
using namespace lt;

#include "EditablePanelViewContainer.hh"
#include "EditablePanelView.hh"
#include "EditorProjectBuilder.hh"
#include "EditorProject.hh"
#include "MRU.hh"
#include "InfoSerializer.hh"
#include "TemplateSerializer.hh"
#include "PanelSerializer.hh"
#include "ViewSerializer.hh"
#include "VariableSerializer.hh"
#include "ConnectionSerializer.hh"
#include "EditorProjectSerializer.hh"
#include "CSV.hh"
#include "PluginManager.hh"
#include "ServerLogging.hh"
#include "ServerLoggingBuilder.hh"
#include "ServerLoggingSerializer.hh"

extern QString srcdir;
extern QString builddir;

class TestEditor : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestEditor );

    CPPUNIT_TEST( testMRU );
    CPPUNIT_TEST( testProject );
    CPPUNIT_TEST( testEditorProjectBuilder );

    CPPUNIT_TEST( testVariableSerializer );
    CPPUNIT_TEST( testConnectionSerializer );
    CPPUNIT_TEST( testInfoSerializer );
    CPPUNIT_TEST( testTemplateSerializer );
    CPPUNIT_TEST( testPanelSerializer );
    CPPUNIT_TEST( testViewSerializer );
    CPPUNIT_TEST( testEditorProjectSerializer );

    CPPUNIT_TEST( testEditorProjectLoadSave );

    //FIXME: I had to disable this test, because it requires X server
    //connection
    //CPPUNIT_TEST( testEPVC );

    CPPUNIT_TEST( testCSV );
    CPPUNIT_TEST( testConn2CSV );
    CPPUNIT_TEST( testCSV2Conn );
    CPPUNIT_TEST( testPluginManager );

    CPPUNIT_TEST ( testServerLogging );
    CPPUNIT_TEST ( testServerLoggingBuilder );
    CPPUNIT_TEST ( testServerLoggingManager );
    CPPUNIT_TEST ( testServerLoggingSerializer );

    CPPUNIT_TEST_SUITE_END();

    // editor settings
    ConfigurationPtr es;

    public:

    void setUp()
    {
        // fill in
        logger = LoggerPtr(new ConsoleLogger);
        // logger = LoggerPtr(new Logger);
        pLTEditorMRU = new MRU;
    }

    void tearDown()
    {
        // fill in
        delete pLTEditorMRU;
    }

    void testXXX()
    {
    }

    void testMRU()
    {
        CPPUNIT_ASSERT ( pLTEditorMRU );

        QString dummyProjectPath = "/home/lintouch/project";
        unsigned int iRecentFileToShow  =  56;
        pLTEditorMRU->setRecentFiles(iRecentFileToShow );

        int iProjectToOpenForTest = iRecentFileToShow + 3 ;
        QStringList openedTestProjectList;

        for( int i = 1; i <= iProjectToOpenForTest ; i++ ) {
            QString project = QString( "%1/Pro_%2.ltp" )
                .arg( dummyProjectPath ).arg( i );
            pLTEditorMRU->addRecentProject( project );
            openedTestProjectList.push_front(project);
        }

        QStringList lstRecentFiles = pLTEditorMRU->recentProjects();
        unsigned int iNumberOfRecentFiles = lstRecentFiles.size();

        CPPUNIT_ASSERT ( iNumberOfRecentFiles <= iRecentFileToShow );

        for ( int iRec = iNumberOfRecentFiles - 1  ; iRec >= 0 ; --iRec ){

            CPPUNIT_ASSERT (
                    lstRecentFiles[ iRec ] == openedTestProjectList[ iRec ] );
        }

        QString earliestOpenedFile = lstRecentFiles.last();
        pLTEditorMRU->addRecentProject( earliestOpenedFile );
        lstRecentFiles = pLTEditorMRU->recentProjects();
        QString lastOpenedFile = lstRecentFiles.first();

        CPPUNIT_ASSERT ( earliestOpenedFile == lastOpenedFile );
        /**
          QStringList::iterator it =
          lstRecentFiles.find( earliestOpenedFile );
          QStringList::iterator it2 =
          lstRecentFiles.find(it,earliestOpenedFile);
          CPPUNIT_ASSERT ( it2 == lstRecentFiles.end() ); **/
    }

    void testProject () {

        EditorProject p;

        // test logging handling
        ServerLogging* l1 = new ServerLogging("TYPE1", PropertyDict());
        ServerLogging* l2 = new ServerLogging("TYPE2", PropertyDict());
        ServerLogging* l3 = new ServerLogging("TYPE3", PropertyDict());
        CPPUNIT_ASSERT(l1);
        CPPUNIT_ASSERT(l2);
        CPPUNIT_ASSERT(l3);

        CPPUNIT_ASSERT ( p.addServerLogging ( "l1", l1 ) == true );
        CPPUNIT_ASSERT ( p.addServerLogging ( "l2", l2 ) == true );
        CPPUNIT_ASSERT ( p.addServerLogging ( "l3", l3 ) == true );
        CPPUNIT_ASSERT ( p.addServerLogging ( "l3", l2 ) == false );

        CPPUNIT_ASSERT ( p.serverLoggings ().count () == 3 );
        CPPUNIT_ASSERT ( p.serverLoggings ().contains( "l1" ) );
        CPPUNIT_ASSERT ( p.serverLoggings ().contains( "l2" ) );
        CPPUNIT_ASSERT ( p.serverLoggings ().contains( "l3" ) );

        CPPUNIT_ASSERT ( p.removeServerLogging ( "l1" ) == true );
        CPPUNIT_ASSERT ( p.removeServerLogging ( "l2" ) == true );
        CPPUNIT_ASSERT ( p.removeServerLogging ( "l3" ) == true );
        CPPUNIT_ASSERT ( p.removeServerLogging ( "l3" ) == false );

        CPPUNIT_ASSERT ( p.serverLoggings ().count () == 0 );
    }

    void testEditorProjectBuilder()
    {
        TemplateManager manager =  TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();

        PluginManager pluginManager(logger);

        ServerLoggingManager slManager(logger);
        CPPUNIT_ASSERT(pluginManager.plugins().count() > 1);

        EditorProjectBuilder pbe(logger, "en_US");
        EditorProject p = pbe.buildEditorProject(
                QDir(srcdir + "/tests").absPath() + "/testproject.zip",
                manager, pluginManager, slManager, es);
        CPPUNIT_ASSERT(p.isValid());

        CPPUNIT_ASSERT(p.info().shortDescription() == "Bacon");

        CPPUNIT_ASSERT(p.bindings().bindingsByTemplate("chimney",
            "temperatureMonitor", "underHeat").count());

        // test presence of server logging section
        const ServerLoggingDict& sl = p.serverLoggings();
        CPPUNIT_ASSERT(sl.count() == 3);
        CPPUNIT_ASSERT(sl.contains("Console"));
        CPPUNIT_ASSERT(sl.contains("File"));
        CPPUNIT_ASSERT(sl.contains("Missing"));

        //This portion tests the removal of resources.
        //The success/failure can be found out from the logger
        //generated info

        p.addResource( QDir(srcdir + "/tests").absPath() + "/testproject.zip" );

        VFSPtr unPackedResc = p.unpackedResources();
        VFSPtr deletedResc = p.deletedResources();

        unPackedResc->mkdir("testDir1");
        unPackedResc->mkdir("testDir2");
        QStringList lstContents = unPackedResc->ls();
        QStringList::iterator i = lstContents.begin();
        while( i != lstContents.end() ) {
            QString absFilePath =
                unPackedResc->rootLocation().path() + *i;
            if( QFileInfo( absFilePath ).isDir() ) {
                VFSPtr tempParent( unPackedResc->subVFS(*i) );
                tempParent->openFile("testFile",IO_WriteOnly);
            }
            ++i;
        }

        deletedResc->mkdir("testDir1");
        deletedResc->mkdir("testDir2");
        lstContents = deletedResc->ls();
        i = lstContents.begin();
        while( i != lstContents.end() ) {
            QString absFilePath =
                deletedResc->rootLocation().path() + *i;
            if( QFileInfo( absFilePath ).isDir() ) {
                VFSPtr tempParent( deletedResc->subVFS(*i) );
                tempParent->openFile("testFile",IO_WriteOnly);
            }
            ++i;
        }
    }

    void testInfoSerializer() {
        Info pi;
        pi.setAuthor("Ashlander");
        pi.setVersion( "1.2" );
        pi.setDate(QDate(2004, 9, 8));
        pi.setShortDescription("Morrowind");
        pi.setLongDescription("A sequel in the Elder Scrolls series");

        ConfigurationPtr conf(new DefaultConfiguration("project-info"));
        CPPUNIT_ASSERT(InfoSerializer::saveToConfiguration(
                    conf, pi, logger));

        Info info = InfoBuilder::buildFromConfiguration(
                conf, logger);

        CPPUNIT_ASSERT(info.author() == "Ashlander");
        CPPUNIT_ASSERT(info.version() == "1.2");
        CPPUNIT_ASSERT(info.date() == QDate(2004, 9, 8));
        CPPUNIT_ASSERT(info.shortDescription() == "Morrowind");
        CPPUNIT_ASSERT(info.longDescription() ==
                "A sequel in the Elder Scrolls series");
    }

    void testVariableSerializer() {
        PropertyDict props;
        props.insert("barcode", new Property("barcode", "string",
            "200410181944"));
        Variable v("number", props);

        ConfigurationPtr conf(new DefaultConfiguration("variable"));
        CPPUNIT_ASSERT(VariableSerializer::saveToConfiguration(
                    conf, v, logger));

        Variable *vv = VariableBuilder::buildFromConfiguration(
                conf, logger);

        CPPUNIT_ASSERT(vv->type() == IOPin::decodeType("number"));
        CPPUNIT_ASSERT(vv->properties().contains("barcode"));
        CPPUNIT_ASSERT(vv->properties()["barcode"]->encodeValue() ==
            "200410181944");
        delete vv;
    }

    void testConnectionSerializer() {
        //create a variable
        PropertyDict props;
        props.insert("barcode", new Property("barcode", "string",
            "200410181944"));
        Variable *v = new Variable("number", props);

        //create the connection
        PropertyDict pc;
        pc.insert("voltage", new Property("voltage", "number", "220"));
        Connection conn("Generator", pc);

        conn.addVariable("coils", v);

        //serialize the connection
        QString tempName = getTempFileName();
        QFile f(tempName);
        f.open(IO_WriteOnly);
        CPPUNIT_ASSERT(ConnectionSerializer::saveToFile(
                    &f, conn, logger));
        f.close();

        f.open(IO_ReadOnly);
        Connection *cc = ConnectionBuilder::buildFromFile(
                &f, logger);

        CPPUNIT_ASSERT(cc->variables().contains("coils"));
        CPPUNIT_ASSERT(cc->variables()["coils"]->type() ==
            IOPin::decodeType("number"));
        f.close();
        QDir().remove(tempName);
        delete cc;
    }

    void testTemplateSerializer() {
        TemplateManager manager = TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();
        Template *t = manager.instantiate("Lamp", logger,
                "mytemplatelibrary");
        CPPUNIT_ASSERT(t);
        CPPUNIT_ASSERT(t->properties().contains("border_off"));

        t->properties()["border_off"]->decodeValue(
                "#00FF00;3;DASHDOTLINE");
        t->setVirtualRect(QRect(78, 97, 100, 100));
        t->setZ(7);
        t->setShortcut(QKeySequence("Alt+K"));

        ConfigurationPtr conf(new DefaultConfiguration("template"));
        CPPUNIT_ASSERT(TemplateSerializer::saveToConfiguration(
                    conf, t, logger));

        Template *tt = TemplateBuilder::buildFromConfiguration(conf,
                LocalizatorPtr(new Localizator), manager,
                logger);

        CPPUNIT_ASSERT(tt);

        CPPUNIT_ASSERT(tt->properties()["border_off"]->encodeValue()
                == "#00FF00;3;DASHDOTLINE");
        CPPUNIT_ASSERT(tt->rect().top() == t->rect().top());
        CPPUNIT_ASSERT(tt->z() == 7);
        CPPUNIT_ASSERT((QString) tt->shortcut() == "Alt+K");

        delete tt;
        delete t;
    }

    void testPanelSerializer() {
        TemplateManager manager = TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();
        Template *t1 = manager.instantiate("Lamp", logger,
                "mytemplatelibrary");
        Template *t2 = manager.instantiate("Lamp", logger,
                "mytemplatelibrary");
        CPPUNIT_ASSERT(t1);
        CPPUNIT_ASSERT(t2);
        CPPUNIT_ASSERT(t1->properties().contains("border_off"));

        t1->properties()["border_off"]->decodeValue(
                "#00FF00;3;DASHDOTLINE");

        Panel p1;
        p1.addTemplate("t1", t1);
        p1.addTemplate("t2", t2);

        ConfigurationPtr conf(new DefaultConfiguration("panel"));
        CPPUNIT_ASSERT(PanelSerializer::saveToConfiguration(
                    conf, p1, logger));

        Panel pp1 = PanelBuilder::buildFromConfiguration(conf,
                LocalizatorPtr(new Localizator), manager,
                logger);

        CPPUNIT_ASSERT(pp1.templates().count() == 2);

        Template *tt1 = pp1.templates()["t1"];
        CPPUNIT_ASSERT(tt1);
        CPPUNIT_ASSERT(tt1->properties().contains("border_off"));
        CPPUNIT_ASSERT(tt1->properties()["border_off"]->encodeValue()
                == "#00FF00;3;DASHDOTLINE");

        Panel p2;

        conf = ConfigurationPtr(new DefaultConfiguration("templates"));
        CPPUNIT_ASSERT(PanelSerializer::saveToConfiguration(
                    conf, p2, logger));
        Panel pp2 = PanelBuilder::buildFromConfiguration(conf,
                LocalizatorPtr(new Localizator), manager,
                logger);
        CPPUNIT_ASSERT(pp2.templates().count() == 0);
    }

    void testViewSerializer() {
        TemplateManager manager = TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();
        View v;
        v.setVirtualWidth(800);
        v.setVirtualHeight(600);
        Panel p;
        v.addPanel("Maar Gnok", p);

        ConfigurationPtr conf(new DefaultConfiguration("project-info"));
        CPPUNIT_ASSERT(ViewSerializer::saveToConfiguration(
                    conf, v, logger));

        View vv = ViewBuilder(1, manager,
                LocalizatorPtr(new Localizator)).buildFromConfiguration(
                conf, logger);

        CPPUNIT_ASSERT(vv.panels().contains("Maar Gnok"));
        CPPUNIT_ASSERT(vv.panels().count() == 1);

        QString tempName = getTempFileName();
        QFile f(tempName);
        f.open(IO_WriteOnly);
        CPPUNIT_ASSERT(ViewSerializer::saveToFile(
                    &f, v, logger));
        f.close();

        f.open(IO_ReadOnly);
        vv = ViewBuilder(1, manager,
                LocalizatorPtr(new Localizator)).buildFromFile(
                &f, logger);

        CPPUNIT_ASSERT(vv.panels().contains("Maar Gnok"));
        CPPUNIT_ASSERT(vv.panels().count() == 1);
        f.close();
        QDir().remove(tempName);
    }

    void testEditorProjectSerializer() {
        TemplateManager manager = TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();
        PluginManager pluginManager(logger);

        ServerLoggingManager slManager(logger);
        CPPUNIT_ASSERT(pluginManager.plugins().count() > 1);

        View v;
        v.setVirtualWidth(800);
        v.setVirtualHeight(600);
        Panel p;
        v.addPanel("Maar Gnok", p);

        Template *t = manager.instantiate("Lamp", logger,
                "mytemplatelibrary");
        CPPUNIT_ASSERT(t != NULL );
        CPPUNIT_ASSERT(t);
        CPPUNIT_ASSERT(t->properties().contains("border_off"));
        t->properties()["border_off"]->decodeValue(
                "#00FF00;3;DASHDOTLINE");
        p.addTemplate("FirstSavedTemplate", t);

        //create info
        Info pi;
        pi.setAuthor("Ashlander");
        pi.setVersion( "1.2" );
        pi.setDate(QDate(2004, 9, 8));
        pi.setShortDescription("Morrowind");
        pi.setLongDescription("A sequel in the Elder Scrolls series");

        //create connections
        Connection *conn = new Connection("Generator", PropertyDict());
        conn->addVariable("coils", new Variable("number", PropertyDict()));

        EditorProject* prj = new EditorProject(
                EditorProjectBuilder(logger->getChildLogger("BuilderNew"))
                .buildEditorProject(manager));
        CPPUNIT_ASSERT(prj->isValid());

        prj->addView("Default", v);
        prj->setInfo(pi);
        prj->addConnection("Electra", conn);

        // create server logging
        PropertyDict slpd;
        slpd.insert("X", new Property("XX", "string", "Y"));
        ServerLogging* sl1 = new ServerLogging("Missing", slpd);
        prj->addServerLogging("Missing Server Logging", sl1);
        CPPUNIT_ASSERT(
                prj->serverLoggings().contains("Missing Server Logging"));

        QString resourcename = srcdir + "/tests/check_editor.cpp";
        CPPUNIT_ASSERT(!prj->addResource(resourcename).isEmpty());

        QString filename = builddir + "/quest.ltp";
        QFile(filename).remove();

        CPPUNIT_ASSERT(EditorProjectSerializer(logger->getChildLogger(
                        "ProjectSerializer")).saveEditorProject(*prj,
                        filename, ConfigurationPtr(
                            new DefaultConfiguration("editor-settings"))));

        delete prj;

        prj = new EditorProject(
                EditorProjectBuilder(logger->getChildLogger("BuilderFile"))
                .buildEditorProject(filename, manager,
                    pluginManager, slManager, es));
        CPPUNIT_ASSERT(prj->isValid());
        CPPUNIT_ASSERT(prj->info().shortDescription() == "Morrowind");
        CPPUNIT_ASSERT(prj->views().contains("Default"));
        CPPUNIT_ASSERT(prj->views()["Default"].panels()
                .contains("Maar Gnok"));
        CPPUNIT_ASSERT(prj->views()["Default"].panels()["Maar Gnok"].
                templates().contains("FirstSavedTemplate"));
        CPPUNIT_ASSERT(prj->connections().contains("Electra"));
        CPPUNIT_ASSERT(prj->connections()["Electra"]->variables()
                .contains("coils"));

        Template *tt1 = prj->views()["Default"].panels()["Maar Gnok"].
            templates()["FirstSavedTemplate"];
        CPPUNIT_ASSERT(tt1);
        CPPUNIT_ASSERT(tt1->properties().contains("border_off"));
        CPPUNIT_ASSERT(tt1->properties()["border_off"]->encodeValue()
                == "#00FF00;3;DASHDOTLINE");

        // test presence of server logging section
        const ServerLoggingDict& sl2 = prj->serverLoggings();
        CPPUNIT_ASSERT(sl2.contains("Missing Server Logging"));

        CPPUNIT_ASSERT(QFile(filename).exists());
        CPPUNIT_ASSERT(prj->unpackedResources()->exists(
                    QFileInfo(resourcename).fileName()));

        prj->addView("02/?AnotherView/*", v);
        CPPUNIT_ASSERT(EditorProjectSerializer(logger->getChildLogger(
                        "ProjectSerializer")).saveEditorProject(*prj,
                        filename, ConfigurationPtr(
                            new DefaultConfiguration("editor-settings"))));

        QFile(filename).remove();
        delete prj;
    }

    void testEditorProjectLoadSave()
    {
        TemplateManager manager =  TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();
        PluginManager pluginManager(logger);

        ServerLoggingManager slManager(logger);
        CPPUNIT_ASSERT(pluginManager.plugins().count() > 1);

        EditorProjectBuilder pbe(logger, "en_US");
        EditorProject prj = pbe.buildEditorProject(
                QDir(srcdir + "/tests").absPath() + "/testproject.zip",
                manager, pluginManager, slManager, es);
        CPPUNIT_ASSERT(prj.isValid());

        QString filename = builddir + "/testsavedproject.ltp";
        QFile(filename).remove();

        CPPUNIT_ASSERT(EditorProjectSerializer(logger->getChildLogger(
                        "ProjectSerializer")).saveEditorProject(prj,
                        filename, ConfigurationPtr(
            new DefaultConfiguration("editor-settings"))));

        EditorProject pp = pbe.buildEditorProject(filename, manager,
            pluginManager, slManager, es);
        CPPUNIT_ASSERT(pp.bindings().bindingsByTemplate("chimney",
            "temperatureMonitor", "underHeat").count());
        QFile(filename).remove();
    }

    void testEPVC()
    {
        TemplateManager manager =  TemplateManagerBuilder(logger)
            .buildFromDefaultDirs();

        Panel p;
        Panel p_cloned;
        View v;
        EditablePanelViewContainer * epvc = NULL;

        Template * t1 = manager.instantiate( "Lamp",
                LoggerPtr( new ConsoleLogger() ), "mytemplatelibrary" );
        Template * t2 = manager.instantiate( "Lamp",
                LoggerPtr( new ConsoleLogger() ), "mytemplatelibrary" );
        Template * t3 = manager.instantiate( "Lamp",
                LoggerPtr( new ConsoleLogger() ), "mytemplatelibrary" );

        CPPUNIT_ASSERT( t1 != NULL );
        CPPUNIT_ASSERT( t2 != NULL );
        CPPUNIT_ASSERT( t3 != NULL );

        p.addTemplate( "t1", t1 );
        p.addTemplate( "t2", t2 );
        p.addTemplate( "t3", t3 );

        CPPUNIT_ASSERT( p.templates().count() == 3 );

        v.addPanel( "p", p );

        CPPUNIT_ASSERT( v.panels().count() == 1 );

        p_cloned = p.clone( Localizator::nullPtr(), manager,
                LoggerPtr( new ConsoleLogger() ) );

        CPPUNIT_ASSERT( p_cloned.templates().count() == 3 );

        epvc = new EditablePanelViewContainer( NULL, "", 0 );

        CPPUNIT_ASSERT( epvc != NULL );

        epvc->setProjectView( v );

        v.addPanel( "p_cloned", p_cloned );

        epvc->addPanelView( "p_cloned", 1 );

        v.removePanel( "p_cloned" );

        epvc->removePanelView( 1 );
    }

    void testCSV()
    {
        // ENCODING

        // normal string
        CPPUNIT_ASSERT( CSVEncodeField(
                    QString( "12345" ) ) ==
                QString( "12345" ) );

        // string with delimiter
        CPPUNIT_ASSERT( CSVEncodeField(
                    QString( "123, 45, 67" ) ) ==
                QString( "\"123, 45, 67\"" ) );

        // string with delimiter and quotes
        CPPUNIT_ASSERT( CSVEncodeField(
                    QString( "123, \"45\", 67" ) ) ==
                QString( "\"123, \"\"45\"\", 67\"" ) );

        // whole record
        QStringList rec;
        rec.append( "12345" );
        rec.append( "12, 3, 45" );
        rec.append( "12, \"3\", 45" );
        rec.append( "" );
        rec.append( "12,345" );

        CPPUNIT_ASSERT( CSVEncodeRecord(
                    rec ) ==
                QString( "12345,\"12, 3, 45\","
                    "\"12, \"\"3\"\", 45\",,\"12,345\"\r\n" ) );

        // DECODING
        QStringList ret;
        QStringList::ConstIterator it;

        // normal string
        ret = CSVDecodeRecord( "12345,6789,nazdar" );
        CPPUNIT_ASSERT( ret.count() == 3 );
        it = ret.begin();
        CPPUNIT_ASSERT( *it == "12345" );
        ++it;
        CPPUNIT_ASSERT( *it == "6789" );
        ++it;
        CPPUNIT_ASSERT( *it == "nazdar" );

        // comma inside string
        ret = CSVDecodeRecord( "12345,\"67, \"\"89\",\"n,a\"\"\"\"zdar\"" );
        CPPUNIT_ASSERT( ret.count() == 3 );
        it = ret.begin();
        CPPUNIT_ASSERT( *it == "12345" );
        ++it;
        CPPUNIT_ASSERT( *it == "67, \"89" );
        ++it;
        CPPUNIT_ASSERT( *it == "n,a\"\"zdar" );

    }

    void testConn2CSV()
    {
        // build connection from our XML syntax
        QFile fi( srcdir + "/tests/data/conn.xml" );
        fi.open( IO_ReadOnly );
        Connection *c = ConnectionBuilder::buildFromFile( &fi );
        fi.close();


        // encode connection into CSV and destroy it
        QStringList ret = connection2csv( c );
        delete c;

        // write encoded connection to the temporary file
        QFile f( "dummy.csv" );
        f.open( IO_Truncate | IO_WriteOnly );
        QStringList::ConstIterator it = ret.begin();
        for( ; it != ret.end(); ++it ) {
            QCString d = ( *it ).local8Bit();
            f.writeBlock( d, strlen( d ) );
        }
        f.close();
    }

    void testCSV2Conn()
    {
        // build connection from CSV
        QFile fi( "dummy.csv" );
        fi.open( IO_ReadOnly );
        QTextStream stream( &fi );
        QString str;
        QStringList lines;

        // real all lines
        while( ( str = stream.readLine() ) != QString::null ) {
            lines.append( str );
        }

        fi.close();

        // decode them from CSV
        Connection * c = csv2connection( lines );

        CPPUNIT_ASSERT( c != NULL );

        const PropertyDict & pd = c->properties();

        CPPUNIT_ASSERT( c->type() == "MyConn" );
        CPPUNIT_ASSERT( pd.count() == 3 );
        CPPUNIT_ASSERT( pd.contains( "Property1" ) );
        CPPUNIT_ASSERT( pd.contains( "Property2" ) );
        CPPUNIT_ASSERT( pd.contains( "Property3" ) );
        CPPUNIT_ASSERT( pd[ "Property1" ]->asString() ==
                "Value with three ,,, commas" );
        CPPUNIT_ASSERT( pd[ "Property2" ]->asString() ==
                "Value with one , comma and one \" quote" );
        CPPUNIT_ASSERT( pd[ "Property3" ]->asString() ==
                "Value with two \"\" quotes" );

        const VariableDict & vd = c->variables();

        CPPUNIT_ASSERT( vd.count() == 9 );
        CPPUNIT_ASSERT( vd.contains( "vb1" ) );
        CPPUNIT_ASSERT( vd[ "vb1" ]->type() == IOPin::BitType );
        CPPUNIT_ASSERT( vd.contains( "vb2" ) );
        CPPUNIT_ASSERT( vd[ "vb2" ]->type() == IOPin::BitType );
        CPPUNIT_ASSERT( vd.contains( "vb3" ) );
        CPPUNIT_ASSERT( vd[ "vb3" ]->type() == IOPin::BitType );
        CPPUNIT_ASSERT( vd.contains( "vn1" ) );
        CPPUNIT_ASSERT( vd[ "vn1" ]->type() == IOPin::NumberType );
        CPPUNIT_ASSERT( vd.contains( "vn2" ) );
        CPPUNIT_ASSERT( vd[ "vn2" ]->type() == IOPin::NumberType );
        CPPUNIT_ASSERT( vd.contains( "vn3" ) );
        CPPUNIT_ASSERT( vd[ "vn3" ]->type() == IOPin::NumberType );
        CPPUNIT_ASSERT( vd.contains( "vars1" ) );
        CPPUNIT_ASSERT( vd[ "vars1" ]->type() == IOPin::StringType );
        CPPUNIT_ASSERT( vd.contains( "vars2" ) );
        CPPUNIT_ASSERT( vd[ "vars2" ]->type() == IOPin::StringType );
        CPPUNIT_ASSERT( vd.contains( "vars3" ) );
        CPPUNIT_ASSERT( vd[ "vars3" ]->type() == IOPin::StringType );

        const PropertyDict & pd1 = vd[ "vb1" ]->properties();
        CPPUNIT_ASSERT( pd1.count() == 2 );
        CPPUNIT_ASSERT( pd1.contains( "pb1" ) );
        CPPUNIT_ASSERT( pd1.contains( "pb2" ) );
        CPPUNIT_ASSERT( pd1[ "pb1" ]->asString() == "vb1val1" );
        CPPUNIT_ASSERT( pd1[ "pb2" ]->asString() == "vb1val2" );

        const PropertyDict & pd2 = vd[ "vn1" ]->properties();
        CPPUNIT_ASSERT( pd2.count() == 5 );
        CPPUNIT_ASSERT( pd2.contains( "pn1" ) );
        CPPUNIT_ASSERT( pd2.contains( "pn2" ) );
        CPPUNIT_ASSERT( pd2.contains( "pn3" ) );
        CPPUNIT_ASSERT( pd2.contains( "pn4" ) );
        CPPUNIT_ASSERT( pd2.contains( "pn5" ) );
        CPPUNIT_ASSERT( pd2[ "pn1" ]->asString() == "vn1val1" );
        CPPUNIT_ASSERT( pd2[ "pn2" ]->asString() == "vn1val2" );
        CPPUNIT_ASSERT( pd2[ "pn3" ]->asString() == "vn1val3" );
        CPPUNIT_ASSERT( pd2[ "pn4" ]->asString() == "vn1val4" );
        CPPUNIT_ASSERT( pd2[ "pn5" ]->asString() == "vn1val5" );

        const PropertyDict & pd3 = vd[ "vars1" ]->properties();
        CPPUNIT_ASSERT( pd3.count() == 4 );
        CPPUNIT_ASSERT( pd3.contains( "pstr1" ) );
        CPPUNIT_ASSERT( pd3.contains( "pstr2" ) );
        CPPUNIT_ASSERT( pd3.contains( "pstr3" ) );
        CPPUNIT_ASSERT( pd3.contains( "pstr4" ) );
        CPPUNIT_ASSERT( pd3[ "pstr1" ]->asString() == "vars1val1" );
        CPPUNIT_ASSERT( pd3[ "pstr2" ]->asString() == "vars1val2" );
        CPPUNIT_ASSERT( pd3[ "pstr3" ]->asString() == "vars1val3" );
        CPPUNIT_ASSERT( pd3[ "pstr4" ]->asString() == "vars1val4" );

    }

    void testPluginManager()
    {
        PluginManager pm ( LoggerPtr( new ConsoleLogger ) );
        CPPUNIT_ASSERT( pm.plugins().count() );
        QMap<QString,QString> m = pm.connectionProperties("Generator");
        CPPUNIT_ASSERT( !m["refresh"].isNull() );
    }

    void testServerLogging () {

        ServerLogging* l1 = new ServerLogging("TYPE1", PropertyDict());
        CPPUNIT_ASSERT(l1);
        ServerLogging* l2 = new ServerLogging("TYPE2", PropertyDict());
        CPPUNIT_ASSERT(l2);

        CPPUNIT_ASSERT(l1->type() == "TYPE1");
        CPPUNIT_ASSERT(l2->type() == "TYPE2");
    }

    void testServerLoggingBuilder () {
        QFile f(srcdir + "/tests/data/logging-file1.xml");
        f.open(IO_ReadOnly);
        ServerLogging *l = ServerLoggingBuilder::buildFromFile(&f, logger);

        CPPUNIT_ASSERT(l);

        CPPUNIT_ASSERT(l->properties().count() == 1);
        CPPUNIT_ASSERT(l->properties().contains("file-name"));
        CPPUNIT_ASSERT(l->properties()["file-name"]->encodeValue()
                == "/tmp/server.log");
        f.close();
        delete l; l = NULL;

        f.setName(srcdir + "/tests/data/logging-console1.xml");
        f.open(IO_ReadOnly);
        l = ServerLoggingBuilder::buildFromFile(&f, logger);
        CPPUNIT_ASSERT(l);
        CPPUNIT_ASSERT(l->properties().count() == 0);
        f.close();
        delete l;
    }

    void testServerLoggingManager()
    {
        ServerLoggingManager slManager ( LoggerPtr( new ConsoleLogger ) );
        // because we look for plugins in this source tree and PRFIX, there
        // may be more than our two test plugins.
        CPPUNIT_ASSERT( slManager.plugins().count() >= 2 );

        CPPUNIT_ASSERT( slManager.plugins().contains("File") );
        QMap<QString,QString> m =
            slManager.serverLoggingProperties("File");
        CPPUNIT_ASSERT( !m["file-name"].isNull() );
    }

    void testServerLoggingSerializer() {
        //create the connection
        PropertyDict pc;
        pc.insert("file-name", new Property("File name", "string", "/tmp/x"));
        ServerLogging sl("File", pc);

        //serialize the connection
        QString tempName = getTempFileName();
        QFile f(tempName);
        f.open(IO_WriteOnly);
        CPPUNIT_ASSERT(ServerLoggingSerializer::saveToFile(
                    &f, sl, logger));
        f.close();

        f.open(IO_ReadOnly);
        ServerLogging *sl2 = ServerLoggingBuilder::buildFromFile(
                &f, logger);
        f.close();
        QDir().remove(tempName);

        CPPUNIT_ASSERT(sl2->type() == "File");
        CPPUNIT_ASSERT(sl2->properties().count() == 1);
        CPPUNIT_ASSERT(sl2->properties().contains("file-name"));
        CPPUNIT_ASSERT(sl2->properties()["file-name"] != NULL);

        delete sl2;
    }

private:
    MRU* pLTEditorMRU;
    LoggerPtr logger;
};

CPPUNIT_TEST_SUITE_REGISTRATION ( TestEditor );
// end of custom tests

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_editor.cpp 1572 2006-04-13 12:04:52Z modesto $
