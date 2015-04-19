// $Id: ViewBuilder.cpp,v 1.16 2004/09/14 16:23:26 mman Exp $
//
//   FILE: ViewBuilder.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 13 November 2003
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

#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationBuilder.hh>

#include "lt/project/PanelBuilder.hh"
#include "lt/project/ViewBuilder.hh"
using namespace lt;

struct ViewBuilder::ViewBuilderPrivate
{
    /**
     * True if the build process has been cancelled.
     */
    bool cancelled;

    /**
     * The constructed template manager.
     */
    const TemplateManager &tm;

    /**
     * The number of all the panels in the project.
     */
    int panelsTotal;

    /**
     * The number of the panel currently being constructed among all panels
     * of the project.
     */
    int currentPanel;

    /**
     * Localizator of the project.
     */
    LocalizatorPtr localizator;

    ViewBuilderPrivate(int panelsTotal_, const TemplateManager &tm_,
        LocalizatorPtr localizator_)
        : cancelled(false), tm(tm_), panelsTotal(panelsTotal_),
        currentPanel(0), localizator(localizator_)
    {
    }
};

ViewBuilder::ViewBuilder(int panelsTotal, const TemplateManager &tm,
    LocalizatorPtr localizator)
    : d(new ViewBuilderPrivate(panelsTotal, tm, localizator))
{
}

ViewBuilder::~ViewBuilder()
{
    delete d;
}

void ViewBuilder::cancel()
{
    d->cancelled = true;
}

View ViewBuilder::buildFromFile(QIODevice *dev,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    QString err;
    int line, col;
    ConfigurationPtr c =
        DefaultConfigurationBuilder().buildFromFile(
            dev, err, line, col);
    return buildFromConfiguration(c, logger);
}

View ViewBuilder::buildFromConfiguration(ConfigurationPtr c,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    enableLogging(logger);

    d->cancelled = false;
    if (c.isNull())
    {
        getLogger().warn("Invalid view configuration");
        return View();
    }

    View result;
    ConfigurationPtr node;

    //virtual dimensions
    node = c->getChild("geometry");
    if (node.isNull())
    {
        getLogger().warn("Missing the geometry in the view configuration");
        return View();
    }
    int width = node->getAttributeAsInt("width", -1);
    if (width == -1)
    {
        getLogger().warn("The width of the view is missing or invalid");
        return View();
    }
    int height = node->getAttributeAsInt("height", -1);
    if (width == -1)
    {
        getLogger().warn("The height of the view is missing or invalid");
        return View();
    }
    result.setVirtualWidth(width);
    result.setVirtualHeight(height);

    //panels - the standard read-container pattern
    node = c->getChild("panels");
    if (!node.isNull())
    {

        //home panel id
        result.setHomePanelId( node->getAttribute("home-panel-id") );

        ConfigurationList panelNodes = node->getChildren("panel");
        //the view is valid from this moment on no matter what
        for (ConfigurationList::const_iterator it =
                const_cast<ConfigurationList&>(panelNodes).begin();
                it != const_cast<ConfigurationList&>(panelNodes).end(); ++it)
        {
            QString name = (*it)->getAttribute("id");
            if (name.isEmpty())
            {
                name = QString::number((long) &(*it), 16);
                getLogger().warn(QString(
                            "The panel is not assigned an id %1, using %2")
                    .arg((*it)->getLocation())
                    .arg(name));
            }

            getLogger().debug(QString("Creating the panel %1").arg(name));

            Panel panel = PanelBuilder::buildFromConfiguration(*it,
                d->localizator, d->tm,
                getLogger().getChildLogger("panel-" + name));
            if (d->cancelled)
                return View();

            result.addPanel(name, panel);
            emit buildProgress(d->currentPanel++, d->panelsTotal);
            if (d->cancelled)
                return View();
        }
    }
    if (result.panels().count() == 0)
        getLogger().warn("No panels valid");

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ViewBuilder.cpp,v 1.16 2004/09/14 16:23:26 mman Exp $
