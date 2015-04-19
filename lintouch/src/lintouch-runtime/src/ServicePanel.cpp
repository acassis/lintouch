// $Id: ServicePanel.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: ServicePanel.cpp -- 
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

#include "ServicePanel.hh"

#include <qlayout.h>
#include <qgroupbox.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

struct ServicePanel::ServicePanelPrivate
{
    Project project;
    QString defaultView;
};

ServicePanel::ServicePanel(QWidget* parent /*= 0*/, const char* name /*= 0*/,
        WFlags fl /*= 0*/) : ServicePanelBase(parent, name, fl),
                             d(new ServicePanelPrivate)
{
    Q_CHECK_PTR(d);

    Q_ASSERT(g1->inherits("QGridLayout"));
    g1->setColStretch(0, 2);
    g1->setColStretch(1, 5);
}

ServicePanel::~ServicePanel()
{
    delete d;
}

void ServicePanel::setHostPortEnabled(bool b)
{
    hostGroup->setShown(b);
}

void ServicePanel::setProjectEnabled(bool b)
{
    projectGroup->setShown(b);
}

void ServicePanel::setServerConsoleEnabled(bool b)
{
    serverGroup->setShown(b);
}

void ServicePanel::setPanelSelectorEnabled(bool b)
{
    panelsGroup->setShown(b);

    // if the panel is visible, restore columns factors
    if(b) {
        g1->setColStretch(0, 2);
        g1->setColStretch(1, 5);
    } else if(viewsGroup->isHidden()) {
        g1->setColStretch(0, 0);
        g1->setColStretch(1, 0);
    }
}

void ServicePanel::setViewSelectorEnabled(bool b)
{
    viewsGroup->setShown(b);

    // if the panel is visible, restore columns factors
    if(b) {
        g1->setColStretch(0, 2);
        g1->setColStretch(1, 5);
    } else if(panelsGroup->isHidden()) {
        g1->setColStretch(0, 0);
        g1->setColStretch(1, 0);
    }
}

void ServicePanel::setHostNPort(const QString& host, int port, bool editable)
{
    hostName->setText(host);
    hostName->setEnabled(editable);
    hostPort->setText(QString::number(port));
    hostPort->setEnabled(editable);
    connectServer->setEnabled(editable);
}

void ServicePanel::setProject(const QString& project, bool editable)
{
    projFilePath->setText(project);
    projFilePath->setEnabled(editable);
    chooseProject->setEnabled(editable);
    runProject->setEnabled(editable);
}

void ServicePanel::startStopServerSlot()
{
    emit(startStopServer());
}

void ServicePanel::connectDisconnectSlot()
{
    emit(connectDisconnect());
}

void ServicePanel::selectProjectSlot()
{
    emit(projectSelectionRequested());
}

void ServicePanel::selectViewRequestedSlot(QListBoxItem* i)
{
    Q_ASSERT(i);
    emit(selectViewPanelRequested(i->text(),
                d->project.views()[i->text()].panels().keyAt(0)
                ));
}

void ServicePanel::selectPanelRequestedSlot(QListBoxItem* i)
{
    Q_ASSERT(i);
    emit(selectViewPanelRequested(views->currentText(), i->text()));
}

void ServicePanel::setProject(const Project& p, const QString& defaultView,
        bool autoopen )
{
    Q_ASSERT(d);
    d->project = p;
    d->defaultView = defaultView;

    views->clear();
    panels->clear();

    const ViewMap& vm = p.views();
    for(unsigned i = 0; i < vm.count(); ++i) {
        views->insertItem(vm.keyAt(i));
    }

    View v;
    if(vm.contains(defaultView)) {
        v = vm[defaultView];
        views->setCurrentItem(vm.indexOf(defaultView));
    } else {
        v = vm[0];
        views->setCurrentItem(0);
    }

    const PanelMap& pm = v.panels();
    for(unsigned i = 0; i < pm.count(); ++i) {
        panels->insertItem(pm.keyAt(i));
    }

    if( autoopen ) {
        emit(selectViewPanelRequested(
                    views->currentText(), v.homePanelId() ) );
    }
}

void ServicePanel::selectViewClickedSlot(QListBoxItem* i)
{
    Q_ASSERT(i);

    QString v = i->text();

    const ViewMap& vm = d->project.views();
    Q_ASSERT(vm.contains(v));

    panels->clear();

    const PanelMap& pm = vm[v].panels();
    for(unsigned i = 0; i < pm.count(); ++i) {
        panels->insertItem(pm.keyAt(i));
    }
}

void ServicePanel::selectPanelClickedSlot(QListBoxItem* i)
{
    Q_ASSERT(i);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServicePanel.cpp 1168 2005-12-12 14:48:03Z modesto $
