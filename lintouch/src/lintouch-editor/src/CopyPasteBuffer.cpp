// $Id: CopyPasteBuffer.cpp 1193 2005-12-22 10:58:02Z modesto $
//
//   FILE: CopyPasteBuffer.cpp --
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 01 September 2004
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

#include "CopyPasteBuffer.hh"
#include "lib/Bindings.hh"

struct CopyPasteBuffer::CopyPasteBufferPrivate
{
    /**
     * The buffer.
     */
    TemplateDict buffer;

    /**
     * Bindings buffer.
     */
    BindingsList bl;

    /**
     * the number of paste operations
     */
    int pasted;
};

CopyPasteBuffer::CopyPasteBuffer(
        QObject* parent /* = 0 */, const char * name /* = 0 */ ) :
    QObject( parent, name ),
    d( new CopyPasteBufferPrivate() )
{
    d->buffer.setAutoDelete( true );
}

CopyPasteBuffer::~CopyPasteBuffer()
{
    clear();

    delete d;
}

void CopyPasteBuffer::copy( const TemplateDict & selection,
        const QString& view, const QString& panel, const EditorProject* ep )
{
    Q_ASSERT(ep);

    clear();

    for( unsigned i = 0; i < selection.count(); i ++ ) {
        d->buffer.insert( selection.keyAt( i ),
                selection[ i ]->clone( Localizator::nullPtr(),
                    TemplateManager(), Logger::nullPtr() ) );
        //qWarning( "CopyPasteBuffer::copy: made a copy of " +
        //        selection[ i ]->type() + " template with name " +
        //        selection.keyAt( i ) );

        //TODO: copy bingings
        BindingsList lbl = ep->bindings().bindingsByTemplate(view, panel,
                selection.keyAt(i));
        for(unsigned i = 0; i < lbl.count(); ++i) {
            d->bl.append(BindingPtr(lbl[i]->clone()));
        }
    }

    d->pasted = 0;
}

TemplateDict CopyPasteBuffer::paste( const EditorProject* ep,
        /* [OUTPUT] */ BindingsList& bl,
        LoggerPtr logger /* = Logger::nullPtr() */ )
{
    Q_ASSERT(ep);

    bl.clear();

    TemplateDict result;

    d->pasted++;
    int shift = 10 * d->pasted;
    Template * tt = NULL;
    QString tn;

    for( unsigned i = 0; i < d->buffer.count(); i ++ ) {
        tn = d->buffer.keyAt( i ) + " " + tr("copy %1");
        result.insert( tn, tt = d->buffer[ i ]->clone(
                    ep->localizator(), ep->templateManager(),
                    logger ) );

        QRect r = tt->virtualRect();
        r.moveLeft( r.left() + shift );
        r.moveRight( r.right() + shift );
        r.moveTop( r.top() + shift );
        r.moveBottom( r.bottom() + shift );
        tt->setVirtualRect( r );

        // prepare pasted bingins
        for(unsigned j = 0; j < d->bl.count(); ++j) {
            bl.append(BindingPtr(new Binding(
                    d->bl[j]->connection(),
                    d->bl[j]->variable(),
                    d->bl[j]->view(),
                    d->bl[j]->panel(),
                    tn,
                    d->bl[j]->iopin())));
        }
    }

    return result;
}

void CopyPasteBuffer::clear()
{
    d->buffer.clear();
    d->bl.clear();
}

bool CopyPasteBuffer::isEmpty() const
{
    return d->buffer.isEmpty();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: CopyPasteBuffer.cpp 1193 2005-12-22 10:58:02Z modesto $
