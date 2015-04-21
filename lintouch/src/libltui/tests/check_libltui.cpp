// $Id: check_libltui.cpp,v 1.6 2004/09/14 13:08:19 mman Exp $
//
//   FILE: check_libltui.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 06 November 2003
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

#include <stdlib.h>
#include <qapplication.h>

// custom tests
#include <cppunit/extensions/HelperMacros.h>

#include "lt/ui/Undo.hh"
using namespace lt;

QString srcdir;
QString builddir;

class TestUI :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE ( TestUI );

    CPPUNIT_TEST ( testUndo1 );
    CPPUNIT_TEST ( testUndo2 );
    CPPUNIT_TEST ( testUndo3 );
    CPPUNIT_TEST ( testUndo4 );
    CPPUNIT_TEST ( testUndo5 );
    CPPUNIT_TEST ( testUndo6 );

    CPPUNIT_TEST_SUITE_END ();

    //////////////////////
    UndoStack* s;

    public:
        void setUp()
        {
            s = new UndoStack(NULL);
            CPPUNIT_ASSERT(s);
        }

        void tearDown()
        {
            delete s;
        }

        void testUndo1()
        {
            UndoStack* s = new UndoStack(NULL);
            CPPUNIT_ASSERT(s);

            CPPUNIT_ASSERT(s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(!s->canUndo());
        }

        void testUndo2()
        {
            UndoCommand* c = new UndoCommand;
            CPPUNIT_ASSERT(c);
            CPPUNIT_ASSERT(c->description() == QString::null);
            c->setDescription("X");
            CPPUNIT_ASSERT(c->description() == "X");
            delete c;

            c = new UndoCommand("LTE");
            CPPUNIT_ASSERT(c);
            CPPUNIT_ASSERT(c->description() == "LTE");
            c->setDescription("Y");
            CPPUNIT_ASSERT(c->description() == "Y");
            delete c;
        }

        void testUndo3()
        {
            UndoCommand* c = new UndoCommand("LTE");
            CPPUNIT_ASSERT(c);

            CPPUNIT_ASSERT(s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(!s->canUndo());

            s->push(c);
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(!s->canUndo());

            s->redo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            c = new UndoCommand("LTE2");
            CPPUNIT_ASSERT(c);

            s->push(c);
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->redo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(!s->canUndo());
        }

        void testUndo4()
        {
            QAction* a = s->createRedoAction(NULL);
            CPPUNIT_ASSERT(a);
            delete a;

            a = s->createUndoAction(NULL);
            CPPUNIT_ASSERT(a);
            delete a;
        }

        void testUndo5()
        {
            QAction* a1 = s->createRedoAction(NULL);
            CPPUNIT_ASSERT(a1);
            QAction* a2 = s->createUndoAction(NULL);
            CPPUNIT_ASSERT(a2);

            CPPUNIT_ASSERT(!a1->isEnabled());
            CPPUNIT_ASSERT(!a2->isEnabled());

            UndoCommand* c = new UndoCommand("LTE");
            CPPUNIT_ASSERT(c);

            QString uR = a1->text();
            QString uD = a2->text();
            s->push(c);
            CPPUNIT_ASSERT(!a1->isEnabled());
            CPPUNIT_ASSERT(a2->isEnabled());
            CPPUNIT_ASSERT(uR == a1->text());
            CPPUNIT_ASSERT(uD != a2->text());

            uR = a1->text();
            uD = a2->text();
            s->undo();
            CPPUNIT_ASSERT(a1->isEnabled());
            CPPUNIT_ASSERT(!a2->isEnabled());
            CPPUNIT_ASSERT(uR != a1->text());
            CPPUNIT_ASSERT(uD != a2->text());

            delete a1;
            delete a2;
        }

        void testUndo6()
        {
            UndoCommand* c = new UndoCommand("LTE");
            CPPUNIT_ASSERT(c);

            CPPUNIT_ASSERT(s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(!s->canUndo());

            s->push(c);
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(!s->canUndo());

            s->redo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            c = new UndoCommand("LTE2");
            CPPUNIT_ASSERT(c);

            s->push(c);
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->redo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(!s->canUndo());

            c = new UndoCommand("LTE3");
            CPPUNIT_ASSERT(c);

            s->push(c);
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());

            s->undo();
            CPPUNIT_ASSERT(s->isClean());
            CPPUNIT_ASSERT(s->canRedo());
            CPPUNIT_ASSERT(!s->canUndo());

            s->redo();
            CPPUNIT_ASSERT(!s->isClean());
            CPPUNIT_ASSERT(!s->canRedo());
            CPPUNIT_ASSERT(s->canUndo());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION ( TestUI );
// end of custom tests

// standard test suite
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "lt/ui/ui.h"

int main ( int argc, char ** argv )
{
    // figure out where our sources are and where we can write
    srcdir = getenv( "srcdir" );
    if( srcdir.isEmpty() ) srcdir = ".";
    builddir = getenv( "top_builddir" );
    if( builddir.isEmpty() ) builddir = ".";

    if ( lt_ui_initialize() == -1 ) {
        fprintf ( stderr, "Unable to initialize Lintouch UI Library\n" );
        return -1;
    }
    atexit ( lt_ui_terminate );

    QApplication a ( argc, argv, false );

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry ();
    runner.addTest ( registry.makeTest () );

    bool wasThereError = runner.run ( "", false );

    // shell expects 0 if successfull
    return !wasThereError;
}
// end of standard test suite

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_libltui.cpp,v 1.6 2004/09/14 13:08:19 mman Exp $

