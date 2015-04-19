// $Id: EditablePanelViewContainer.hh 1169 2005-12-12 15:22:15Z modesto $
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

#ifndef _EDITABLEPANELVIEWCONTAINER_HH
#define _EDITABLEPANELVIEWCONTAINER_HH

#include <qtabwidget.h>

#include <lt/templates/ViewAdaptor.hh>
#include <lt/project/View.hh>

#include "EditablePanelView.hh"

class QPopupMenu;

namespace lt {

    /**
     * EditablePanelViewContainer.
     *
     * EditablePanelViewContainer visualizes panels contained within one
     * Project View. When you create EditablePanelViewContainer you must
     * tell it through setProjectView which View should be visualized. if
     * you pass in View() the EditablePanelViewContainer will not display
     * anything.
     *
     * All panels from the view are always visible within the tab widget.
     *
     * @author Martin Man <Martin.Man@seznam.cz>
     */
    class EditablePanelViewContainer :
        public QTabWidget, public ViewAdaptor
    {

        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            EditablePanelViewContainer( QWidget * parent = NULL, 
                    const char * name = NULL, WFlags f = 0 );

            /**
             * Destructor.
             */
            virtual ~EditablePanelViewContainer();

        private:

            /**
             * Disable unintentional copying.
             */
            EditablePanelViewContainer( const EditablePanelViewContainer & );
            EditablePanelViewContainer & operator =(
                    const EditablePanelViewContainer & );

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
             * found in view. use given zoom factor to display panels.
             */
            virtual void setProjectView( const View & pView,
                    double zoom = 1.0,
                    const QString & startPanel = QString::null );

            /**
             * Creates (and adds to the tab widget) EditablePanelView for
             * given panel name. use given zoom factor to display panel.
             */
            virtual void addPanelView( const QString & panel,
                    double zoom = 1.0, int index = -1 );

            /**
             * Destroys (and deletes from the tab widget) EditablePanelView
             * at the given position.
             */
            virtual void removePanelView( int index );

            /**
             * Recreates respective panel view after panel rename.
             */
            virtual void renamePanelView( int index,
                    const QString & newName, double zoom = 1.0 );

            /**
             * Updates all panels so that they have correct virtual size set
             * and so that they are scaled according to given zoom
             * coeficient. when negative zoom is given, the views will be
             * fitting visible space.
             */
            virtual void resizePanelViews( double zoom = 1.0 );

        public:

            /**
             * Sets the menu that should be displayed on right mouse click.
             */
            void setContextMenu( QPopupMenu * menu );

            /**
             * Returns the operation mode of the container.
             */
            EditablePanelView::OperationMode operationMode() const;

            /**
             * Sets the operation mode of the container.
             */
            void setOperationMode( EditablePanelView::OperationMode m );

            /**
             * Sets the mouse cursor to be used by panel views in magnify
             * mode.
             */
            void setMagnifyCursor( bool zoom_in );

        public:

            /**
             * Returns the EditablePanelView of the currently displayed
             * panel
             */
            EditablePanelView * currentPanelView();

            /**
             * Returns the EditablePanelView for given panel.
             */
            EditablePanelView * panelView( const QString & panel );

        public slots:

            /**
             * Display Panel with given id. Will emit signal
             * panelDisplayed( QString & id )
             */
            virtual void displayPanel( const QString & id );

            /**
             * Schedule update for currently displayed panel.
             */
            virtual void updatePanelOnTop();

            /**
             * Schedule update for currently displayed panel + make all
             * template reread their properties. Usefull when resource has
             * changed.
             */
            virtual void fullUpdatePanelOnTop();

        signals:

            /**
             * Emitted as a result of displayPanel.
             */
            void panelDisplayed( const QString & id );

            /**
             * Emitted when selection at currently displayed panel changes.
             */
            void selectionChanged();

            /**
             * Emitted when selection at currently displayed panel changes.
             */
            void selectionDoubleClicked(QMouseEvent*);

            /**
             * emitted when the selection at currently displayed panel is
             * moved.  originalRects contains the positions from
             * where the templates in selection moved.
             */
            void selectionMoved( const LTMap <QRect> & origRects );

            /**
             * emited when a template is placed at currently displayed panel
             * from within Place mode.
             */
            void templatePlaced( const QRect & position );

            /**
             * emitted when some templates at currently displayed panel are
             * resized.  originalRects contains the original rects.
             */
            void selectionResized( TemplateDict & templates,
                    const LTMap <QRect> & origRects );

            /**
             * emitted when mouse is clicked in magnify mode.
             * the given point in original contents has been pressed.
             */
            void zoomInRequested( const QPoint & );

            /**
             * emitted when mouse is shift-clicked in magnify mode.
             * the given point in original contents has been pressed.
             */
            void zoomOutRequested( const QPoint & );

            /**
             * Emits on every contentsMouseReleaseEvent call EPV. Can be
             * used for, for example, global context menu on the right
             * click. If the QMouseEvent is accepted, this
             * contentsMouseReleaseEvent returns imediately.
             *
             * \param e The QMouseEvent event object that describes current
             * mouse position and button status.
             */
            void contentsMouseReleaseSignal(QMouseEvent* e);

        public:

            /**
             * Set grid color
             */
            void setGridColor( const QColor & color );

            /**
             * Set horizontal grid spacing (in pixels)
             */
            void setGridXSpacing( unsigned spacing );

            /**
             * Set vertical grid spacing (in pixels)
             */
            void setGridYSpacing( unsigned spacing );

            /**
             * Set grid highlight (in grid lines).
             * Every Nth grid line is painted with thicker pen.
             */
            void setGridHighlight( unsigned n );

            /**
             * Show/Hide the grid in this project container.
             */
            void setGridVisible( bool on );

            /**
             * Activate/Deactivate Snap To Grid in this container.
             * Grid can be hidden while snap to grid is activated.
             */
            void setGridSnap( bool on );

        protected slots:

            /**
             * Invoked when user selects another tab.
             */
            void currentChangedSlot( QWidget * w );

        private:

            typedef struct EditablePanelViewContainerPrivate;
            EditablePanelViewContainerPrivate * d;

    };

} /* namespace lt */

#endif /* _EDITABLEPANELVIEWCONTAINER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditablePanelViewContainer.hh 1169 2005-12-12 15:22:15Z modesto $
