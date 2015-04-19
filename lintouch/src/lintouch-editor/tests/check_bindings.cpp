// $Id: check_bindings.cpp 1203 2006-01-05 11:37:22Z modesto $
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

#include "Bindings.hh"
#include "Binding.hh"

extern QString srcdir;
extern QString builddir;

class TestBindings : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestBindings );

    CPPUNIT_TEST( testBinding );

    CPPUNIT_TEST( testBindingClone );

    CPPUNIT_TEST( testBindingsAdd );
    CPPUNIT_TEST( testBindingsAdd2 );
    CPPUNIT_TEST( testBindingsAdd3 );
    CPPUNIT_TEST( testBindingsAdd4 );
    CPPUNIT_TEST( testBindingsAdd5 );

    CPPUNIT_TEST( testBindingsDel );

    CPPUNIT_TEST( testBindingsRenameConn );
    CPPUNIT_TEST( testBindingsRenameVar );
    CPPUNIT_TEST( testBindingsRenameView );
    CPPUNIT_TEST( testBindingsRenamePanel );
    CPPUNIT_TEST( testBindingsRenameTempl );

    CPPUNIT_TEST_SUITE_END();

    public:

    void setUp()
    {
    }

    void tearDown()
    {
    }

    void testBinding()
    {
        Binding a;
        CPPUNIT_ASSERT(a.valid() == false);

        Binding b("C", "V", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT(b.valid() == true);

        Binding c = b;
        CPPUNIT_ASSERT(c == &b);
        CPPUNIT_ASSERT(c.connection() == "C");

        // NOT supported because of API change
        // c.connection = "CC";
        // CPPUNIT_ASSERT(c.connection() == "CC");
        // CPPUNIT_ASSERT(b.connection() == "C");

        BindingPtr bp = BindingPtr(new Binding);
        CPPUNIT_ASSERT(!bp.isNull());
    }

    void testBindingClone()
    {
        Binding a("C", "VAR", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT(a.valid() == true);

        Binding* b = a.clone();
        CPPUNIT_ASSERT(b);

        CPPUNIT_ASSERT("C" == b->connection());
        CPPUNIT_ASSERT("VAR" == b->variable());
        CPPUNIT_ASSERT("VIEW" == b->view());
        CPPUNIT_ASSERT("P" == b->panel());
        CPPUNIT_ASSERT("T" == b->templ());
        CPPUNIT_ASSERT("I" == b->iopin());

        Binding aa("X", "VAR", "VIEW", "P", "T", "I");
        a = aa;
        CPPUNIT_ASSERT("C" == b->connection());
        CPPUNIT_ASSERT("X" == a.connection());
    }

    void testBindingsAdd()
    {
        Bindings bs;

        CPPUNIT_ASSERT(bs.binding().isNull());
        CPPUNIT_ASSERT(bs.binding("","","","","","").isNull());
        CPPUNIT_ASSERT(bs.bindingsByView("").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("","").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("","","").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByConnection("").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("","").count() == 0);

        const BindingPtr b2 = bs.addBinding("C", "VAR", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT("C" == b2->connection());
        CPPUNIT_ASSERT("VAR" == b2->variable());
        CPPUNIT_ASSERT("VIEW" == b2->view());
        CPPUNIT_ASSERT("P" == b2->panel());
        CPPUNIT_ASSERT("T" == b2->templ());
        CPPUNIT_ASSERT("I" == b2->iopin());

        const BindingPtr b3 = bs.binding("C", "VAR", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT(b2 == b3);

        const BindingsList& l0 = bs.bindingsByConnection("C");
        CPPUNIT_ASSERT(l0.count() == 1);
        CPPUNIT_ASSERT(l0[0] == b2);
        CPPUNIT_ASSERT(l0[0] == b3);

        const BindingsList& l1 = bs.bindingsByVariable("C", "VAR");
        CPPUNIT_ASSERT(l1.count() == 1);
        CPPUNIT_ASSERT(l1[0] == b2);
        CPPUNIT_ASSERT(l1[0] == b3);

        const BindingsList l4 = bs.bindingsByView("VIEW");
        CPPUNIT_ASSERT(l4.count() == 1);
        CPPUNIT_ASSERT(l4[0] == b2);
        CPPUNIT_ASSERT(l4[0] == b3);

        const BindingsList l2 = bs.bindingsByPanel("VIEW","P");
        CPPUNIT_ASSERT(l2.count() == 1);
        CPPUNIT_ASSERT(l2[0] == b2);
        CPPUNIT_ASSERT(l2[0] == b3);

        const BindingsList l3 = bs.bindingsByTemplate("VIEW","P","T");
        CPPUNIT_ASSERT(l3.count() == 1);
        CPPUNIT_ASSERT(l3[0] == b2);
        CPPUNIT_ASSERT(l3[0] == b3);

        const BindingPtr b4 = bs.binding();
        CPPUNIT_ASSERT(b4.isNull());
        CPPUNIT_ASSERT(bs.binding("","","","","","").isNull());
    }

    void testBindingsAdd2()
    {
        // duplicite bindings test
        Bindings bs;

        //no duplicites created
        const BindingPtr b1 = bs.addBinding("C", "VAR", "VIEW", "P", "T", "I");
        const BindingPtr b2 = bs.addBinding("C", "VAR", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT(b1 == b2);

        const BindingPtr b3 = bs.binding("C", "VAR", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT(b1 == b3);
        CPPUNIT_ASSERT(b2 == b3);

        const BindingsList& l0 = bs.bindingsByConnection("C");
        CPPUNIT_ASSERT(l0.count() == 1);
        CPPUNIT_ASSERT(l0[0] == b2);
        CPPUNIT_ASSERT(l0[0] == b3);

        const BindingsList& l1 = bs.bindingsByVariable("C", "VAR");
        CPPUNIT_ASSERT(l1.count() == 1);
        CPPUNIT_ASSERT(l1[0] == b2);
        CPPUNIT_ASSERT(l1[0] == b3);

        const BindingsList& l4 = bs.bindingsByView("VIEW");
        CPPUNIT_ASSERT(l4.count() == 1);
        CPPUNIT_ASSERT(l4[0] == b2);
        CPPUNIT_ASSERT(l4[0] == b3);

        const BindingsList& l2 = bs.bindingsByPanel("VIEW","P");
        CPPUNIT_ASSERT(l2.count() == 1);
        CPPUNIT_ASSERT(l2[0] == b2);
        CPPUNIT_ASSERT(l2[0] == b3);

        const BindingsList& l3 = bs.bindingsByTemplate("VIEW","P","T");
        CPPUNIT_ASSERT(l3.count() == 1);
        CPPUNIT_ASSERT(l3[0] == b2);
        CPPUNIT_ASSERT(l3[0] == b3);

        const BindingPtr b4 = bs.binding();
        CPPUNIT_ASSERT(b4.isNull());
        CPPUNIT_ASSERT(bs.binding("","","","","","").isNull());
    }

    void testBindingsAdd3()
    {
        // multiple bindings test
        Bindings bs;

        const BindingPtr b1 =
            bs.addBinding("CC", "VAR", "VIEW1", "P1", "T1", "I1");
        const BindingPtr b2 =
            bs.addBinding("CC", "VAR", "VIEW2", "P2", "T2", "I2");
        CPPUNIT_ASSERT(b1 != b2);

        CPPUNIT_ASSERT(bs.binding().isNull());
        CPPUNIT_ASSERT(bs.binding("", "", "", "", "", "").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CC", "VARR", "VIEW", "P", "T", "I").isNull());

        const BindingPtr b3 =
            bs.binding("CC", "VAR", "VIEW1", "P1", "T1", "I1");
        CPPUNIT_ASSERT(b1 == b3);

        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByConnection("C").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByConnection("").count() == 0);

        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", QString::null).count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("", QString::null).count() == 0);
        CPPUNIT_ASSERT(
                bs.bindingsByVariable(QString::null, QString::null).count() == 0);

        const BindingsList& l6 = bs.bindingsByView("VIEW1");
        CPPUNIT_ASSERT(l6.count() == 1);
        CPPUNIT_ASSERT(l6[0] == b1);

        const BindingsList& l2 = bs.bindingsByPanel("VIEW1","P1");
        CPPUNIT_ASSERT(l2.count() == 1);
        CPPUNIT_ASSERT(l2[0] == b1);

        const BindingsList& l3 = bs.bindingsByTemplate("VIEW1","P1","T1");
        CPPUNIT_ASSERT(l3.count() == 1);
        CPPUNIT_ASSERT(l3[0] == b1);

        const BindingsList& l7 = bs.bindingsByView("VIEW2");
        CPPUNIT_ASSERT(l7.count() == 1);
        CPPUNIT_ASSERT(l7[0] == b2);

        const BindingsList& l4 = bs.bindingsByPanel("VIEW2","P2");
        CPPUNIT_ASSERT(l4.count() == 1);
        CPPUNIT_ASSERT(l4[0] == b2);

        const BindingsList& l5 = bs.bindingsByTemplate("VIEW2","P2","T2");
        CPPUNIT_ASSERT(l5.count() == 1);
        CPPUNIT_ASSERT(l5[0] == b2);
    }

    void testBindingsAdd4()
    {
        // empty bindings test
        Bindings bs;

        const BindingPtr b1 = bs.addBinding("", "", "", "", "", "");
        CPPUNIT_ASSERT(!b1.isNull());

        const BindingPtr b2 = bs.addBinding( QString::null, QString::null,
                QString::null, QString::null, QString::null);
        CPPUNIT_ASSERT(!b2.isNull());

        CPPUNIT_ASSERT(!bs.binding("","","","","","").isNull());
        CPPUNIT_ASSERT(!bs.binding().isNull());

        // QString::null is NOT handled the same as ""
        CPPUNIT_ASSERT(b1 != b2);

        CPPUNIT_ASSERT(bs.bindingsByView("").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("","").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("","","").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("","").count() == 1);

        CPPUNIT_ASSERT(
                bs.bindingsByView(QString::null).count() == 1);
        CPPUNIT_ASSERT(
                bs.bindingsByPanel(QString::null,QString::null).count() == 1);
        CPPUNIT_ASSERT(
                bs.bindingsByTemplate(QString::null,QString::null,QString::null)
                .count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection(QString::null).count() == 1);
        CPPUNIT_ASSERT(
                bs.bindingsByVariable(QString::null,QString::null) .count() == 1);

        CPPUNIT_ASSERT(
                bs.bindingsByView(QString::null).count() == 1);
        CPPUNIT_ASSERT(
                bs.bindingsByPanel(QString::null,QString::null).count() == 1);
        CPPUNIT_ASSERT(
                bs.bindingsByTemplate(QString::null,QString::null,QString::null)
                .count() == 1);
        CPPUNIT_ASSERT(
                bs.bindingsByConnection(QString::null).count() == 1);
        CPPUNIT_ASSERT(
                bs.bindingsByVariable(QString::null,QString::null).count() == 1);
    }

    void testBindingsAdd5()
    {
        Bindings bs;

        const BindingPtr b1 = bs.addBinding("C", "VAR", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT(!b1.isNull());
        CPPUNIT_ASSERT(bs.addBinding(b1) == false); // already there
        CPPUNIT_ASSERT(bs.binding(b1));

        CPPUNIT_ASSERT(bs.bindingsByConnection("C").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 1);

        const BindingPtr b2 = bs.addBinding("CC", "VAR", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT(!b2.isNull());
        CPPUNIT_ASSERT(bs.addBinding(b2) == false); // already there
        CPPUNIT_ASSERT(bs.binding(b2));

        CPPUNIT_ASSERT(bs.bindingsByConnection("C").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 2);

        bs.removeBinding(b1);

        CPPUNIT_ASSERT(!bs.binding(b1));
        CPPUNIT_ASSERT(bs.addBinding(b1) == true); // not there yet
        CPPUNIT_ASSERT(bs.binding(b1));

        CPPUNIT_ASSERT(bs.bindingsByConnection("C").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 2);
    }

    void testBindingsDel()
    {
        Bindings bs;

        CPPUNIT_ASSERT(bs.binding().isNull());
        CPPUNIT_ASSERT(bs.bindingsByView("").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("","").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("","","").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByConnection("").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("","").count() == 0);

        BindingPtr b1 = bs.addBinding("CC", "VAR", "VIEW", "P", "T", "I");

        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 1);

        bs.removeBinding(b1);

        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC","VAR").count() == 0);

        BindingPtr b2 = bs.binding("CC", "VAR", "VIEW", "P", "T", "I");
        CPPUNIT_ASSERT(b2 != b1);
        CPPUNIT_ASSERT(b2.isNull());
    }

    void testBindingsRenameConn()
    {
        Bindings bs;

        BindingPtr b1 = bs.addBinding("CC", "VAR", "VIEW", "P", "T", "I");
        BindingPtr b2 = bs.addBinding("C", "V", "VIEW", "P", "T", "I");
        BindingPtr b3 = bs.addBinding("CCC", "V", "VIEW", "P", "T", "I");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());

        // rename unknow connection; nothing should happen
        bs.renameConnection("YYY","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());

        // rename to new name
        bs.renameConnection("CC","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 0);

        CPPUNIT_ASSERT(bs.bindingsByConnection("XXX").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("XXX", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("XXX", "VAR", "VIEW", "P", "T", "I").isNull());

        // rename to existing name; nothing should change except some error
        // messages in console...
        bs.renameConnection("XXX","CCC");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 0);

        CPPUNIT_ASSERT(bs.bindingsByConnection("XXX").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("XXX", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("XXX", "VAR", "VIEW", "P", "T", "I").isNull());

        // rename back
        bs.renameConnection("XXX","CC");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("XXX").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("XXX", "VAR").count() == 0);

        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("XXX", "VAR", "VIEW", "P", "T", "I").isNull());
    }

    void testBindingsRenameVar()
    {
        Bindings bs;

        BindingPtr b1 = bs.addBinding("CC", "VAR", "VIEW", "P", "T", "I");
        BindingPtr b2 = bs.addBinding("C", "V", "VIEW", "P", "T", "I");
        BindingPtr b3 = bs.addBinding("C", "VAR", "VIEW", "", "", "");
        BindingPtr b4 = bs.addBinding("CCC", "V", "VIEW", "P", "T", "I");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 4);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());

        // rename unknow variable; nothing should happen
        bs.renameVariable("C","YYY","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 4);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());

        // rename to new name
        bs.renameVariable("C","V","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "XXX").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "XXX").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 4);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "XXX", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "XXX", "VIEW", "P", "T", "I").isNull());

        // rename to existing name; nothing should change except some error
        // messages in console...
        bs.renameVariable("C","XXX","VAR");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "XXX").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "XXX").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 4);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "XXX", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "XXX", "VIEW", "P", "T", "I").isNull());

        // rename back
        bs.renameVariable("C","XXX","V");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 4);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CC", "XXX", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "XXX", "VIEW", "P", "T", "I").isNull());
    }

    void testBindingsRenameView()
    {
        Bindings bs;

        bs.addBinding("C", "V", "VIEW", "P", "T", "I");
        bs.addBinding("CC", "VAR", "VIEW", "P", "T", "I");
        bs.addBinding("CCC", "V", "VIEW", "P", "T", "I");
        bs.addBinding("CCC", "V", "YYY", "P", "T", "I");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByView("YYY").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("YYY","P").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("YYY","P","T").count() == 1);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());

        // rename unknow variable; nothing should happen
        bs.renameView("ZZZ","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByView("YYY").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("YYY","P").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("YYY","P","T").count() == 1);

        // rename to new name
        bs.renameView("VIEW","XXX");

        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByView("XXX").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("XXX","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("XXX","P","T").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByView("YYY").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("YYY","P").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("YYY","P","T").count() == 1);

        CPPUNIT_ASSERT(
                bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("C", "V", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "V", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "YYY", "P", "T", "I").isNull());

        // rename to existing name; nothing should change except some error
        // messages in console...
        bs.renameView("XXX","YYY");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByView("XXX").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("XXX","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("XXX","P","T").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByView("YYY").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("YYY","P").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("YYY","P","T").count() == 1);

        CPPUNIT_ASSERT(
                bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("C", "V", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "V", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "YYY", "P", "T", "I").isNull());

        // rename back
        bs.renameView("XXX","VIEW");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByConnection("C").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("C", "V").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CC").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CC", "VAR").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByConnection("CCC").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByVariable("CCC", "V").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByView("XXX").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("XXX","P").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("XXX","P","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P","T").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByView("YYY").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("YYY","P").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("YYY","P","T").count() == 1);

        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CC", "VAR", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("C", "V", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "V", "XXX", "P", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "YYY", "P", "T", "I").isNull());
    }

    void testBindingsRenamePanel()
    {
        Bindings bs;

        bs.addBinding("C", "V", "VIEW", "P1", "T", "I");
        bs.addBinding("CCC", "V", "VIEW", "P2", "T", "I");
        bs.addBinding("CC", "VAR", "VIEW2", "P1", "T2", "I");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW2","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);

        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW2", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());

        // rename unknow panel; nothing should happen
        bs.renamePanel("VIEW","ZZZ","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW2","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);

        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW2", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());

        // rename to new name
        bs.renamePanel("VIEW","P1","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","XXX").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","XXX","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW2","XXX","T2").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW2","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);

        CPPUNIT_ASSERT(
                bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW2", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "XXX", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CC", "VAR", "VIEW2", "XXX", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());

        // rename to existing name; nothing should change except some error
        // messages in console...
        bs.renamePanel("VIEW","XXX","P2");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","XXX").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","XXX","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW2","XXX","T2").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW2","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);

        CPPUNIT_ASSERT(
                bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW2", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "XXX", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CC", "VAR", "VIEW2", "XXX", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());

        // rename back
        bs.renamePanel("VIEW","XXX","P1");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","XXX").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","XXX","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW2","XXX","T2").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW2","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);

        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW2", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("C", "V", "VIEW", "XXX", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CC", "VAR", "VIEW2", "XXX", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());
    }

    void testBindingsRenameTempl()
    {
        Bindings bs;

        bs.addBinding("C", "V", "VIEW", "P1", "T", "I");
        bs.addBinding("CC", "VAR", "VIEW", "P1", "T2", "I");
        bs.addBinding("CCC", "V", "VIEW", "P2", "T", "I");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);

        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());

        // rename unknow template; nothing should happen
        bs.renameTemplate("VIEW","P1","ZZZ","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);

        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());

        // rename to new name
        bs.renameTemplate("VIEW","P1","T","XXX");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","XXX").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","XXX").count() == 0);

        CPPUNIT_ASSERT(
                bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P1", "XXX", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "V", "VIEW", "P2", "XXX", "I").isNull());

        // rename to existing name; nothing should change except some error
        // messages in console...
        bs.renameTemplate("VIEW","P1","XXX","T2");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","XXX").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","XXX").count() == 0);

        CPPUNIT_ASSERT(
                bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P1", "XXX", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "V", "VIEW", "P2", "XXX", "I").isNull());

        // rename back
        bs.renameTemplate("VIEW","P1","XXX","T");

        // consistency test
        CPPUNIT_ASSERT(bs.bindingsByView("VIEW").count() == 3);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P1").count() == 2);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","XXX").count() == 0);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P1","T2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByPanel("VIEW","P2").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","T").count() == 1);
        CPPUNIT_ASSERT(bs.bindingsByTemplate("VIEW","P2","XXX").count() == 0);

        CPPUNIT_ASSERT(
                !bs.binding("C", "V", "VIEW", "P1", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("C", "V", "VIEW", "P1", "XXX", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CC", "VAR", "VIEW", "P1", "T2", "I").isNull());
        CPPUNIT_ASSERT(
                !bs.binding("CCC", "V", "VIEW", "P2", "T", "I").isNull());
        CPPUNIT_ASSERT(
                bs.binding("CCC", "V", "VIEW", "P2", "XXX", "I").isNull());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION ( TestBindings );
// end of custom tests

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_bindings.cpp 1203 2006-01-05 11:37:22Z modesto $
