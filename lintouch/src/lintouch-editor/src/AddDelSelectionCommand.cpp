// $Id: AddDelSelectionCommand.cpp 1196 2006-01-03 12:18:25Z modesto $
//
//   FILE: AddDelSelectionCommand.cpp -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 02 September 2004
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
#include <qmap.h>

#include <lt/project/Project.hh>
#include <lt/project/Panel.hh>

#include "Binding.hh"
#include "BindingsHelper.hh"
#include "Bindings.hh"
#include "CommandType.hh"
#include "EditablePanelViewContainer.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "AddDelSelectionCommand.hh"

using namespace lt;

struct AddDelSelectionCommand::AddDelSelectionCommandPrivate
{

    /**
     * View name where we're doing modifications.
     */
    QString viewName;

    /**
     * Panel name where we're doing modifications.
     */
    QString panelName;

    /**
     * Panel we are working at.
     */
    Panel panel;

    /**
     * The templates to be added/deleted.
     */
    TemplateDict templates;

    /**
     * The templates that were selected before.
     */
    TemplateDict selectedTemplates;

    /**
     * Positions of templates before they were deleted.
     */
    LTMap <unsigned> templatePositions;

    /**
     * mode of this command
     */
    bool adding;

    /**
     * Bindings of deleted templates.
     */
    BindingsList bl;
};


AddDelSelectionCommand::AddDelSelectionCommand(
        const TemplateDict & templates,
        const BindingsList& bl,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw,
            qApp->translate("AddDelSelectionCommand", "Add Template(s)") ),
    d(new AddDelSelectionCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->viewName = pw->currentView();
    d->panelName = pw->currentPanel();
    d->panel = pw->project()->views()[ d->viewName ].panels()[ d->panelName ];

    // remember previous selection before adding some templates
    d->selectedTemplates =
        pw->m_viewContainer->currentPanelView()->currentSelection();

    //walk through the given templates and generate safe names for them
    for( unsigned i = 0; i < templates.count(); i ++ ) {
        QString tn = Command::generateSafeName( templates.keyAt( i ), 0,
                d->panel.templates().mapping() );
        d->templates.insert( tn, templates[ i ] );

        // fill the d->bl acording the paramter bl
        for(unsigned j=0; j<bl.count(); ++j) {
            if(bl[j]->templ() == templates.keyAt(i)) {
                d->bl.append(BindingPtr(new Binding(
                                bl[j]->connection(),
                                bl[j]->variable(),
                                d->viewName, // current view
                                d->panelName, // current panel
                                tn,
                                bl[j]->iopin())));
            }
        }
    }

    d->adding = true;
}

AddDelSelectionCommand::AddDelSelectionCommand(
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw,
            qApp->translate("AddDelSelectionCommand", "Delete Selection") ),
    d(new AddDelSelectionCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->viewName = pw->currentView();
    d->panelName = pw->currentPanel();
    d->panel = pw->project()->views()[ d->viewName ].panels()[ d->panelName ];

    d->templates =
        pw->m_viewContainer->currentPanelView()->currentSelection();

    // no selection after deleting some templates
    d->selectedTemplates = TemplateDict();

    d->adding = false;
}

AddDelSelectionCommand::~AddDelSelectionCommand()
{
    delete d;
}

CommandType AddDelSelectionCommand::rtti() const
{
    return LTE_CMD_SELECTION_ADD_DEL;
}

void AddDelSelectionCommand::redo()
{
    Q_ASSERT(d);

    CommandType t;

    // This is the only approved exception from the rule that no command can
    // access GUI related classes.
    // we are possibly adding/deleting as a result of undo/redo while
    // displaying some other view/panel ---> panelView could be NULL
    EditablePanelView * panelView = NULL;
    if(m_pw->currentView() == d->viewName) {
        // only look for panelView if the current view is the one we have
        // templates for
        panelView = m_pw->m_viewContainer->panelView( d->panelName );
    }

    Q_ASSERT( m_pw->project()->views().contains( d->viewName ) );
    Q_ASSERT( m_pw->project()->views()[ d->viewName ].panels()
            .contains( d->panelName ) );

    if( d->adding ) {
        // need to add templates to the end of the current panel.
        t = LTE_CMD_SELECTION_ADD;

        for( unsigned i = 0; i < d->templates.count(); i++ ) {
            d->panel.addTemplate( d->templates.keyAt( i ), d->templates[ i ] );
            if( panelView ) {
                panelView->addTemplate( d->templates.keyAt( i ) );
            }
        }
        if(!d->bl.isEmpty()) {
            // bind for pasted templates
            BindingsHelper::restoreBindingsAndBind(m_pw->project(), d->bl);
        }

    } else {
        // need to delete the templates while remembering their positions.
        t = LTE_CMD_SELECTION_DEL;

        QString key;

        // remember position of a template and delete it
        for( unsigned i = 0; i < d->templates.count(); i++ ) {
            key = d->templates.keyAt( i );
            int oldpos = d->panel.templates().indexOf(key);
            d->templatePositions.insert(key, oldpos );
            if( panelView ) {
                panelView->removeTemplate(key);
            }
            d->panel.removeTemplate(key);
        }

        // save Bindings
        d->bl = BindingsHelper::saveTemplatesBindingAndUnbind(m_pw->project(),
                d->viewName, d->panelName, d->templates);
    }

    // notify all
    notifyMainWindow( t );
}

void AddDelSelectionCommand::undo()
{
    Q_ASSERT(d);

    CommandType t;

    // This is the only approved exception from the rule that no command can
    // access GUI related classes.
    // we are possibly adding/deleting as a result of undo/redo while
    // displaying some other view/panel ---> panelView could be NULL
    EditablePanelView * panelView = NULL;
    if(m_pw->currentView() == d->viewName) {
        // only look for panelView if the current view is the one we have
        // templates for
        panelView = m_pw->m_viewContainer->panelView( d->panelName );
    }

    Q_ASSERT( m_pw->project()->views().contains( d->viewName ) );
    Q_ASSERT( m_pw->project()->views()[ d->viewName ].panels()
            .contains( d->panelName ) );

    if( d->adding ) {
        // need to delete them from the end
        for( unsigned i = 0; i < d->templates.count(); i++ ) {
            if( panelView ) {
                panelView->removeTemplate( d->templates.keyAt( i ) );
            }
            d->panel.removeTemplate( d->templates.keyAt( i ) );
        }

        // unbind Bindings
        BindingsHelper::saveTemplatesBindingAndUnbind(m_pw->project(),
                d->viewName, d->panelName, d->templates);

        t = LTE_CMD_SELECTION_DEL;
    } else {
        // need to add them to the remembered positions.
        t = LTE_CMD_SELECTION_ADD;

        // add from the end, so that the positions are correct
        int size = d->templates.count();
#define index (size - 1 - i)
        for( unsigned i = 0; i < d->templates.count(); i++ ) {
            int oldpos = d->templatePositions[ index ];
            //qWarning( "should add " + d->templates.keyAt( index ) +
            //        " to pos %d", oldpos );
            d->panel.addTemplate( d->templates.keyAt( index ),
                    d->templates[ index ] );
            d->panel.templates().move(
                    d->panel.templates().count() - 1, oldpos );
            if( panelView ) {
                panelView->addTemplate( d->templates.keyAt( index ) );
            }
        }

        // bind acording the d->bl
        BindingsHelper::restoreBindingsAndBind(m_pw->project(), d->bl);
        d->bl = BindingsList();
#undef index
    }

    // notify all
    notifyMainWindow( t );
}

QString AddDelSelectionCommand::viewName() const
{
    Q_ASSERT(d);
    return d->viewName;
}

QString AddDelSelectionCommand::panelName() const
{
    Q_ASSERT(d);
    return d->panelName;
}

const TemplateDict & AddDelSelectionCommand::templates() const
{
    Q_ASSERT(d);
    return d->templates;
}

const TemplateDict & AddDelSelectionCommand::selectedTemplates() const
{
    Q_ASSERT(d);
    return d->selectedTemplates;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelSelectionCommand.cpp 1196 2006-01-03 12:18:25Z modesto $
