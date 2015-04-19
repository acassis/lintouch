// $Id: RenameResourceCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenameResourceCommand.cpp --
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 11th Aug 2004
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

#include <qapplication.h>

#include "RenameResourceCommand.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"
#include <EditorProject.hh>

struct RenameResourceCommand::RenameResourceCommandPrivate{
        RenameResourceCommandPrivate( const QString& on,
        const QString& nn):oldName(on), newName(nn){}

    ~RenameResourceCommandPrivate(){}

    /**
     * Old resource name.
     */
    QString oldName;

    /**
     * New resource name.
     */
    QString newName;

    /**
     * True on redo action, false on undo action.
     */
    bool redo;
};

RenameResourceCommand::RenameResourceCommand(
        const QString& oldName, const QString& newName,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw,
            qApp->translate("RenameResourceCommand", "Rename Resource") ),
    d( new RenameResourceCommandPrivate( oldName, newName ) ){
        Q_CHECK_PTR( d );
        Q_ASSERT( d->oldName.isNull() == false );
        Q_ASSERT( d->newName.isNull() == false );
}

RenameResourceCommand::~RenameResourceCommand()
{
    delete d;
}

CommandType RenameResourceCommand::rtti() const
{
    return LTE_CMD_RESOURCE_RENAME;
}

void RenameResourceCommand::undo()
{
    Q_ASSERT( d );
    Q_ASSERT( d->oldName.isNull() == false );
    Q_ASSERT( d->newName.isNull() == false );
    d->redo = false;

    Q_ASSERT( m_pw->project()->renameResource( d->newName, d->oldName ) );

    const ViewMap& vm = m_pw->project()->views();

    for ( unsigned iv = 0; iv < vm.count(); iv++ ){
        const View& v = vm[ iv ];
        const PanelMap& pm = v.panels();

        for ( unsigned ip = 0; ip < pm.count(); ip++ ){
            const Panel& p = pm[ ip ];
            TemplateDict td = p.templates();

            for( unsigned it = 0; it < td.count(); it++ ) {
                Template* t = td[ it ];
                Q_ASSERT( t );
                PropertyDict pd = t->properties();

                for( unsigned itp = 0; itp < pd.count(); itp++ ){
                    Property* tp = pd[ itp ];
                    Q_ASSERT( tp );

                    if ( tp->type() == Property::ResourceType
                        && tp->asResource().stripWhiteSpace().
                        compare( d->newName.stripWhiteSpace() ) == 0 ){
                        tp->setResourceValue( d->oldName );
                        t->propertiesChanged();
                    }
                }
            }
        }
    }

    notifyMainWindow( LTE_CMD_RESOURCE_RENAME );
}

void RenameResourceCommand::redo()
{
    Q_ASSERT( d );
    Q_ASSERT( d->oldName.isNull() == false );
    Q_ASSERT( d->newName.isNull() == false );
    d->redo = true;

    Q_ASSERT( m_pw->project()->renameResource( d->oldName, d->newName ) );

    const ViewMap& vm = m_pw->project()->views();

    for ( unsigned iv = 0; iv < vm.count(); iv++ ){

        const View& v = vm[ iv ];
        const PanelMap& pm = v.panels();

        for ( unsigned ip = 0; ip < pm.count(); ip++ ){
            const Panel& p = pm[ ip ];
            TemplateDict td = p.templates();

            for( unsigned it = 0; it < td.count(); it++ ) {
                Template* t = td[ it ];
                Q_ASSERT( t );

                PropertyDict pd = t->properties();

                for( unsigned itp = 0; itp < pd.count(); itp++ ){
                    Property* tp = pd[ itp ];
                    Q_ASSERT( tp );

                    if ( tp->type() == Property::ResourceType
                        && tp->asResource().stripWhiteSpace().
                        compare( d->oldName.stripWhiteSpace() ) == 0 ){
                        tp->setResourceValue( d->newName );
                        t->propertiesChanged();
                    }
                }
            }
        }
    }
    notifyMainWindow( LTE_CMD_RESOURCE_RENAME );
}

QString RenameResourceCommand::oldResourceName() const
{
    if ( d->redo ){
        return d->oldName;
    }
    return d->newName;
}

QString RenameResourceCommand::newResourceName() const
{
    if ( d->redo ){
        return d->newName;
    }
    return d->oldName;
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameResourceCommand.cpp 1169 2005-12-12 15:22:15Z modesto $

