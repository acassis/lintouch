// $Id: SelectionPropsChangedCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: SelectionPropsChangedCommand.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 09 September 2004
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

#ifndef _LTEDITORTEMPLATEPROPSCHANGEDCOMMAND_HH
#define _LTEDITORTEMPLATEPROPSCHANGEDCOMMAND_HH

#include <lt/templates/Template.hh>

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class SelectionPropsChangedCommand : public Command
{
public:
    /**
     * ctor. Adds a new view with default generic name.
     */
    SelectionPropsChangedCommand( const lt::TemplateDict& s,
            const QString& propertyKey, const QString& newValue,
            ProjectWindow* pw, MainWindow* mw );

    /**
     * dtor
     */
    virtual ~SelectionPropsChangedCommand();

    /**
     * TODO
     */
    virtual void redo();

    /**
     * TODO
     */
    virtual void undo();

    /**
     * RTTI
     */
    virtual CommandType rtti() const;

    /**
     * Returns key of the Property that has changed
     */
    QString propertyKey() const;

    /**
     * Returns dict of all templates that has been touched by this command.
     */
    const TemplateDict& templates() const;

private:
    struct SelectionPropsChangedCommandPrivate;
    SelectionPropsChangedCommandPrivate* d;
};

#endif /* _LTEDITORTEMPLATEPROPSCHANGEDCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: SelectionPropsChangedCommand.hh 1169 2005-12-12 15:22:15Z modesto $
