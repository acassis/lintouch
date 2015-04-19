// $Id: TestPlugins.cpp,v 1.11 2004/12/16 12:26:34 modesto Exp $
//
//   FILE: TestPlugins.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 15 January 2003
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
#include <qfile.h>
#include <qdir.h>

#include "lt/templates/templates.h"

#include "lt/templates/TemplateManager.hh"
#include "lt/templates/PluginTemplateLibraryLoader.hh"
using namespace lt;

#include "lt/logger/ConsoleLogger.hh"

extern QString srcdir;
extern QString builddir;

class TestPlugins : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestPlugins );

    CPPUNIT_TEST( testTemplateLibrary );
    CPPUNIT_TEST( testPluginTemplateLibraryLoader );
    CPPUNIT_TEST( testTemplateManager );
    CPPUNIT_TEST( testTemplateInstantiation );
    CPPUNIT_TEST( testTemplateCloning );

    CPPUNIT_TEST_SUITE_END();

    public:

        void setUp()
        {
            // fill in
        }

        void tearDown()
        {
            // fill in
        }

        void testTemplateLibrary()
        {
            TemplateLibrary l;

            CPPUNIT_ASSERT(l.isA("lt::TemplateLibrary"));
            CPPUNIT_ASSERT(l.inherits("lt::TemplateLibrary"));
            CPPUNIT_ASSERT(l.inherits("QObject"));

            CPPUNIT_ASSERT( ! l.isValid() );
            l.validate();
            CPPUNIT_ASSERT( l.isValid() );

            CPPUNIT_ASSERT( l.info().author().isEmpty() );
            CPPUNIT_ASSERT( l.info().version().isEmpty() );
            CPPUNIT_ASSERT( l.info().shortDescription().isEmpty() );
            CPPUNIT_ASSERT( l.info().longDescription().isEmpty() );

            CPPUNIT_ASSERT( l.filename().isEmpty() );
            l.setFilename( "dummy/filename.so" );
            CPPUNIT_ASSERT( l.filename() == "dummy/filename.so" );

            CPPUNIT_ASSERT( l.instantiate( "whatever" ) == NULL );
        }

        void testPluginTemplateLibraryLoader()
        {
            TemplateLibrary * t = NULL;

            t = PluginTemplateLibraryLoader::loadFrom(
                    "build/mytemplatelibrary", QString::null,
                    LoggerPtr( new ConsoleLogger() ) );

            CPPUNIT_ASSERT( t != NULL );

            CPPUNIT_ASSERT( t->templates().count() == 4 );

            CPPUNIT_ASSERT( t->templates().contains( "Lamp" ) );
            CPPUNIT_ASSERT( t->templates().contains( "Button" ) );
            CPPUNIT_ASSERT( t->templates().contains( "Image" ) );
            CPPUNIT_ASSERT( t->templates().contains( "SimpleTrafficLights" ) );

            delete t;
        }

        void testTemplateManager()
        {
            TemplateManager m;
            TemplateLibrary * t = NULL, * t1 = NULL, * t2 = NULL;
            TemplateLibraryDict d;

            t = PluginTemplateLibraryLoader::loadFrom(
                    "build/mytemplatelibrary", QString::null,
                    LoggerPtr( new ConsoleLogger() ) );
            t1 = PluginTemplateLibraryLoader::loadFrom(
                    "build/mytemplatelibrary", QString::null,
                    LoggerPtr( new ConsoleLogger() ) );
            t2 = PluginTemplateLibraryLoader::loadFrom(
                    "build/mytemplatelibrary", QString::null,
                    LoggerPtr( new ConsoleLogger() ) );

            CPPUNIT_ASSERT( t->isValid() );
            CPPUNIT_ASSERT( t1->isValid() );
            CPPUNIT_ASSERT( t->filename().contains(
                    "build/") );
            CPPUNIT_ASSERT( t->filename().contains(
                    "mytemplatelibrary") );

            CPPUNIT_ASSERT( m.templateLibraries().count() == 0 );

            CPPUNIT_ASSERT( m.registerTemplateLibrary( "my", t ) );

            CPPUNIT_ASSERT( m.templateLibraries().count() == 1 );
            CPPUNIT_ASSERT( m.templateLibraries().contains( "my" ) );

            CPPUNIT_ASSERT( m.registerTemplateLibrary( "my1", t1 ) );

            CPPUNIT_ASSERT( m.templateLibraries().count() == 2 );
            CPPUNIT_ASSERT( m.templateLibraries().contains( "my1" ) );

            m.unregisterAllTemplateLibraries();

            CPPUNIT_ASSERT( m.templateLibraries().count() == 0 );

            CPPUNIT_ASSERT( m.registerTemplateLibrary( "my2", t2 ) );

            CPPUNIT_ASSERT( m.templateLibraries().count() == 1 );
            CPPUNIT_ASSERT( m.templateLibraries().contains( "my2" ) );
        }

        void testTemplateInstantiation()
        {
            TemplateLibrary * t = NULL;

            t = PluginTemplateLibraryLoader::loadFrom(
                    "build/mytemplatelibrary", QString::null,
                    LoggerPtr( new ConsoleLogger() ) );

            CPPUNIT_ASSERT( t != NULL );

            Template * it = t->instantiate( "Lamp",
                    LoggerPtr( new ConsoleLogger() ) );

            CPPUNIT_ASSERT( it != NULL );

            CPPUNIT_ASSERT( it->properties().count() == 16 );
            CPPUNIT_ASSERT( it->properties().contains( "link" ) );
            CPPUNIT_ASSERT( it->properties().contains( "border_off" ) );
            CPPUNIT_ASSERT( it->properties().contains( "border_on" ) );
            CPPUNIT_ASSERT( it->properties().contains( "fill_off" ) );
            CPPUNIT_ASSERT( it->properties().contains( "fill_on" ) );
            CPPUNIT_ASSERT( it->properties().contains( "fancy.animated" ) );
            CPPUNIT_ASSERT( it->properties().contains( "fancy.steps" ) );
            CPPUNIT_ASSERT( it->properties().contains( "shape" ) );
            CPPUNIT_ASSERT( it->properties().contains( "text_off" ) );
            CPPUNIT_ASSERT( it->properties().contains( "text_on" ) );
            CPPUNIT_ASSERT( it->properties().contains( "text_color_off" ) );
            CPPUNIT_ASSERT( it->properties().contains( "text_color_on" ) );
            CPPUNIT_ASSERT( it->properties().contains( "text_font_off" ) );
            CPPUNIT_ASSERT( it->properties().contains( "text_font_on" ) );
            CPPUNIT_ASSERT( it->properties().contains( "text_valign" ) );
            CPPUNIT_ASSERT( it->properties().contains( "text_halign" ) );

            CPPUNIT_ASSERT( it->iopins().count() == 1 );
            CPPUNIT_ASSERT( it->iopins().contains( "input" ) );

            CPPUNIT_ASSERT( it->iopinGroups().count() == 1 );

            CPPUNIT_ASSERT( it->iopins( "Inputs" ).count() == 1 );
            CPPUNIT_ASSERT( it->iopins( "Inputs" ).contains( "input" ) );

            CPPUNIT_ASSERT( it->iopins( "nonexistent" ).count() == 0 );

            CPPUNIT_ASSERT( it->properties().count() == 16 );
            CPPUNIT_ASSERT( it->propertyGroups().count() == 3 );

            CPPUNIT_ASSERT( it->properties(
                        "Inactive" ).count() == 9 );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "link" ) );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "shape" ) );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "text_halign" ) );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "text_valign" ) );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "border_off" ) );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "fill_off" ) );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "text_off" ) );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "text_color_off" ) );
            CPPUNIT_ASSERT( it->properties( "Inactive" )
                    .contains( "text_font_off" ) );

            CPPUNIT_ASSERT( it->properties(
                        "Active" ).count() == 5 );
            CPPUNIT_ASSERT( it->properties( "Active" )
                    .contains( "border_on" ) );
            CPPUNIT_ASSERT( it->properties( "Active" )
                    .contains( "fill_on" ) );
            CPPUNIT_ASSERT( it->properties( "Active" )
                    .contains( "text_on" ) );
            CPPUNIT_ASSERT( it->properties( "Active" )
                    .contains( "text_color_on" ) );
            CPPUNIT_ASSERT( it->properties( "Active" )
                    .contains( "text_font_on" ) );

            CPPUNIT_ASSERT( it->properties(
                        "Fancy Shape" ).count() == 2 );
            CPPUNIT_ASSERT( it->properties( "Fancy Shape" )
                    .contains( "fancy.steps" ) );
            CPPUNIT_ASSERT( it->properties( "Fancy Shape" )
                    .contains( "fancy.animated" ) );
            delete it;
            delete t;

        }

        void testTemplateCloning()
        {
            TemplateLibrary * t = NULL;

            t = PluginTemplateLibraryLoader::loadFrom(
                    "build/mytemplatelibrary", QString::null,
                    LoggerPtr( new ConsoleLogger() ) );

            CPPUNIT_ASSERT( t != NULL );

            Template * it = t->instantiate( "Lamp",
                    LoggerPtr( new ConsoleLogger() ) );

            CPPUNIT_ASSERT( it != NULL );

            it->properties()[ "link" ]->decodeValue( "dummy_link" );

            // clone modified template
            Template * it1 = it->clone( Localizator::nullPtr(),
                    TemplateManager(), LoggerPtr( new ConsoleLogger() ) );

            CPPUNIT_ASSERT( it1 != NULL );

            // check that it has been cloned properly
            CPPUNIT_ASSERT( it1->type() == "Lamp" );

            CPPUNIT_ASSERT( it1->properties() [ "link" ]->encodeValue()
                    == "dummy_link" );

            // now change the properties independently
            it->properties()[ "link" ]->decodeValue( "crash" );
            it1->properties()[ "link" ]->decodeValue( "boom" );

            // and check that they are not shared
            CPPUNIT_ASSERT( it->properties() [ "link" ]->encodeValue()
                    == "crash" );
            CPPUNIT_ASSERT( it1->properties() [ "link" ]->encodeValue()
                    == "boom" );

            delete it;
            delete it1;
        }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestPlugins );
// end of custom tests

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestPlugins.cpp,v 1.11 2004/12/16 12:26:34 modesto Exp $
