// $Id: CopyPasteBuffer.hh 1193 2005-12-22 10:58:02Z modesto $
//
//   FILE: CopyPasteBuffer.hh --
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 01 September 2004
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

#ifndef _LTEDITORCOPYPASTEBUFFER_HH
#define _LTEDITORCOPYPASTEBUFFER_HH

#include <qobject.h>

#include <lt/templates/Template.hh>
#include <lt/templates/TemplateManager.hh>

#include "EditorProject.hh"
using namespace lt;

/**
 * Copy paste buffer.
 *
 * Copy paste buffer remembers set of templates.
 *
 * You can feed existing templates to the buffer via copy(), internal copies
 * will be made, and you can create new copies from these via paste().
 *
 * @author Martin Man <Martin.Man@seznam.cz>
 *
 */
class CopyPasteBuffer : public QObject
{
    Q_OBJECT

    public:

        /**
         * Creates copy paste buffer.
         */
        CopyPasteBuffer( QObject* parent = 0, const char* name = 0 );

        /**
         * Destroys copy paste buffer and any currently remembered
         * templates.
         */
        virtual ~CopyPasteBuffer();

    private:

        /**
         * Disable unintentional copying.
         */
        CopyPasteBuffer( const CopyPasteBuffer & );
        CopyPasteBuffer & operator =( const CopyPasteBuffer & );

    public:

        /**
         * Creates copies of given templates and stores them in the internal
         * buffer.
         */
        void copy( const TemplateDict & selection, const QString& view,
                const QString& panel, const EditorProject* ep );

        /**
         * Creates new copies of remembered templates, they can be right
         * ahead placed on some panel.
         *
         * since they can be possibly pasted to another project, you have to
         * specify the EditorProject and Logger for them.
         *
         * The bl is list of bindings for for returned TemplateDict.
         */
        TemplateDict paste( const EditorProject* ep,
                /* [OUTPUT] */ BindingsList& bl,
                LoggerPtr logger = Logger::nullPtr() );

        /**
         * Return true if the buffer is empty.
         */
        bool isEmpty() const;

    protected:

        /**
         * Clears the buffer, while deleting any remembered templates.
         */
        void clear();

    private:

        struct CopyPasteBufferPrivate;
        CopyPasteBufferPrivate * d;
};

#endif /* _LTEDITORCOPYPASTEBUFFER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: CopyPasteBuffer.hh 1193 2005-12-22 10:58:02Z modesto $
