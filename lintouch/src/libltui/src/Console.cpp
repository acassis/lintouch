// $Id: Console.cpp,v 1.16 2004/08/15 18:17:59 mman Exp $
//
//   FILE: Console.cpp -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 1 February 2005
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

#include <qprocess.h>

#include "lt/ui/Console.hh"
using namespace lt;

struct Console::ConsolePrivate
{
    ConsolePrivate() :
    process( NULL ) {}

    QProcess * process;
};

Console::Console( QWidget* parent, const char* name /* = 0 */ ) :
        QTextEdit( parent, name ), d( new ConsolePrivate )
{
    Q_CHECK_PTR( d );

    setReadOnly( true );
    setTextFormat( Qt::RichText );

    connect( this, SIGNAL( textChanged() ), SLOT( scrollToBottom() ) );
}

Console::~Console()
{
}

void Console::clear()
{
    Q_CHECK_PTR( d );

    QTextEdit::clear();
}

void Console::appendStdout( const QString & m )
{
    Q_CHECK_PTR( d );

    QTextEdit::append( m );
}

void Console::appendStderr( const QString & m )
{
    Q_CHECK_PTR( d );

    QTextEdit::append( m );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Console.cpp,v 1.16 2004/08/15 18:17:59 mman Exp $
