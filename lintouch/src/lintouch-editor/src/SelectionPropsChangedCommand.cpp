// $Id: SelectionPropsChangedCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: SelectionPropsChangedCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 09 September 2004
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

#include <lt/templates/Property.hh>

#include "EditablePanelViewContainer.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "SelectionPropsChangedCommand.hh"

using namespace lt;

struct SelectionPropsChangedCommand::
    SelectionPropsChangedCommandPrivate
{
    /*
     * ctor
     */
    SelectionPropsChangedCommandPrivate() {}

    /*
     * dict of templates that we operate on
     */
    TemplateDict selection;

    /*
     * key to property to change
     */
    QString key;

    /*
     * new value of property
     */
    QString newValue;

    /*
     * old values
     */
    QMap<int,QString> oldValues;

    /*
     * used view
     */
    QString viewName;

    /*
     * used panel
     */
    QString panelName;
};

SelectionPropsChangedCommand::SelectionPropsChangedCommand(
        const TemplateDict& s, const QString& propertyKey,
        const QString& newValue, ProjectWindow* pw,
        MainWindow* mw ) : Command( pw, mw,
            qApp->translate("SelectionPropsChangedCommand",
                "Change Selection Property") ),
    d(new SelectionPropsChangedCommandPrivate)
{
    Q_CHECK_PTR(d);

    Q_ASSERT(pw);

    for(unsigned i=0; i < s.count(); ++i) {
        const Template* t = s[i];
        Q_ASSERT(t);

        // store the Template in our TemplateDict
        d->selection.insert( s.keyAt(i), t );

        // store old value
        Q_ASSERT(t->properties()[propertyKey]);
        d->oldValues.insert( i,
                t->properties()[propertyKey]->encodeValue() );
    }
    d->key = propertyKey;
    d->newValue = newValue;

    d->viewName = pw->currentView();
    d->panelName = pw->currentPanel();
}

SelectionPropsChangedCommand::~SelectionPropsChangedCommand()
{
    delete d;
}

void SelectionPropsChangedCommand::redo()
{
    Q_ASSERT(d);
    Q_ASSERT(d->oldValues.count() == d->selection.count());
    Q_ASSERT(!d->viewName.isNull());
    Q_ASSERT(!d->panelName.isNull());
    Q_ASSERT(m_pw);
    Q_ASSERT(m_pw->project());
    Q_ASSERT(m_pw->project()->views().contains(d->viewName));
    Q_ASSERT(m_pw->project()->views()[d->viewName].
            panels().contains(d->panelName));

    Template* t = NULL;
    Property* p = NULL;
    // over all templates in selection
    for(unsigned i=0; i < d->selection.count(); ++i) {
        // get one template
        t = d->selection[i];
        Q_ASSERT(t);
        Q_ASSERT(t->properties().contains(d->key));

        // get the property
        p = t->properties()[d->key];
        Q_ASSERT(p);
#ifdef DEBUG
        if(p->type() == Property::FontType) {
            Q_ASSERT(p->encodeValue().upper() == d->oldValues[i].upper());
        } else {
            Q_ASSERT(p->encodeValue() == d->oldValues[i]);
        }
#endif

        // set the new value
        bool b = p->decodeValue(d->newValue);
        Q_ASSERT(b);

#ifdef DEBUG
        if(p->type() == Property::FontType) {
            //fonts are case-insensitive
            Q_ASSERT(p->encodeValue().upper() == d->newValue.upper());
        } else {
            Q_ASSERT(p->encodeValue() == d->newValue);
        }
#endif

        // notify template about the change
        t->propertiesChanged();
    }

    // update the panel view entirely after the changes
    EditablePanelView * panelView =
        m_pw->m_viewContainer->panelView( d->panelName );
    Q_ASSERT(panelView);
    panelView->canvas()->update();

    notifyMainWindow( LTE_CMD_SELECTION_PROPS_CHANGE );
}

void SelectionPropsChangedCommand::undo()
{
    Q_ASSERT(d);
    Q_ASSERT(d->oldValues.count() == d->selection.count());
    Q_ASSERT(!d->viewName.isNull());
    Q_ASSERT(!d->panelName.isNull());
    Q_ASSERT(m_pw);
    Q_ASSERT(m_pw->project());
    Q_ASSERT(m_pw->project()->views().contains(d->viewName));
    Q_ASSERT(m_pw->project()->views()[d->viewName].
            panels().contains(d->panelName));

    Template* t = NULL;
    Property* p = NULL;
    // over all templates in selection
    for(unsigned i=0; i < d->selection.count(); ++i) {
        // get one template
        t = d->selection[i];
        Q_ASSERT(t);
        Q_ASSERT(t->properties().contains(d->key));

        // get the property
        p = t->properties()[d->key];
        Q_ASSERT(p);

#ifdef DEBUG
        if(p->type() == Property::FontType) {
            //fonts are case-insensitive
            Q_ASSERT(p->encodeValue().upper() == d->newValue.upper());
        } else {
            Q_ASSERT(p->encodeValue() == d->newValue);
        }
#endif


        // set the new value
        p->decodeValue(d->oldValues[i]);

        // notify template about the change
        t->propertiesChanged();
    }

    // update the panel view entirely after the changes
    EditablePanelView * panelView =
        m_pw->m_viewContainer->panelView( d->panelName );
    Q_ASSERT(panelView);
    panelView->canvas()->update();

    notifyMainWindow( LTE_CMD_SELECTION_PROPS_CHANGE );
}

CommandType SelectionPropsChangedCommand::rtti() const
{
    return LTE_CMD_SELECTION_PROPS_CHANGE;
}

QString SelectionPropsChangedCommand::propertyKey() const
{
    Q_ASSERT(d);
    return d->key;
}

const TemplateDict& SelectionPropsChangedCommand::templates() const
{
    Q_ASSERT(d);
    return d->selection;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: SelectionPropsChangedCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
