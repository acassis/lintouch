// $Id: MoveNResizeSelectionCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: MoveNResizeSelectionCommand.hh -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 01 September 2004
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

#ifndef _LTEDITORMOVENRESIZESELECTIONCOMMAND_H
#define _LTEDITORMOVENRESIZESELECTIONCOMMAND_H

#include <lt/templates/Template.hh>
using namespace lt;

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * Move and Resize Selection Command.
 *
 * Undo/Redo command applying x,y,z + w,h change to one or more templates.
 *
 * @author Martin Man <Martin.Man@seznam.cz>
 *
 */
class MoveNResizeSelectionCommand : public Command
{
    public:

        /**
         * Constructor.
         *
         * Creates a command which changes rectangle and z coordinate of one
         * or more templates.
         *
         * @param templates The dictionary of templates that should be
         * resized.
         * @param sizes Their new sizes.
         * @param layers Their new z-coordinates.
         * @param pw ProjectWindow containing these templates.
         * @param mw MainWidow.
         */
        MoveNResizeSelectionCommand(
                TemplateDict templates,
                LTMap <QRect> sizes,
                LTMap <double> layers,
                ProjectWindow * pw,
                MainWindow * mw );

        /**
         * Constructor.
         *
         * Creates a command which remembers the given original rects and
         * takes the current rects as the new one.
         *
         * (used for interactive move & resize where templates are already
         * there before invoking redo)
         *
         * @param sizes original rects.
         * @param templates The dictionary of templates that should be
         * resized.
         * @param pw ProjectWindow containing these templates.
         * @param mw MainWidow.
         */
        MoveNResizeSelectionCommand(
                LTMap <QRect> sizes,
                TemplateDict templates,
                ProjectWindow * pw,
                MainWindow * mw );

        /**
         * Destructor.
         */
        virtual ~MoveNResizeSelectionCommand();

        /**
         * Applies the command.
         */
        virtual void redo();

        /**
         * Undoes the command.
         */
        virtual void undo();

        /**
         * RTTI
         */
        virtual CommandType rtti() const;

        /**
         * Returns the templates the change was applied to.
         */
        const TemplateDict & templates() const;

        /**
         * Returns the templates that were selected at the time of the
         * change.
         */
        const TemplateDict & selectedTemplates() const;

        /**
         * Returns the panel where the templates are moved/resized.
         */
        QString panelName() const;

        /**
         * Returns the view where the templates are moved/resized.
         */
        QString viewName() const;

    public: // static

        /**
         * Return the dictionary of rectangles of given templates.
         */
        static LTMap <QRect> rects2Map( const TemplateDict & templates );

        /**
         * Return the dictionary of layers of given templates.
         */
        static LTMap <double> layers2Map( const TemplateDict & templates );

    private:

        struct MoveNResizeSelectionCommandPrivate;
        MoveNResizeSelectionCommandPrivate* d;
};

#endif /* _LTEDITORMOVENRESIZESELECTIONCOMMAND_H */

// vim: set et ts=4 sw=4 tw=76:
// $Id: MoveNResizeSelectionCommand.hh 1169 2005-12-12 15:22:15Z modesto $
