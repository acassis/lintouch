// $Id: TemplateLibrariesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateLibrariesDialog.hh -- 
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

#ifndef _LTEDITORTEMPLATELIBRARIESDIALOG_HH
#define _LTEDITORTEMPLATELIBRARIESDIALOG_HH

#include <lt/templates/TemplateManager.hh>

#include "TemplateLibrariesDialogBase.h"

/**
 * Dialog used to show status of loaded template library(ies) in tabular
 * form
 */
class TemplateLibrariesDialog :
    public TemplateLibrariesDialogBase
{
    Q_OBJECT

    public:

    /**
     * Constructor.
     */
    TemplateLibrariesDialog( const lt::TemplateManager & tm,
            QWidget* parent, const char* name = 0,
            WFlags fl = Qt::WType_TopLevel );

    /**
     * Destructor.
     */
    virtual ~TemplateLibrariesDialog();

    protected:
    /**
     * Dialog event handling when dialog is resized.
     */
    virtual void resizeEvent( QResizeEvent* e );

    /**
     * Dialog event handling when dialog is shown.
     */
    virtual void showEvent( QShowEvent* );

    /**
     * Sets the column width of the table columns.
     */
    void setColumnWidths();

    /**
     * Calculate minimum column width for a column,
     * minimum column width depends on the width of the column header label.
     */
    int minimumColumnWidth( int col ) const;

    /**
     * Calculate the allowed column width for a column.
     */
    int allowedColumnWidth( int col ) const;

    /**
     * Getter for the current column width for a column.
     */
    int knownColumnWidth( int col ) const;

    /**
     * Calculate the width (in pixels) of the text.
     */
    int pixelWidthOf( const QString& content ) const;

    /**
     * This is a overloaded memeber function, provided for convinience.
     * it behaves essentially like the above function.
     * Calculates the width (in pixels ) of the charecter.
     */
    int pixelWidthOf( QChar ch ) const;

    /**
     * Function to populate the table with template library info.
     */
    void fillProjectLibrariesInfo( const lt::TemplateManager & tm );

    private:

    typedef struct TemplateLibrariesDialogPrivate;
    TemplateLibrariesDialogPrivate * d;

};

#endif /* _LTEDITORTEMPLATELIBRARIESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateLibrariesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
