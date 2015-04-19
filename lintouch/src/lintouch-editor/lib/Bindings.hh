// $Id: Bindings.hh 1203 2006-01-05 11:37:22Z modesto $
//
//   FILE: Bindings.hh -- 
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

#ifndef _BINDINGS_HH
#define _BINDINGS_HH

#include <qstring.h>
#include <qvaluelist.h>

#include "lib/Binding.hh"

//#define _COMPILE_IN_DUMP_

/**
 * Predefined type for easy handling.
 */
typedef QValueList<BindingPtr> BindingsList;

/**
 * Handles bindings. Stores the in a bug table with fast indexed access.
 */
class Bindings
{
public:
    /**
     * ctor
     */
    Bindings();

    /**
     * dtor
     */
    ~Bindings();

    ////////////////////////////////////////////////////////////
    // API
    ////////////////////////////////////////////////////////////

    /**
     * Add new binding.
     */
    const BindingPtr addBinding(
            const QString& conn = QString::null,
            const QString& var = QString::null,
            const QString& view = QString::null,
            const QString& panel = QString::null,
            const QString& templ = QString::null,
            const QString& iopin = QString::null);

    /**
     * Add new binding. Returns false if the binding is already there.
     */
    const bool addBinding(const BindingPtr b);

    /**
     * Delete a binding.
     */
    void removeBinding(BindingPtr b);

    /**
     * Get a binding. Doesn't remove it!
     */
    const BindingPtr binding(
            const QString& conn = QString::null,
            const QString& var = QString::null,
            const QString& view = QString::null,
            const QString& panel = QString::null,
            const QString& templ = QString::null,
            const QString& iopin = QString::null);

    /**
     * Returns true if the binding is in the Bindings.
     */
    const bool binding(const BindingPtr b);

    /**
     * Get a list of bindings. Doesn't remove it!
     */
    const BindingsList& bindingsByView(const QString& view) const;
    const BindingsList& bindingsByPanel(const QString& view,
            const QString& p) const;
    const BindingsList& bindingsByTemplate(const QString& view,
            const QString& p, const QString& templ) const;

    const BindingsList& bindingsByConnection(const QString& c) const;
    const BindingsList& bindingsByVariable(const QString& c,
            const QString& var) const;

    /**
     * Rename a Connection in all bindings. Update indexes as well.
     */
    void renameConnection(const QString& oldName, const QString& newName);

    /**
     * Rename a Variable in all bindings. Update indexes as well.
     */
    void renameVariable(const QString& connection,
            const QString& oldName, const QString& newName);

    /**
     * Rename a View in all bindings. Update indexes as well.
     */
    void renameView(const QString& oldName, const QString& newName);

    /**
     * Rename a Panel in all bindings. Update indexes as well.
     */
    void renamePanel(const QString& view,
            const QString& oldName, const QString& newName);

    /**
     * Rename a Template in all bindings. Update indexes as well.
     */
    void renameTemplate(const QString& view, const QString& panel,
            const QString& oldName, const QString& newName);

#ifdef _COMPILE_IN_DUMP_
    /**
     * Dump
     */
    void dump() const;
    static void dump(const BindingsList& bl);
#endif

private:
    struct BindingsPrivate;
    BindingsPrivate* d;
};

#endif /* _BINDINGS_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Bindings.hh 1203 2006-01-05 11:37:22Z modesto $
