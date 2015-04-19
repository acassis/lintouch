// $Id: Command.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: Command.cpp -- 
// AUTHOR: patrik <modesto@swac.cz>
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

#include "ProjectWindow.hh"
#include "MainWindow.hh"
#include "Command.hh"

Command::Command( ProjectWindow* pw,
        MainWindow* mw,
        const QString& description /*= QString::null*/,
        bool canMerge /*= false*/ ) :
    UndoCommand( description ), m_pw( pw ), m_mw( mw )
{
    Q_ASSERT( pw );
    Q_ASSERT( mw );
}

Command::~Command()
{
}

void Command::notifyMainWindow( CommandType type )
{
    Q_ASSERT(m_pw);
    m_pw->setLastCommand( this );
    m_mw->projectChangedSlot( m_pw, type );
}

//////////////////////////////////////////////////////////////////////////////
// STATIC!!
QString Command::generateSafeName( const QString& pattern,
        int startAt, const QMap<QString,int>& existing )
{
    QString name;
    // find a name that is not yet in the existing map
    for(;;) {
        name = QString(pattern).arg(startAt);
        if( !existing.contains(name) ) {
            break;
        }
        ++startAt;
    }

    Q_ASSERT(name.isNull() == false );
    return name;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Command.cpp 1169 2005-12-12 15:22:15Z modesto $
