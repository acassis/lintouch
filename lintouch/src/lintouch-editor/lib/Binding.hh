// $Id: Binding.hh 1193 2005-12-22 10:58:02Z modesto $
//
//   FILE: Binding.hh -- 
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

#ifndef _BINDING_HH
#define _BINDING_HH

#include <qstring.h>

#include <lt/SharedPtr.hh>
using namespace lt;

class Binding;

/**
 * New type for easy Binding use with SharedPtr.
 */
typedef SharedPtr<Binding> BindingPtr;

/**
 * Container for all info needed by one binding.
 */
class Binding
{
    /**
     * Only the Bindings class can access the setters!
     */
    friend class Bindings;

public:
    /**
     * ctor. Initialize and set valid to false.
     */
    Binding() {}

    /**
     * ctor. Initialize and set valid to false.
     */
    Binding( const QString& conn, const QString& var, const QString& vi,
            const QString& p, const QString& t, const QString& iop) :
        c(conn), var(var), vi(vi), p(p), t(t), io(iop) {}

    /**
     * Clone the binding. Returns pointer to the new cloned binding.
     */
    Binding* clone() const {
        return new Binding(c, var, vi, p, t, io);
    }

    /**
     * Returns true if this content is the same as the given Binding
     * content.
     */
    bool operator==(const Binding* b) {
        Q_ASSERT(b);
        return c==b->c && var== b->var && vi== b->vi && p== b->p &&
            t== b->t && io== b->io;
    }

    /**
     * Returns true if this content is the same as the given Binding
     * content.
     */
    bool operator==(const Binding& b) {
        return c==b.c && var== b.var && vi== b.vi && p== b.p &&
            t== b.t && io== b.io;
    }

    /**
     * Valid flag.
     */
    bool valid() {
        return !(c.isEmpty() || var.isEmpty() ||
            vi.isEmpty() || p.isEmpty() || t.isEmpty() ||
            io.isEmpty());
    }

    /**
     * Connections name.
     */
    QString connection() { return c; }

    /**
     * Variable name.
     */
    QString variable() { return var; }

    /**
     * View name.
     */
    QString view() { return vi; }

    /**
     * Panel name.
     */
    QString panel() { return p; }

    /**
     * Template name.
     */
    QString templ() { return t; }

    /**
     * IOPin name.
     */
    QString iopin() { return io; }

protected:
    /**
     * Connections name.
     */
    void setConnection(const QString& d) { c=d; }

    /**
     * Variable name.
     */
    void setVariable(const QString& d) { var=d; }

    /**
     * View name.
     */
    void setView(const QString& d) { vi=d; }

    /**
     * Panel name.
     */
    void setPanel(const QString& d) { p=d; }

    /**
     * Template name.
     */
    void setTempl(const QString& d) { t=d; }

    /**
     * IOPin name.
     */
    void setIOPin(const QString& d) { io=d; }

private:
    /**
     * Connections name.
     */
    QString c;

    /**
     * Variable name.
     */
    QString var;

    /**
     * View name.
     */
    QString vi;

    /**
     * Panel name.
     */
    QString p;

    /**
     * Template name.
     */
    QString t;

    /**
     * IOPin name.
     */
    QString io;
};

#endif /* _BINDING_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Binding.hh 1193 2005-12-22 10:58:02Z modesto $
