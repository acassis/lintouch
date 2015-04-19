// $Id: PanelViewContainer.hh,v 1.9 2004/08/31 15:50:36 mman Exp $
//
//   FILE: PanelViewContainer.hh -- 
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

#ifndef _PANELVIEWCONTAINER_HH
#define _PANELVIEWCONTAINER_HH

#include <qwidgetstack.h>

#include <lt/templates/ViewAdaptor.hh>
#include <lt/project/View.hh>

namespace lt {

    /**
     * PanelViewContainer.
     *
     * PanelViewContainer visualizes panels contained within one Project
     * View. When you create PanelViewContainer you must tell it through
     * setProjectView which View should be visualized. if you pass in
     * View() the PanelViewContainer will not display anything.
     *
     * Once you feed in some View, its home panel is automatically
     * displayed. Whenever a panel is displayed or closed signals
     * panelDisplayed(id) and panelClosed(id) are emitted.
     *
     * You can display new panel by invoking displayPanel(id). You can
     * also close the panel which is currently displayed by calling
     * closePanelOnTop(). Signal panelClosingEnabled(bool) will be
     * emitted automatically to let you know wheter you can safely call
     * closePanel(without leaving the area empty).
     *
     * When you close a panel on top, the one that was visible before
     * will be displayed automatically.
     *
     * @author Martin Man <Martin.Man@seznam.cz>
     */
    class PanelViewContainer : public QWidgetStack, public ViewAdaptor
    {

        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            PanelViewContainer( QWidget * parent = NULL, 
                    const char * name = NULL, WFlags f = 0 );

            /**
             * Destructor.
             */
            virtual ~PanelViewContainer();

        private:

            /**
             * Disable unintentional copying.
             */
            PanelViewContainer( const PanelViewContainer & );
            PanelViewContainer & operator =( const PanelViewContainer & );

        public:

            /**
             * Returns the Project View this PanelViewContainer is
             * visualizing.
             */
            View projectView() const;

            /**
             * Give us Project View this PanelViewContainer should
             * visualize. possibly start at given panel, when null is
             * given use either home panel (if defined) or the first one
             * found in view.
             */
            virtual void setProjectView( const View & pView,
                    const QString & startPanel = QString::null );

            /**
             * Set ServicePanel.
             */
            virtual void setServicePanel(QWidget* sp);

        public slots:

            /**
             * Display Panel with given id. Will emit signal
             * panelDisplayed( QString & id )
             */
            virtual void displayPanel( const QString & id );

            /**
             * Close Panel which is currently displayed. Will emit
             * signal panelClosed( QString & id )
             */
            virtual void closePanelOnTop();

            /**
             * Schedule update for currently displayed panel.
             */
            virtual void updatePanelOnTop();

            /**
             * Show ServicePanel.
             */
            virtual void showServicePanel();

            /**
             * Hide ServicePanel.
             */
            virtual void hideServicePanel();

            /**
             * Is ServicePanel visible right now?
             */
            virtual bool isServicePanelVisible();

        signals:

            /**
             * Emitted as a result of displayPanel.
             */
            void panelDisplayed( const QString & id );

            /**
             * Emitted as a result of closePanel.
             */
            void panelClosed( const QString & id );

            /**
             * Emitted with either true or false as an argument
             * according to the internal state of panel cache.
             */
            void panelClosingEnabled( bool enabled );

        protected:

            /**
             * build a PanelView from Panel with given id and remember
             * it in the internal PanelView cache. Also add it to the
             * WidgetStack, raise it and emit panelDisplayed. 
             * no checks on id are performed.
             */
            void buildPanelViewForPanelWithId( const QString & id );

        private:

            typedef struct PanelViewContainerPrivate;
            PanelViewContainerPrivate * d;

    };

} /* namespace lt */

#endif /* _PANELVIEWCONTAINER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelViewContainer.hh,v 1.9 2004/08/31 15:50:36 mman Exp $
