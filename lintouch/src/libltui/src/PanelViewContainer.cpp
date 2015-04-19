// $Id: PanelViewContainer.cpp,v 1.16 2004/08/31 15:50:36 mman Exp $
//
//   FILE: PanelViewContainer.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 07 January 2004
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

#include "lt/ui/PanelViewContainer.hh"
#include "lt/ui/PanelView.hh"
using namespace lt;

struct PanelViewContainer::PanelViewContainerPrivate
{

    PanelViewContainerPrivate() :
        sp(NULL), lastPanel(NULL)
    {
        //automatically delete cached PanelViews
        panelViews.setAutoDelete( true );
    }

    //view we are displaying
    View view;
    //cached PanelViews
    PanelViewDict panelViews;
    //display History
    QValueList <QString> history;

    // ServicePanel
    QWidget* sp;
    // last visible panel
    QWidget* lastPanel;
};

PanelViewContainer::PanelViewContainer( QWidget * parent,
        const char * name, WFlags f ) :
    QWidgetStack( parent, name, f ),
    d( new PanelViewContainerPrivate )
{
}

PanelViewContainer::~PanelViewContainer()
{
    delete d;
}

View PanelViewContainer::projectView() const
{
    return d->view;
}

void PanelViewContainer::setProjectView( const View & pView,
        const QString & startPanel /* = QString::null */ )
{

    //first remove all PanelViews from the WidgetStack
    //qWarning( "going to clean up widget stact" );
    PanelViewDictIterator i( d->panelViews );
    for(; i.current(); ++i ) {
        //qWarning( "going to remove widget " + i.currentKey() );
        removeWidget( i.current() );
    }

    // add the ServicePanel if we have it
    if(d->sp) {
        addWidget(d->sp, 0);
    }
    d->lastPanel = NULL;

    //and then delete them while emptying the dictionary
    d->panelViews.clear();
    //also empty the history
    d->history.clear();

    //now set the view we will be using from now on
    d->view = pView;

    //if the view contains at least one panel, display the first one as the
    //default one
    if( d->view.panels().count() > 0 ) {

        QString showPanel = d->view.homePanelId();

        // when we should start on specific panel and view does have this
        // panel, we display it
        if( ! startPanel.isNull() && d->view.panels().contains( startPanel ) ) {
            showPanel = startPanel;
        }

        //build given panel, show it and emit appropriate signals
        buildPanelViewForPanelWithId( showPanel );

    } else {
        emit( panelClosingEnabled( false ) );
    }
}

void PanelViewContainer::displayPanel( const QString & id )
{

    //qWarning( "some nasty boy wants us to show panel with id " + id );

    // if we have it in a cache, simply show it
    if( d->panelViews.find( id ) ) {
        //qWarning( "and we have it in a cache" );
        raiseWidget( d->panelViews[ id ] );
        d->panelViews[ id ]->setFocus();

        emit( panelDisplayed( id ) );
        //update history
        d->history.remove( id );
        d->history.push_front( id );
        return;
    }

    // if we don't have it in a cache but it exists in a project, build it
    // and show it
    if( d->view.panels().contains( id ) ) {
        //qWarning( "we don't have it but we can build it" );
        buildPanelViewForPanelWithId( id );
        return;
    }

    //qWarning( "crap!!! such panel not existare" );
}

void PanelViewContainer::closePanelOnTop()
{

    // if the only one in stack or of it is the ServicePanel, do nothing
    if( d->panelViews.count() < 1 || id(visibleWidget()) == 0) {
        return;
    }

    // remove the top widget from the widget stack
    removeWidget( visibleWidget() );

    // and delete it from the cache
    QString pid = d->history [ 0 ];
    d->panelViews.remove( pid );
    emit( panelClosed( pid ) );
    d->history.remove( pid );

    //qWarning( "removed and killed panel with id " + pid );

    //figure out what to display
    QString key = d->history [ 0 ];
    raiseWidget( d->panelViews[ key ] );
    d->panelViews[ key ]->setFocus();
    //qWarning( "now the top should be" + key );

    // figure out PanelView id of the widget on top
    emit( panelDisplayed( key ) );

    // if now the only one in stack, notify others no other close op is
    // enabled
    if( d->panelViews.count() <= 1 ) {
        emit( panelClosingEnabled( false ) );
    }
}

void PanelViewContainer::updatePanelOnTop()
{
    QString pid = d->history [ 0 ];
    // do not update when we have no panels
    if( pid.isNull() ) {
        return;
    }
    PanelView * onTop = d->panelViews[ pid ];
    if( onTop ) onTop->canvas()->update();
}

void PanelViewContainer::buildPanelViewForPanelWithId( const QString & id )
{
    //qWarning( "going to build panel with id" + id );

    //assert here because buildPanelViewForPanelWithId must not be invoked
    //without checking the passed id
    Q_ASSERT( d->view.panels().contains( id ) );

    lt::Panel & p = d->view.panels()[ id ];

    // construct panel view, we are the parents
    PanelView* pv = new PanelView( this, NULL, 0 );
    Q_CHECK_PTR( pv );

    QSize s( d->view.virtualWidth(), d->view.virtualHeight() );

    pv->setVirtualSize( s );
    // pv->setSize( s );
    pv->setProjectPanel( p, this );

    // place it to the cache
    d->panelViews.insert( id, pv );
    // and remember it's on top
    d->history.push_front( id );
    // now place it on top and notify it's visible
    addWidget( pv );
    raiseWidget( pv );
    pv->setFocus();
    emit( panelDisplayed( id ) );

    // and finally if we have more than one panel, enable closing
    emit( panelClosingEnabled( d->panelViews.count() > 1 ) );

    //PanelViewDictIterator k( d->panelViews );
    //for( ; k.current(); ++k ) {
    //    qWarning( "we have now id: " + k.currentKey() + " in a cache" );
    //}
}

void PanelViewContainer::setServicePanel(QWidget* sp)
{
    Q_ASSERT(d);

    if(d->sp) {
        removeWidget(d->sp);
    }
    d->sp = sp;
    addWidget(d->sp, 0);
}

void PanelViewContainer::showServicePanel()
{
    Q_ASSERT(d);
    Q_ASSERT(d->sp);

    d->lastPanel = visibleWidget();
    raiseWidget(d->sp);
    d->sp->setFocus();
}

void PanelViewContainer::hideServicePanel()
{
    Q_ASSERT(d);
    if(d->lastPanel) {
        raiseWidget(d->lastPanel);
        d->lastPanel->setFocus();
    }
}

bool PanelViewContainer::isServicePanelVisible()
{
    Q_ASSERT(d);
    return ( visibleWidget() == d->sp );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelViewContainer.cpp,v 1.16 2004/08/31 15:50:36 mman Exp $
