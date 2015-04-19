// $Id: TemplatePalette.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplatePalette.cpp -- 
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

#include "TemplatePalette.hh"

#include <qtooltip.h>

using namespace lt;

#define QDEBUG_RECT( r ) qDebug(QString(#r"(%5): %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()).arg((int)this))

/**
 * Private QIconViewItem for Template palette.
 */
class TemplatePaletteItem : public QIconViewItem
{
public:
    enum {
        LTEDITORTEMPLATEPALETTEITEM = 1006
    };

    /**
     * ctor
     */
    TemplatePaletteItem(QIconView* parent,
            TemplatePaletteItem* after,
            const QString& tln, const QString& tn, const Info& i) :
        QIconViewItem(parent, after)
    {
        setText(i.shortDescription());
        setPixmap(i.icon());
        this->tln = tln;
        this->tn = tn;
        this->i = i;
    }

    /**
     * rtti
     */
    virtual int rtti() const
    {
        return LTEDITORTEMPLATEPALETTEITEM;
    }

    /**
     * Getter for TemplateLibrary name.
     */
    QString templateLibraryName() const
    {
        return tln;
    }

    /**
     * Getter for TemplateLibrary name.
     */
    QString templateName() const
    {
        return tn;
    }

    /**
     * Returns current Info.
     */
    const Info& info() const
    {
        return i;
    }

protected:
    /**
     * TemplateLibrary name and Template name.
     */
    QString tln, tn;

    /**
     * Property to hold the Info.
     */
    Info i;
};


/**
 * Private Tooltip class
 */
class TemplatePaletteToolTip : public QToolTip
{
public:
    TemplatePaletteToolTip(QWidget* parent,
            TemplatePalette* palette, QToolTipGroup* group = 0)
        : QToolTip(parent,group), pl(palette)
    {
        Q_ASSERT(pl);
    }

protected:
    virtual void maybeTip(const QPoint& p)
    {
        TemplatePaletteItem* i = dynamic_cast<TemplatePaletteItem*>(
                pl->findItem(pl->viewportToContents(p)));
        if(i) {
            QRect r(pl->contentsToViewport(i->rect().topLeft()),
                    i->rect().size());
            tip(r, QString("%1\n%2, %3\n%4\n%5")
                    .arg(i->info().shortDescription())
                    .arg(i->info().version())
                    .arg(i->info().date().toString())
                    .arg(i->info().author())
                    .arg(i->info().longDescription()));
        }
    }

    TemplatePalette* pl;
};

/**
 * Private palette data
 */
struct TemplatePalette::TemplatePalettePrivate
{
    TemplatePalettePrivate() : toolTip(NULL) {}

    TemplateManager tm;
    QString tln;
    QString tn;

    TemplatePaletteToolTip* toolTip;
};

///////////////////////////////////////////////////////////////////////

TemplatePalette::TemplatePalette( lt::TemplateManager tm,
        QWidget* parent) : QIconView(parent,"TemplatePalette"),
                           d(new TemplatePalettePrivate)
{
    Q_CHECK_PTR(d);

    d->toolTip = new TemplatePaletteToolTip(viewport(), this);

    d->tm = tm;
    setAutoArrange(true);
    setSorting(false);
    setResizeMode(Adjust);
    setItemsMovable(false);
    setShowToolTips(false);

    setWordWrapIconText(true);
    setArrangement(LeftToRight);

    // fixed grid width & height
    setGridX(70);
    setGridY(70);

    connect(this, SIGNAL(selectionChanged(QIconViewItem*)),
            SLOT(selectionChangedSlot(QIconViewItem*)));
}

TemplatePalette::~TemplatePalette()
{
    delete d->toolTip;
    delete d;
}

Template* TemplatePalette::instantiate(LoggerPtr logger) const
{
    return d->tm.instantiate( d->tn, logger, d->tln);
}

void TemplatePalette::currentLibraryChangedSlot(const QString& tln)
{
    Q_ASSERT(d);

    const TemplateLibraryDict& tld = d->tm.templateLibraries();
    Q_ASSERT(tld.contains(tln));

    d->tln = tln;

    clear();

    const TemplateLibrary* tl = tld[tln];
    Q_ASSERT(tl);
    Q_ASSERT(tl->isValid());

    TemplatePaletteItem* item = NULL;

    const InfoMap& infos = tl->templates();
    for(unsigned i=0; i<infos.count(); ++i) {
        item = new TemplatePaletteItem(this, item,
                tln, infos.keyAt(i), infos[i]);
        Q_CHECK_PTR(item);
    }

    arrangeItemsInGrid(true);
}

void TemplatePalette::selectionChangedSlot(QIconViewItem* i)
{
    Q_ASSERT(d);
    Q_ASSERT(i);
    Q_ASSERT(i->rtti() ==
            TemplatePaletteItem::LTEDITORTEMPLATEPALETTEITEM);

    // removed dynamic_cast because it segfaults when compiled as ActiveQt
    // server
    TemplatePaletteItem* item = (TemplatePaletteItem*)i;
    Q_ASSERT(item);

    Q_ASSERT(d->tln == item->templateLibraryName());
    d->tn = item->templateName();

    emit(templateSelected());
}

void TemplatePalette::clearSelection()
{
    QIconView::clearSelection();
    d->tn = QString::null;

    emit(templateUnselected());
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplatePalette.cpp 1169 2005-12-12 15:22:15Z modesto $
