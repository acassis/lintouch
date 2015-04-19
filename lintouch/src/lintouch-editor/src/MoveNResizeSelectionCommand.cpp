// $Id: MoveNResizeSelectionCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: MoveNResizeSelectionCommand.cpp -- 
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

#include <qapplication.h>

#include "MoveNResizeSelectionCommand.hh"

#include "CommandType.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "EditablePanelViewContainer.hh"

struct
MoveNResizeSelectionCommand::MoveNResizeSelectionCommandPrivate 
{

    /**
     * Panel name contaning the templates to be moved.
     */
    QString view;

    /**
     * Panel name contaning the templates to be moved.
     */
    QString panel;

    /**
     * The templates to be moved/resized.
     */
    TemplateDict templates;

    /**
     * The templates that were selected.
     */
    TemplateDict selectedTemplates;

    /**
     * Original coordinates (before redo was applied)
     */
    LTMap <QRect> originalRects;

    /**
     * Original layers (before redo was applied)
     */
    LTMap <double> originalLayers;

    /**
     * New coordinates (after redo will be applied)
     */
    LTMap <QRect> newRects;

    /**
     * New layers (after redo will be applied)
     */
    LTMap <double> newLayers;
};

MoveNResizeSelectionCommand::MoveNResizeSelectionCommand(
        TemplateDict templates,
        LTMap <QRect> sizes,
        LTMap <double> layers,
        ProjectWindow * pw,
        MainWindow * mw ) :
    Command( pw, mw, qApp->translate("MoveNResizeSelectionCommand",
                "Move/Resize Selection")),
    d( new MoveNResizeSelectionCommandPrivate() )
{
    Q_CHECK_PTR(d);

    EditablePanelView * pv = pw->m_viewContainer->currentPanelView();

    // remember selection
    d->selectedTemplates = pv->currentSelection();

    // remember templates to be resized
    d->templates = templates;
    // remember original rectangles
    d->originalRects = rects2Map( templates );
    d->originalLayers = layers2Map( templates );
    // remember new rectangles
    d->newRects = sizes;
    d->newLayers = layers;

    // remember the current panel name
    d->panel = pw->currentPanel();
    // remember the current view name
    d->view = pw->currentView();
}

MoveNResizeSelectionCommand::MoveNResizeSelectionCommand(
        LTMap <QRect> sizes,
        TemplateDict templates,
        ProjectWindow * pw,
        MainWindow * mw ) :
    Command( pw, mw, qApp->translate("MoveNResizeSelectionCommand",
                "Move/Resize Selection") ),
    d( new MoveNResizeSelectionCommandPrivate() )
{
    Q_CHECK_PTR(d);

    EditablePanelView * pv = pw->m_viewContainer->currentPanelView();

    // remember selection
    d->selectedTemplates = pv->currentSelection();

    // remember templates to be resized
    d->templates = templates;
    // remember original rectangles
    d->originalRects = sizes;
    d->originalLayers = layers2Map( templates );
    // remember new rectangles
    d->newRects = rects2Map( templates );
    d->newLayers = d->originalLayers;

    // remember the current panel name
    d->panel = pw->currentPanel();
    // remember the current view name
    d->view = pw->currentView();
}

MoveNResizeSelectionCommand::~MoveNResizeSelectionCommand()
{
    delete d;
}

CommandType MoveNResizeSelectionCommand::rtti() const
{
    return LTE_CMD_SELECTION_MOVE_RESIZE;
}

void MoveNResizeSelectionCommand::redo()
{
    Q_ASSERT(d);

    // qWarning( "MoveNResizeSelectionCommand::redo" );

    // we are possibly moving as a result of undo/redo while displaying some
    // other view/panel ---> panelView could be NULL
    // EditablePanelView * panelView =
    //     m_pw->m_viewContainer->panelView( d->panel );

    for( unsigned i = 0; i < d->templates.count(); i ++ ) {
        //qWarning( "moving template " + d->templates.keyAt( i ) );
        d->templates[ i ]->setVirtualRect( d->newRects[ i ] );
        d->templates[ i ]->setZ( d->newLayers[ i ] );
    }

    // notify all
    notifyMainWindow( LTE_CMD_SELECTION_MOVE_RESIZE );
}

void MoveNResizeSelectionCommand::undo()
{
    Q_ASSERT(d);

    // qWarning( "MoveNResizeSelectionCommand::undo" );

    // we are possibly moving as a result of undo/redo while displaying some
    // other view/panel ---> panelView could be NULL
    // EditablePanelView * panelView =
    //     m_pw->m_viewContainer->panelView( d->panel );

    for( unsigned i = 0; i < d->templates.count(); i ++ ) {
        d->templates[ i ]->setVirtualRect( d->originalRects[ i ] );
        d->templates[ i ]->setZ( d->originalLayers[ i ] );
    }

    // notify all
    notifyMainWindow( LTE_CMD_SELECTION_MOVE_RESIZE );
}

const TemplateDict & MoveNResizeSelectionCommand::templates() const
{
    Q_ASSERT(d);
    return d->templates;
}

const TemplateDict &
MoveNResizeSelectionCommand::selectedTemplates() const
{
    Q_ASSERT(d);
    return d->selectedTemplates;
}

QString MoveNResizeSelectionCommand::panelName() const
{
    Q_ASSERT(d);
    return d->panel;
}

QString MoveNResizeSelectionCommand::viewName() const
{
    Q_ASSERT(d);
    return d->view;
}

LTMap <QRect> MoveNResizeSelectionCommand::rects2Map(
        const TemplateDict & templates )
{
    LTMap <QRect> result;

    for( unsigned i = 0; i < templates.count(); i++ ) {
        result.insert( templates.keyAt( i ), templates[ i ]->virtualRect() );
    }

    return result;
}

LTMap <double> MoveNResizeSelectionCommand::layers2Map(
        const TemplateDict & templates )
{
    LTMap <double> result;

    for( unsigned i = 0; i < templates.count(); i++ ) {
        result.insert( templates.keyAt( i ), templates[ i ]->z() );
    }

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: MoveNResizeSelectionCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
