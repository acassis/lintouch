// $Id: BindingsHelper.cpp 1211 2006-01-06 12:11:28Z modesto $
//
//   FILE: BindingsHelper.cpp --
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 29 October 2004
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

#include <lt/templates/Connection.hh>
#include <lt/templates/Variable.hh>
#include <lt/templates/Template.hh>
#include <lt/templates/IOPin.hh>
#include <lt/project/View.hh>
#include <lt/project/Panel.hh>

#include "Binding.hh"
#include "BindingsHelper.hh"
#include "EditorProject.hh"

using namespace lt;

BindingsList BindingsHelper::saveVariableBindings( EditorProject* p,
        const QString& cn, const QString& vn /*= QString::null*/)
{
    Q_ASSERT(p);
#ifdef _CONSISTENCY_CHECKS_
    qDebug(">>> saveVariableBindingsAndUnbind()");
#ifdef _COMPILE_IN_DUMP_
    p->bindings().dump();
#endif
    consistencyCheck(p);
#endif

    BindingsList bl;

    // save and remove bindings from Bindings class
    if(vn.isEmpty()) {
        bl = p->bindings().bindingsByConnection(cn);
    } else {
        bl = p->bindings().bindingsByVariable(cn, vn);
    }

#ifdef _CONSISTENCY_CHECKS_
    qDebug("---");
#ifdef _COMPILE_IN_DUMP_
    p->bindings().dump();
#endif
    consistencyCheck(p);
    qDebug("<<< saveVariableBindingsAndUnbind()");
#endif
    return bl;
}

BindingsList BindingsHelper::saveVariableBindingsAndUnbind(
        EditorProject* p, const QString& cn,
        const QString& vn /*= QString::null*/)
{
    Q_ASSERT(p);
#ifdef _CONSISTENCY_CHECKS_
    qDebug(">>> saveVariableBindingsAndUnbind()");
#ifdef _COMPILE_IN_DUMP_
    p->bindings().dump();
#endif
    consistencyCheck(p);
#endif

    BindingsList bl = saveVariableBindings(p, cn, vn);

    Bindings& pb = p->bindings();

    for(unsigned i=0; i < bl.count(); ++i) {
        Q_ASSERT(pb.binding(bl[i]));
        pb.removeBinding(bl[i]);
    }

#ifdef _CONSISTENCY_CHECKS_
    qDebug("---");
#ifdef _COMPILE_IN_DUMP_
    p->bindings().dump();
#endif
    consistencyCheck(p);
    qDebug("<<< saveVariableBindingsAndUnbind()");
#endif
    return bl;
}

BindingsList BindingsHelper::saveTemplatesBindingAndUnbind(
        EditorProject* p, const QString& vn, const QString& pn,
        TemplateDict& td)
{
    Q_ASSERT(p);
#ifdef _CONSISTENCY_CHECKS_
    qDebug(">>> saveTemplatesBindingAndUnbind()");
#ifdef _COMPILE_IN_DUMP_
    p->bindings().dump();
#endif
    consistencyCheck(p);
#endif

    BindingsList bl;

    Bindings& pb = p->bindings();

    for( unsigned i = 0; i < td.count(); i++ ) {
        // save Bindings
        bl += pb.bindingsByTemplate(vn, pn, td.keyAt(i));
    }

    // delete the bindings from the global container
    for(BindingsList::iterator it=bl.begin(); it != bl.end(); ++it) {
        pb.removeBinding(*it);
    }

#ifdef _CONSISTENCY_CHECKS_
    qDebug("---");
#ifdef _COMPILE_IN_DUMP_
    p->bindings().dump();
#endif
    consistencyCheck(p);
    qDebug("<<< saveTemplatesBindingAndUnbind()");
#endif
    return bl;
}

void BindingsHelper::restoreBindingsAndBind(EditorProject* p,
        const BindingsList& bl)
{
    Q_ASSERT(p);

#ifdef _CONSISTENCY_CHECKS_
    qDebug(">>> restoreBindingsAndBind()");
#ifdef _COMPILE_IN_DUMP_
    p->bindings().dump(bl);
#endif
    consistencyCheck(p);
#endif

    QString oldC, oldV;
    Connection* c = NULL;
    Variable* v = NULL;
    Template* t = NULL;
    IOPin* io = NULL;

    // bind acording the bl
    for(BindingsList::const_iterator it = bl.begin(); it != bl.end(); ++it) {
        bool ok = p->bindings().addBinding((*it));
        Q_ASSERT(ok);
    }
#ifdef _CONSISTENCY_CHECKS_
    qDebug("---");
#ifdef _COMPILE_IN_DUMP_
    p->bindings().dump();
#endif
    consistencyCheck(p);
    qDebug("<<< restoreBindingsAndBind()");
#endif
}

#ifdef _CONSISTENCY_CHECKS_
void BindingsHelper::consistencyCheck(lt::EditorProject* p)
{
    Q_ASSERT(p);
    qDebug(">>> consistencyCheck");

    const Bindings& bs = p->bindings();
    const ViewMap& views = p->views();

    const Connection* c = NULL;
    const Variable* v = NULL;
    const Template* t = NULL;
    const IOPin* io = NULL;

    const ConnectionDict& cd = p->connections();
    for(unsigned i = 0; i<cd.count(); ++i) {
        c = cd[i];
        Q_ASSERT(c);
        qDebug("conn: %d: %s", i, cd.keyAt(i).latin1());

        const VariableDict& vd = c->variables();
        for(unsigned j = 0; j<vd.count(); ++j) {
            v = vd[j];
            Q_ASSERT(v);
            qDebug("var: %#x: %s", (void*)vd[j], vd.keyAt(j).latin1());

            const BindingsList& bl = bs.bindingsByVariable(
                    cd.keyAt(i), vd.keyAt(j));

            for(BindingsList::const_iterator it = bl.begin(); it != bl.end();
                    ++it) {
                Q_ASSERT((*it)->connection() == cd.keyAt(i));
                Q_ASSERT((*it)->variable() == vd.keyAt(j));

                t = p->views()[(*it)->view()]
                    .panels()[(*it)->panel()]
                    .templates()[(*it)->templ()];
                if(!t) {
                    qWarning("Could not find template: %s:%s:%s!",
                            (*it)->view().latin1(), (*it)->panel().latin1(),
                            (*it)->templ().latin1());
                    continue;
                }

                io = t->iopins()[(*it)->iopin()];
                if(!io) {
                    qWarning("Could not find template's iopin: %s:%s:%s:%s!",
                            (*it)->view().latin1(), (*it)->panel().latin1(),
                            (*it)->templ().latin1(), (*it)->iopin().latin1());
                    continue;
                }

                if(io->boundTo() != v) {
                    qWarning("iopin: %s:%s:%s:%s is not bound"
                            " to var %s:%s but %#x!",
                            (*it)->view().latin1(), (*it)->panel().latin1(),
                            (*it)->templ().latin1(), (*it)->iopin().latin1(),
                            (*it)->connection().latin1(),
                            (*it)->variable().latin1(), (void*)io->boundTo());
                    continue;
                }
            }
        }
    }
    qDebug("<<< consistencyCheck");
}
#endif

// vim: set et ts=4 sw=4 tw=76:
// $Id: BindingsHelper.cpp 1211 2006-01-06 12:11:28Z modesto $
