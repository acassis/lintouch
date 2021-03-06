// $Id: TemplateLibrarySelectionCombo.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateLibrarySelectionCombo.hh --
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 26 July 2004
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

#ifndef _LTEDITORTEMPLATELIBRARYSELECTIONCOMBO_HH
#define _LTEDITORTEMPLATELIBRARYSELECTIONCOMBO_HH

#include <qcombobox.h>
#include <qstring.h>

#include <lt/templates/TemplateManager.hh>

class QWidget;
class QButtonGroup;

/**
 * Specialized combo that stores loaded template library( ies )
 */
class TemplateLibrarySelectionCombo:public QComboBox
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    TemplateLibrarySelectionCombo( const lt::TemplateManager
        & tm, QWidget * parent = 0, const char * name = 0);

    /**
     * Destructor.
     */
    ~TemplateLibrarySelectionCombo();

    /**
     * Calculates the recomended size for the combobox.
     */
    QSize sizeHint() const;

    /**
     * Calculates the recomended minimum size for the combobox.
     */
    QSize minimumSizeHint() const;

    /**
     * Returns the default layout behavior of the combobox.
     */
    QSizePolicy sizePolicy() const;

    /**
     * Sets the index of the current item in the combobox to index.
     */
    void setCurrentItem( int index );

    /**
     * Sets the text combobox's current item.
     */
    void setCurrentText( const QString& text );

    signals:

    /**
     * Signal emitted when a library name is selected in the combobox.
     */
    void libraryChanged( const QString& libKey );
protected:
protected slots:

    /**
     * Slot against itemActivated(..) signal, used mainly to emit
     * libraryChanged signal.
     */
    void comboItemActivated( const QString& );
private:

    /**
     * A initializer, used to load combox items from TemplateManager.
     */
    void setLoadedLibrariesFromTemplateManager(
        const lt::TemplateManager & tm );

    typedef struct TemplateLibrarySelectionComboPrivate;
    TemplateLibrarySelectionComboPrivate * d;
};
#endif /* _LTEDITORTEMPLATELIBRARYSELECTIONCOMBO_HH */
                                                                                                                             
// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateLibrarySelectionCombo.hh 1169 2005-12-12 15:22:15Z modesto $

