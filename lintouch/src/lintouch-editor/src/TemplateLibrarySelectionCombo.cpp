// $Id: TemplateLibrarySelectionCombo.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateLibrarySelectionCombo.cpp --
// AUTHOR: shivaji <shivaji@swac.cz>
//   DATE: 26 July 2004
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

#include <qwidget.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qptrlist.h>

#include <lt/templates/Info.hh>
#include <lt/templates/TemplateLibrary.hh>

#include "TemplateLibrarySelectionCombo.hh"

using namespace lt;

struct TemplateLibrarySelectionCombo::
    TemplateLibrarySelectionComboPrivate{

    TemplateLibrarySelectionComboPrivate(){
    }

    ~TemplateLibrarySelectionComboPrivate(){
    }

    //Map and map iterator for mapping library names to its key value
    typedef QMap< QString, QString > libraryKeyMap;
    typedef QMapIterator< QString, QString > libraryKeyMapIterator;
    libraryKeyMap   libraryKeys;    
};

TemplateLibrarySelectionCombo::
        TemplateLibrarySelectionCombo(
        const lt::TemplateManager &tm, QWidget * parent /*=0*/,
        const char * name /*=0*/ ):QComboBox( true, parent, name ),
        d( new TemplateLibrarySelectionComboPrivate() ) {

    //it is not editable
    setEditable( false );

    setLoadedLibrariesFromTemplateManager( tm );

    connect( this, SIGNAL( activated( const QString& ) ), this,
            SLOT( comboItemActivated( const QString& ) ) );
}

TemplateLibrarySelectionCombo::
    ~TemplateLibrarySelectionCombo(){
}

void TemplateLibrarySelectionCombo::
        setLoadedLibrariesFromTemplateManager(
        const lt::TemplateManager & tm ){

    //populate the combobox
    TemplateLibraryDict libraries = tm.templateLibraries();

    // TemplateLibraryDictIterator i( libraries );

    for (unsigned i=0 ; i<libraries.count(); ++i ) {
        if( ! libraries[i]->isValid() ) continue;
        if( ! libraries[i]->isAvailable() ) continue;
        if( libraries.keyAt( i ) == "mytemplatelibrary" ) continue;
        insertItem( libraries[i]->info().shortDescription() );
        d->libraryKeys[ libraries[i]->info().shortDescription() ] =
                libraries.keyAt(i);
    }
}

QSize TemplateLibrarySelectionCombo::sizeHint() const
{
    return QComboBox::sizeHint();
}

QSize TemplateLibrarySelectionCombo::minimumSizeHint() const
{
    return QComboBox::minimumSizeHint();
}

QSizePolicy TemplateLibrarySelectionCombo::sizePolicy() const
{
    return QComboBox::sizePolicy();
}

void TemplateLibrarySelectionCombo::setCurrentItem( int index )
{
    QComboBox::setCurrentItem( index );
    emit( activated( currentText() ) );
}

void TemplateLibrarySelectionCombo::setCurrentText( const QString&
        text )
{
    QComboBox::setCurrentText( text );
    emit( activated( text ) );
}

void TemplateLibrarySelectionCombo::comboItemActivated(
        const QString & libShortDesc ){

    TemplateLibrarySelectionComboPrivate::libraryKeyMapIterator i =
            d->libraryKeys.find( libShortDesc );

    if ( i != d->libraryKeys.end() ){
        emit( libraryChanged( i.data() ) );
    }
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateLibrarySelectionCombo.cpp 1169 2005-12-12 15:22:15Z modesto $

