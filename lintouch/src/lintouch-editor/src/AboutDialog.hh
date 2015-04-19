// $Id: AboutDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AboutDialog.hh -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 27 January 2004
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

#ifndef _ABOUTDIALOG_HH
#define _ABOUTDIALOG_HH

#include "AboutDialogBase.h"

/**
 * TODO
 * \author Patrik Modesto <modesto@swac.cz>
 */
class AboutDialog : public AboutDialogBase
{
    Q_OBJECT

public:
    AboutDialog( QWidget* parent,
            const char* name = 0, bool modal = false,
            WFlags f = 0 );

protected slots:
    /**
     * Qt Designer specific method. Updates all controls that has text. We
     * control the version label ourselfs, so we need to reimplement it.
     */
    virtual void languageChange();
};

#endif /* _ABOUTDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: AboutDialog.hh 1169 2005-12-12 15:22:15Z modesto $
