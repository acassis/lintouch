// $Id: TemplatePropertiesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplatePropertiesDialog.cpp --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 15 July 2004
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

#include <qcolordialog.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmap.h>
#include <qptrlist.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qwidget.h>
#include <qwidgetstack.h>

#include <lt/templates/Connection.hh>
#include <lt/templates/Property.hh>
#include <lt/templates/Template.hh>
#include <lt/templates/Variable.hh>

#include "Binding.hh"
#include "Bindings.hh"
#include "EditablePanelViewContainer.hh"
#include "EditablePanelView.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"
#include "TemplateBindingsControls.hh"
#include "TemplatePropertiesControls.hh"

#include "ChangeBindingCommand.hh"
#include "SelectionPropsChangedCommand.hh"
#include "RenameTemplateCommand.hh"
#include "MoveNResizeSelectionCommand.hh"

#include "TemplatePropertiesDialog.hh"

using namespace lt;

#define ITEM_GENERAL    "G"
#define ITEM_PROPERTY   "P"
#define ITEM_IOPIN      "I"

/*
 * private data
 */
struct TemplatePropertiesDialog::TemplatePropertiesDialogPrivate
{
    /*
     * ctor
     */
    TemplatePropertiesDialogPrivate() : mw(NULL), pw(NULL),
            clearOrFill(false), genpage(NULL), genName(NULL),
            genType(NULL), genX(NULL), genY(NULL), genZ(NULL),
            genW(NULL), genH(NULL) {}

    /*
     * Returns list of variables for given connection name, including one
     * empty line.
     */
    const QStringList connectionsList() const {

        Q_ASSERT(pw);

        QStringList l;
        l.append("");

        const ConnectionDict::Keys keys = pw->project()->connections().keys();
        unsigned keys_count = pw->project()->connections().count();
        for(unsigned i=0; i<keys_count; ++i) {
            l.append(keys[i]);
        }
        return l;
    }

    /*
     * Returns list of variables for given connection name, including one
     * empty line.
     */
    const QStringList
        variablesList(const QString& id, const IOPin::TypeList& tl) const {

        Q_ASSERT(pw);

        QStringList l;
        l.append("");

        if(id.isEmpty() || !pw->project()->connections().contains(id)) {
            return l;
        }

        const Connection* c = pw->project()->connections()[id];
        Q_ASSERT(c);

        const VariableDict& vd = c->variables();
        unsigned count = c->variables().count();
        for(unsigned i=0; i<count; ++i) {
            if(tl.findIndex(vd[i]->type()) >= 0) {
                l.append(vd.keyAt(i));
            }
        }
        return l;
    }

    /*
     * Update all comboboxes for new values so we don't need to recreate
     * everything on simple binding change.
     */
    void updateAll() {

        bool o_cof = clearOrFill;
        clearOrFill = true;

        const Bindings& bs = pw->project()->bindings();
        const BindingsList& bl = bs.bindingsByTemplate(pw->currentView(),
                pw->currentPanel(), selection.currentKey());

        // get all connection comboboxes
        QValueList<const TemplateBindingsCombobox*> connections =
            conn2var.keys();
        for(QValueListIterator<const TemplateBindingsCombobox*> it =
                connections.begin();
                it != connections.end(); ++it) {

            TemplateBindingsCombobox* vc = conn2var[*it];
            TemplateBindingsCombobox* cc = var2conn[vc];

            const QString& iopin = (*it)->iopin();

            const BindingPtr b = findIOPin(bl, iopin);

            Q_ASSERT(selection.count() == 1);
            Q_ASSERT(selection.current() != NULL);
            Q_ASSERT(selection.current()->iopins().contains(iopin));
            const IOPin::TypeList& tl = selection.current()->
                iopins()[iopin]->types();

            // clear and refill the connections always
            cc->clear();
            cc->insertStringList(connectionsList());

            // clear the variables and insert one empty line only
            vc->clear();

            if(!b.isNull()) {
                cc->setCurrentText(b->connection());
                vc->insertStringList(variablesList(b->connection(), tl));
                vc->setCurrentText(b->variable());
            } else {
                vc->insertStringList(variablesList(QString::null, tl));
                vc->setCurrentText("");
            }
        }
        clearOrFill = o_cof;
    }

    /*
     * Find and return BindingPtr from BindingsList with requested
     * iopin.
     */
    const BindingPtr findIOPin(const BindingsList& bl, const QString& iopin) {

        if(bl.count() < 1) {
            // no bindings to find in
            return BindingPtr();
        }

        for(BindingsList::const_iterator it=bl.begin(); it != bl.end(); ++it) {
            if((*it)->iopin() == iopin) {
                return *it;
            }
        }
        return BindingPtr();
    }

    /*
     * Create the General page.
     */
    void createGenPage(QWidget* parent, TemplatePropertiesDialog* tpd) {

        Q_ASSERT(genpage == NULL);
        Q_ASSERT(parent);
        Q_ASSERT(tpd);

        genpage = new QWidget(parent);
        Q_CHECK_PTR(genpage);

        int line = 0;
        QGridLayout* gl = new QGridLayout(genpage, 6, 4, 0, 5);
        Q_CHECK_PTR(gl);

        // create the nice page-label
        QLabel* lb = new QLabel(tr("General"), genpage);
        Q_CHECK_PTR(lb);
        QFont f = lb->font(); f.setBold(true);
        lb->setFont(f);
        lb->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        lb->setTextFormat(Qt::PlainText);
        lb->setFrameShape(QFrame::Box);
        lb->setFrameShadow(QFrame::Sunken);
        lb->setSizePolicy(
                QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
        lb->setPaletteBackgroundColor(lb->paletteBackgroundColor().dark(120));
        gl->addMultiCellWidget(lb, line, line, 0, 7);
        ++line;

        // type + edit
        QLabel* tl2 = new QLabel(tr("Type:"), genpage);
        Q_CHECK_PTR(tl2);
        tl2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        gl->addWidget(tl2, line, 0);
        genType = new QLabel(genpage);
        gl->addMultiCellWidget(genType, line, line, 1, 3);
        ++line;

        // name + edit
        QLabel* tl1 = new QLabel(tr("Name:"), genpage);
        Q_CHECK_PTR(tl1);
        tl1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        gl->addWidget(tl1, line, 0);
        genName = new QLineEdit(genpage);
        gl->addMultiCellWidget(genName, line, line, 1, 3);
        connect(genName, SIGNAL(returnPressed()), tpd, SLOT(nameEditedSlot()));
        connect(genName, SIGNAL(lostFocus()), tpd, SLOT(nameEditedSlot()));
        ++line;

        // position x
        QLabel* tl3 = new QLabel(tr("X:"), genpage);
        Q_CHECK_PTR(tl3);
        tl3->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        gl->addWidget(tl3, line, 0);
        genX = new QSpinBox(0, INT_MAX, 1, genpage);
        gl->addWidget(genX, line, 1);
        connect(genX, SIGNAL(valueChanged(int)),
                tpd, SLOT(posXEditedSlot(int)));

        // width
        QLabel* tl5 = new QLabel(tr("Width:"), genpage);
        Q_CHECK_PTR(tl5);
        tl5->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        gl->addWidget(tl5, line, 2);
        genW = new QSpinBox(1, INT_MAX, 1, genpage);
        gl->addWidget(genW, line, 3);
        connect(genW, SIGNAL(valueChanged(int)),
                tpd, SLOT(posWEditedSlot(int)));
        ++line;

        // position y
        QLabel* tl4 = new QLabel(tr("Y:"), genpage);
        Q_CHECK_PTR(tl4);
        tl4->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        gl->addWidget(tl4, line, 0);
        genY = new QSpinBox(0, INT_MAX, 1, genpage);
        gl->addWidget(genY, line, 1);
        connect(genY, SIGNAL(valueChanged(int)),
                tpd, SLOT(posYEditedSlot(int)));

        // height
        QLabel* tl6 = new QLabel(tr("Height:"), genpage);
        Q_CHECK_PTR(tl6);
        tl6->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        gl->addWidget(tl6, line, 2);
        genH = new QSpinBox(1, INT_MAX, 1, genpage);
        gl->addWidget(genH, line, 3);
        connect(genH, SIGNAL(valueChanged(int)),
                tpd, SLOT(posHEditedSlot(int)));
        ++line;

        // layer
        QLabel* tl7 = new QLabel(tr("Layer:"), genpage);
        Q_CHECK_PTR(tl7);
        tl7->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        gl->addWidget(tl7, line, 0);
        genZ = new QSpinBox(0, INT_MAX, 1, genpage);
        gl->addWidget(genZ, line, 1);
        connect(genZ, SIGNAL(valueChanged(int)),
                tpd, SLOT(posZEditedSlot(int)));
        ++line;

        QSpacerItem* spacer1 = new QSpacerItem(20, 40,
                QSizePolicy::Minimum, QSizePolicy::Expanding);
        gl->addItem(spacer1, line, 0);
    }

    /*
     * Update general page from given template
     */
    void updateGenPage(const TemplateDict& td) {

        Q_ASSERT(td.count() > 0);

        bool o_cof = clearOrFill;
        clearOrFill = true;

        const Template* t = td[0];
        Q_ASSERT(t);

        genName->setText(td.keyAt(0));
        genType->setText(QString("%1 (%2)").arg(t->type()).arg(t->library()));

        const QRect& r = t->virtualRect();

        genX->setValue(r.left());
        genY->setValue(r.top());

        genZ->setValue((int)t->z());

        genW->setValue(r.width());
        genH->setValue(r.height());

        updateGenPageValues();

        clearOrFill = o_cof;
    }

    /*
     * Update general page from given template
     */
    void updateGenPageValues() {
        genNameVal = genName->text().stripWhiteSpace();
        genTypeVal = genType->text().stripWhiteSpace();
        genXVal = genX->value();
        genYVal = genY->value();
        genZVal = genZ->value();
        genWVal = genW->value();
        genHVal = genH->value();
    }

    /*
     * QMap of old values of String Properties
     */
    QMap<TemplateLineEdit*,QString> oldValues;

    /*
     * QPtrList of all link type controls
     */
    QPtrList<TemplatePropertyChoice> links;

    /*
     * QPtrList of all link type controls
     */
    QPtrList<TemplatePropertyChoice> resources;

    /**
     * MainWindow pointer
     */
    MainWindow* mw;

    /**
     * The currently focused ProjectWindow.
     */
    ProjectWindow* pw;

    /*
     * Flag. True if clearTabs method is running. Inside these methods we
     * don't want to handle valueChange signals.
     */
    bool clearOrFill;

    /*
     * Current selection
     */
    TemplateDict selection;

    /*
     * Current selected template library.
     */
    QString templateLibrary;

    /*
     * Current selected template library.
     */
    QString templateType;

    /*
     * Last selected Tab
     */
    QString currentTab;

    /*
     * map list item to qwidget
     */
    QMap<QListViewItem*,QWidget*> pages;

    /*
     * Maps connections combobox to variables combobox so we can refill the
     * variables combobox on connections combobox change very quickly.
     */
    QMap<const TemplateBindingsCombobox*,
        TemplateBindingsCombobox*> conn2var;

    /*
     * Maps variables combobox to connections combobox so we can find the
     * right values for creating undo-command very quickly.
     */
    QMap<const TemplateBindingsCombobox*,
        TemplateBindingsCombobox*> var2conn;

    BindingsList bl;

    /**
     * General page
     */
    QWidget* genpage;

    /*
     * General page line edits
     */
    QLineEdit* genName;
    QLabel* genType;
    QSpinBox* genX;
    QSpinBox* genY;
    QSpinBox* genZ;
    QSpinBox* genW;
    QSpinBox* genH;

    /*
     * General page line edits old values
     */
    QString genNameVal;
    QString genTypeVal;
    int genXVal;
    int genYVal;
    int genZVal;
    int genWVal;
    int genHVal;
};

TemplatePropertiesDialog::TemplatePropertiesDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/) :
    TemplatePropertiesDialogBase(parent, name, fl),
    d(new TemplatePropertiesDialogPrivate)
{
    Q_CHECK_PTR(d);

    Q_ASSERT(m_grid);
    Q_ASSERT(m_grid->inherits("QGridLayout"));
    m_grid->setColStretch(0, 3);
    m_grid->setColStretch(1, 5);

    Q_ASSERT(parent);
    d->mw = parent;

    m_list->setSorting(-1);
    m_list->header()->hide();
    m_list->setColumnWidthMode(0, QListView::Maximum);

    clearAll();
    connect(m_list, SIGNAL(selectionChanged(QListViewItem*)),
            this, SLOT(currentSelectionChangedSlot(QListViewItem*)));

    d->createGenPage(m_stack, this);
    Q_ASSERT(d->genpage);
}

TemplatePropertiesDialog::~TemplatePropertiesDialog()
{
    delete d;
}

void TemplatePropertiesDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t)
{
    Q_ASSERT(d);

    if(pw != d->pw) {

        d->pw = pw;

        clearAll();

        updateCaption();

        if(pw && pw->m_viewContainer->currentPanelView()) {

            if(reconstructTabs(
                        d->pw->m_viewContainer->
                        currentPanelView()->currentSelection())) {

                m_list->setEnabled(true);
                m_stack->setEnabled(true);
                d->updateGenPage(
                        d->pw->m_viewContainer->
                        currentPanelView()->currentSelection());
            }

        } else {

            // disable the tabs
            m_list->setEnabled(false);
            m_stack->setEnabled(false);
        }
    } else if(t == LTE_CMD_SELECTION_CHANGE) {

        EditablePanelView* epv = d->pw->m_viewContainer->currentPanelView();

        TemplateDict td;

        if(epv) {
            td = epv->currentSelection();
        }

        clearAll();

        updateCaption();

        if(td.currentIndex() != -1) {

            Q_ASSERT(pw);
            Q_ASSERT(pw->m_viewContainer->currentPanelView());
            if(reconstructTabs(td)) {
                d->updateGenPage(td);
                m_list->setEnabled(true);
                m_stack->setEnabled(true);
            }
        }

    } else if(t == LTE_CMD_SELECTION_PROPS_CHANGE) {
        Q_ASSERT(d->pw==pw);
        if(pw->lastCommand() &&
                pw->lastCommand()->rtti() == LTE_CMD_SELECTION_PROPS_CHANGE) {

            SelectionPropsChangedCommand* cmd =
                (SelectionPropsChangedCommand*)pw->lastCommand();
            Q_ASSERT(cmd);

            d->clearOrFill = true;
            emit(propertyChanged(cmd->propertyKey()));
            d->clearOrFill = false;
        }

    } else if(t == LTE_CMD_SELECTION_MOVE_RESIZE) {
        Q_ASSERT(d->pw==pw);
        if(pw->lastCommand() &&
                pw->lastCommand()->rtti() == LTE_CMD_SELECTION_MOVE_RESIZE) {

            MoveNResizeSelectionCommand* cmd =
                (MoveNResizeSelectionCommand*)pw->lastCommand();
            Q_ASSERT(cmd);

            const TemplateDict& td = cmd->templates();
            d->updateGenPage(td);
        }

    } else if(t == LTE_CMD_RESOURCE_ADD || t == LTE_CMD_RESOURCE_DELETE ||
            t == LTE_CMD_RESOURCE_RENAME) {

        QStringList ls = d->pw->project()->unpackedResources()->ls();
        for(QPtrListIterator<TemplatePropertyChoice>
                it(d->resources); it.current(); ++it) {
            (*it)->fillFromResources(ls);
        }


    } else if(t == LTE_CMD_PANEL_ADD || t == LTE_CMD_PANEL_DELETE ||
            t == LTE_CMD_PANEL_RENAME || t == LTE_CMD_PANEL_ORDER_CHANGE) {

        const PanelMap& pm = d->pw->project()->
            views()[d->pw->currentView()].panels();
        for(QPtrListIterator<TemplatePropertyChoice> it(d->links);
                it.current(); ++it) {
            (*it)->fillFromPanelMap(pm);
        }

    } else if(t == LTE_CMD_BINDING_CHANGE ||
            t == LTE_CMD_CONNECTION_ADD || t == LTE_CMD_CONNECTION_DEL ||
            t == LTE_CMD_CONNECTION_RENAME || t == LTE_CMD_VARIABLE_ADD ||
            t == LTE_CMD_VARIABLE_DEL || t == LTE_CMD_VARIABLE_RENAME ||
            t == LTE_CMD_TEMPLATE_RENAME) {

        d->updateAll();

        if(t == LTE_CMD_TEMPLATE_RENAME) {
            updateCaption();
        }
    }
}

void TemplatePropertiesDialog::updateCaption()
{
    Q_ASSERT(d);

    if(d->pw && d->pw->m_viewContainer->currentPanelView()) {

        const TemplateDict& td = d->pw->m_viewContainer->currentPanelView()
            ->currentSelection();

        if(td.currentIndex() < 0) {
            setCaption(
                    QString(tr("%1 Template Properties"
                            " - Lintouch Editor"))
                    .arg(d->pw->projectFileInfo().baseName()));
        } else {
            setCaption(
                    QString(tr("%1/%2 Template Properties"
                            " - Lintouch Editor"))
                    .arg(d->pw->projectFileInfo().baseName())
                    .arg(td.currentKey()));
        }

    } else {
        setCaption(tr("Template Properties - Lintouch Editor"));
    }
}

void TemplatePropertiesDialog::clearAll()
{
    Q_ASSERT(d);

    bool o_cof = d->clearOrFill;
    d->clearOrFill = true;

    // prepare the empty list
    m_list->clear();
    for(QMap<QListViewItem*,QWidget*>::iterator it = d->pages.begin();
            it != d->pages.end(); ++it) {

        if(it.data() != d->genpage) {
            // delete only data, keys are deleted by m_list->clear()
            delete it.data();
        }
    }
    d->pages.clear();

    // add at least one empty widget to fix artefacts
    QWidget* w = new QWidget(m_stack);
    Q_CHECK_PTR(w);
    m_stack->addWidget(w);
    m_stack->raiseWidget(w);

    d->var2conn.clear();
    d->conn2var.clear();

    d->oldValues.clear();

    d->selection.clear();

    d->resources.clear();
    d->links.clear();

    m_list->setEnabled(false);
    m_stack->setEnabled(false);

    d->clearOrFill = o_cof;
}

bool TemplatePropertiesDialog::reconstructTabs(const TemplateDict& td)
{
    Q_ASSERT(d);
    Q_ASSERT(d->pw);

    if(td.currentIndex() == -1) {
        // return if there is no template selected
        return false;
    }

    // make up our own selection with only one template for now.
    d->selection.insert(td.keyAt(td.currentIndex()), td[td.currentKey()]);

    const Template* t = d->selection.current();
    Q_ASSERT(t);

    // iopins stuff
    EditorProject* ep = d->pw->project();
    Q_ASSERT(ep);
    d->bl = ep->bindings().bindingsByTemplate(d->pw->currentView(),
            d->pw->currentPanel(), d->selection.currentKey());

    QListViewItem* item = NULL;

    // general page
    item = new QListViewItem(m_list, NULL, tr("General"),
            ITEM_GENERAL);
    Q_CHECK_PTR(item);
    d->pages[item] = d->genpage;

    // PROPERTIES
    const LTMap<PropertyDict>& groups = t->propertyGroups();

    for(unsigned i = 0; i < groups.count(); ++i) {
        item = new QListViewItem(m_list, item, groups.keyAt(i), ITEM_PROPERTY);
        Q_CHECK_PTR(item);
    }

    // IOPINS
    const LTMap <IOPinDict>& igroups = t->iopinGroups();
    for(unsigned i = 0; i < igroups.count(); ++i) {
        item = new QListViewItem(m_list, item, igroups.keyAt(i), ITEM_IOPIN);
        Q_CHECK_PTR(item);
    }

    // PREVIOUSLY SELECTED TAB
    if(t->library() == d->templateLibrary && t->type() == d->templateType) {
        item = m_list->findItem(d->currentTab,0);
        m_list->setSelected(item, true);

    } else {
        m_list->setSelected(m_list->firstChild(), true);
    }

    d->templateLibrary = t->library();
    d->templateType = t->type();

    return true;
}

void TemplatePropertiesDialog::currentSelectionChangedSlot(QListViewItem* i)
{
    Q_ASSERT(d);
    Q_ASSERT(i);

    QString label = i->text(0);
    QWidget* w = NULL;

    if(d->pages.contains(i)) {
        w = d->pages[i];
    } else {
        if((i->text(1)==ITEM_PROPERTY)) {
            w = createTab(label);
        } else {
            w = createIOTab(label);
        }
        Q_ASSERT(w);
        d->pages[i] = w;
    }

    Q_ASSERT(w);
    m_stack->raiseWidget(w);

    d->currentTab = label;

    // store old values so we can create undo-commands with old and new
    // values
    if(w == d->genpage) {
        d->updateGenPageValues();
    }
}

/////////////////////////////////// PROPERTIES //////////////////////////////
void TemplatePropertiesDialog::textChangedSlot(
        TemplateLineEdit* p, const QString& text)
{
    Q_ASSERT(p);
    Q_ASSERT(d);
    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    Template* t = d->selection.current();
    const QString& key = p->key();

    Q_ASSERT(t->properties().contains(key));

    // get the property
    Property* pp = t->properties()[key];
    Q_ASSERT(pp);

    // set the new value
    pp->decodeValue(text);

    // notify template about the change
    t->propertiesChanged();

    d->pw->m_viewContainer->updatePanelOnTop();
}

void TemplatePropertiesDialog::textEditFinishedSlot(
        TemplateLineEdit* x)
{
    Q_ASSERT(d);
    Q_ASSERT(x);
    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    // check we have a current template
    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    // check we have the old value
    Q_ASSERT(d->oldValues.contains(x));

    QString oldVal = d->oldValues[x];

    // reset String properties of all Templates in selection to old
    // value so the new undo command can fetch them; need not to call
    // propertiesChanged because the value will change in few CPU cycles
    // to new value
    Template* t = d->selection.current();
    Q_ASSERT(t);

    Property* p = t->properties()[x->key()];
    Q_ASSERT(p);

    if(oldVal != x->text()) {

        bool b = p->decodeValue(oldVal);
        Q_ASSERT(b);

        Q_ASSERT(((oldVal.isNull())?
                    (p->encodeValue().isEmpty()):(p->encodeValue() == oldVal)));

        executeCommand(x->key(), x->text());

        d->oldValues[(TemplateLineEdit*)x] = x->text();
    }
}

void TemplatePropertiesDialog::intChangedSlot(
        TemplateSpinBox* sb, int val)
{
    Q_ASSERT(sb);
    Q_ASSERT(d);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    executeCommand(sb->key(), QString::number(val));
}

void TemplatePropertiesDialog::penPropertyChangedSlot(
        TemplatePropertyPen* tp, const QPen& p)
{
    Q_ASSERT(tp);
    Q_ASSERT(d);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    executeCommand(tp->key(), Property::penToString(p));
}

void TemplatePropertiesDialog::brushPropertyChangedSlot(
        TemplatePropertyBrush* tb, const QBrush& b)
{
    Q_ASSERT(tb);
    Q_ASSERT(d);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    executeCommand(tb->key(), Property::brushToString(b));
}

void TemplatePropertiesDialog::fontPropertyChangedSlot(
        TemplatePropertyFont* tf, const QFont& f)
{
    Q_ASSERT(tf);
    Q_ASSERT(d);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    executeCommand(tf->key(), Property::fontToString(f));
}

void TemplatePropertiesDialog::colorPropertyChangedSlot(
        TemplatePropertyColor* tc, const QColor& c)
{
    Q_ASSERT(tc);
    Q_ASSERT(d);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    executeCommand(tc->key(), Property::colorToString(c));
}

void TemplatePropertiesDialog::doubleEditFinishedSlot(
        TemplateDoubleEdit* x, double val)
{
    Q_ASSERT(x);
    Q_ASSERT(d);
    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    Template* t = d->selection.current();
    const QString& key = x->key();

    // get the property
    Q_ASSERT(t->properties().contains(key));
    if(t->properties()[key]->asFloatingPoint() == val) {
        return;
    }

    executeCommand(x->key(), QString::number(val));
}

void TemplatePropertiesDialog::choicePropertyChanged(
        TemplatePropertyChoice* x, const QString& val)
{
    Q_ASSERT(x);
    Q_ASSERT(d);

    if(d->clearOrFill) {
        // ignore signals when we're filling the values
        return;
    }

    executeCommand(x->key(), val);
}

void TemplatePropertiesDialog::executeCommand(const QString& key,
        const QString& value)
{
    Q_ASSERT(d);
    Q_ASSERT(d->mw);

    // check we have a current template
    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    SelectionPropsChangedCommand* cmd =
        new SelectionPropsChangedCommand(d->selection,
                key, value, d->pw, d->mw);
    Q_CHECK_PTR(cmd);
    d->pw->executeCommand(cmd);
}

QWidget* TemplatePropertiesDialog::createTab(const QString& gr) {

    Q_ASSERT(d);

    const Template* t = d->selection.current();
    Q_ASSERT(t);

    const PropertyDict& p = t->properties(gr);

    QWidget* w = new QWidget(m_stack);
    Q_CHECK_PTR(w);

    if(p.count() < 1) {
        // quite extreme sutiation, should not happen
        return w;
    }

    QGridLayout* gl = new QGridLayout(w, p.count() + 2, 2, 5);
    Q_CHECK_PTR(gl);
    gl->setMargin(0);

    // create the nice page-label
    QLabel* lb = new QLabel(gr, w);
    Q_CHECK_PTR(lb);
    QFont f = lb->font(); f.setBold(true);
    lb->setFont(f);
    lb->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    lb->setTextFormat(Qt::PlainText);
    lb->setFrameShape(QFrame::Box);
    lb->setFrameShadow(QFrame::Sunken);
    lb->setSizePolicy(
            QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
    lb->setPaletteBackgroundColor(lb->paletteBackgroundColor().dark(120));
    gl->addMultiCellWidget(lb, 0, 0, 0, 1);

    QWidget* x = NULL;
    for(unsigned j = 0; j < p.count(); ++j) {
        x = new QLabel(p[j]->label(), w);
        Q_CHECK_PTR(x);
        x->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        gl->addWidget(x, j+1, 0);

        Property::Type type = p[j]->type();
        if(type == Property::StringType) {
            x = new TemplateLineEdit(p.keyAt(j), p, w);
            Q_CHECK_PTR(x);
            connect(x,
                    SIGNAL(textChanged(TemplateLineEdit*,
                            const QString&)),
                    this,
                    SLOT(textChangedSlot(TemplateLineEdit*,
                            const QString&)));
            connect(x,
                    SIGNAL(textEditFinished(TemplateLineEdit*)),
                    this,
                    SLOT(textEditFinishedSlot(TemplateLineEdit*)));
            d->oldValues[(TemplateLineEdit*)x] = p[j]->asString();

        } else if(type == Property::IntegerType) {
            x = new TemplateSpinBox(p.keyAt(j), p,
                    INT_MIN, INT_MAX, w);
            Q_CHECK_PTR(x);
            connect(x, SIGNAL(valueChanged(TemplateSpinBox*,int)),
                    this, SLOT(intChangedSlot(TemplateSpinBox*,int)));

        } else if(type == Property::FloatingPointType) {
            x = new TemplateDoubleEdit(p.keyAt(j), p, w);
            Q_CHECK_PTR(x);
            connect(x,
                    SIGNAL(
                    doubleEditFinished(TemplateDoubleEdit*,double)),
                    this,
                    SLOT(
                    doubleEditFinishedSlot(TemplateDoubleEdit*,double)));

        } else if(type == Property::LinkType || type == Property::ChoiceType ||
                type == Property::ResourceType) {

            x = new TemplatePropertyChoice(p.keyAt(j), p, w);
            Q_CHECK_PTR(x);
            TemplatePropertyChoice* pc =
                (TemplatePropertyChoice*)x;

            if(type == Property::LinkType) {
                pc->updateFromLinkProperty(p[j],
                        d->pw->project()->
                        views()[d->pw->currentView()].panels());
                d->links.append(pc);

            } else if(type == Property::ChoiceType) {
                pc->updateFromChoiceProperty(p[j]);

            } else if(type == Property::ResourceType) {
                pc->updateFromResourceProperty(p[j],
                        d->pw->project()->unpackedResources()->ls());
                d->resources.append(pc);
            }

            connect(pc, SIGNAL(itemActivated(TemplatePropertyChoice*,
                            const QString&)),
                    this,
                    SLOT(choicePropertyChanged(TemplatePropertyChoice*,
                            const QString&)));

        } else if (type == Property::PenType) {
            x = new TemplatePropertyPen(p.keyAt(j), p, w);
            Q_CHECK_PTR(x);
            connect(x,
                    SIGNAL(
                        penPropertyChanged(TemplatePropertyPen*,
                            const QPen&)),
                    this, SLOT(
                        penPropertyChangedSlot(TemplatePropertyPen*,
                            const QPen&)));

        } else if (type == Property::BrushType) {
            x = new TemplatePropertyBrush(p.keyAt(j), p, w);
            Q_CHECK_PTR(x);
            connect(x,
                    SIGNAL(
                        brushPropertyChanged(TemplatePropertyBrush*,
                            const QBrush&)),
                    this, SLOT(
                        brushPropertyChangedSlot(TemplatePropertyBrush*,
                            const QBrush&)));

        } else if (type == Property::FontType) {
            x = new TemplatePropertyFont(p.keyAt(j),p , w);
            Q_CHECK_PTR(x);
            connect(x,
                    SIGNAL(
                        fontPropertyChanged(TemplatePropertyFont*,
                            const QFont&)),
                    this,
                    SLOT(
                        fontPropertyChangedSlot(TemplatePropertyFont*,
                            const QFont&)));

        } else if(type == Property::ColorType) {
            x = new TemplatePropertyColor(p.keyAt(j), p, w);
            Q_CHECK_PTR(x);
            connect(x,
                    SIGNAL(colorPropertyChanged(
                            TemplatePropertyColor*,const QColor&)),
                    this,
                    SLOT(colorPropertyChangedSlot(
                            TemplatePropertyColor*,const QColor&)));

        } else {
            x = new QLabel(tr("unsupported property type"), w);
            Q_CHECK_PTR(x);
        }

        if(!x->isA("QLabel")) {
            // connect the control to update signal
            connect(this,
                    SIGNAL(propertyChanged(const QString&)),
                    x,
                    SLOT(updateSlot(const QString&)));

            // enable/disable property
            x->setEnabled(p[j]->isEnabled());
        }

        gl->addWidget(x, j+1, 1);
    }

    gl->addItem(new QSpacerItem(1, 1,
                QSizePolicy::Minimum, QSizePolicy::Expanding),
            p.count()+1, 1);

    return w;
}

/////////////////////////////////// IOPINS //////////////////////////////

void TemplatePropertiesDialog::connectionComboboxChanged(
        TemplateBindingsCombobox* c,
        const QString& val)
{
    Q_ASSERT(d);
    Q_ASSERT(c);
    Q_ASSERT(d->mw);

    // ignore if there is no real change
    if(c->oldValue() == val) {
        return;
    }

    //update variables combo; fill it in with variables of selected
    //connection!
    TemplateBindingsCombobox* v = d->conn2var[c];
    Q_ASSERT(v);

    // create a undo-command for this change
    ChangeBindingCommand* cmd =
        new ChangeBindingCommand(
                d->selection.currentKey(), c->iopin(),
                c->oldValue(), val, // connections
                v->oldValue(), "",
                d->pw, d->mw);
    Q_CHECK_PTR(cmd);

    d->pw->executeCommand(cmd);
}

void TemplatePropertiesDialog::variableComboboxChanged(
        TemplateBindingsCombobox* v,
        const QString& val)
{
    Q_ASSERT(d);
    Q_ASSERT(v);
    Q_ASSERT(d->mw);

    // ignore if there is no real change
    if(v->oldValue() == val) {
        return;
    }

    // find the corresponding connection combobox
    TemplateBindingsCombobox* c = d->var2conn[v];
    Q_ASSERT(c);

    // create a undo-command for this change
    ChangeBindingCommand* cmd =
        new ChangeBindingCommand(
                d->selection.currentKey(), c->iopin(),
                c->oldValue(), c->oldValue(), // connections are the same
                v->oldValue(), val,
                d->pw, d->mw);
    Q_CHECK_PTR(cmd);

    d->pw->executeCommand(cmd);
}

QWidget* TemplatePropertiesDialog::createIOTab(const QString& gr) {

    Q_ASSERT(d);
    Q_ASSERT(d->mw);

    const ConnectionDict& cd = d->pw->project()->connections();

    const Template* t = d->selection.current();
    Q_ASSERT(t);

    const IOPinDict& pd = t->iopins(gr);

    QWidget* w = new QWidget(m_stack);
    Q_CHECK_PTR(w);

    if(pd.count() < 1) {
        // empty group?
        return w;
    }

    QGridLayout* gl = new QGridLayout(w, pd.count() + 2, 3, 5);
    Q_CHECK_PTR(gl);
    gl->setMargin(0);

    // create the nice page-label
    QLabel* lb = new QLabel(gr, w);
    Q_CHECK_PTR(lb);
    QFont f = lb->font(); f.setBold(true);
    lb->setFont(f);
    lb->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    lb->setTextFormat(Qt::PlainText);
    lb->setFrameShape(QFrame::Box);
    lb->setFrameShadow(QFrame::Sunken);
    lb->setSizePolicy(
            QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
    lb->setPaletteBackgroundColor(lb->paletteBackgroundColor().dark(120));
    gl->addMultiCellWidget(lb, 0, 0, 0, 2);

    QString conn, var;
    bool enable;

    for(unsigned j = 0; j < pd.count(); ++j) {
        // values to set to connections and variables comboboexs
        conn = "";
        var = "";

        // find the Binding to this iopin
        if(d->bl.count()) {
            for(BindingsList::const_iterator it = d->bl.begin();
                    it != d->bl.end(); ++it) {

                if((*it)->iopin() == pd.keyAt(j)) {
                    conn = (*it)->connection();
                    var = (*it)->variable();
                    break;
                }
            }
        }

        // create the label for iopin
        QLabel* l = new QLabel(pd[j]->label(), w);
        Q_CHECK_PTR(l);
        l->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        gl->addWidget(l, j+1, 0);

        // create the connections combobox
        TemplateBindingsCombobox* cc =
            new TemplateBindingsCombobox(pd.keyAt(j), false, w);
        Q_CHECK_PTR(cc);
        // set list of connections
        cc->insertStringList(d->connectionsList());
        cc->setCurrentText(conn);
        connect(cc, SIGNAL(activated(
                        TemplateBindingsCombobox*,const QString&)),
                this, SLOT(
                    connectionComboboxChanged(
                        TemplateBindingsCombobox*,const QString&)));
        gl->addWidget(cc, j+1, 1);

        // create the variables combobox
        TemplateBindingsCombobox* vc =
            new TemplateBindingsCombobox(pd.keyAt(j), false, w);
        Q_CHECK_PTR(vc);
        //fill in the varibales for the connection
        vc->insertStringList(d->variablesList(conn, pd[j]->types()));
        vc->setCurrentText(var);
        connect(vc, SIGNAL(activated(
                        TemplateBindingsCombobox*,const QString&)),
                this, SLOT(
                    variableComboboxChanged(
                        TemplateBindingsCombobox*,const QString&)));
        d->conn2var[cc] = vc;
        d->var2conn[vc] = cc;
        gl->addWidget(vc, j+1, 2);

        // disable controls if the connection is not available
        enable = conn.isEmpty() || cd.contains(conn);
        vc->setEnabled(enable);
        cc->setEnabled(enable);
    }

    gl->addItem(new QSpacerItem(1, 1,
                QSizePolicy::Minimum, QSizePolicy::Expanding),
            pd.count()+1, 1);

    return w;
}

//////////////////////////////// GENERAL PAGE ///////////////////////////
void TemplatePropertiesDialog::nameEditedSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(d->genName);

    QString newName = d->genName->text().stripWhiteSpace();

    if(d->clearOrFill || d->genNameVal.isNull() || newName == d->genNameVal) {
        // no change or already changed but not refreshed
        return;
    }

    if(newName.isNull() || newName.isEmpty()) {
        // invalid value
        d->genName->setText(d->genNameVal);
        return;
    }

    RenameTemplateCommand* cmd = new RenameTemplateCommand(d->genNameVal,
            newName, d->pw, (MainWindow*)parent());
    Q_CHECK_PTR(cmd);
    d->pw->executeCommand(cmd);

    // signal that rename is in progress
    d->genNameVal = QString::null;
}

void TemplatePropertiesDialog::posXEditedSlot(int v)
{
    Q_ASSERT(d);
    Q_ASSERT(d->genX);

    if(d->clearOrFill || d->genXVal == -1 || v == d->genXVal) {
        // no change or already changed but not refreshed
        return;
    }

    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    LTMap<QRect> rm = MoveNResizeSelectionCommand::rects2Map(d->selection);
    LTMap<double> lm = MoveNResizeSelectionCommand::layers2Map(d->selection);

    rm[0].moveLeft(v);

    // signal that reX is in progress
    d->genXVal = -1;

    genMoveNResizeCommand(rm, lm);
}

void TemplatePropertiesDialog::posYEditedSlot(int v)
{
    Q_ASSERT(d);
    Q_ASSERT(d->genY);

    if(d->clearOrFill || d->genYVal == -1 || v == d->genYVal) {
        // no change or already changed but not refreshed
        return;
    }

    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    LTMap<QRect> rm = MoveNResizeSelectionCommand::rects2Map(d->selection);
    LTMap<double> lm = MoveNResizeSelectionCommand::layers2Map(d->selection);

    rm[0].moveTop(v);

    // signal that reY is in progress
    d->genYVal = -1;

    genMoveNResizeCommand(rm, lm);
}

void TemplatePropertiesDialog::posWEditedSlot(int v)
{
    Q_ASSERT(d);
    Q_ASSERT(d->genW);

    if(d->clearOrFill || d->genWVal == -1 || v == d->genWVal) {
        // no change or already changed but not refreshed
        return;
    }

    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    LTMap<QRect> rm = MoveNResizeSelectionCommand::rects2Map(d->selection);
    LTMap<double> lm = MoveNResizeSelectionCommand::layers2Map(d->selection);

    rm[0].setWidth(v);

    // signal that reW is in progress
    d->genWVal = -1;

    genMoveNResizeCommand(rm, lm);
}

void TemplatePropertiesDialog::posHEditedSlot(int v)
{
    Q_ASSERT(d);
    Q_ASSERT(d->genH);

    if(d->clearOrFill || d->genHVal == -1 || v == d->genHVal) {
        // no change or already changed but not refreshed
        return;
    }

    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    LTMap<QRect> rm = MoveNResizeSelectionCommand::rects2Map(d->selection);
    LTMap<double> lm = MoveNResizeSelectionCommand::layers2Map(d->selection);

    rm[0].setHeight(v);

    // signal that reH is in progress
    d->genHVal = -1;

    genMoveNResizeCommand(rm, lm);
}

void TemplatePropertiesDialog::posZEditedSlot(int v)
{
    Q_ASSERT(d);
    Q_ASSERT(d->genZ);

    if(d->clearOrFill || d->genZVal == -1 || v == d->genZVal) {
        // no change or already changed but not refreshed
        return;
    }

    Q_ASSERT(d->selection.count() == 1);
    Q_ASSERT(d->selection.currentIndex() == 0);

    LTMap<QRect> rm = MoveNResizeSelectionCommand::rects2Map(d->selection);
    LTMap<double> lm = MoveNResizeSelectionCommand::layers2Map(d->selection);

    lm[0] = v;

    // signal that reZ is in progress
    d->genZVal = -1;

    genMoveNResizeCommand(rm, lm);
}

void TemplatePropertiesDialog::genMoveNResizeCommand(
        lt::LTMap<QRect> rm, lt::LTMap<double> lm)
{
    MoveNResizeSelectionCommand* cmd = new MoveNResizeSelectionCommand(
            d->selection, rm, lm, d->pw, (MainWindow*)parent());
    Q_CHECK_PTR(cmd);
    d->pw->executeCommand(cmd);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplatePropertiesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
