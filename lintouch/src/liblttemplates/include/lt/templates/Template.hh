// $Id: Template.hh 1645 2006-04-28 21:28:08Z hynek $
//
//   FILE: Template.hh --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 03 October 2003
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

#ifndef _TEMPLATE_HH
#define _TEMPLATE_HH

#include <qcanvas.h>
#include <qkeysequence.h>
#include <qpainter.h>

#include <lt/logger/LogEnabled.hh>

#include "lt/LTDict.hh"
#include "lt/LTMap.hh"

#include "lt/templates/Info.hh"
#include "lt/templates/IOPin.hh"
#include "lt/templates/Localizator.hh"
#include "lt/templates/Property.hh"
#include "lt/templates/ViewAdaptor.hh"
using namespace lt;



namespace lt {

        class TemplateManager;

        class Template;

        typedef LTDict <Template> TemplateDict;

        typedef QPtrList <Template> TemplateList;
        typedef QPtrListIterator <Template> TemplateListIterator;

        /**
         * Template.
         *
         * Template is a base class for visualization primitives used in
         * Lintouch.
         *
         * Template very much resembles ordinary GUI widget in the following
         * points:
         *
         * - it occupies rectangular area set by Template::setRect
         * - it can receive keyboard events when focused
         * - it can receive mouse events
         * - it's painted by reimplementing Template::drawShape
         * - it can contain another Templates (requested by
         *   Template::requestNestedTemplate.
         *
         * Template differes from ordinary widgets in the following points:
         *
         * - can access arbitrary files via Localizator API (localizator
         *   being set via Template::localize
         * - can return summary info
         * - can be externally configured via properties.
         * - can react to changes sent to its iopins.
         *
         * Lifecycle of the template:
         *
         * <pre>
         * t = new Template()
         * t->enableLogging( logger )
         * t->localize( localizator )
         * t->compose( templateManager )
         *
         * t->setRect( rect )
         * t->setZ( z )
         *
         * //set properties
         * t->properties()["..."]->decodeValue("...")
         * t->propertiesChanged()
         * </pre>
         *
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class Template : public QCanvasPolygonalItem, public LogEnabled
        {

            // ************************************************************
            // ************************************************************
            // ************************************************************
            // The following methods should be reimplemented by subclasses
            // ************************************************************
            // ************************************************************
            // ************************************************************

            public:

                /**
                 * Create template while specifying its type.
                 */
                Template( const QString & type,
                        LoggerPtr logger = Logger::nullPtr() );

                /**
                 * Destructor.
                 */
                virtual ~Template();

            protected:

                /**
                 * Draw this Template.
                 *
                 * Draw this template using the given painter. Subclasses
                 * are expected to reimplement this method. you don't need
                 * to paint your nested templates, they will be painted
                 * automatically by the canvas if they're visible.
                 */
                virtual void drawShape( QPainter & p );

            public:

                // Mouse event handling
                // ********************************************************

                /**
                 * Handle mouse event.
                 * Should call e->accept() if the event has been consumend.
                 */
                virtual void mousePressEvent( QMouseEvent * ) {};

                /**
                 * Handle mouse event.
                 * Should call e->accept() if the event has been consumend.
                 */
                virtual void mouseReleaseEvent( QMouseEvent * ) {};

                /**
                 * Handle mouse event.
                 * Should call e->accept() if the event has been consumend.
                 */
                virtual void mouseDoubleClickEvent( QMouseEvent * ) {};

                /**
                 * Handle mouse event.
                 * Should call e->accept() if the event has been consumend.
                 */
                virtual void mouseMoveEvent( QMouseEvent * ) {};

                /**
                 * Handle wheel event.
                 * Should call e->accept() if the event has been consumend.
                 */
                virtual void wheelEvent( QWheelEvent * ) {};

                // Keyboard event handling
                // ********************************************************

                /**
                 * Handle keyboard event.
                 * Should call e->accept() if the event has been consumend.
                 *
                 * Template will receive all keyboard events if it's
                 * focused. Additionally shortcut will be reported even if
                 * the template is not focused.
                 */
                virtual void keyPressEvent( QKeyEvent * ) {};

                /**
                 * Handle keyboard event.
                 * Should call e->accept() if the event has been consumend.
                 *
                 * Template will receive all keyboard events if it's
                 * focused. Additionally shortcut will be reported even if
                 * the template is not focused.
                 */
                virtual void keyReleaseEvent( QKeyEvent * ) {};

                /**
                 * Converts given key event to appropriate QKeySequence.
                 */
                static QKeySequence keyEvent2Sequence( const QKeyEvent * );

                /**
                 * Sets the shortcut sequence to be used by this template.
                 */
                virtual void setShortcut( const QKeySequence & sq );

                /**
                 * Returns the shortcut defined for this template.
                 */
                const QKeySequence & shortcut() const;

                // Focus event handling
                // ********************************************************

                /**
                 * Return the focus policy of this template.
                 * Only the templates that return QWidget::StrongFocus will
                 * obtain focusInEvent/focusOutEvent.
                 */
                virtual QWidget::FocusPolicy focusPolicy() const
                {
                    return QWidget::NoFocus;
                }

                /**
                 * Handle focus event.
                 */
                virtual void focusInEvent ( QFocusEvent * );

                /**
                 * Handle focus event.
                 */
                virtual void focusOutEvent ( QFocusEvent * );

            public:

                // Access to localized resources
                // ********************************************************

                /**
                 * Pass Localizator to the template.
                 *
                 * Template can use the given localizator to retrieve
                 * localized resources(pixmaps, messages, etc.).
                 */
                virtual void localize( LocalizatorPtr localizator );

                // Nested templates management
                // ********************************************************

                /**
                 * Pass TemplateManager to the template.
                 *
                 * The given template mamager is used to create instances of
                 * nested templates that this template registered previously
                 * by calling registerNestedTemplate(). The created nested
                 * templates can later be retreived by calling
                 * nestedTemplate(). You can override this method to
                 * manually control creation process of nested templates.
                 * Don't forget to register any such created template by
                 * calling registerNestedTemplate
                 */
                virtual void compose( const TemplateManager & tm );

                /**
                 * Clone the template. The returned instance will have
                 * the same iopins and properties as the original one and
                 * will be of the same type.
                 *
                 * When you are reimplementing this method, you are required
                 * to instantiate your template and then use method
                 * setupAfterClone to assign the same values of properties
                 * and bindings of iopins to the new instance.
                 *
                 * @see setupAfterClone.
                 */
                virtual Template * clone(
                        const LocalizatorPtr & loc,
                        const TemplateManager & tm,
                        LoggerPtr logger = Logger::nullPtr() ) const;

            public:

                // Handling of property changes
                // ********************************************************

                /**
                 * Notify the Template that one or more of their
                 * properties() have changed.
                 *
                 * You can reconstruct your off-screen pixmaps here, adopt
                 * your painting algoritm and call update() to schedule
                 * canvas repaint.
                 */
                virtual void propertiesChanged() {};

                // Handling of iopin value changes
                // ********************************************************

                /**
                 * Notify the Template that one or more of their
                 * iopins() have changed.
                 *
                 * You can reconstruct your off-screen pixmaps here, adopt
                 * your painting algoritm and call update() to schedule
                 * canvas repaint.
                 */
                virtual void iopinsChanged() {};

            // ************************************************************
            // ************************************************************
            // ************************************************************
            // The following methods should be used by the Template
            // ************************************************************
            // ************************************************************
            // ************************************************************

            protected:

                /**
                 * Register info.
                 */
                bool registerInfo( const Info & info );

                /**
                 * Register given property. The property will be registered
                 * to the given group and under the given name. The given
                 * name must be unique within this template.
                 */
                bool registerProperty( const QString & group,
                        const QString & name,
                        Property * prop );

                /**
                 * Enable or disable all properties from given property
                 * group. Properties can be also enabled/disabled
                 * individually by using Property::setEnabled(bool).
                 */
                void setPropertyGroupEnabled( const QString & group,
                        bool enabled );

                /**
                 * Register given iopin. The iopin will be registered
                 * to the given group and under the given name. The given
                 * name must be unique within this template.
                 */
                bool registerIOPin( const QString & group,
                        const QString & name,
                        IOPin * pin );

                /**
                 * Register request for creation of a nested template with
                 * given name and type.
                 */
                bool requestNestedTemplate( const QString & name,
                        const QString & type,
                        const QString & library = QString::null );

                /**
                 * Register created nested template with
                 * given name.
                 */
                bool registerNestedTemplate( const QString & name,
                        Template * tmpl );

                /**
                 * Populate the property values and iopin bindings of the
                 * given instance with the values of our instance.
                 *
                 * Can be used to simplify the implementation of clone()
                 * method.
                 *
                 * @see clone.
                 */
                virtual void setupAfterClone( Template * newTemplate,
                        const LocalizatorPtr & loc,
                        const TemplateManager & tm ) const;

                /**
                 * Return the font scaled according to the ratio of
                 * virtualRect/rect. This method safely computes the font
                 * size that should be used to draw text to our rect in a
                 * way that it will look the same way as if it had been
                 * written using the original font into our virtualRect.
                 *
                 * Warning: this method requires that both, rect and
                 * virtualRect are set up correctly.
                 *
                 * Warning: text arg is unused at the moment.
                 */
                QFont makeFont( const QFont & original,
                        const QString & text = "MMMMMMMMMM" ) const;

            // ************************************************************
            // ************************************************************
            // ************************************************************
            // The following methods can be used by parent container
            // ************************************************************
            // ************************************************************
            // ************************************************************

            public:

                /**
                 * Return the type of this Template.
                 */
                QString type() const;

                /**
                 * Return the template library this Template comes from. The
                 * library can be set with Template::setLibrary.
                 */
                QString library() const;

                /**
                 * Sets the library this template has been instantiated
                 * from.
                 */
                void setLibrary( const QString & lib );

                /**
                 * Return the info about this Template.
                 */
                const Info & info() const;

                /**
                 * Return the dictionary of of iopin groups defined by this
                 * template.
                 */
                const LTMap <IOPinDict> & iopinGroups() const;

                /**
                 * Return the dictionary of of iopin groups defined by this
                 * template.
                 */
                LTMap <IOPinDict> & iopinGroups();

                /**
                 * Return the IOPins from given group.
                 *
                 * When QString::null is passed in, all iopins defined
                 * by this template are returned.
                 */
                const IOPinDict & iopins(
                        const QString & group = QString::null ) const;

                /**
                 * Return the IOPins from given group.
                 *
                 * When QString::null is passed in, all iopins defined
                 * by this template are returned.
                 */
                IOPinDict & iopins(
                        const QString & group = QString::null );

                /**
                 * Return the property groups defined by this
                 * template.
                 */
                const LTMap<PropertyDict> & propertyGroups() const;

                /**
                 * Return the property groups defined by this
                 * template.
                 */
                LTMap<PropertyDict> & propertyGroups();

                /**
                 * Return the Properties from given group.
                 *
                 * When QString::null is passed in, all properties defined
                 * by this template are returned.
                 */
                const PropertyDict & properties(
                        const QString & group = QString::null ) const;

                /**
                 * Return the Properties from given group.
                 *
                 * When QString::null is passed in, all properties defined
                 * by this template are returned.
                 */
                PropertyDict & properties(
                        const QString & group = QString::null );

                /**
                 * Return the nested template with given name.
                 *
                 * Template can get access to given nested template by
                 * providing it's name. Nested templates are created as soon
                 * as the TemplateManager is provided to the compose method,
                 * later on Template can modify properties of nested
                 * templates, paint them, etc.
                 */
                Template * nestedTemplate( const QString & name );

            public:

                /**
                 * Give us the ViewAdaptor through which we can control the
                 * View that contains us.
                 */
                virtual void setViewAdaptor( ViewAdaptor * va );

                /**
                 * Return the ViewAdaptor that contains us.
                 * this function is guaranteed to never return NULL.
                 */
                ViewAdaptor * viewAdaptor() const;

            public:

                // Geometry stuff
                // ********************************************************

                /**
                 * Set the real rectangle occupied by this template.
                 *
                 * The given rectangle is in coordinate system defined by a
                 * canvas containing this template.
                 */
                virtual void setRect( const QRect & r );

                /**
                 * Set the virtual rectangle occupied by this template.
                 *
                 * The given rectangle is in virtual coordinate system
                 * defined by a panel containing this template.
                 */
                virtual void setVirtualRect( const QRect & r );

                /*
                 * Return the real rectangle occupied by this template.
                 *
                 * The returned rectangle is in coordinate system defined by
                 * a canvas containing this template.
                 */
                QRect rect() const;

                /*
                 * Return the virtual rectangle occupied by this template.
                 *
                 * The returned rectangle is in coordinate system defined by
                 * a panel containing this template.
                 */
                QRect virtualRect() const;

            public:

                /**
                 * Show or hide this template.
                 *
                 * You are expected here to show/hide your nested templates
                 * where appropriate.
                 *
                 * You can also use this method as a hook to get notified
                 * that you have been placed/removed to/from a canvas.
                 */
                virtual void setVisible( bool visible );

                /**
                 * Return the bounging area which is occupied by this
                 * Template.
                 *
                 * By default bounding rectangle as defined by setRect() is
                 * returned.
                 */
                virtual QPointArray areaPoints() const;

                /**
                 * Return the bounding region where this Template reacts to
                 * mouse events.
                 *
                 * The region is used by collidesWith(QPoint
                 * &) to determine whether the event should be forwarder to
                 * this template.
                 */
                virtual QRegion collisionRegion() const;

                /**
                 * Does this Template collide with the given point?
                 *
                 * This method uses areaPoints to determine whether the
                 * mouse points at this Template.
                 */
                bool collidesWith( const QPoint & p ) const;

                /**
                 * Draw the CanvasItem. Do not reimplement this method in
                 * subclasses, use drawShape instead.
                 *
                 * @see drawShape
                 */
                virtual void draw( QPainter & p );

                /**
                 * Draw the focus around the template. A template have to
                 * call this method itself the focus is not painted
                 * automaticaly.
                 */
                virtual void drawFocusRect( QPainter& p);

            public:

                /**
                 * Sync requested values of our iopins with iopins that we
                 * are bound to. will also clear dirty flags on synced
                 * iopins.
                 *
                 * Will also post
                 * QCustomEvent(IOPinRequestedValuesChangedEvent) to
                 * qApp object so that qApp can examine what to do.
                 */
                void syncIOPinsRequestedValues();

                /**
                 * Sync real values of our iopins with iopins are bound to
                 * us.  will also clear dirty flags on synced iopins.
                 *
                 * Will also post QCustomEvent(IOPinRealValuesChangedEvent)
                 * to qApp object so that qApp can examine what to do.
                 */
                void syncIOPinsRealValues();

            public:

                /**
                 * QCustomEvent type fired when requested values are synced.
                 * data() of this event is an instance of TemplateList
                 * containing the templates that should be notified with
                 * iopinsChanged()
                 * @see syncIOPinsRequestedValues
                 */
                enum { IOPinRequestedValuesChangedEvent = 1111 };
                /**
                 * QCustomEvent type fired when real values are synced.
                 * data() of this event is an instance of TemplateList
                 * containing the templates that should be notified with
                 * iopinsChanged()
                 * @see syncIOPinsRealValues
                 */
                enum { IOPinRealValuesChangedEvent = 1112 };

                /**
                 * rtti value used by QCanvas to find our Templates.
                 */
                enum { RTTI = 1111 };

                /**
                 * rtti.
                 */
                virtual int rtti() const;

            private:

                typedef struct TemplatePrivate;
                TemplatePrivate * d;

            private:

                /**
                 * Disable unintentional copying.
                 */
                Template( const Template & );
                Template & operator=( const Template & );
        };

} // namespace lt

#endif /* _TEMPLATE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Template.hh 1645 2006-04-28 21:28:08Z hynek $
