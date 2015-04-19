// $Id: TemplatePalette.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplatePalette.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 16 December 2004
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

#ifndef _LTEDITORTEMPLATEPALETTE_HH
#define _LTEDITORTEMPLATEPALETTE_HH

#include <qiconview.h>

#include <lt/templates/Template.hh>
#include <lt/templates/TemplateManager.hh>
#include <lt/templates/TemplateLibrary.hh>

/**
 * Simple QIconView to show all templates from currently selected Template
 * Library.
 *
 * \author Patrik Modesto <modesto@swac.cz>
 */
class TemplatePalette : public QIconView
{
    Q_OBJECT

public:
    /**
     * ctor
     */
    TemplatePalette(lt::TemplateManager tm, QWidget* parent);

    /**
     * dtor
     */
    virtual ~TemplatePalette();

    /**
     * Instantiate currently selected template.
     */
    lt::Template* instantiate(LoggerPtr logger) const;

    /**
     * Clears the selection.
     */
    void clearSelection();

public slots:
    /**
     * Call when current Template Library gets changed.
     *
     * \param tl The name of the newly selected TL
     */
    void currentLibraryChangedSlot(const QString& tln);

protected slots:
    /**
     * Called when a selection is changed.
     */
    void selectionChangedSlot(QIconViewItem* i);

signals:
    /**
     * Emitted when a Template is selected to be placed.
     */
    void templateSelected();

    /**
     * Emitted when selection is cleared.
     */
    void templateUnselected();

private:
    struct TemplatePalettePrivate;
    TemplatePalettePrivate* d;
};

#endif /* _LTEDITORTEMPLATEPALETTE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplatePalette.hh 1169 2005-12-12 15:22:15Z modesto $
