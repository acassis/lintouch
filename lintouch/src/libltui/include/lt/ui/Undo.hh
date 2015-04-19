// $Id: Undo.hh 1168 2005-12-12 14:48:03Z modesto $
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

#ifndef _UNDO_HH
#define _UNDO_HH

#include <qaction.h>

namespace lt {
    class UndoCommand;
    class UndoAction;

    /**
     * Implements simple undo stack.
     */
    class UndoStack : public QObject
    {
        Q_OBJECT

        public:
            /**
             * ctor
             */
            UndoStack(QObject* parent);

            /**
             * dtor
             */
            virtual ~UndoStack();

            /* UndoStack API */
        public:
            bool canRedo() const;
            bool canUndo() const;

            QAction* createRedoAction(QObject* parent) const;
            QAction* createUndoAction(QObject* parent) const;

            bool isClean() const;
            void clear();

            void push(UndoCommand* command);

        public slots:
            void redo();
            void undo();

        signals:
            void canRedoChanged(bool enabled);
            void canUndoChanged(bool enabled);

            void cleanChanged(bool clean);

            // @internal
            void undoDescriptionChanged(const QString& newDescription);
            void redoDescriptionChanged(const QString& newDescription);

        protected:
            // @internal
            QString redoDescription() const;
            QString undoDescription() const;

        private:
            struct UndoStackPrivate;
            UndoStackPrivate* d;
    };

    /**
     * Base class for undo commands.
     */
    class UndoCommand
    {
        public:
            /**
             * ctor
             */
            UndoCommand(const QString& description = QString::null);

            /**
             * dtor
             */
            virtual ~UndoCommand();

            virtual void redo();
            virtual void undo();

            void setDescription(const QString& description);
            QString description() const;

        protected:
            QString desc;
    };

    /**
     * UndoAction class. Get notified of UndoStack changes.
     */
    class UndoAction : public QAction
    {
        Q_OBJECT

        public:
            UndoAction(QObject* parent);
            virtual ~UndoAction();

        public slots:
            void setTextSlot(const QString &text);
    };
}

#endif /* _UNDO_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Undo.hh 1168 2005-12-12 14:48:03Z modesto $
