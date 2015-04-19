// $Id: DummyProjectBuilder.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: DummyProjectBuilder.hh --
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 11 August 2004
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

#ifndef _DUMMYPROJECTBUILDER_HH
#define _DUMMYPROJECTBUILDER_HH

#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>

#include <lt/templates/TemplateManager.hh>
using namespace lt;

#include "EditorProject.hh"

/**
 * Factory for the Project class.
 *
 * Pointer-based class.
 */
class DummyProjectBuilder
{
    public:

        /**
         * Initialize the class.
         */
        DummyProjectBuilder(LoggerPtr logger = Logger::nullPtr(),
                QString locale = QString::null );

        /**
         * Builds a "dummy" project which will have given number of views,
         * every view will have a given number of panels and at every panels
         * there will be given number of templates. the given template
         * library (tl) and template type (t) will be placed on panels.
         */
        EditorProject * buildDummyProject( int nViews, int nPanels,
                int nTemplates, const TemplateManager & manager,
                const QString & tl, const QString & t );

    private:

        /**
         * Hide the copy constructor.
         */
        DummyProjectBuilder(const DummyProjectBuilder &other);

        /**
         * Hide the copy assignment operator.
         */
        DummyProjectBuilder &operator =(
                const DummyProjectBuilder &other);

    private:

        LoggerPtr m_logger;

        QString m_locale;

};

#endif /* _DUMMYPROJECTBUILDER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: DummyProjectBuilder.hh 1169 2005-12-12 15:22:15Z modesto $
