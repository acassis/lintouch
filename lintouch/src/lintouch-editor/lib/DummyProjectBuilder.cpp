// $Id: DummyProjectBuilder.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: DummyProjectBuilder.cpp --
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

#include "DummyProjectBuilder.hh"
DummyProjectBuilder::DummyProjectBuilder(
    LoggerPtr logger /* = Logger::nullPtr() */,
    QString locale /* = QString::null */)
{
    m_logger = logger;
    m_locale = locale;
}

EditorProject * DummyProjectBuilder::buildDummyProject(
        int nViews, int nPanels,
        int nTemplates, const TemplateManager & manager,
        const QString & tl, const QString & t )
{
    EditorProject * result = new EditorProject;

    // create nViews views
    for( int i = 0; i < nViews; i ++ ) {
        View v;

        v.setVirtualWidth( 800 );
        v.setVirtualHeight( 600 );

        int shx = 0;
        int shy = 0;
        if( nTemplates > 1 ) {
            shx = (800-20-100)/(nTemplates-1);
            shy = (600-20-100)/(nTemplates-1);
        }

        // create nPanels panels
        for( int j = 0; j < nPanels; j ++ ) {
            Panel p;

            // create nTemplates templates
            for( int k = 0; k < nTemplates; k ++ ) {

                Template * tmpl = manager.instantiate(
                        tl, m_logger, t );
                Q_ASSERT( t );

                if( tmpl != NULL ) {
                    tmpl->setVirtualRect( QRect( 10 + (shx * k), 10 + (shy * k),
                                100, 100 ) );
                    tmpl->setZ( k );
                    tmpl->propertiesChanged();
                    p.addTemplate(
                            QString( "Template nr. %1" ).arg( k ),
                            tmpl );
                }
            }

            v.addPanel(
                    QString( "Panel nr. %1" ).arg( j ),
                    p );
        }

        result->addView(
                QString( "View nr. %1" ).arg( i ), v );
    }

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: DummyProjectBuilder.cpp 1169 2005-12-12 15:22:15Z modesto $
