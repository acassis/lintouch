// $Id: TemplatePalette.cpp,v 1.3 2004/08/15 18:17:59 mman Exp $
//
//   FILE: TemplatePalette.cpp -- 
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 30 October 2003
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

#include <qlistbox.h>

#include <lt/ui/TemplatePalette.hh>
using namespace lt;

//debug only
#include <qpixmap.h>

struct TemplatePalette::TemplatePalettePrivate
{
    // map used to associate ListBoxItems with templates
    QMap <QListBoxItem*, TemplateLibrary*> lib4key;
};

TemplatePalette::TemplatePalette(QWidget * parent /* = NULL */,
    const char* name /* = NULL */, WFlags f /* = 0 */)
    : QToolBox(parent, name, f), d( new TemplatePalettePrivate() )
{
}

bool TemplatePalette::addTemplateLibrary( TemplateLibrary * lib )
{
    if( lib == NULL ) {
        return false;
    }

    QString label = lib->info().shortDescription();

    //fight the duplicates
    for(int i = 0; i < count(); i++)
        if(itemLabel(i) == label)
            return false;

    QListBox *lb = new QListBox(this);
    const InfoMap templates = lib->templates();
    // fill in listbox
    for( unsigned i = 0; i < templates.count(); i++ ) {

        // item is inserted automatically when listbox is the parent
        QListBoxPixmap * item = 
            new QListBoxPixmap( lb, templates[ i ].icon(), templates.keyAt( i ) );

        // associate listbox item with template
        d->lib4key.insert( item, lib );
    }

    // add listbox to the palette
    addItem(lb, label);

    // connect listbox::selected to our private slot
    connect( lb, SIGNAL( selected( QListBoxItem * ) ),
            this, SLOT( lbItemSelected( QListBoxItem * ) ) );

    return true;
}

void TemplatePalette::lbItemSelected( QListBoxItem * item )
{
    if( d->lib4key.contains( item ) ) {
        //FIXME: here we need access to TemplateManager, for now we simply
        //don't have it but we will in the future
        Template * t = d->lib4key [ item ]->instantiate( item->text() );
        if ( t ) {
            t->compose( TemplateManager() );
            emit( placeTemplate( t ) );
        }
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplatePalette.cpp,v 1.3 2004/08/15 18:17:59 mman Exp $
