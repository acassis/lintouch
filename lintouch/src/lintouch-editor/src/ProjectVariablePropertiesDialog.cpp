// $Id: ProjectVariablePropertiesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectVariablePropertiesDialog.cpp -- 
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

#include <lt/templates/Variable.hh>
#include "lt/templates/Property.hh"
#include <lt/templates/IOPin.hh>

#include "AddDelVariableCommand.hh"
#include "VariablePropsChangedCommand.hh"
#include "Bindings.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectConnectionPropertiesControls.hh"
#include "ProjectWindow.hh"

#include "ProjectVariablePropertiesDialog.hh"

using namespace lt;

struct ProjectVariablePropertiesDialog::
ProjectVariablePropertiesDialogPrivate
{
    ProjectVariablePropertiesDialogPrivate() : pw(NULL), mw(NULL),
        asNew(false), cmd(NULL), next_var(1) {}

    /**
     * Create a new QWidget with properties of current connection name.
     */
    QWidget* createPropsWidget(const QString& vt,
            ProjectVariablePropertiesDialog* target, QWidget* parent)
    {
        Q_ASSERT(!cn.isEmpty());

        QWidget* w = new QWidget(parent);
        Q_CHECK_PTR(w);

        const QMap<QString, QString>& props =
            mw->pluginManager().variablesProperties(ct, IOPin::decodeType(vt));

        if(props.count() < 1) {
            // no properties for this connection
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
                new ProjectConnectionPropertiesEdit(vn,
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

        conn2edits[ckey(ct, vt)] = el;

        return w;
    }

    /**
     * Fills edits of current connection with thier default values.
     */
    void fillWithDefaults() {

        fillWithDefaults("bit");
        fillWithDefaults("number");
        fillWithDefaults("string");
    }

    /**
     * Fills edits of current connection with thier default values.
     */
    void fillWithDefaults(const QString& vt) {

        QPtrList<ProjectConnectionPropertiesEdit> & el =
            conn2edits[ckey(ct,vt)];

        if(el.count() > 0) {
            // get the map of default properties for current connection
            const QMap<QString,QString>& props =
                mw->pluginManager().variablesProperties(ct,
                        IOPin::decodeType(vt));

            // iterate over all Edits
            for( QPtrListIterator<ProjectConnectionPropertiesEdit>
                    it(el); it.current(); ++it) {

                // set the value of the Edit from defalts
                (*it)->setText(props[(*it)->property()]);
            }
        }
    }

    /**
     * Fills all edits with the values in connection properties.
     */
    void fillFromVar(const QString& vt) {

        Q_ASSERT(pw);

        // list of connections
        const Connection* c = pw->project()->connections()[cn];
        Q_ASSERT(c);

        const Variable* v = c->variables()[vn];
        Q_ASSERT(v);

        // dict of properties of this one variable
        const PropertyDict& pd = v->properties();

        // for(unsigned i=0;i<pd.count();++i){
        //     qDebug("pd: %s=%s", pd.keyAt(i).latin1(),
        //             pd[i]->asString().latin1());
        // }

        // get list of edits that bellongs to this connection type
        QPtrList<ProjectConnectionPropertiesEdit> & el =
            conn2edits[ckey(ct, vt)];
        // for( QMap<QString,ProjectConnectionPropertiesEdit*>
        //         ::iterator it = el.begin(); it != el.end(); ++it) {
        //     qDebug("el: %s=%s", it.key().latin1(),
        //             it.data()->text().latin1());
        // }

        // iterate over all Edits
        for( QPtrListIterator<ProjectConnectionPropertiesEdit>
                it(el); it.current(); ++it) {

            // set the value of the Edit from defalts
            if(pd.contains((*it)->property())) {
                (*it)->setText(pd[(*it)->property()]->asString());
            }
        }
    }

    /*
     * Fill the oldValues of given Connection
     */
    void fillOldFor(const Variable* v)
    {
        // dict of properties of this one connection
        const PropertyDict& pd = v->properties();

        // iterate over all Connections properties and sotre them to
        // oldValues
        for(unsigned j = 0; j < pd.count(); ++j) {
            oldValues[pd.keyAt(j)] = pd[j]->asString();
        }
    }

    /*
     * Create a QString key from connection type and variable type
     */
    const QString ckey(const QString& ct, const QString& vt)
    {
        return QString("%1*%2").arg(ct).arg(vt);
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
    QString cn;

    /*
     * Type of the connection
     */
    QString ct;

    /*
     * Name of the connection
     */
    QString vn;

    /*
     * Map of connection type+variable type to QWidget of it's properties
     */
    QMap<QString,QWidget*> conn2widget;

    /*
     * Map of connection type to connection properties map
     */
    QMap<QString, QMap<QString,QString> > conn2props;

    /*
     * Map of old values of one Variable. Used to create undo command for
     * Variable properties change only
     */
    QMap<QString,QString> oldValues;

    /*
     * List of ProjectConnectionPropertiesEdit.
     */
    QMap<QString, QPtrList<ProjectConnectionPropertiesEdit> >
        conn2edits;

    /*
     * True if the dialog is started from NewVariable.
     */
    bool asNew;

    /*
     * An undocommand for runNew mode.
     */
    Command* cmd;

    /*
     * Next var's No.
     */
    int next_var;
};

ProjectVariablePropertiesDialog::
    ProjectVariablePropertiesDialog( QWidget* parent,
        bool modal /*= true*/) :
    ProjectVariablePropertiesDialogBase(parent, NULL, modal, 0),
    d(new ProjectVariablePropertiesDialogPrivate)
{
    Q_CHECK_PTR(d);

    //refill m_varType with strings that for sure will not be translated,
    //make sure the *Base.ui class does not fill anything to the combo
    m_varType->clear();
    m_varType->insertItem( "bit" );
    m_varType->insertItem( "number" );
    m_varType->insertItem( "string" );
}

ProjectVariablePropertiesDialog::
    ~ProjectVariablePropertiesDialog()
{
    delete d;
}

void ProjectVariablePropertiesDialog::accept()
{
    apply();
    ProjectVariablePropertiesDialogBase::accept();
}

void ProjectVariablePropertiesDialog::apply()
{
    Q_ASSERT(d);

    // force all edits to save their values
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
        d->cmd = new AddDelVariableCommand(
                d->cn, d->vn, m_varType->currentText(),
                d->conn2props[d->ckey(d->ct,m_varType->currentText())],
                d->pw, d->mw);
        Q_CHECK_PTR(d->cmd);

        ++d->next_var;

    } else {
        if(d->cmd) {
            // if there is previous command, delete it!
            delete d->cmd;
        }

        d->cmd = new VariablePropsChangedCommand(
                d->cn, d->vn,
                d->oldValues,
                d->conn2props[d->ckey(d->ct,m_varType->currentText())],
                d->pw, d->mw);
        Q_CHECK_PTR(d->cmd);
    }

    m_applyButton->setEnabled(false);
}

void ProjectVariablePropertiesDialog::done(int r)
{
    Q_ASSERT(d);

    // exeute command posibly created by apply method
    if(d->cmd) {
        d->pw->executeCommand(d->cmd);
        d->cmd = NULL;
    }

    ProjectVariablePropertiesDialogBase::done(r);
}

void ProjectVariablePropertiesDialog::typeChangedSlot(const QString& t)
{
    Q_ASSERT(d);

    m_applyButton->setEnabled(true);

    const QString ckey = d->ckey(d->ct, t);

    if(!d->conn2widget.contains(ckey)) {
        // create page with properties
        QWidget* w = d->createPropsWidget(t, this, m_widgetStack);
        Q_ASSERT(w);
        d->conn2widget[ckey] = w;  // map the conn. type to the widget
        m_widgetStack->addWidget(w); // add it to the widgetstack
        d->fillWithDefaults(t);
        if(!d->asNew) {
            d->fillFromVar(t);
        }
    }

    m_widgetStack->raiseWidget(d->conn2widget[ckey]);
    m_widgetStack->setFocus();
}

void ProjectVariablePropertiesDialog::propertyChangedSlot(
        const QString& /*vn*/, const QString& p, const QString& v)
{
    Q_ASSERT(d);

    const QString ckey = d->ckey(d->ct,m_varType->currentText());

    if(!d->conn2props[ckey].contains(p) || d->conn2props[ckey][p] != v) {
        d->conn2props[ckey][p] = v;
        Q_ASSERT(d->conn2props[ckey][p] == v);

        m_applyButton->setEnabled(true);
    }
}

int ProjectVariablePropertiesDialog::runNew(const QString& cn,
        ProjectWindow* pw, MainWindow* mw)
{
    Q_ASSERT(d);
    Q_ASSERT(cn.isNull() == false);
    Q_ASSERT(pw);

    d->pw = pw;
    d->mw = mw;
    d->cn = cn;

    const Connection* c = pw->project()->connections()[cn];
    Q_ASSERT(c);
    d->ct = c->type();

    d->vn = Command::generateSafeName( tr("Variable%1"), d->next_var,
            c->variables().mapping());
    d->asNew = true;

    QString ckey = d->ckey(d->ct, m_varType->currentText());

    // clear the old values
    d->conn2props.remove(ckey);

    // specia caption for edited Variable
    setCaption(tr("%1/New Variable - Lintouch Editor").arg(cn));

    d->fillWithDefaults();

    // enable connection type combo for edit new Variable
    m_varType->setEnabled(true);
    typeChangedSlot(m_varType->currentText());

    // disable the buttons that need not to be enabled now
    m_applyButton->setEnabled(false);

    return exec();
}

int ProjectVariablePropertiesDialog::runEdit(const QString& cn,
        const QString& vn, ProjectWindow* pw, MainWindow* mw)
{
    Q_ASSERT(d);
    Q_ASSERT(cn.isNull() == false);
    Q_ASSERT(vn.isNull() == false);
    Q_ASSERT(pw);

    // qDebug("runEdit(%s,%s)", cn.latin1(), vn.latin1());

    d->pw = pw;
    d->mw = mw;
    d->cn = cn;
    d->vn = vn;
    d->asNew = false;

    const Connection* c = pw->project()->connections()[cn];
    Q_ASSERT(c);

    d->ct = c->type();

    const Variable* v = c->variables()[vn];
    Q_ASSERT(v);

    QString ckey = d->ckey(d->ct, m_varType->currentText());

    // clear the old values
    d->conn2props.remove(ckey);
    d->oldValues.clear();

    // specia caption for edited Variable
    setCaption(tr("%1/%2 Properties - Lintouch Editor").arg(cn).arg(vn));

    // set type of the edited variable
    IOPin::TypeList tl;
    tl.append(v->type());
    m_varType->setCurrentText(IOPin::encodeTypes(tl));

    // disable connection type combo for edit existing Variable
    m_varType->setEnabled(false);

    d->fillWithDefaults();

    // fill or use widget with properties for the Variable type
    d->fillFromVar(m_varType->currentText());
    typeChangedSlot(m_varType->currentText());

    d->fillOldFor(v);

    // disable the buttons that need not to be enabled now
    m_applyButton->setEnabled(false);

    return exec();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectVariablePropertiesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
