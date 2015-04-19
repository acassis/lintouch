// $Id: ViewBuilder.hh,v 1.9 2004/09/14 16:23:26 mman Exp $
//
//   FILE: ViewBuilder.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 13 November 2003
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

#ifndef _VIEWBUILDER_HH
#define _VIEWBUILDER_HH

#include <qiodevice.h>

#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>

#include <lt/templates/TemplateManager.hh>

#include "lt/project/View.hh"

namespace lt {

    /**
     * Factory for the View class. Once initialized, it is ready
     * to create all the views in the project.
     *
     * Pointer-based class.
     */

    class ViewBuilder:
        public QObject,
        public LogEnabled
    {
        Q_OBJECT
        public:

            /**
             * Initialize the ViewBuilder for all the views in the
             * project. The panelsTotal is the number of all the
             * panels in the project.
             *
             * @param panelsTotal total number of palens in a project
             * @param tm TemplateManager
             * @param localizator Localizator
             */
            ViewBuilder(int panelsTotal, const TemplateManager &tm,
                    LocalizatorPtr localizator);

            /**
             * Destructor.
             */
            virtual ~ViewBuilder();

            /**
             * Build a view instance from the file.
             *
             * @param dev The device providing access to XML description of
             * the view.
             * @param logger Logger
             * @return Newly built view
             */
            View buildFromFile(QIODevice *dev,
                    LoggerPtr logger = Logger::nullPtr());

            /**
             * Build a view instance from the configuration.
             *
             * @param c The XML description of
             * the view.
             * @param logger Logger
             * @return Newly built view
             */
            View buildFromConfiguration(ConfigurationPtr c,
                    LoggerPtr logger = Logger::nullPtr());

            /**
             * Cancel the build in the progress. This function actually
             * does not stop building the current panel; it returns
             * right after that.
             */
            void cancel();

        signals:

            /**
             * Report the progress of the build process.
             */
            void buildProgress(int panel, int panelsTotal);

        private:

            /**
             * Hide the copy constructor.
             */
            ViewBuilder(const ViewBuilder &other);

            /**
             * Hide the copy assignment operator.
             */
            ViewBuilder &operator =(const ViewBuilder &other);

            struct ViewBuilderPrivate;
            ViewBuilderPrivate *d;
    };
}

#endif /* _VIEWBUILDER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ViewBuilder.hh,v 1.9 2004/09/14 16:23:26 mman Exp $
