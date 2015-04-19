// $Id: Undo.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Undo.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
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

#include <qptrlist.h>

#include "lt/ui/Undo.hh"
using namespace lt;

struct UndoStack::UndoStackPrivate
{
    UndoStackPrivate()
    {
        last = -1;
    }

    QPtrList<UndoCommand> l;
    int last;
};

UndoStack::UndoStack(QObject* parent) : QObject(parent),
    d(new UndoStackPrivate)
{
    Q_CHECK_PTR(d);

    d->l.setAutoDelete(true);
}

UndoStack::~UndoStack()
{
    delete d;
}

bool UndoStack::canRedo() const
{
    if(d->l.isEmpty() || d->l.at(d->last) == d->l.last()) return false;

    return true;
}

bool UndoStack::canUndo() const
{
    if(d->l.isEmpty() || d->last < 0) return false;

    return true;
}

QAction* UndoStack::createRedoAction(QObject* parent) const
{
    UndoAction* a = new UndoAction(parent);
    Q_CHECK_PTR(a);

    connect(a, SIGNAL(activated()), this, SLOT(redo()));
    connect(this, SIGNAL(canRedoChanged(bool)), a, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoDescriptionChanged(const QString&)),
           a, SLOT(setTextSlot(const QString&)));

    a->setText(redoDescription());
    a->setEnabled(canRedo());

    return a;
}

QAction* UndoStack::createUndoAction(QObject* parent) const
{
    UndoAction* a = new UndoAction(parent);
    Q_CHECK_PTR(a);

    connect(a, SIGNAL(activated()), this, SLOT(undo()));
    connect(this, SIGNAL(canUndoChanged(bool)), a, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(undoDescriptionChanged(const QString&)),
           a, SLOT(setTextSlot(const QString&)));

    a->setText(undoDescription());
    a->setEnabled(canUndo());

    return a;
}

bool UndoStack::isClean() const
{
    Q_ASSERT(d);
    return d->last < 0;
}

void UndoStack::clear()
{
    Q_ASSERT(d);

    d->l.clear();
    d->last = -1;

    emit(redoDescriptionChanged(redoDescription()));
    emit(undoDescriptionChanged(undoDescription()));
    emit(cleanChanged(true));
    emit(canUndoChanged(false));
    emit(canRedoChanged(false));
}

void UndoStack::push(UndoCommand* command)
{
    Q_ASSERT(d);
    Q_ASSERT(command);

    if(canRedo()) {
        // remove orphaned commands
        for(int i = d->last+1; d->l.at(i); d->l.remove(i));
    }

    d->l.append(command);

    redo();
}

void UndoStack::redo()
{
    Q_ASSERT(d);

    if(!canRedo()) return;

    bool couldRedo = canRedo();
    bool couldUndo = canUndo();
    bool wasClean = isClean();

    ++d->last;
    UndoCommand* c = d->l.at(d->last);
    Q_ASSERT(c);
    c->redo();

    emit(redoDescriptionChanged(redoDescription()));
    emit(undoDescriptionChanged(undoDescription()));
    if(couldRedo != canRedo()) {
        emit(canRedoChanged(!couldRedo));
    }
    if(couldUndo != canUndo()) {
        emit(canUndoChanged(!couldUndo));
    }
    if(wasClean != isClean()) {
        emit(cleanChanged(!wasClean));
    }
}

void UndoStack::undo()
{
    Q_ASSERT(d);

    if(!canUndo()) return;

    bool couldRedo = canRedo();
    bool couldUndo = canUndo();
    bool wasClean = isClean();

    UndoCommand* c = d->l.at(d->last);
    Q_ASSERT(c);
    c->undo();
    --d->last;

    emit(redoDescriptionChanged(redoDescription()));
    emit(undoDescriptionChanged(undoDescription()));
    if(couldRedo != canRedo()) {
        emit(canRedoChanged(!couldRedo));
    }
    if(couldUndo != canUndo()) {
        emit(canUndoChanged(!couldUndo));
    }
    if(wasClean != isClean()) {
        emit(cleanChanged(!wasClean));
    }
}

QString UndoStack::redoDescription() const
{
    QString t = tr("Redo") + " ";
    if(canRedo()) {
        Q_ASSERT(d->last+1 >= 0);
        Q_ASSERT(d->last < (int)d->l.count());
        t += d->l.at(d->last + 1)->description();
    }
    return t;
}

QString UndoStack::undoDescription() const
{
    QString t = tr("Undo") + " ";
    if(canUndo()) {
        Q_ASSERT(d->last >= 0);
        Q_ASSERT(d->last < (int)d->l.count());
        t += d->l.at(d->last)->description();
    }
    return t;
}

/////////////////////////////// UndoCommand //////////////////////////////////

UndoCommand::UndoCommand(const QString& description /*= QString::null*/) :
    desc(description)
{
}

UndoCommand::~UndoCommand()
{
}

void UndoCommand::setDescription(const QString& description)
{
    desc = description;
}

QString UndoCommand::description() const
{
    return desc;
}

void UndoCommand::redo()
{
}

void UndoCommand::undo()
{
}

//////////////////////////////// UndoAction //////////////////////////////////

UndoAction::UndoAction(QObject* parent) : QAction(parent)
{
}

UndoAction::~UndoAction()
{
}

void UndoAction::setTextSlot(const QString &text)
{
   setText(text);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Undo.cpp 1168 2005-12-12 14:48:03Z modesto $
