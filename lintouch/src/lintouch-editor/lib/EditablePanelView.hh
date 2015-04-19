// $Id: EditablePanelView.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: EditablePanelView.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 17 October 2003
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

#ifndef _EDITABLEPANELVIEW_HH
#define _EDITABLEPANELVIEW_HH

#include "lt/ui/PanelView.hh"

class QPopupMenu;

namespace lt {

    class EditablePanelView;

    typedef QDict<EditablePanelView> EditablePanelViewDict;
    typedef QDictIterator<EditablePanelView> EditablePanelViewDictIterator;

    /**
     * EditablePanelView.
     *
     * @author Martin Man <Martin.Man@seznam.cz>
     */
    class EditablePanelView : public PanelView
    {

        Q_OBJECT

        public:

            typedef enum
            {
                /**
                 * When new template is being placed on a panel by mouse.
                 */
                Place,
                /**
                 * Move, Resize, Select.
                 */
                Modify,
                /**
                 * When interactively zooming.
                 */
                Magnify,
                /**
                 * When interactively panning.
                 */
                Pan,
                /**
                 * When interactively previewing.
                 */
                Preview
            } OperationMode;


        public:

            /**
             * Constructor.
             */
            EditablePanelView( QWidget * parent = NULL, 
                    const char * name = NULL,
                    WFlags f = 0 );

            /**
             * Destructor.
             */
            virtual ~EditablePanelView();

        private:

            /**
             * Disable unintentional copying.
             */
            EditablePanelView( const EditablePanelView & );
            EditablePanelView & operator =( const EditablePanelView & );

        public:

            /**
             * Display the given panel.
             */
            virtual void setProjectPanel( const lt::Panel & panel,
                    ViewAdaptor * va );

            /**
             * Put template with given key to the PanelView, the item must
             * exist.
             */
            virtual void addTemplate( const QString & key );

            /**
             * Remove template with given key from the PanelView, the item
             * must still exist.
             */
            virtual void removeTemplate( const QString & key );

            /**
             * Set the real size of the underlying canvas. All templates
             * placed at this Panel view have their rect positioned within
             * this coordinate system.
             */
            void setSize( const QSize & size );

        public:

            /**
             * Returns the mode of this window.
             */
            EditablePanelView::OperationMode operationMode() const;

            /**
             * Set the editable state of this panel.
             */
            void setOperationMode( EditablePanelView::OperationMode m );

            /**
             * Sets the mouse cursor to be used in magnify
             * mode.
             */
            void setMagnifyCursor( bool zoom_in );

        public:

            /**
             * Sets the menu that should be displayed on right mouse click.
             */
            void setContextMenu( QPopupMenu * menu );

        public slots:

            /**
             * Select all templates on this panel.
             */
            virtual void selectAllTemplates();

            /**
             * Deselect all templates on this panel.
             */
            virtual void unselectAllTemplates();

            /**
             * Invert current selection.
             */
            virtual void invertSelection();

            /**
             * Select given templates.
             */
            virtual void selectTemplates( const TemplateDict & t );

            /**
             * Add given template to the current selection.
             */
            virtual void selectTemplate( Template * t, bool notify = false );

            /**
             * Remove given template from the current selection.
             */
            virtual void unselectTemplate( Template * t, bool notify = false );

            /**
             * Resizes given templates.
             */
            virtual void resizeTemplates(TemplateDict& td);

        public:

            /**
             * Returns the list of templates that are selected.
             */
            virtual const TemplateDict & currentSelection() const;

            /**
             * Rename given template from oldName to newName.
             * Updates selection so that it's still valid.
             */
            virtual void renameTemplate( const QString & oldName,
                    const QString & newName );

        signals:

            /**
             * emitted when currentSelection changes.
             */
            void selectionChanged();

            /**
             * emitted when one of the selected templates is doubleclicked.
             */
            void selectionDoubleClicked(QMouseEvent*);

            /**
             * emitted when the selection is moved.  originalRects
             * contains the positions from where the templates in selection
             * moved.
             */
            void selectionMoved(
                    const LTMap <QRect> & origRects );

            /**
             * emitted when some templates are resized.  originalRects
             * contains the original rects.
             */
            void selectionResized( TemplateDict & templates,
                    const LTMap <QRect> & origRects );

            /**
             * emited when a template is placed from within Place mode.
             */
            void templatePlaced( const QRect & position );

            /**
             * emitted when mouse is clicked in magnify mode.
             * the given point in original contents has been clicked.
             */
            void zoomInRequested( const QPoint & );

            /**
             * emitted when mouse is shift-clicked in magnify mode.
             * the given point in original contents has been clicked.
             */
            void zoomOutRequested( const QPoint & );

            /**
             * Emits on every contentsMouseReleaseEvent call. Can be used
             * for, for example, global context menu on the right click. If
             * the QMouseEvent is accepted, this contentsMouseReleaseEvent
             * returns imediately.
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
             * Set horizontal and vertical grid spacing (in pixels)
             */
            void setGridXYSpacing( unsigned xspacing, unsigned yspacing );

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

        protected:

            /**
             * Return the adjustment needed to so that the p will snap to the
             * grid after being moved by diff
             */
            QPoint adjustToGrid( const QPoint & p, const QPoint & diff );

            /**
             * Reconstruct grid.
             */
            void reconstructXGrid();

            /**
             * Reconstruct grid.
             */
            void reconstructYGrid();

            /**
             * Show/Hide grid.
             */
            void showGrid( bool visible );

            /**
             * Change grid props.
             */
            void changeGridProps( const QColor & color, unsigned highlight );

        protected:

            /**
             * mouse event handler
             */
            virtual void contentsMouseDoubleClickEvent( QMouseEvent * e );

            /**
             * mouse event handler
             */
            virtual void contentsMouseMoveEvent( QMouseEvent * e );

            /**
             * mouse event handler
             */
            virtual void contentsMousePressEvent( QMouseEvent * e );

            /**
             * mouse event handler
             */
            virtual void contentsMouseReleaseEvent( QMouseEvent * e );

        protected:
            /**
             * Gets called when a QShowEvent arrives
             */
            virtual void showEvent(QShowEvent* e);

        public:

            /**
             * keyboard handler (public so that it can be invoked externally
             * via event filter).
             */
            virtual void keyPressEvent( QKeyEvent * e );

            /**
             * keyboard handler (public so that it can be invoked externally
             * via event filter).
             */
            virtual void keyReleaseEvent( QKeyEvent * e );

        protected:

            /**
             * start the move by remembering original rectangles of selected
             * templates.
             */
            void startMove();

            /**
             * start the resize by remembering original rectangle of the
             * selected template and the resize direction. direction
             * corresponds to the Template::handleAtPosition return value.
             */
            void startResize( Template * t, int direction );

        private:

            typedef struct EditablePanelViewPrivate;

            EditablePanelViewPrivate * d;

    };

} /* namespace lt */

#endif /* _EDITABLEPANELVIEW_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditablePanelView.hh 1169 2005-12-12 15:22:15Z modesto $
