// $Id: BindingsHelper.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: BindingsHelper.hh -- 
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

#ifndef _BINDINGSHELPER_HH
#define _BINDINGSHELPER_HH

#include <qstring.h>

#include <lt/templates/Template.hh>

namespace lt {
    class EditorProject;
};

#include "Bindings.hh"

// #define _CONSISTENCY_CHECKS_

/**
 * Class of static Bindings helper methods. Mainly for saving and restoring
 * Bindings. Should help undo-commands that need to save bindings to do it
 * in only one and tested way.
 */
class BindingsHelper
{
public:
    /**
     * Saves Bindings of one variable of given connection of given project.
     * Nothing is deleted nor unbound.
     *
     * \param p The project that contains the connection to unbind
     * \param cn The connection name to save+unbind+delete bindigns for
     * \param vn The variable name to save+unbind+delete bindigns for
     *
     * \return List of bindings that represents all bindings of the
     * connection+variable
     */
    static BindingsList saveVariableBindings(lt::EditorProject* p,
            const QString& cn, const QString& vn = QString::null);

    /**
     * Saves Bindings of one variable of given connection of given project,
     * deletes the Bindings and unbinds all iopins that are connected to the
     * variable of the connection.
     *
     * \param p The project that contains the connection to unbind
     * \param cn The connection name to save+unbind+delete bindigns for
     * \param vn The variable name to save+unbind+delete bindigns for
     *
     * \return List of bindings that represents all bindings of the
     * connection+variable
     */
    static BindingsList saveVariableBindingsAndUnbind(lt::EditorProject* p,
            const QString& cn, const QString& vn = QString::null);

    /**
     * Saves Bindings of all template from given list of template from
     * project+view+panel names. Also deletes the Bindings and unbinds all
     * iopins of the templates.
     *
     * \param p The project that contains the connection to unbind
     * \param vn The View name to save+unbind+delete bindigns for
     * \param pn The Panel name to save+unbind+delete bindigns for
     * \param td The TemplateDict of templates to save+unbind+delete
     * bindigns for
     *
     * \return List of bindings that represents all bindings of the
     * connection+variable
     */
    static BindingsList saveTemplatesBindingAndUnbind(lt::EditorProject* p,
            const QString& vn, const QString& pn, TemplateDict& td);

    /**
     * Restore + bind bindings from the given BindingsList.
     *
     * \param p The project that contains the connection to unbind
     * \param bl The list of bindings to bind
     */
    static void restoreBindingsAndBind(lt::EditorProject* p,
            const BindingsList& bl);

#ifdef _CONSISTENCY_CHECKS_
    /**
     * Test the consistency of all bindings versus the real
     * variable-to-iopin bindings. Reports differences to console.
     */
    static void consistencyCheck(lt::EditorProject* p);
#endif
};

#endif /* _BINDINGSHELPER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: BindingsHelper.hh 1169 2005-12-12 15:22:15Z modesto $
