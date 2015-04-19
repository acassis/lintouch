// $Id: TestClasses.cpp,v 1.18 2004/12/02 11:54:21 mman Exp $
//
//   FILE: TestClasses.cpp --
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
#include "lt/templates/Info.hh"
#include "lt/templates/IOPin.hh"
#include "lt/templates/Variable.hh"
#include "lt/templates/Property.hh"
#include "lt/templates/Connection.hh"
using namespace lt;

#include "lt/logger/ConsoleLogger.hh"

class DummyTemplate: public Template
{
    public:

        bool notified;

        DummyTemplate() : Template( "Dummy" ), notified( false )
        {
            registerIOPin( "Inputs", "input",
                    new IOPin( this, "Input", "bit" ) );
            registerIOPin( "Outputs", "output",
                    new IOPin( this, "Output", "number,string" ) );
        }

        virtual void iopinsChanged()
        {
            notified = true;
        }
};

class TestClasses : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestClasses );

    CPPUNIT_TEST( testInfo );
    CPPUNIT_TEST( testInfoSharingNCloning );

    CPPUNIT_TEST( testProperty );
    CPPUNIT_TEST( testPropertyTypes );
    CPPUNIT_TEST( testPropertyShadowing );
    CPPUNIT_TEST( testPropertyValues );

    CPPUNIT_TEST( testIOPinTypeDecoding );

    CPPUNIT_TEST( testVariable );
    CPPUNIT_TEST( testConnection );

    CPPUNIT_TEST( testIOPin );
    CPPUNIT_TEST( testIOPinBindings );

    CPPUNIT_TEST( testTemplateShortcutNLibrary );

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

        void testInfo()
        {
            Info i;

            i.setAuthor( "martin" );
            CPPUNIT_ASSERT( i.author() == "martin" );

            i.setVersion( "0.1.2" );
            CPPUNIT_ASSERT( i.version() == "0.1.2" );

            QDate now = QDate();
            i.setDate( now );
            CPPUNIT_ASSERT( i.date() == now );

            i.setShortDescription( "short" );
            CPPUNIT_ASSERT( i.shortDescription() == "short" );

            i.setLongDescription( "long" );
            CPPUNIT_ASSERT( i.longDescription() == "long" );

            //FIXME: mman
            //don't know yet how to test pixmaps for equality
            //i.setIcon( "0.1.2" );
            //CPPUNIT_ASSERT( i.icon() == "0.1.2" );
        }

        void testInfoSharingNCloning()
        {

            Info i, j, k;

            i.setAuthor( "mman" );

            CPPUNIT_ASSERT( i.author() == "mman" );

            j = i;

            CPPUNIT_ASSERT( j.author() == "mman" );

            i.setAuthor( "namm" );

            CPPUNIT_ASSERT( i.author() == "namm" );
            CPPUNIT_ASSERT( j.author() == "namm" );

            k = i.clone();

            k.setAuthor( "kokotek" );

            CPPUNIT_ASSERT( i.author() == "namm" );
            CPPUNIT_ASSERT( k.author() == "kokotek" );

        }

        void testProperty()
        {
            Property inv( "Dummy invalid property", "blablabla" );
            Property i( "Dummy integer property", "integer" );
            Property f( "Dummy float property", "float" );
            Property s( "Dummy string property", "string" );
            Property c( "Dummy choice property", "choice(one,two,three)" );

            Property p( "Dummy pen property", "pen" );
            Property b( "Dummy brush property", "brush" );
            Property fn( "Dummy font property", "font" );

            Property rt( "Dummy rect property", "rect" );
            Property pn( "Dummy point property", "point" );

            Property rs( "Dummy resource property", "resource" );
            Property ln( "Dummy link property", "link" );

            Property shdw( "Dummy shadow property", &i );

            CPPUNIT_ASSERT( inv.label() == "Dummy invalid property" );
            CPPUNIT_ASSERT( inv.type() == Property::InvalidType );
            CPPUNIT_ASSERT( inv.isDefault() );
            CPPUNIT_ASSERT( i.label() == "Dummy integer property" );
            CPPUNIT_ASSERT( i.type() == Property::IntegerType );
            CPPUNIT_ASSERT( i.isDefault() );
            CPPUNIT_ASSERT( f.label() == "Dummy float property" );
            CPPUNIT_ASSERT( f.type() == Property::FloatingPointType );
            CPPUNIT_ASSERT( f.isDefault() );
            CPPUNIT_ASSERT( s.label() == "Dummy string property" );
            CPPUNIT_ASSERT( s.type() == Property::StringType );
            CPPUNIT_ASSERT( s.isDefault() );
            CPPUNIT_ASSERT( c.label() == "Dummy choice property" );
            CPPUNIT_ASSERT( c.type() == Property::ChoiceType );
            CPPUNIT_ASSERT( c.isDefault() );
            CPPUNIT_ASSERT( p.label() == "Dummy pen property" );
            CPPUNIT_ASSERT( p.type() == Property::PenType );
            CPPUNIT_ASSERT( p.isDefault() );
            CPPUNIT_ASSERT( b.label() == "Dummy brush property" );
            CPPUNIT_ASSERT( b.type() == Property::BrushType );
            CPPUNIT_ASSERT( b.isDefault() );
            CPPUNIT_ASSERT( fn.label() == "Dummy font property" );
            CPPUNIT_ASSERT( fn.type() == Property::FontType );
            CPPUNIT_ASSERT( fn.isDefault() );

            CPPUNIT_ASSERT( rt.label() == "Dummy rect property" );
            CPPUNIT_ASSERT( rt.type() == Property::RectType );
            CPPUNIT_ASSERT( rt.isDefault() );
            CPPUNIT_ASSERT( pn.label() == "Dummy point property" );
            CPPUNIT_ASSERT( pn.type() == Property::PointType );
            CPPUNIT_ASSERT( pn.isDefault() );

            CPPUNIT_ASSERT( rs.label() == "Dummy resource property" );
            CPPUNIT_ASSERT( rs.type() == Property::ResourceType );
            CPPUNIT_ASSERT( rs.isDefault() );
            CPPUNIT_ASSERT( ln.label() == "Dummy link property" );
            CPPUNIT_ASSERT( ln.type() == Property::LinkType );
            CPPUNIT_ASSERT( ln.isDefault() );

            CPPUNIT_ASSERT( shdw.label() == "Dummy shadow property" );
            CPPUNIT_ASSERT( shdw.type() == Property::IntegerType );
            CPPUNIT_ASSERT( shdw.isDefault() );

            // test enabled flag
            CPPUNIT_ASSERT( i.isEnabled() == true );
            CPPUNIT_ASSERT( shdw.isEnabled() == true );

            i.setEnabled( false );

            CPPUNIT_ASSERT( i.isEnabled() == false );
            CPPUNIT_ASSERT( shdw.isEnabled() == false );
        }

        void testPropertyTypes()
        {
            LoggerPtr lll( new ConsoleLogger() );

            Property i( "", "Integer", "", lll );
            Property f( "", "fLoat", "", lll );
            Property s( "", "stRing", "", lll );
            Property c( "", "choIce(one,two,three)", "", lll );
            Property col( "", "cOloR", "", lll );
            Property p( "", "PEN", "", lll );
            Property b( "", "bruSh", "", lll );
            Property fn( "", "fonT", "", lll );
            Property r( "", "reSouRce", "", lll );
            Property l( "", "LinK", "", lll );
            Property k( "", "kuul", "", lll );
            Property pn( "", "pOInt", "", lll );
            Property rt( "", "reCT", "", lll );

            CPPUNIT_ASSERT( i.type() == Property::IntegerType );
            CPPUNIT_ASSERT( i.isDefault() );
            CPPUNIT_ASSERT( f.type() == Property::FloatingPointType );
            CPPUNIT_ASSERT( f.isDefault() );
            CPPUNIT_ASSERT( s.type() == Property::StringType );
            CPPUNIT_ASSERT( s.isDefault() );
            CPPUNIT_ASSERT( c.type() == Property::ChoiceType );
            CPPUNIT_ASSERT( c.isDefault() );
            CPPUNIT_ASSERT( p.type() == Property::PenType );
            CPPUNIT_ASSERT( p.isDefault() );
            CPPUNIT_ASSERT( col.type() == Property::ColorType );
            CPPUNIT_ASSERT( col.isDefault() );
            CPPUNIT_ASSERT( b.type() == Property::BrushType );
            CPPUNIT_ASSERT( b.isDefault() );
            CPPUNIT_ASSERT( fn.type() == Property::FontType );
            CPPUNIT_ASSERT( fn.isDefault() );
            CPPUNIT_ASSERT( r.type() == Property::ResourceType );
            CPPUNIT_ASSERT( r.isDefault() );
            CPPUNIT_ASSERT( l.type() == Property::LinkType );
            CPPUNIT_ASSERT( l.isDefault() );
            CPPUNIT_ASSERT( k.type() == Property::InvalidType );
            CPPUNIT_ASSERT( k.isDefault() );
            CPPUNIT_ASSERT( pn.type() == Property::PointType );
            CPPUNIT_ASSERT( pn.isDefault() );
            CPPUNIT_ASSERT( rt.type() == Property::RectType );
            CPPUNIT_ASSERT( rt.isDefault() );
        }

        void testPropertyShadowing()
        {
            Property i( "", "integer", "100" );
            Property j( "", &i );

            CPPUNIT_ASSERT( i.asInteger() == 100 );
            CPPUNIT_ASSERT( j.asInteger() == 100 );
            CPPUNIT_ASSERT( i.isDefault() );
            CPPUNIT_ASSERT( j.isDefault() );

            i.decodeValue( "200" );

            CPPUNIT_ASSERT( i.asInteger() == 200 );
            CPPUNIT_ASSERT( j.asInteger() == 200 );
            CPPUNIT_ASSERT( ! i.isDefault() );
            CPPUNIT_ASSERT( j.isDefault() );

            j.decodeValue( "100" );

            CPPUNIT_ASSERT( i.asInteger() == 200 );
            CPPUNIT_ASSERT( j.asInteger() == 100 );
            CPPUNIT_ASSERT( ! i.isDefault() );
            CPPUNIT_ASSERT( ! j.isDefault() );

            i.decodeValue( "300" );
            j.decodeValue( "500" );

            CPPUNIT_ASSERT( i.asInteger() == 300 );
            CPPUNIT_ASSERT( j.asInteger() == 500 );
            CPPUNIT_ASSERT( ! i.isDefault() );
            CPPUNIT_ASSERT( ! j.isDefault() );

            i.resetToDefault();

            CPPUNIT_ASSERT( i.asInteger() == 100 );
            CPPUNIT_ASSERT( j.asInteger() == 500 );
            CPPUNIT_ASSERT( i.isDefault() );
            CPPUNIT_ASSERT( ! j.isDefault() );

            j.resetToDefault();

            CPPUNIT_ASSERT( i.asInteger() == 100 );
            CPPUNIT_ASSERT( j.asInteger() == 100 );
            CPPUNIT_ASSERT( i.isDefault() );
            CPPUNIT_ASSERT( j.isDefault() );
        }

        void testPropertyValues()
        {

            // integer
            Property i( "", "integer" );

            CPPUNIT_ASSERT( i.encodeType() == "integer" );

            i.decodeValue( "0" );
            CPPUNIT_ASSERT( i.asInteger() == 0 );
            CPPUNIT_ASSERT( i.encodeValue() == "0" );

            i.decodeValue( "-1" );
            CPPUNIT_ASSERT( i.asInteger() == -1 );
            CPPUNIT_ASSERT( i.encodeValue() == "-1" );

            i.decodeValue( "100" );
            CPPUNIT_ASSERT( i.asInteger() == 100 );
            CPPUNIT_ASSERT( i.encodeValue() == "100" );

            i.setIntegerValue( 99 );
            CPPUNIT_ASSERT( i.asInteger() == 99 );

            // float
            Property f( "", "float" );

            CPPUNIT_ASSERT( f.encodeType() == "float" );

            f.decodeValue( "0" );
            CPPUNIT_ASSERT( f.asFloatingPoint() == 0 );
            CPPUNIT_ASSERT( f.encodeValue() == "0" );

            f.decodeValue( "-1.5" );
            CPPUNIT_ASSERT( f.asFloatingPoint() == -1.5 );
            CPPUNIT_ASSERT( f.encodeValue() == "-1.5" );

            f.decodeValue( "100.25" );
            CPPUNIT_ASSERT( f.asFloatingPoint() == 100.25 );
            CPPUNIT_ASSERT( f.encodeValue() == "100.25" );

            f.setFloatingPointValue( 3.141592654 );
            CPPUNIT_ASSERT( f.asFloatingPoint() == 3.141592654 );

            // string
            Property s( "", "string" );

            CPPUNIT_ASSERT( s.encodeType() == "string" );

            s.decodeValue( "" );
            CPPUNIT_ASSERT( s.asString().isEmpty() );
            CPPUNIT_ASSERT( s.encodeValue().isEmpty() );

            s.decodeValue( "-1.5" );
            CPPUNIT_ASSERT( s.asString() == "-1.5" );
            CPPUNIT_ASSERT( s.encodeValue() == "-1.5" );

            s.decodeValue( "100.25" );
            CPPUNIT_ASSERT( s.asString() == "100.25" );
            CPPUNIT_ASSERT( s.encodeValue() == "100.25" );

            s.setStringValue( "LinTouch" );
            CPPUNIT_ASSERT( s.asString() == "LinTouch" );

            // choice
            Property c( "", "choice(one,two,three)" );

            CPPUNIT_ASSERT( c.encodeType() == "choice(one,two,three)" );

            // check that the default is the first one when unspecified
            CPPUNIT_ASSERT( c.asString() == "one" );
            CPPUNIT_ASSERT( c.encodeValue() == "one" );

            c.decodeValue( "one" );
            CPPUNIT_ASSERT( c.asString() == "one" );
            CPPUNIT_ASSERT( c.encodeValue() == "one" );

            c.setChoiceValue( "two" );
            CPPUNIT_ASSERT( c.asString() == "two" );
            CPPUNIT_ASSERT( c.encodeValue() == "two" );

            c.setChoiceValue( "cool" );
            CPPUNIT_ASSERT( c.asString() == "two" );
            CPPUNIT_ASSERT( c.encodeValue() == "two" );

            // QPen
            Property p( "", "pen" );

            CPPUNIT_ASSERT( p.encodeType() == "pen" );

            p.decodeValue( "" );
            CPPUNIT_ASSERT( p.asPen() == QPen( Qt::SolidLine ) );

            // NoPen handling
            p.setPenValue( QPen( Qt::NoPen ) );
            CPPUNIT_ASSERT( p.asPen() == QPen( Qt::NoPen ) );
            // Bad optimization. We loose the color if "" is returned
            // CPPUNIT_ASSERT( p.encodeValue() == "" );

            p.decodeValue( "#FFEE55;1;SolidLine" );
            CPPUNIT_ASSERT( p.asPen() ==
                    QPen( QColor("#FFEE55"), 1, Qt::SolidLine ) );
            CPPUNIT_ASSERT( p.encodeValue() ==
                    "#FFEE55;1;SOLIDLINE" );

            CPPUNIT_ASSERT( !p.decodeValue( "#xxxxxx;1;SolidLine" ) );

            CPPUNIT_ASSERT( !p.decodeValue( ";" ) );

            p.setPenValue( QPen( Qt::blue, 10, Qt::SolidLine ) );
            CPPUNIT_ASSERT( p.asPen() ==
                    QPen( Qt::blue, 10, Qt::SolidLine ) );

            // QColor
            Property xxx( "", "color" );

            CPPUNIT_ASSERT( xxx.encodeType() == "color" );

            xxx.decodeValue( "" );
            CPPUNIT_ASSERT( xxx.asColor() == QColor( Qt::black ) );

            xxx.decodeValue( "#FFEE55" );
            CPPUNIT_ASSERT( xxx.asColor() ==
                    QColor("#FFEE55") );
            CPPUNIT_ASSERT( xxx.encodeValue() ==
                    "#FFEE55" );

            CPPUNIT_ASSERT( !xxx.decodeValue( "#xxxxxx" ) );

            xxx.setColorValue( QColor( Qt::blue ) );
            CPPUNIT_ASSERT( xxx.asColor() ==
                    QColor( Qt::blue ) );

            // QBrush
            Property b( "", "brush" );

            CPPUNIT_ASSERT( b.encodeType() == "brush" );

            b.decodeValue( "" );
            CPPUNIT_ASSERT( b.asBrush() == QBrush( Qt::NoBrush ) );
            // Bad optimization. We loose the width and color if "" is
            // returned
            // CPPUNIT_ASSERT( b.encodeValue() == "" );

            b.decodeValue( "#FFEE55;SolidPattern" );
            CPPUNIT_ASSERT( b.asBrush() ==
                    QBrush( QColor("#FFEE55"), Qt::SolidPattern ) );
            CPPUNIT_ASSERT( b.encodeValue() ==
                    "#FFEE55;SOLIDPATTERN" );

            CPPUNIT_ASSERT( !b.decodeValue( "#xxxxxx;SolidPattern" ) );

            CPPUNIT_ASSERT( !b.decodeValue( ";" ) );

            b.setBrushValue( QBrush( Qt::blue, Qt::CrossPattern ) );
            CPPUNIT_ASSERT( b.asBrush() ==
                    QBrush( Qt::blue, Qt::CrossPattern ) );

            // QFont
            Property fn( "", "font" );
            QFont tf;

            CPPUNIT_ASSERT( fn.encodeType() == "font" );

            fn.decodeValue( "" );
            CPPUNIT_ASSERT( fn.asFont() == QFont() );
            CPPUNIT_ASSERT( fn.encodeValue() ==
                    Property( "", "font" ).encodeValue() );

            tf = QFont( "VERDANA" );
            tf.setPointSize( 12 );
            tf.setWeight( QFont::Bold );
            tf.setItalic( true );
            fn.decodeValue( "VERDANA;12;Bold;Italic" );

            CPPUNIT_ASSERT( fn.asFont() == tf );
            CPPUNIT_ASSERT( fn.encodeValue() == "VERDANA;12;BOLD;ITALIC" );

            tf.setItalic( false );
            fn.decodeValue( "VERDANA;12;Bold" );
            CPPUNIT_ASSERT( fn.asFont() == tf );
            CPPUNIT_ASSERT( fn.encodeValue() == "VERDANA;12;BOLD" );

            CPPUNIT_ASSERT( !fn.decodeValue( ";;" ) );

            CPPUNIT_ASSERT( !fn.decodeValue( "xxx;xxxx;xxx;xx" ) );

            tf = QFont( "VERDANA" );
            tf.setPointSize( 12 );
            fn.decodeValue( "VERDANA;12;Fat" );
            CPPUNIT_ASSERT( fn.asFont() == tf );
            CPPUNIT_ASSERT( fn.encodeValue() == "VERDANA;12;NORMAL" );

            tf = QFont( "TAHOMA", 10, QFont::Bold, true );
            tf.setPointSize( 10 );
            fn.setFontValue( tf );
            CPPUNIT_ASSERT( fn.asFont() == tf );

            tf = QFont( "ARIAL", 24 );
            CPPUNIT_ASSERT( Property::fontToString( tf ) ==
                    QString("ARIAL;24;NORMAL") );

            // rect
            Property r( "", "rect" );

            CPPUNIT_ASSERT( r.encodeType() == "rect" );

            r.decodeValue( "" );
            CPPUNIT_ASSERT( r.asRect() == QRect() );
            CPPUNIT_ASSERT( r.encodeValue() == "0;0;0;0" );

            r.decodeValue( "10;10;100;100" );
            CPPUNIT_ASSERT( r.asRect() ==
                    QRect( QPoint(10,10), QSize(100,100) ) );
            CPPUNIT_ASSERT( r.encodeValue() == "10;10;100;100" );

            r.decodeValue( "100;100" );
            CPPUNIT_ASSERT( r.asRect() ==
                    QRect( QPoint(0,0), QSize(100,100) ) );
            CPPUNIT_ASSERT( r.encodeValue() == "0;0;100;100" );

            CPPUNIT_ASSERT( !r.decodeValue( ";" ) );

            CPPUNIT_ASSERT( !r.decodeValue( "x;x" ) );

            r.decodeValue( "-10;-20;-30;-40" );
            CPPUNIT_ASSERT( r.asRect() ==
                    QRect( QPoint(-10,-20), QSize(-30,-40) ) );
            CPPUNIT_ASSERT( r.encodeValue() == "-10;-20;-30;-40" );

            r.setRectValue( QRect( 10, 10, 20, 20 ) );
            CPPUNIT_ASSERT( r.asRect() == QRect( 10, 10, 20, 20 ) );

            // point
            Property pt( "", "point" );

            CPPUNIT_ASSERT( pt.encodeType() == "point" );

            pt.decodeValue( "" );
            CPPUNIT_ASSERT( pt.asPoint() == QPoint() );
            CPPUNIT_ASSERT( pt.encodeValue() == "0;0" );

            CPPUNIT_ASSERT( !pt.decodeValue( ";" ) );

            CPPUNIT_ASSERT( !pt.decodeValue( "x;x" ) );

            pt.decodeValue( "10;20" );
            CPPUNIT_ASSERT( pt.asPoint() == QPoint(10,20) );
            CPPUNIT_ASSERT( pt.encodeValue() == "10;20" );

            pt.decodeValue( "-10;-20" );
            CPPUNIT_ASSERT( pt.asPoint() == QPoint(-10,-20) );
            CPPUNIT_ASSERT( pt.encodeValue() == "-10;-20" );

            pt.setPointValue( QPoint( 5, 5 ) );
            CPPUNIT_ASSERT( pt.asPoint() == QPoint( 5, 5 ) );

            // resource
            Property rs( "", "resource" );

            CPPUNIT_ASSERT( rs.encodeType() == "resource" );

            rs.decodeValue( "" );
            CPPUNIT_ASSERT( rs.asResource().isEmpty() );
            CPPUNIT_ASSERT( rs.encodeValue().isEmpty() );

            rs.decodeValue( "-1.5" );
            CPPUNIT_ASSERT( rs.asResource() == "-1.5" );
            CPPUNIT_ASSERT( rs.encodeValue() == "-1.5" );

            rs.decodeValue( "100.25" );
            CPPUNIT_ASSERT( rs.asResource() == "100.25" );
            CPPUNIT_ASSERT( rs.encodeValue() == "100.25" );

            rs.setResourceValue( "LinTouch" );
            CPPUNIT_ASSERT( rs.asResource() == "LinTouch" );

            // link
            Property l( "", "link" );

            CPPUNIT_ASSERT( l.encodeType() == "link" );

            l.decodeValue( "" );
            CPPUNIT_ASSERT( l.asLink().isEmpty() );
            CPPUNIT_ASSERT( l.encodeValue().isEmpty() );

            l.decodeValue( "-1.5" );
            CPPUNIT_ASSERT( l.asLink() == "-1.5" );
            CPPUNIT_ASSERT( l.encodeValue() == "-1.5" );

            l.decodeValue( "100.25" );
            CPPUNIT_ASSERT( l.asLink() == "100.25" );
            CPPUNIT_ASSERT( l.encodeValue() == "100.25" );

            l.setResourceValue( "LinTouch" );
            CPPUNIT_ASSERT( l.asLink() == "LinTouch" );
        }

        void testIOPinTypeDecoding()
        {
            IOPin::Type t;

            t = IOPin::decodeType( "" );
            CPPUNIT_ASSERT( t == IOPin::InvalidType );
            t = IOPin::decodeType( "blabla" );
            CPPUNIT_ASSERT( t == IOPin::InvalidType );
            t = IOPin::decodeType( " BiT    " );
            CPPUNIT_ASSERT( t == IOPin::BitType );
            t = IOPin::decodeType( "NuMBER  " );
            CPPUNIT_ASSERT( t == IOPin::NumberType );
            t = IOPin::decodeType( "  stRING" );
            CPPUNIT_ASSERT( t == IOPin::StringType );
            t = IOPin::decodeType( " SeqUENCE   " );
            CPPUNIT_ASSERT( t == IOPin::SequenceType );

            IOPin::TypeList l;

            l = IOPin::decodeTypes( "   " );
            CPPUNIT_ASSERT( l.count() == 4 );
            CPPUNIT_ASSERT( l.contains( IOPin::BitType ) );
            CPPUNIT_ASSERT( l.contains( IOPin::NumberType ) );
            CPPUNIT_ASSERT( l.contains( IOPin::StringType ) );
            CPPUNIT_ASSERT( l.contains( IOPin::SequenceType ) );

            l = IOPin::decodeTypes( " bit, number, invalid   " );
            CPPUNIT_ASSERT( l.count() == 0 );

            l = IOPin::decodeTypes( " bit " );
            CPPUNIT_ASSERT( l.count() == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::BitType ) );

            l = IOPin::decodeTypes( " string , bit " );
            CPPUNIT_ASSERT( l.count() == 2 );
            CPPUNIT_ASSERT( l.contains( IOPin::BitType ) );
            CPPUNIT_ASSERT( l.contains( IOPin::StringType ) );

            l = IOPin::decodeTypes( " number,string , bit " );
            CPPUNIT_ASSERT( l.count() == 3 );
            CPPUNIT_ASSERT( l.contains( IOPin::BitType ) );
            CPPUNIT_ASSERT( l.contains( IOPin::StringType ) );
            CPPUNIT_ASSERT( l.contains( IOPin::NumberType ) );

            l = IOPin::decodeTypes( " number,sequence , bit " );
            CPPUNIT_ASSERT( l.count() == 3 );
            CPPUNIT_ASSERT( l.contains( IOPin::BitType ) );
            CPPUNIT_ASSERT( l.contains( IOPin::SequenceType ) );
            CPPUNIT_ASSERT( l.contains( IOPin::NumberType ) );
        }

        void testVariable()
        {
            // dummy properties for new variable
            Property p1( "", "string" );
            Property p2( "", "integer" );
            PropertyDict pd;
            pd.insert( "p1", &p1 );
            pd.insert( "p2", &p2 );

            // some variables to test
            Variable vi( "  ", pd );
            Variable vv( " bIT ", pd );

            CPPUNIT_ASSERT( vi.type() == IOPin::InvalidType );
            CPPUNIT_ASSERT( vv.type() == IOPin::BitType );

            PropertyDict pdd;

            pdd = vi.properties();
            CPPUNIT_ASSERT( pdd.count() == 2 );
            CPPUNIT_ASSERT( pdd.contains( "p1" ) );
            CPPUNIT_ASSERT( pdd.contains( "p2" ) );

            pdd = vv.properties();
            CPPUNIT_ASSERT( pdd.count() == 2 );
            CPPUNIT_ASSERT( pdd.contains( "p1" ) );
            CPPUNIT_ASSERT( pdd.contains( "p2" ) );
        }

        void testConnection()
        {
            // dummy properties for new connection
            Property p1( "", "string", "P1" );
            Property p2( "", "integer", "2" );
            PropertyDict pd;
            pd.insert( "p1", &p1 );
            pd.insert( "p2", &p2 );

            // dummy variables for new connection
            Variable * v1 = new Variable( "bit", PropertyDict() );
            Variable * v2 = new Variable( "bit", PropertyDict() );
            Variable * v3 = new Variable( "bit", PropertyDict() );

            Connection c( "my_connection", pd );

            CPPUNIT_ASSERT( c.type() == "my_connection" );
            CPPUNIT_ASSERT( c.var_set_t() != NULL );
            CPPUNIT_ASSERT( c.variables().count() == 0 );

            PropertyDict& pdd = c.properties();

            CPPUNIT_ASSERT( pdd.count() == 2 );
            CPPUNIT_ASSERT( pdd.contains( "p1" ) );
            CPPUNIT_ASSERT( pdd.contains( "p2" ) );

            // set property check
            CPPUNIT_ASSERT( pdd["p1"]->asString() == "P1" );
            pdd["p1"]->decodeValue("X1");
            CPPUNIT_ASSERT( c.properties()["p1"]->asString() == "X1" );

            c.addVariable( "v1", v1 );
            c.addVariable( "v2", v2 );
            c.addVariable( "v3", v3 );

            VariableDict& vd = c.variables();

            CPPUNIT_ASSERT( vd.count() == 3 );
            CPPUNIT_ASSERT( vd.contains( "v1" ) );
            CPPUNIT_ASSERT( vd.contains( "v2" ) );
            CPPUNIT_ASSERT( vd.contains( "v3" ) );
            CPPUNIT_ASSERT( ! vd.contains( "v4" ) );

            CPPUNIT_ASSERT( c.variable( "v1" ) != NULL );
            CPPUNIT_ASSERT( c.variable( "v2" ) != NULL );
            CPPUNIT_ASSERT( c.variable( "v3" ) != NULL );
            CPPUNIT_ASSERT( c.variable( "v4" ) == NULL );

            c.removeVariable( "v1" );

            CPPUNIT_ASSERT( vd.count() == 2 );
            CPPUNIT_ASSERT( ! vd.contains( "v1" ) );
            CPPUNIT_ASSERT( vd.contains( "v2" ) );
            CPPUNIT_ASSERT( vd.contains( "v3" ) );
            CPPUNIT_ASSERT( ! vd.contains( "v4" ) );

            CPPUNIT_ASSERT( c.variable( "v1" ) == NULL );
            CPPUNIT_ASSERT( c.variable( "v2" ) != NULL );
            CPPUNIT_ASSERT( c.variable( "v3" ) != NULL );
            CPPUNIT_ASSERT( c.variable( "v4" ) == NULL );

            c.addVariable( "v1", v1 );

            CPPUNIT_ASSERT( vd.count() == 3 );
            CPPUNIT_ASSERT( vd.contains( "v1" ) );
            CPPUNIT_ASSERT( vd.contains( "v2" ) );
            CPPUNIT_ASSERT( vd.contains( "v3" ) );
            CPPUNIT_ASSERT( ! vd.contains( "v4" ) );

            CPPUNIT_ASSERT( c.variable( "v1" ) != NULL );
            CPPUNIT_ASSERT( c.variable( "v2" ) != NULL );
            CPPUNIT_ASSERT( c.variable( "v3" ) != NULL );
            CPPUNIT_ASSERT( c.variable( "v4" ) == NULL );

            lt_var_set_t * vset = c.var_set_t();

            CPPUNIT_ASSERT( vset != NULL );

            apr_hash_t * vars = lt_var_set_variables_get( vset );

            CPPUNIT_ASSERT( vars != NULL );

            CPPUNIT_ASSERT( apr_hash_get( vars, "v1", APR_HASH_KEY_STRING )
                    == v1->var_t() );
            CPPUNIT_ASSERT( apr_hash_get( vars, "v2", APR_HASH_KEY_STRING )
                    == v2->var_t() );
            CPPUNIT_ASSERT( apr_hash_get( vars, "v3", APR_HASH_KEY_STRING )
                    == v3->var_t() );
            CPPUNIT_ASSERT( apr_hash_get( vars, "v4", APR_HASH_KEY_STRING )
                    == NULL );
        }

        void testIOPin()
        {
            // let's use NULL instead of real parent to verify the basic
            // functionality
            IOPin io1( NULL, "any pin", "     " );
            IOPin invalid( NULL, "invalid pin", "whatever,something_else," );
            IOPin iob( NULL, "bit pin", "bit" );
            IOPin ion( NULL, "number pin", "number" );
            IOPin ios( NULL, "string pin", "string" );
            IOPin iosq( NULL, "sequence pin", "sequence" );

            IOPin ioml( NULL, "another any pin", " bit, string, sequence" );

            IOPin::TypeList l;

            // empty should support all types
            l = io1.types();
            CPPUNIT_ASSERT( io1.isValid() );
            CPPUNIT_ASSERT( io1.label() == "any pin" );
            CPPUNIT_ASSERT( l.count() == 4 );
            CPPUNIT_ASSERT( l.contains( IOPin::BitType ) == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::NumberType ) == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::StringType ) == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::SequenceType ) == 1 );
            CPPUNIT_ASSERT( io1.realValueType() == IOPin::BitType );
            CPPUNIT_ASSERT( io1.requestedValueType() == IOPin::BitType );
            CPPUNIT_ASSERT( io1.realBitValue() == 0 );
            CPPUNIT_ASSERT( io1.requestedBitValue() == 0 );

            // invalid is not valid, but its value is null
            CPPUNIT_ASSERT( ! invalid.isValid() );
            CPPUNIT_ASSERT( invalid.label() == "invalid pin" );
            CPPUNIT_ASSERT( invalid.realValueType() == IOPin::NullType );
            CPPUNIT_ASSERT( invalid.requestedValueType()
                    == IOPin::NullType );

            // bit should support only bit
            l = iob.types();
            CPPUNIT_ASSERT( iob.isValid() );
            CPPUNIT_ASSERT( iob.label() = "bit pin" );
            CPPUNIT_ASSERT( l.count() == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::BitType ) == 1 );
            CPPUNIT_ASSERT( iob.realValueType() == IOPin::BitType );
            CPPUNIT_ASSERT( iob.requestedValueType() == IOPin::BitType );
            CPPUNIT_ASSERT( iob.realBitValue() == 0 );
            CPPUNIT_ASSERT( iob.requestedBitValue() == 0 );

            // number should support only number
            l = ion.types();
            CPPUNIT_ASSERT( ion.isValid() );
            CPPUNIT_ASSERT( ion.label() = "number pin" );
            CPPUNIT_ASSERT( l.count() == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::NumberType ) == 1 );
            CPPUNIT_ASSERT( ion.realValueType() == IOPin::NumberType );
            CPPUNIT_ASSERT( ion.requestedValueType() == IOPin::NumberType );
            CPPUNIT_ASSERT( ion.realNumberValue() == 0 );
            CPPUNIT_ASSERT( ion.requestedNumberValue() == 0 );

            // string should support only string
            l = ios.types();
            CPPUNIT_ASSERT( ios.isValid() );
            CPPUNIT_ASSERT( ios.label() = "string pin" );
            CPPUNIT_ASSERT( l.count() == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::StringType ) == 1 );
            CPPUNIT_ASSERT( ios.realValueType() == IOPin::StringType );
            CPPUNIT_ASSERT( ios.requestedValueType() == IOPin::StringType );
            CPPUNIT_ASSERT( ios.realStringValue() == "" );
            CPPUNIT_ASSERT( ios.requestedStringValue() == "" );

            // sequence should support only sequence, but we don't handle
            // them yet, thus values will be null
            l = iosq.types();
            CPPUNIT_ASSERT( iosq.isValid() );
            CPPUNIT_ASSERT( iosq.label() = "sequence pin" );
            CPPUNIT_ASSERT( l.count() == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::SequenceType ) == 1 );
            CPPUNIT_ASSERT( invalid.realValueType() == IOPin::NullType );
            CPPUNIT_ASSERT( invalid.requestedValueType()
                    == IOPin::NullType );

            // ml should support defined types
            l = ioml.types();
            CPPUNIT_ASSERT( ioml.isValid() );
            CPPUNIT_ASSERT( ioml.label() = "another any pin" );
            CPPUNIT_ASSERT( l.count() == 3 );
            CPPUNIT_ASSERT( l.contains( IOPin::BitType ) == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::StringType ) == 1 );
            CPPUNIT_ASSERT( l.contains( IOPin::SequenceType ) == 1 );
            CPPUNIT_ASSERT( ioml.realValueType() == IOPin::BitType );
            CPPUNIT_ASSERT( ioml.requestedValueType() == IOPin::BitType );
            CPPUNIT_ASSERT( ioml.realBitValue() == 0 );
            CPPUNIT_ASSERT( ioml.requestedBitValue() == 0 );
        }

        void testIOPinBindings()
        {
            DummyTemplate t1, t2;

            Connection cB( "dummy bit connection", PropertyDict() );
            Connection cNS( "dummy number/string connection", PropertyDict() );

            Variable * vB = new Variable( "bit", PropertyDict() );
            Variable * vS = new Variable( "string", PropertyDict() );
            Variable * vN = new Variable( "number", PropertyDict() );

            cB.addVariable( "bit_var", vB );
            cNS.addVariable( "num_var", vN );
            cNS.addVariable( "str_var", vS );

            CPPUNIT_ASSERT( t1.notified == false );
            CPPUNIT_ASSERT( t2.notified == false );

            // now setup several bindings from the iopin side
            t1.iopins()[ "input" ]->bindToVariable( vB );
            t2.iopins()[ "input" ]->bindToVariable( vB );

            t1.iopins()[ "output" ]->bindToVariable( vN );
            t2.iopins()[ "output" ]->bindToVariable( vS );

            // check the bindings
            CPPUNIT_ASSERT( vB->connectedIOPins().contains(
                        t1.iopins()[ "input" ] ) );
            CPPUNIT_ASSERT( vB->connectedIOPins().contains(
                        t2.iopins()[ "input" ] ) );
            CPPUNIT_ASSERT( vN->connectedIOPins().contains(
                        t1.iopins()[ "output" ] ) );
            CPPUNIT_ASSERT( vS->connectedIOPins().contains(
                        t2.iopins()[ "output" ] ) );

            // now test whether the bindings had been set up properly
            CPPUNIT_ASSERT( t1.iopins()[ "input" ]->boundTo() == vB );
            CPPUNIT_ASSERT( t2.iopins()[ "input" ]->boundTo() == vB );
            CPPUNIT_ASSERT( t1.iopins()[ "output" ]->boundTo() == vN );
            CPPUNIT_ASSERT( t2.iopins()[ "output" ]->boundTo() == vS );

            // now test propagation from the variables to the iopins
            // warning: this lt_io_*_create is creating memory leak here,
            // because created value is immediatelly deepcopied and the
            // origin stays in the space,
            // but for a testcase we can live with it
            lt_var_real_value_set( vB->var_t(),
                    lt_io_value_bit_create( 0, NULL ) );
            lt_var_real_value_set( vB->var_t(),
                    lt_io_value_bit_create( 1, NULL ) );

            lt_var_real_value_set( vN->var_t(),
                    lt_io_value_number_create( +15, NULL ) );
            lt_var_real_value_set( vN->var_t(),
                    lt_io_value_number_create( -95, NULL ) );

            lt_var_real_value_set( vS->var_t(),
                    lt_io_value_string_create2(
                        ( const uint8_t * )"ahoij", NULL ) );
            lt_var_real_value_set( vS->var_t(),
                    lt_io_value_string_create2(
                        ( const uint8_t * )"kacka", NULL ) );

            cB.syncRealValues();
            cNS.syncRealValues();

            CPPUNIT_ASSERT( t1.iopins()[ "input" ]->realBitValue() == 1 );
            CPPUNIT_ASSERT( t1.iopins()[ "input" ]->isRealValueDirty() == 1 );
            CPPUNIT_ASSERT( t2.iopins()[ "input" ]->realBitValue() == 1 );
            CPPUNIT_ASSERT( t2.iopins()[ "input" ]->isRealValueDirty() == 1 );

            CPPUNIT_ASSERT( t1.iopins()[ "output" ]->realNumberValue()
                    == -95 );
            CPPUNIT_ASSERT( t1.iopins()[ "output" ]->isRealValueDirty() == 1 );
            CPPUNIT_ASSERT( t2.iopins()[ "output" ]->realStringValue()
                    == "kacka" );
            CPPUNIT_ASSERT( t2.iopins()[ "output" ]->isRealValueDirty() == 1 );

            CPPUNIT_ASSERT( t1.notified == true );
            CPPUNIT_ASSERT( t2.notified == true );

            t1.syncIOPinsRealValues();
            CPPUNIT_ASSERT( t1.iopins()[ "input" ]->isRealValueDirty() == 0 );
            CPPUNIT_ASSERT( t1.iopins()[ "output" ]->isRealValueDirty() == 0 );

            t2.syncIOPinsRealValues();
            CPPUNIT_ASSERT( t2.iopins()[ "input" ]->isRealValueDirty() == 0 );
            CPPUNIT_ASSERT( t2.iopins()[ "output" ]->isRealValueDirty() == 0 );

            // now test propagation from the iopins to the variables
            t1.iopins()[ "input" ]->setRequestedBitValue( 1 );
            t1.iopins()[ "input" ]->setRequestedBitValue( 0 );
            t1.iopins()[ "output" ]->setRequestedNumberValue( 100 );
            t1.syncIOPinsRequestedValues();

            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( vN->var_t() ) ) == 100 );

            CPPUNIT_ASSERT( lt_io_value_bit_get(
                        lt_var_requested_value_get( vB->var_t() ) ) == 0 );

            t2.iopins()[ "output" ]->setRequestedStringValue( "test1" );
            t2.syncIOPinsRequestedValues();

            const lt_io_value_t * val = lt_var_requested_value_get(
                    vS->var_t() );
            const char * str = ( const char * )lt_io_value_string_get( val );

            CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_STRING( val ) );
            CPPUNIT_ASSERT( str != NULL );

            CPPUNIT_ASSERT( strcmp( "test1", str ) == 0 );
        }

        void testTemplateShortcutNLibrary()
        {
            DummyTemplate t1;

            CPPUNIT_ASSERT( t1.shortcut() == QKeySequence() );

            t1.setShortcut( QKeySequence( Qt::Key_F11 ) );

            CPPUNIT_ASSERT( t1.shortcut() == QKeySequence( Qt::Key_F11 ) );

            t1.setShortcut( QKeySequence() );

            CPPUNIT_ASSERT( t1.shortcut() == QKeySequence() );

            CPPUNIT_ASSERT( t1.library().isEmpty() );

            t1.setLibrary( "xxx" );

            CPPUNIT_ASSERT( t1.library() == "xxx" );
        }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestClasses );
// end of custom tests

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestClasses.cpp,v 1.18 2004/12/02 11:54:21 mman Exp $
