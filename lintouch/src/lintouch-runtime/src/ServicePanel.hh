// $Id: ServicePanel.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: ServicePanel.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 15 February 2005
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

#ifndef _SERVICEPANEL_HH
#define _SERVICEPANEL_HH

#include "ServicePanelBase.h"

#include <lt/project/Project.hh>

class QWidget;
class QListBoxItem;

/**
 * ServicePanel
 *
 * \author Patrik Modesto <modesto@swac.cz>
 */
class ServicePanel : public ServicePanelBase
{
    Q_OBJECT

public:
    /**
     * ctor
     */
    ServicePanel( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );

    /**
     * ctor
     */
    virtual ~ServicePanel();

    /**
     * Set the project that is (to be) loaded, fills the views and panels
     * from it. If the autoopen is true, proceed directly to the default or
     * fisrt view and its home panel.
     * \param p The Project
     * \param defaultView The view to be selected
     * \param autoopen automatically open the given view
     */
    void setProject(const Project& p, const QString& defaultView,
            bool autoopen);

public slots:
    virtual void setHostPortEnabled(bool b);
    virtual void setProjectEnabled(bool b);
    virtual void setServerConsoleEnabled(bool b);
    virtual void setPanelSelectorEnabled(bool b);
    virtual void setViewSelectorEnabled(bool b);

    virtual void setHostNPort(const QString& host, int port, bool editable);
    virtual void setProject(const QString& project, bool editable);

signals:
    /**
     * Emitted when user clicks the <Connect/Disconnect> in the so called
     * Project group.
     */
    void startStopServer();

    /**
     * Emitted when user clicks the <Connect/Disconnect> in the so called
     * HostPort group.
     */
    void connectDisconnect();

    /**
     * Emitted when user double-clicks a panel in the Panels QListBox.
     * \param view Name of the selected View.
     * \param panel Name of the selected Panel.
     */
    void selectViewPanelRequested(const QString& view, const QString& panel);

    /**
     * Emitted when user clicks the Project selection tool button.
     */
    void projectSelectionRequested();

protected slots:
    virtual void startStopServerSlot();
    virtual void connectDisconnectSlot();
    virtual void selectProjectSlot();

    virtual void selectViewClickedSlot(QListBoxItem*);
    virtual void selectPanelClickedSlot(QListBoxItem*);

    virtual void selectViewRequestedSlot(QListBoxItem*);
    virtual void selectPanelRequestedSlot(QListBoxItem*);

private:
    struct ServicePanelPrivate;
    ServicePanelPrivate* d;
};

#endif /* _SERVICEPANEL_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServicePanel.hh 1168 2005-12-12 14:48:03Z modesto $
