// $Id: HelpWindow.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: HelpWindow.hh -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 23 March 2004
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

#ifndef _HELPDIALOG_HH
#define _HELPDIALOG_HH

#include "HelpWindowBase.h"

/**
 * TODO
 * 
 * \author Patrik Modesto <modesto@swac.cz>
 */
class HelpWindow : public HelpWindowBase
{
    Q_OBJECT

public:
    HelpWindow( const QString& html,
            QWidget* parent = 0, const char* name = 0,
            WFlags fl = 0 );
    ~HelpWindow();

};

#endif /* _HELPDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: HelpWindow.hh 1169 2005-12-12 15:22:15Z modesto $
