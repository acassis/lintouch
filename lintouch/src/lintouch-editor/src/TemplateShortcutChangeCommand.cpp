// $Id: TemplateShortcutChangeCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateShortcutChangeCommand.cpp -- 
// AUTHOR: abdul < henry@swac.cz>
//   DATE: 4th September 2004
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

#include "TemplateShortcutChangeCommand.hh"

#include "CommandType.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include <EditorProject.hh>

using namespace lt;

struct TemplateShortcutChangeCommand::
        TemplateShortcutChangeCommandPrivate {
    TemplateShortcutChangeCommandPrivate() :
    selectedTemplate( NULL ), stolenFromTemplate( NULL )
    {
    }
    /**
     * This points to the selected Template
     */
    Template* selectedTemplate;

    /**
     * This points to the Template( if any ) from which the short
     * was stolen
     */
    Template* stolenFromTemplate;

    /**
    * This stores the new shortcut 
    */
     QString newShortCut;

    /**
     * This stores the old shortcut
     */
     QString oldShortCut;

    /**
     * This holds the panel name of the template
     */
    QString panelName;

    /**
     * This holds the view name of the template
     */
    QString viewName;

    /**
     * This holds the name of the current template name
     */
    QString selectedTemplateName;

    /**
     * This holds the name of the template whose shortcut got stollen
     */
    QString stolenFromTemplateName;

    /**
     * True on redo action, false on undo action.
     */
    bool redo;

};

TemplateShortcutChangeCommand::
        TemplateShortcutChangeCommand (
                const QString& selectedTemplate, const QString& shortCut,
                const QString& stolenFromTemplate,
                ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw,
            qApp->translate("TemplateShortcutChangeCommand",
                "Change Template Shortcut") ),
    d( new TemplateShortcutChangeCommandPrivate )
{
    Q_CHECK_PTR(d);
    Q_ASSERT( !selectedTemplate.isEmpty() );

    d->viewName = pw->currentView();
    d->panelName = pw->currentPanel();

    View view =  pw->project()->views()[ d->viewName ];

    Panel panel = view.panels()[ d->panelName ];

    d->selectedTemplate = panel.templates()[ selectedTemplate ];
    Q_ASSERT( d->selectedTemplate );
    d->selectedTemplateName = selectedTemplate;

    if( !stolenFromTemplate.isEmpty() ) {
        d->stolenFromTemplate = 
            panel.templates()[ stolenFromTemplate ];
    }
   
    d->newShortCut = shortCut;
    d->oldShortCut = ( QString ) d->selectedTemplate->shortcut();
    d->stolenFromTemplateName = stolenFromTemplate.stripWhiteSpace();
}

TemplateShortcutChangeCommand::
        ~TemplateShortcutChangeCommand()
{
    delete d;
}

CommandType TemplateShortcutChangeCommand::rtti() const
{
    return LTE_CMD_TEMPLATE_SHORTCUT_CHANGE;
}

void TemplateShortcutChangeCommand::redo()
{
    Q_ASSERT( d );
    Q_ASSERT( d->selectedTemplate );

    d->redo = true;

    QString oldShortCut = ( QString ) d->selectedTemplate->shortcut();
    Q_ASSERT( oldShortCut == d->oldShortCut );

    if( d->stolenFromTemplate ) {
        QString stolenShortCut = 
                ( QString ) d->stolenFromTemplate->shortcut();
        Q_ASSERT( stolenShortCut == d->newShortCut );

        d->stolenFromTemplate->setShortcut( QKeySequence() );
    }

    d->selectedTemplate->setShortcut( QKeySequence( d->newShortCut ) );

    notifyMainWindow( LTE_CMD_TEMPLATE_SHORTCUT_CHANGE );
}

void TemplateShortcutChangeCommand::undo()
{
    Q_ASSERT( d );
    Q_ASSERT( d->selectedTemplate );

    d->redo = false;

    QString newShortCut = ( QString ) d->selectedTemplate->shortcut();
    Q_ASSERT( newShortCut == d->newShortCut );

    if( d->stolenFromTemplate ) {
        d->stolenFromTemplate->setShortcut( QKeySequence( d->newShortCut ) );
    }
                                                                                                                             
    d->selectedTemplate->setShortcut( QKeySequence( d->oldShortCut ) );

    notifyMainWindow( LTE_CMD_TEMPLATE_SHORTCUT_CHANGE );
}

QString TemplateShortcutChangeCommand::panelName()
{
    Q_ASSERT( d );
    return d->panelName;
}

QString TemplateShortcutChangeCommand::viewName()
{
    Q_ASSERT( d );
    return d->viewName;
}

QString TemplateShortcutChangeCommand::currentTemplateName()
{
    Q_ASSERT( d );
    return d->selectedTemplateName; 
}

QString TemplateShortcutChangeCommand::stolenFromTemplateName()
{
    Q_ASSERT( d );
    return d->stolenFromTemplateName;
}

QString TemplateShortcutChangeCommand::oldShortCutKey()
{
    Q_ASSERT(d);
    if( d->redo == true ) {
        return d->oldShortCut;
    } else {
        return d->newShortCut;
    }
}

QString TemplateShortcutChangeCommand::newShortCutKey()
{
    Q_ASSERT(d);
    if( d->redo == false ) {
        return d->oldShortCut;
    } else {
        return d->newShortCut;
    }
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateShortcutChangeCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
