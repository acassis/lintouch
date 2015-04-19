// $Id: ProjectServerLoggingPropertiesDialog.cpp 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: ProjectServerLoggingPropertiesDialog.cpp --
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: Fri, 07 Apr 2006
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
#include <qlistbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qptrlist.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qwidgetstack.h>

#include "lt/templates/Property.hh"
#include "ServerLogging.hh"

#include "AddDelServerLoggingCommand.hh"
#include "ServerLoggingPropsChangedCommand.hh"
#include "Bindings.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectConnectionPropertiesControls.hh"
#include "ProjectServerLoggingPropertiesDialog.hh"
#include "ProjectWindow.hh"

using namespace lt;

struct ProjectServerLoggingPropertiesDialog::
ProjectServerLoggingPropertiesDialogPrivate
{
    ProjectServerLoggingPropertiesDialogPrivate() : pw(NULL), mw(NULL),
        asNew(false), cmd(NULL), next_sl(1) {}

    /**
     * Create a new QWidget with properties of given ServerLogging type
     */
    QWidget* createTypePropsWidget(const QString& type,
            ProjectServerLoggingPropertiesDialog* target,
            QWidget* parent)
    {
        QWidget* w = new QWidget(parent);
        Q_CHECK_PTR(w);

        const QMap<QString, QString>& props = type2properties[type];

        if(props.count() < 1) {
            // no properties for this ServerLogging type
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

        // iterator over all ServerLogging types
        QMapIterator<QString,
            QPtrList<ProjectConnectionPropertiesEdit> > it =
                type2edits.begin();

        // iterate over all ServerLogging types
        for(; it != type2edits.end(); ++it) {
            // get the list of Edits for the ServerLogging type
            QPtrList<ProjectConnectionPropertiesEdit> & el =
                it.data();

            if(el.count() > 0) {
                // get the map of default properties for current
                // ServerLogging
                const QMap<QString,QString>& props =
                    mw->serverLoggingManager().serverLoggingProperties(
                            it.key());

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
     * Fills all edits with the values in ServerLogging properties.
     */
    void fillWithValues(const ServerLogging* c) {

        Q_ASSERT(pw);

        if(c->isAvailable()) {

            // dict of properties of this one ServerLogging
            const PropertyDict& pd = c->properties();

            // get list of edits that bellongs to this ServerLogging type
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
     * Fill the oldValues of given ServerLogging
     */
    void fillOldFor(const ServerLogging* c)
    {
        // dict of properties of this one ServerLogging
        const PropertyDict& pd = c->properties();

        // iterate over all ServerLoggings properties and sotre them to
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
     * Name of the ServerLogging
     */
    QString serverLoggingName;

    /*
     * Map of ServerLogging type to QWidget of it's properties
     */
    QMap<QString,QWidget*> type2widget;

    /*
     * Map of ServerLogging type to ServerLogging properties map
     */
    QMap<QString, QMap<QString,QString> > type2properties;

    /*
     * Map of old values of one ServerLogging. Used to create undo command for
     * ServerLogging properties change only
     */
    QMap<QString,QString> oldValues;

    /*
     * Map of ServerLogging type to list of
     * ProjectConnectionPropertiesEdit.
     */
    QMap<QString, QPtrList<ProjectConnectionPropertiesEdit> >
        type2edits;

    /*
     * True if the dialog is started from NewServerLogging.
     */
    bool asNew;

    /*
     * An undocommand for runNew mode.
     */
    Command* cmd;

    /*
     * Next Conn's No.
     */
    int next_sl;
};

ProjectServerLoggingPropertiesDialog:: ProjectServerLoggingPropertiesDialog(
        MainWindow* mw, QWidget* parent, bool modal /*= true*/) :
    ProjectServerLoggingPropertiesDialogBase(parent, NULL, modal, 0),
    d(new ProjectServerLoggingPropertiesDialogPrivate)
{
    Q_CHECK_PTR(d);
    d->mw = mw;

    // fill in the list of available ServerLogging types
    const QStringList& pl = mw->serverLoggingManager().plugins();
    Q_ASSERT(m_slType);
    for(QStringList::const_iterator it = pl.begin(); it != pl.end(); ++it) {
        if(mw->serverLoggingManager().isAvailable(*it)) {
            m_slType->insertItem(*it);
        }
    }
}

ProjectServerLoggingPropertiesDialog:: ~ProjectServerLoggingPropertiesDialog()
{
    delete d;
}

void ProjectServerLoggingPropertiesDialog::accept()
{
    apply();
    ProjectServerLoggingPropertiesDialogBase::accept();
}

void ProjectServerLoggingPropertiesDialog::apply()
{
    Q_ASSERT(d);
    //create an undo command and execute it

    emit(finishEdit());

    if(d->asNew) {
        // just create the command and *dont* execute it, we'll do it on
        // close, because there may be quite a mess if the user selects a
        // ServerLogging type, press <apply> then selects different type and
        // again press <apply>.

        if(d->cmd) {
            // if there is previous command, delete it!
            delete d->cmd;
        }
        d->cmd = new AddDelServerLoggingCommand(
                d->serverLoggingName, m_slType->currentText(),
                d->type2properties[m_slType->currentText()],
                d->pw, d->mw);
        Q_CHECK_PTR(d->cmd);

        ++d->next_sl;

    } else {
        if(d->cmd) {
            // if there is previous command, delete it!
            delete d->cmd;
        }

        d->cmd = new ServerLoggingPropsChangedCommand(
                d->serverLoggingName,
                d->oldValues, d->type2properties[m_slType->currentText()],
                d->pw, d->mw);
        Q_CHECK_PTR(d->cmd);
    }

    m_applyButton->setEnabled(false);
}

void ProjectServerLoggingPropertiesDialog::done(int r)
{
    Q_ASSERT(d);

    // exeute command posibly created by apply method
    if(d->cmd) {
        d->pw->executeCommand(d->cmd);
        d->cmd = NULL;
    }

    ProjectServerLoggingPropertiesDialogBase::done(r);
}

void ProjectServerLoggingPropertiesDialog::typeChangedSlot(
        const QString& t)
{
    Q_ASSERT(d);

    m_applyButton->setEnabled(true);

    if(!d->type2properties.contains(t)) {
        // fill in default properties
        Q_ASSERT(d->mw);
        d->type2properties[t] =
            d->mw->serverLoggingManager().serverLoggingProperties(t);
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

void ProjectServerLoggingPropertiesDialog::propertyChangedSlot(
        const QString& c, const QString& p, const QString& v)
{
    Q_ASSERT(d);

    d->type2properties[c][p] = v;
    Q_ASSERT(d->type2properties[c][p] == v);

    m_applyButton->setEnabled(true);
}

int ProjectServerLoggingPropertiesDialog::runNew(const QString& cn,
        ProjectWindow* pw)
{
    Q_ASSERT(d);
    Q_ASSERT(cn.isNull());
    Q_ASSERT(pw);
    Q_ASSERT(d->mw);

    d->pw = pw;
    d->asNew = true;

    // create local list of all ServerLogging keys
    QMap<QString,int> keys(d->pw->project()->serverLoggings().mapping());

    d->serverLoggingName = Command::generateSafeName(
            tr("ServerLogging%1"), d->next_sl, keys);

    // clear the old values
    d->type2properties.clear();
    d->fillWithDefaults();

    // specia caption for new ServerLogging
    setCaption(tr("New ServerLogging Properties - Lintouch Editor"));

    // enable ServerLogging type combo for new ServerLogging
    m_slType->setEnabled(true);
    m_slType->setCurrentItem(0);
    typeChangedSlot(m_slType->currentText());

    // disable the buttons that need not to be enabled now
    m_applyButton->setEnabled(false);

    return exec();
}

int ProjectServerLoggingPropertiesDialog::runEdit(const QString& cn,
        ProjectWindow* pw)
{
    Q_ASSERT(d);
    Q_ASSERT(cn.isNull() == false);
    Q_ASSERT(pw);
    Q_ASSERT(d->mw);

    d->pw = pw;
    d->serverLoggingName = cn;
    d->asNew = false;

    // clear the old values
    d->type2properties.clear();
    d->oldValues.clear();
    d->fillWithDefaults();

    // specia caption for new ServerLogging
    setCaption(tr("%1 Properties - Lintouch Editor").arg(cn));

    // enable ServerLogging type combo for new ServerLogging
    const ServerLogging* c = pw->project()->serverLoggings()[cn];
    Q_ASSERT(c);
    if(m_slType->listBox()->findItem(c->type()) == NULL) {
        qWarning("UNKNOWN TYPE!");
    } else {
        m_slType->setCurrentText(c->type());
        typeChangedSlot(c->type());
    }
    m_slType->setEnabled(false);

    // fill or use widget with properties for the ServerLogging type
    d->fillWithValues(c);

    d->fillOldFor(c);

    // disable the buttons that need not to be enabled now
    m_applyButton->setEnabled(false);

    return exec();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectServerLoggingPropertiesDialog.cpp 1564 2006-04-07 13:33:15Z modesto $
