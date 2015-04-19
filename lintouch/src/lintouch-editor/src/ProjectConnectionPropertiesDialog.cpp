// $Id: ProjectConnectionPropertiesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectConnectionPropertiesDialog.cpp --
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 29 September 2004
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

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qptrlist.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qwidgetstack.h>

#include <lt/templates/Connection.hh>
#include "lt/templates/Property.hh"

#include "AddDelConnectionCommand.hh"
#include "ConnectionPropsChangedCommand.hh"
#include "Bindings.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectConnectionPropertiesControls.hh"
#include "ProjectConnectionPropertiesDialog.hh"
#include "ProjectWindow.hh"

using namespace lt;

struct ProjectConnectionPropertiesDialog::
ProjectConnectionPropertiesDialogPrivate
{
    ProjectConnectionPropertiesDialogPrivate() : pw(NULL), mw(NULL),
        asNew(false), cmd(NULL), next_conn(1) {}

    /**
     * Create a new QWidget with properties of given Connection type
     */
    QWidget* createTypePropsWidget(const QString& type,
            ProjectConnectionPropertiesDialog* target,
            QWidget* parent)
    {
        QWidget* w = new QWidget(parent);
        Q_CHECK_PTR(w);

        const QMap<QString, QString>& props = type2properties[type];

        if(props.count() < 1) {
            // no properties for this connection type
            QVBoxLayout* l = new QVBoxLayout(w);
            Q_CHECK_PTR(l);
            l->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum,
                        QSizePolicy::Expanding));
            l->addWidget(new QLabel(tr("There are no properties."), w));
            l->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum,
                        QSizePolicy::Expanding));
            return w;
        }

        QPtrList<ProjectConnectionPropertiesEdit> el;

        QGridLayout* gl = new QGridLayout(w, props.count()+1, 2, 5);
        Q_CHECK_PTR(gl);

        int i=0;
        for(QMapConstIterator<QString,QString> it = props.begin();
                it != props.end(); ++it) {

            QLabel* l = new QLabel(it.key(),w);
            Q_CHECK_PTR(l);
            gl->addWidget(l, i, 0);

            ProjectConnectionPropertiesEdit* e =
                new ProjectConnectionPropertiesEdit(type,
                        it.key(), w);
            Q_CHECK_PTR(e);
            e->setText(it.data());  // set default value
            el.append(e);           // add in the edit list
            gl->addWidget(e, i, 1);
            connect(e, SIGNAL(valueChanged(const QString&,
                            const QString&,const QString&)),
                    target, SLOT(propertyChangedSlot(const QString&,
                            const QString&,const QString&)));

            connect(target, SIGNAL(finishEdit()), e, SLOT(valueReadySlot()));

            // line counter
            ++i;
        }
        // spacer as the last control; takes the space between lowes edit
        // and the bottom if the widgetstack
        gl->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum,
                    QSizePolicy::Expanding));

        type2edits[type] = el;

        return w;
    }

    /**
     * Fills all edits with thier default values.
     */
    void fillWithDefaults() {

        // iterator over all connection types
        QMapIterator<QString,
            QPtrList<ProjectConnectionPropertiesEdit> > it =
                type2edits.begin();

        // iterate over all connections types
        for(; it != type2edits.end(); ++it) {
            // get the list of Edits for the connection type
            QPtrList<ProjectConnectionPropertiesEdit> & el =
                it.data();

            if(el.count() > 0) {
                // get the map of default properties for current connection
                const QMap<QString,QString>& props =
                    mw->pluginManager().connectionProperties(it.key());

                // iterate over all Edits
                for( QPtrListIterator<
                        ProjectConnectionPropertiesEdit>
                        ei(el); ei.current(); ++ei) {

                    // set the value of the Edit from defalts
                    (*ei)->setText(props[(*ei)->property()]);
                }
            }
        }
    }

    /**
     * Fills all edits with the values in connection properties.
     */
    void fillWithValues(const Connection* c) {

        Q_ASSERT(pw);

        if(c->isAvailable()) {

            // dict of properties of this one connection
            const PropertyDict& pd = c->properties();

            // get list of edits that bellongs to this connection type
            QPtrList<ProjectConnectionPropertiesEdit> & el =
                type2edits[c->type()];

            // iterate over all Edits
            for( QPtrListIterator<
                    ProjectConnectionPropertiesEdit>
                    ei(el); ei.current(); ++ei) {

                // set the value of the Edit from defalts
                if(pd.contains((*ei)->property())) {
                    (*ei)->setText(pd[(*ei)->property()]->asString());
                }
            }
        }
    }

    /*
     * Fill the oldValues of given Connection
     */
    void fillOldFor(const Connection* c)
    {
        // dict of properties of this one connection
        const PropertyDict& pd = c->properties();

        // iterate over all Connections properties and sotre them to
        // oldValues
        for(unsigned j = 0; j < pd.count(); ++j) {
            oldValues[pd.keyAt(j)] = pd[j]->asString();
        }
    }

    /*
     * Current ProjectWindow holder
     */
    ProjectWindow* pw;

    /*
     * Current ProjectWindow holder
     */
    MainWindow* mw;

    /*
     * Name of the connection
     */
    QString connectionName;

    /*
     * Map of connection type to QWidget of it's properties
     */
    QMap<QString,QWidget*> type2widget;

    /*
     * Map of connection type to connection properties map
     */
    QMap<QString, QMap<QString,QString> > type2properties;

    /*
     * Map of old values of one connections. Used to create undo command for
     * connection properties change only
     */
    QMap<QString,QString> oldValues;

    /*
     * Map of connection type to list of
     * ProjectConnectionPropertiesEdit.
     */
    QMap<QString, QPtrList<ProjectConnectionPropertiesEdit> >
        type2edits;

    /*
     * True if the dialog is started from NewConnection.
     */
    bool asNew;

    /*
     * An undocommand for runNew mode.
     */
    Command* cmd;

    /*
     * Next Conn's No.
     */
    int next_conn;
};

ProjectConnectionPropertiesDialog:: ProjectConnectionPropertiesDialog(
        MainWindow* mw, QWidget* parent, bool modal /*= true*/) :
    ProjectConnectionPropertiesDialogBase(parent, NULL, modal, 0),
    d(new ProjectConnectionPropertiesDialogPrivate)
{
    Q_CHECK_PTR(d);
    d->mw = mw;

    // fill in the list of available connection types
    const QStringList& pl = mw->pluginManager().plugins();
    Q_ASSERT(m_connType);
    for(QStringList::const_iterator it = pl.begin(); it != pl.end(); ++it) {
        if(mw->pluginManager().isAvailable(*it)) {
            m_connType->insertItem(*it);
        }
    }
}

ProjectConnectionPropertiesDialog:: ~ProjectConnectionPropertiesDialog()
{
    delete d;
}

void ProjectConnectionPropertiesDialog::accept()
{
    apply();
    ProjectConnectionPropertiesDialogBase::accept();
}

void ProjectConnectionPropertiesDialog::apply()
{
    Q_ASSERT(d);
    //create an undo command and execute it

    emit(finishEdit());

    if(d->asNew) {
        // just create the command and *dont* execute it, we'll do it on
        // close, because there may be quite a mess if the user selects a
        // connection type, press <apply> then selects different type and
        // again press <apply>.

        if(d->cmd) {
            // if there is previous command, delete it!
            delete d->cmd;
        }
        d->cmd = new AddDelConnectionCommand(
                d->connectionName, m_connType->currentText(),
                d->type2properties[m_connType->currentText()],
                d->pw, d->mw);
        Q_CHECK_PTR(d->cmd);

        ++d->next_conn;

    } else {
        if(d->cmd) {
            // if there is previous command, delete it!
            delete d->cmd;
        }

        d->cmd = new ConnectionPropsChangedCommand(
                d->connectionName,
                d->oldValues, d->type2properties[m_connType->currentText()],
                d->pw, d->mw);
        Q_CHECK_PTR(d->cmd);
    }

    m_applyButton->setEnabled(false);
}

void ProjectConnectionPropertiesDialog::done(int r)
{
    Q_ASSERT(d);

    // exeute command posibly created by apply method
    if(d->cmd) {
        d->pw->executeCommand(d->cmd);
        d->cmd = NULL;
    }

    ProjectConnectionPropertiesDialogBase::done(r);
}

void ProjectConnectionPropertiesDialog::typeChangedSlot(
        const QString& t)
{
    Q_ASSERT(d);

    m_applyButton->setEnabled(true);

    if(!d->type2properties.contains(t)) {
        // fill in default properties
        Q_ASSERT(d->mw);
        d->type2properties[t] =
            d->mw->pluginManager().connectionProperties(t);
    }

    if(!d->type2widget.contains(t)) {
        // create page with properties
        QWidget* w = d->createTypePropsWidget(t, this, m_widgetStack);
        Q_ASSERT(w);
        d->type2widget[t] = w;  // map the conn. type to the widget
        m_widgetStack->addWidget(w); // add it to the widgetstack
    }

    m_widgetStack->raiseWidget(d->type2widget[t]);
    m_widgetStack->setFocus();
}

void ProjectConnectionPropertiesDialog::propertyChangedSlot(
        const QString& c, const QString& p, const QString& v)
{
    Q_ASSERT(d);

    d->type2properties[c][p] = v;
    Q_ASSERT(d->type2properties[c][p] == v);

    m_applyButton->setEnabled(true);
}

int ProjectConnectionPropertiesDialog::runNew(const QString& cn,
        ProjectWindow* pw)
{
    Q_ASSERT(d);
    Q_ASSERT(cn.isNull());
    Q_ASSERT(pw);
    Q_ASSERT(d->mw);

    d->pw = pw;
    d->asNew = true;

    // create local list of all connections keys
    QMap<QString,int> keys(d->pw->project()->connections().mapping());
    const ConnectionDict& dcd = d->pw->project()->disabledConnections();

    for(unsigned i = 0; i < dcd.count(); ++i) {
        keys.insert(dcd.keyAt(i), i);
    }

    d->connectionName = Command::generateSafeName(
            tr("Connection%1"), d->next_conn, keys);

    // clear the old values
    d->type2properties.clear();
    d->fillWithDefaults();

    // specia caption for new Connection
    setCaption(tr("New Connection Properties - Lintouch Editor"));

    // enable connection type combo for new Connection
    m_connType->setEnabled(true);
    m_connType->setCurrentItem(0);
    typeChangedSlot(m_connType->currentText());

    // disable the buttons that need not to be enabled now
    m_applyButton->setEnabled(false);

    return exec();
}

int ProjectConnectionPropertiesDialog::runEdit(const QString& cn,
        ProjectWindow* pw)
{
    Q_ASSERT(d);
    Q_ASSERT(cn.isNull() == false);
    Q_ASSERT(pw);
    Q_ASSERT(d->mw);

    d->pw = pw;
    d->connectionName = cn;
    d->asNew = false;

    // clear the old values
    d->type2properties.clear();
    d->oldValues.clear();
    d->fillWithDefaults();

    // specia caption for new Connection
    setCaption(tr("%1 Properties - Lintouch Editor").arg(cn));

    // enable connection type combo for new Connection
    const Connection* c = pw->project()->connections()[cn];
    Q_ASSERT(c);
    m_connType->setCurrentText(c->type());
    m_connType->setEnabled(false);
    typeChangedSlot(c->type());

    // fill or use widget with properties for the Connection type
    d->fillWithValues(c);

    d->fillOldFor(c);

    // disable the buttons that need not to be enabled now
    m_applyButton->setEnabled(false);

    return exec();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectConnectionPropertiesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
