// $Id: DeleteResourceCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: DeleteResourceCommand.cpp --
// AUTHOR: shivaji <shivaji@swac.cz>
//   DATE: 10th Aug 2004
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

#include <lt/templates/Property.hh>
#include <lt/templates/Template.hh>
#include <lt/project/Panel.hh>
#include <lt/project/View.hh>

#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "DeleteResourceCommand.hh"

typedef struct TemplateLinked{

    TemplateLinked( const QString& vk, const QString& pk,
         const QString& tk, const QString& tpk ){
        view_key = vk;
        panel_key = pk;
        template_key = tk;
        property_key = tpk;
    }
    TemplateLinked(){
        view_key = "";
        panel_key = "";
        template_key = "";
        property_key = "";
    }

    QString view_key;
    QString panel_key;
    QString template_key;
    QString property_key;
};

typedef QValueList< TemplateLinked > TemplateLinkedToResource;

struct DeleteResourceCommand::DeleteResourceCommandPrivate{

    DeleteResourceCommandPrivate( const QString& k ):key(k),
            undoName(""){}
    ~DeleteResourceCommandPrivate(){}

    QString key;
    QString undoName;
    TemplateLinkedToResource templateLinkedToResource;
};

DeleteResourceCommand::DeleteResourceCommand(
        const QString& key, ProjectWindow* pw,
        MainWindow* mw ) : Command( pw, mw,
        qApp->translate("DeleteResourceCommand", "Delete Resource") ),
        d( new DeleteResourceCommandPrivate( key ) ){
    Q_CHECK_PTR( d );
}

DeleteResourceCommand::~DeleteResourceCommand()
{
    delete d;
}

CommandType DeleteResourceCommand::rtti() const
{
    return LTE_CMD_RESOURCE_DELETE;
}

void DeleteResourceCommand::undo()
{
    Q_ASSERT( d );
    Q_ASSERT( d->key.isNull() == false );
    Q_ASSERT( d->undoName.isNull() == false );

    Q_ASSERT( m_pw->project()->undeleteResource( d->key, d->undoName) );

    if ( !d->templateLinkedToResource.isEmpty() ){
         QValueListConstIterator <TemplateLinked> i =
                d->templateLinkedToResource.constBegin();

        while ( i != d->templateLinkedToResource.constEnd() ){
            const TemplateLinked& tl = *i;

            Q_ASSERT( !tl.view_key.isEmpty()
                    && !tl.panel_key.isEmpty()
                    && !tl.template_key.isEmpty() );

            Q_ASSERT( m_pw->project()->views().contains( tl.view_key ) );
            const View& v = m_pw->project()->views()[ tl.view_key ];

            Q_ASSERT( v.panels().contains( tl.panel_key ) );
            const Panel& p = v.panels()[ tl.panel_key ];

            TemplateDict td = p.templates();
            Q_ASSERT( td.contains( tl.template_key ) );
            Template* t = td[ tl.template_key ];
            Q_ASSERT( t );

            PropertyDict pd = t->properties();
            Q_ASSERT( pd.contains( tl.property_key ) );
            Property* tp = pd[ tl.property_key ];
            Q_ASSERT( tp );
                                                                                                                             
            tp->setResourceValue( d->key );
            t->propertiesChanged();

            i++;
        };
        d->templateLinkedToResource.clear();
    }
    notifyMainWindow( LTE_CMD_RESOURCE_ADD );
}

void DeleteResourceCommand::redo()
{
    Q_ASSERT( d );
    Q_ASSERT( d->key.isNull() == false );

    //delete the resource and store the undoName
    d->undoName = m_pw->project()->deleteResource( d->key );

    Q_ASSERT( d->undoName.isNull() == false );

    if ( !d->templateLinkedToResource.isEmpty() ){
        d->templateLinkedToResource.clear();
    }

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
                        compare( d->key.stripWhiteSpace() ) == 0 ){
                        tp->setResourceValue( "" );//reset
                        t->propertiesChanged();
                        d->templateLinkedToResource.append( TemplateLinked(
                                vm.keyAt( iv ), pm.keyAt( ip ),
                                td.keyAt( it ), pd.keyAt( itp ) ) );
/*
                        QString m = QString(
                                    "View : %1, Panel : %2, Template : %3" ).
                                    arg( vm.keyAt( iv ) ).
                                    arg( pm.keyAt( ip ) ).
                                    arg( td.keyAt( it ) );
                        qWarning( m ); 
*/
                    }
                }
            }
        }
    }
    notifyMainWindow( LTE_CMD_RESOURCE_DELETE );
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: DeleteResourceCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
