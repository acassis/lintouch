// $Id: PanelView.hh,v 1.20 2004/09/14 08:14:57 mman Exp $
//
//   FILE: PanelView.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 09 October 2003
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

#ifndef _PANELVIEW_HH
#define _PANELVIEW_HH

#include <qcanvas.h>

#include <lt/project/Panel.hh>

namespace lt {

    class ViewAdaptor;

    class PanelView;

    typedef QDict<PanelView> PanelViewDict;
    typedef QDictIterator<PanelView> PanelViewDictIterator;

    /**
     * PanelView.
     *
     * @author Martin Man <Martin.Man@seznam.cz>
     */
    class PanelView : public QCanvasView
    {

        Q_OBJECT
            Q_PROPERTY( QSize virtualSize 
                    READ virtualSize WRITE setVirtualSize )
            Q_PROPERTY( QSize size 
                    READ size WRITE setSize )
            Q_PROPERTY( bool lockAspect READ lockAspect WRITE setLockAspect )
            Q_PROPERTY( bool autoFit READ autoFit WRITE setAutoFit )
            Q_PROPERTY( bool animated READ animated WRITE setAnimated )

        public:

            /**
             * Constructor.
             */
            PanelView( QWidget * parent = NULL, const char * name = NULL,
                    WFlags f = 0, bool animated = true );

            /**
             * Destructor.
             */
            virtual ~PanelView();

        private:

            /**
             * Disable unintentional copying.
             */
            PanelView( const PanelView & );
            PanelView & operator =( const PanelView & );

        public:

            /**
             * Set the virtual dimmensions used by this PanelView. All
             * templates placed at this PanelView have their virtual rect
             * positioned within this virtual coordinate system.
             */
            void setVirtualSize( const QSize & size );

            /**
             * Return the virtual dimensions of the underlying canvas.
             */
            QSize virtualSize() const;

            /**
             * Set the real size of the underlying canvas. All templates
             * placed at this Panel view have their rect positioned within
             * this coordinate system.
             */
            void setSize( const QSize & size );

            /**
             * Return the real size of the underlying canvas.
             */
            QSize size() const;

            /**
             * Returns the matrix used to transform from virtual to real
             * coordinates.
             */
            const QWMatrix & matrix() const;

            /**
             * Returns the matrix used to transform from real to virtual
             * coordinates.
             */
            const QWMatrix & inverseMatrix() const;

            /**
             * Enable/Disable animation.
             */
            void setAnimated( bool animated );

            /**
             * Are we animated.
             */
            bool animated() const;

        public:

            /**
             * Display the given panel.
             */
            virtual void setProjectPanel( const lt::Panel & panel,
                    ViewAdaptor * va );

            /**
             * Returns the panel we are displaying
             */
            const lt::Panel & projectPanel() const;

            /**
             * Return the dictionary items this panel view contains.
             */
            virtual const TemplateDict & templates() const;

            /**
             * Return the dictionary items this panel view contains.
             */
            virtual TemplateDict & templates();

        public:

            /**
             * Is aspect ratio of this PanelView locked?
             */
            bool lockAspect() const;

            /**
             * Set the aspect ratio to be locked or not. This setting is
             * actually used only when auto fitting is turned on.
             */
            void setLockAspect( bool lock );

            /**
             * Is automatic fitting upon resize events performed?
             */
            bool autoFit() const;

            /**
             * Set automatic fitting to be performed upon resize events.
             */
            void setAutoFit( bool autoFit );

        public slots:

            /**
             * Recalculate the view's world matrix so that the
             * underlying canvas fits the visible view area according to
             * autoFit property configuration.
             */
            virtual void fitVisible();

        protected:

            /**
             * resizeEvent handler.
             */
            virtual void resizeEvent( QResizeEvent * e );

            /**
             * showEvent handler.
             */
            virtual void showEvent( QShowEvent * e );

            /**
             * hideEvent handler.
             */
            virtual void hideEvent( QHideEvent * e );

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

            /**
             * keyboard handler
             */
            virtual void keyPressEvent( QKeyEvent * e );

            /**
             * keyboard handler
             */
            virtual void keyReleaseEvent( QKeyEvent * e );

        protected:

            /**
             * removes all items from the underlying canvas
             */
            virtual void clear();

            /**
             * updates the focus map by looking for templates that have
             * StrongFocus.
             */
            virtual void updateFocusMap();

            /**
             * chooses the first template to be focused and focuses it.
             */
            virtual void enterFocusMode();

            /**
             * clear focus from all templates.
             */
            virtual void clearFocus();

        public:

            /**
             * PanelViewItem state flags
             */
            enum ItemState {
                ItemNormal = 0x0,
                ItemEnabled = 0x1,
                ItemSelected = 0x2,
                ItemActive = 0x4,
            };

        protected:

            /**
             * Recompute internal matrix so that it represents the
             * tranformation from virtual size to size.
             */
            void updateTransformationMatrix();

            /**
             * Applies transformation matrix to all templates.
             */
            void resizeItems();

            /**
             * Will resize canvas to the currently set values of width and
             * height.
             */
            void resizeCanvas();

            /**
             * Will resize frame.
             */
            void resizeFrame();

        public:

            /**
             * Applies transformation matrix to given template.
             * The real coordinates of the given template recomputed from
             * their virtual counterparts.
             */
            void resizeTemplate( Template * t );

        protected:

            /**
             * Find the item which is under the mouse
             * cursor and which has the given state flags set.
             * when exact is true, we check whether e collides with the
             * template, when false we use it's bounding rectangle.
             */
            Template * findItemOnTop( QMouseEvent * e,
                    int flags = ItemNormal, bool exact = true );

        private:

            typedef struct PanelViewPrivate;

            PanelViewPrivate * d;

    };

} /* namespace lt */

#endif /* _PANELVIEW_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelView.hh,v 1.20 2004/09/14 08:14:57 mman Exp $
