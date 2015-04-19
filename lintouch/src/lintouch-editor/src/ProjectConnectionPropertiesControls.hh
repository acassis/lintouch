// $Id: ProjectConnectionPropertiesControls.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectConnectionPropertiesControls.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 18 October 2004
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

#ifndef _LTEDITORPROJECTCONNECTIONPROPERTIESCONTROLS_HH
#define _LTEDITORPROJECTCONNECTIONPROPERTIESCONTROLS_HH

#include <qlineedit.h>

/**
 * Special version of QLineEdit to suit our need to edit a Connection
 * property.
 */
class ProjectConnectionPropertiesEdit : public QLineEdit
{
    Q_OBJECT
public:
    /**
     * ctor
     */
    ProjectConnectionPropertiesEdit(const QString& c,
            const QString& p, QWidget* parent) :
        QLineEdit(parent), conn(c), prop(p)
    {
        connect(this, SIGNAL(returnPressed()), SLOT(valueReadySlot()));
        connect(this, SIGNAL(lostFocus()), SLOT(valueReadySlot()));
        connect(this, SIGNAL(textChanged(const QString&)),
                this, SLOT(textChangedSlot(const QString&)));
    }

    /**
     * dtor
     */
    virtual ~ProjectConnectionPropertiesEdit() {}

    /**
     * Property id getter.
     */
    const QString& property() const { return prop; }

public slots:
    /**
     * Called on returnPressed and lostFocus signals; emits the new value
     * along with connection and property ids.
     */
    void valueReadySlot() {
        emit(valueChanged(conn, prop, text()));
    }

protected slots:
    /**
     * Reemit the valueChanged on textChanged signal.
     */
    void textChangedSlot(const QString& s) {
        emit(valueChanged(conn, prop, s));
    }

signals:
    /*
     * Emits the new value along with connection and property ids.
     */
    void valueChanged(const QString& c, const QString& p, const QString& v);

private:
    QString conn;
    QString prop;
};

#endif /* _LTEDITORPROJECTCONNECTIONPROPERTIESCONTROLS_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectConnectionPropertiesControls.hh 1169 2005-12-12 15:22:15Z modesto $
