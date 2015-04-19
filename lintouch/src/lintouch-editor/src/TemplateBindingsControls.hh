// $Id: TemplateBindingsControls.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateBindingsControls.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 13 October 2004
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

#ifndef _LTEDITORTEMPLATEBINDINGSCONTROLS_HH
#define _LTEDITORTEMPLATEBINDINGSCONTROLS_HH

#include <qcombobox.h>

/**
 * Special version of QComboBox to suite our needs to edit a Binding.
 */
class TemplateBindingsCombobox : public QComboBox {

    Q_OBJECT
public:
    /**
     * ctor
     */
    TemplateBindingsCombobox(const QString& iopin, bool rw,
            QWidget* parent)
        : QComboBox(rw, parent), iop(iopin)
    {
        connect( this, SIGNAL(activated(const QString&)),
                SLOT(activatedSlot(const QString&)));
    }

    /**
     * dtor
     */
    virtual ~TemplateBindingsCombobox() {}

    /**
     * Getter for oldVal.
     */
    const QString& oldValue() const { return oldVal; }

    /**
     * Getter for oldVal.
     */
    void setOldValue(const QString& v) { oldVal = v; }

    /**
     * Sets current text of combobox; also sets the oldValue for us.
     */
    virtual void setCurrentText(const QString& v) {
        setOldValue(v);
        QComboBox::setCurrentText(v);
    }

    /**
     * Getter for iopin key.
     */
    const QString& iopin() const { return iop; }

public slots:
    /**
     * Clears all including oldValue.
     */
    void clear() {
        QComboBox::clear();
        oldVal = QString::null;
    }

protected slots:
    /**
     * Slots to reemit activated
     */
    void activatedSlot(const QString& val) {
        emit activated(this, val);
    }

signals:
    /**
     * Reemits activated with additiona parameter.
     */
    void activated(TemplateBindingsCombobox*, const QString&);

private:
    /**
     * The old value
     */
    QString oldVal;

    /**
     * The iopin key.
     */
    QString iop;
};

#endif /* _LTEDITORTEMPLATEBINDINGSCONTROLS_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateBindingsControls.hh 1169 2005-12-12 15:22:15Z modesto $
