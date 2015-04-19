// $Id: Bindings.cpp 1203 2006-01-05 11:37:22Z modesto $
//
//   FILE: Bindings.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 05 October 2004
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

#include "lib/Bindings.hh"

#include <qmap.h>
#include <qstringlist.h>

struct Bindings::BindingsPrivate
{
    typedef QMap<QString,BindingsList> IndexLevel1;
    typedef QMap<QString,QMap<QString,BindingsList> > IndexLevel2;
    typedef QMap<QString,QMap<QString,QMap<QString,BindingsList> > >
        IndexLevel3;

    /**
     * Actual bindings list.
     */
    BindingsList bindings;

    /**
     * Actual bindings list.
     */
    BindingsList null;

    /**
     * Connection index.
     */
    IndexLevel1 indexConn;

    /**
     * Connection+Variable index.
     */
    IndexLevel2 indexConnVar;

    /**
     * View index.
     */
    IndexLevel1 indexView;

    /**
     * View+Panel
     */
    IndexLevel2 indexViewPanel;

    /**
     * View+Panel+Template
     */
    IndexLevel3 indexViewPanelTempl;

    /**
     * Update all indexes for the one *added* Binding.
     */
    void updateIndexesOnAdd(BindingPtr b)
    {
        BindingsList& l1 = indexConn[b->connection()];
        l1.append(b);

        IndexLevel1& cv1 = indexConnVar[b->connection()];
        BindingsList& l2 = cv1[b->variable()];
        l2.append(b);

        BindingsList& v1 = indexView[b->view()];
        v1.append(b);

        IndexLevel1& vp1 = indexViewPanel[b->view()];
        BindingsList& l3 = vp1[b->panel()];
        l3.append(b);

        IndexLevel2& vpt1 = indexViewPanelTempl[b->view()];
        IndexLevel1& vpt2 = vpt1[b->panel()];
        BindingsList& l4 = vpt2[b->templ()];
        l4.append(b);
    }

    /**
     * Update all indexes for the one *delete* Binding.
     */
    void updateIndexesOnDel(BindingPtr b)
    {
        BindingsList& l1 = indexConn[b->connection()];
        l1.remove(b);

        IndexLevel1& cv1 = indexConnVar[b->connection()];
        BindingsList& l2 = cv1[b->variable()];
        l2.remove(b);

        BindingsList& v1 = indexView[b->view()];
        v1.remove(b);

        IndexLevel1& vp1 = indexViewPanel[b->view()];
        BindingsList& l3 = vp1[b->panel()];
        l3.remove(b);

        IndexLevel2& vpt1 = indexViewPanelTempl[b->view()];
        IndexLevel1& vpt2 = vpt1[b->panel()];
        BindingsList& l4 = vpt2[b->templ()];
        l4.remove(b);
    }
};

Bindings::Bindings() : d(new BindingsPrivate)
{
    Q_CHECK_PTR(d);
}

Bindings::~Bindings()
{
    delete d;
}

const BindingPtr Bindings::addBinding(
        const QString& conn /*= QString::null*/,
        const QString& var /*= QString::null*/,
        const QString& view /*= QString::null*/,
        const QString& panel /*= QString::null*/,
        const QString& templ /*= QString::null*/,
        const QString& iopin /*= QString::null*/)
{
    Q_ASSERT(d);

    BindingPtr bp = binding(conn, var, view, panel, templ, iopin);
    if(!bp.isNull()) {
        return bp;
    }

    bp = BindingPtr(new Binding(conn, var, view, panel, templ, iopin));
    addBinding(bp);

    return bp;
}

const bool Bindings::addBinding(const BindingPtr b)
{
    Q_ASSERT(d);

    if(binding(b)) {
        return false;   // it's already there
    }

    d->bindings.append(b);
    d->updateIndexesOnAdd(b);

    return true;
}

void Bindings::removeBinding(BindingPtr b)
{
    Q_ASSERT(d);

    if(b.isNull()) {
        // empty SharedPtr
        return;
    }

    // remove from big table
    BindingsList::iterator it = d->bindings.find(b);
    if(it == d->bindings.end()) {
        // not in the big table
        Q_ASSERT(false);
        return;
    }
    d->bindings.remove(it);

    d->updateIndexesOnDel(b);
}

const BindingPtr Bindings::binding(
        const QString& conn /*= QString::null*/,
        const QString& var /*= QString::null*/,
        const QString& view /*= QString::null*/,
        const QString& panel /*= QString::null*/,
        const QString& templ /*= QString::null*/,
        const QString& iopin /*= QString::null*/)
{
    Q_ASSERT(d);

    Binding b(conn, var, view, panel, templ, iopin);

    for(BindingsList::const_iterator it = d->bindings.begin();
            it != d->bindings.end(); ++it) {

        if(**it == b) {
            return *it;
        }
    }

    return BindingPtr();
}

const bool Bindings::binding(const BindingPtr b)
{
    Q_ASSERT(d);

    for(BindingsList::const_iterator it = d->bindings.begin();
            it != d->bindings.end(); ++it) {

        if(**it == *b) {
            return true;
        }
    }

    return false;
}

const BindingsList& Bindings::bindingsByView(const QString& view) const
{
    Q_ASSERT(d);
    if(d->indexView.contains(view)) {
        return d->indexView[view];
    }
    return d->null;
}

const BindingsList& Bindings::bindingsByPanel(const QString& view,
        const QString& p) const
{
    Q_ASSERT(d);
    if(d->indexViewPanel.contains(view) &&
            d->indexViewPanel[view].contains(p)) {

        return d->indexViewPanel[view][p];
    }
    return d->null;
}

const BindingsList& Bindings::bindingsByTemplate(const QString& view,
        const QString& p, const QString& templ) const
{
    Q_ASSERT(d);
    if(!d->indexViewPanelTempl.contains(view) ||
            !d->indexViewPanelTempl[view].contains(p) ||
            !d->indexViewPanelTempl[view][p].contains(templ)) {
        return d->null;
    }

    return d->indexViewPanelTempl[view][p][templ];
}

const BindingsList& Bindings::bindingsByConnection(const QString& conn) const
{
    Q_ASSERT(d);
    if(d->indexConn.contains(conn)) {
        return d->indexConn[conn];
    }
    return d->null;
}

const BindingsList& Bindings::bindingsByVariable(const QString& conn,
        const QString& var) const
{
    Q_ASSERT(d);
    if(!d->indexConnVar.contains(conn) ||
            !d->indexConnVar[conn].contains(var)) {
        return d->null;
    }

    return d->indexConnVar[conn][var];
}

void Bindings::renameConnection(const QString& oldName,
        const QString& newName)
{
    Q_ASSERT(d);

    if(!d->indexConn.contains(oldName)) {
        // connection name doesn't exist
        return;
    }
    Q_ASSERT(d->indexConn.contains(oldName));
    Q_ASSERT(d->indexConnVar.contains(oldName));

    // duplicates check in indexConn
    if(d->indexConn.contains(newName)) {
        qWarning("Detected renameConnection command"
                " to yet existing connection!");
        return;
    }
    // duplicates check in indexConnVar
    if(d->indexConnVar.contains(newName)) {
        qWarning("Detected renameConnection command"
                " to yet existing connection!");
        return;
    }
    Q_ASSERT(!d->indexConn.contains(newName));
    Q_ASSERT(!d->indexConnVar.contains(newName));

    // rename affected bindings
    BindingsList& bc = d->indexConn[oldName];

    for(BindingsList::iterator it = bc.begin(); it != bc.end(); ++it) {
        Q_ASSERT((*it)->connection() == oldName);
        (*it)->setConnection(newName);
    }

    // update indexes

    // indexConn
    BindingsList l = d->indexConn[oldName];
    d->indexConn.remove(oldName);
    d->indexConn.insert(newName, l);

    // indexConnVar
    BindingsPrivate::IndexLevel1 cv1 = d->indexConnVar[oldName];
    d->indexConnVar.remove(oldName);
    Q_ASSERT(!d->indexConnVar.contains(oldName));
    d->indexConnVar.insert(newName, cv1);
    Q_ASSERT(d->indexConnVar.contains(newName));
}

void Bindings::renameVariable(const QString& connection,
        const QString& oldName, const QString& newName)
{
    Q_ASSERT(d);

    if(!d->indexConnVar.contains(connection)) {
        // connection name doesn't exist
        return;
    }
    Q_ASSERT(d->indexConnVar.contains(connection));

    BindingsPrivate::IndexLevel1& vars = d->indexConnVar[connection];

    if(!vars.contains(oldName)) {
        // no such variable here
        return;
    }

    if(vars.contains(newName)) {
        qWarning("Detected renameVariable command"
                " to yet existing variable!");
        return;
    }
    Q_ASSERT(!vars.contains(newName));

    BindingsList& bl = vars[oldName];

    // rename affected bindings
    for(BindingsList::iterator it = bl.begin(); it != bl.end(); ++it) {
        Q_ASSERT((*it)->variable() == oldName);
        (*it)->setVariable(newName);
    }

    // update indexes

    // save the old list
    BindingsList l = vars[oldName];

    Q_ASSERT(vars.contains(oldName));
    vars.remove(oldName);
    Q_ASSERT(!vars.contains(oldName));
    Q_ASSERT(!d->indexConnVar[connection].contains(oldName));

    Q_ASSERT(!vars.contains(newName));
    vars.insert(newName, l);
    Q_ASSERT(vars.contains(newName));
    Q_ASSERT(d->indexConnVar[connection].contains(newName));
}

void Bindings::renameView(const QString& oldName,
        const QString& newName)
{
    Q_ASSERT(d);

    if(!d->indexView.contains(oldName)) {
        // connection name doesn't exist
        return;
    }
    Q_ASSERT(d->indexView.contains(oldName));
    Q_ASSERT(d->indexViewPanel.contains(oldName));
    Q_ASSERT(d->indexViewPanelTempl.contains(oldName));

    // duplicates check in indexView
    if(d->indexView.contains(newName)) {
        qWarning("Detected renameView command to yet existing view!");
        return;
    }
    // duplicates check in indexViewPanel
    if(d->indexViewPanel.contains(newName)) {
        qWarning("Detected renameView command to yet existing view!");
        return;
    }
    // duplicates check in indexViewPanelTempl
    if(d->indexViewPanelTempl.contains(newName)) {
        qWarning("Detected renameView command to yet existing view!");
        return;
    }
    Q_ASSERT(!d->indexView.contains(newName));
    Q_ASSERT(!d->indexViewPanel.contains(newName));
    Q_ASSERT(!d->indexViewPanelTempl.contains(newName));

    // rename affected bindings
    for(BindingsList::iterator it = d->bindings.begin();
            it != d->bindings.end(); ++it) {

        if((*it)->view() == oldName) {
            (*it)->setView(newName);
        }
    }

    // Update Indexes

    // indexView
    BindingsList v1 = d->indexView[oldName];
    d->indexView.remove(oldName);
    Q_ASSERT(!d->indexView.contains(oldName));
    Q_ASSERT(!d->indexView.contains(newName));
    d->indexView.insert(newName, v1);
    Q_ASSERT(d->indexView.contains(newName));

    // indexViewPanel
    BindingsPrivate::IndexLevel1 vp1 = d->indexViewPanel[oldName];
    d->indexViewPanel.remove(oldName);
    Q_ASSERT(!d->indexViewPanel.contains(oldName));
    Q_ASSERT(!d->indexViewPanel.contains(newName));
    d->indexViewPanel.insert(newName, vp1);
    Q_ASSERT(d->indexViewPanel.contains(newName));

    // indexViewPanelTempl
    BindingsPrivate::IndexLevel2 vpt1 = d->indexViewPanelTempl[oldName];
    d->indexViewPanelTempl.remove(oldName);
    Q_ASSERT(!d->indexViewPanelTempl.contains(oldName));
    Q_ASSERT(!d->indexViewPanelTempl.contains(newName));
    d->indexViewPanelTempl.insert(newName, vpt1);
    Q_ASSERT(d->indexViewPanelTempl.contains(newName));
}

void Bindings::renamePanel(const QString& view, const QString& oldName,
        const QString& newName)
{
    Q_ASSERT(d);

    if(!d->indexViewPanel.contains(view)) {
        // view name doesn't exist
        return;
    }
    Q_ASSERT(d->indexViewPanel.contains(view));

    BindingsPrivate::IndexLevel1& panels = d->indexViewPanel[view];
    if(!panels.contains(oldName)) {
        // there is no such panel
        return;
    }

    // duplicates check in indexViewPanel
    if(panels.contains(newName)) {
        qWarning("Detected renamePanel command to yet existing panel!");
        return;
    }
    Q_ASSERT(!panels.contains(newName));

    Q_ASSERT(panels.contains(oldName));
    BindingsList& bl = panels[oldName];

    // rename affected bindings
    for(BindingsList::iterator it = bl.begin(); it != bl.end(); ++it) {
        Q_ASSERT((*it)->panel() == oldName);
        (*it)->setPanel(newName);
    }

    // update indexes

    // indexViewPanel
    BindingsList l = panels[oldName];
    panels.remove(oldName);
    panels.insert(newName, l);

    // indexViewPanelTempl
    BindingsPrivate::IndexLevel2& panels2 = d->indexViewPanelTempl[view];
    BindingsPrivate::IndexLevel1 tt = panels2[oldName];
    panels2.remove(oldName);
    panels2.insert(newName, tt);
}

void Bindings::renameTemplate( const QString& view, const QString& panel,
        const QString& oldName, const QString& newName)
{
    Q_ASSERT(d);

    if(!d->indexViewPanelTempl.contains(view)) {
        // view name doesn't exist
        return;
    }
    Q_ASSERT(d->indexViewPanelTempl.contains(view));
    BindingsPrivate::IndexLevel2& panels = d->indexViewPanelTempl[view];

    if(!panels.contains(panel)) {
        // panel name doesn't exist
        return;
    }
    Q_ASSERT(panels.contains(panel));
    BindingsPrivate::IndexLevel1& templates = panels[panel];

    if(!templates.contains(oldName)) {
        // there is no such template
        return;
    }
    Q_ASSERT(templates.contains(oldName));

    // duplicates check in indexViewPanelTempl
    if(templates.contains(newName)) {
        qWarning("Detected renameTemplate command to yet existing template!");
        return;
    }
    Q_ASSERT(!templates.contains(newName));

    BindingsList& bl = templates[oldName];

    // rename affected bindings
    for(BindingsList::iterator it = bl.begin(); it != bl.end(); ++it) {
        Q_ASSERT((*it)->templ() == oldName);
        (*it)->setTempl(newName);
    }

    // update indexes

    // indexViewPanelTempl
    BindingsList l = templates[oldName];
    templates.remove(oldName);
    templates.insert(newName, l);
}

#ifdef _COMPILE_IN_DUMP_
void Bindings::dump() const
{
    dump(d->bindings);
}

void Bindings::dump(const BindingsList& bl)
{
    qDebug(">>> dump");
    for(BindingsList::const_iterator it = bl.begin();
            it != bl.end(); ++it) {
        qDebug("%s;%s;%s;%s;%s;%s",
                (*it)->connection().latin1(),
                (*it)->variable().latin1(),
                (*it)->view().latin1(),
                (*it)->panel().latin1(),
                (*it)->templ().latin1(),
                (*it)->iopin().latin1());
    }
    qDebug("<<< dump");
}
#endif

// vim: set et ts=4 sw=4 tw=76:
// $Id: Bindings.cpp 1203 2006-01-05 11:37:22Z modesto $
