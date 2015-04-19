// $Id: ProjectPreviewDialogPrivate.hh 1200 2006-01-04 10:33:22Z modesto $
//
//   FILE: ProjectPreviewDialog.cpp --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 15 July 2004
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

#ifndef _LTEDITORPROJECTPREVIEWDIALOGPROVATE_HH
#define _LTEDITORPROJECTPREVIEWDIALOGPROVATE_HH

#include "ProjectPreviewDialog.hh"

#include <qpushbutton.h>
#include <qtable.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

typedef struct VariableData{

    VariableData(){
        type = IOPin::InvalidType;
        iv = 0;
        sv = "";
        bv = false;
    }

    ~VariableData(){
    }

    IOPin::Type type;
    int         iv;
    QString     sv;
    bool        bv;

};

typedef QMap< QString, VariableData* > VariableMap;
typedef QMap< QString, VariableMap  > ConnectionVariableMap;


struct ProjectPreviewDialog::ProjectPreviewDialogPrivate
{
    ProjectPreviewDialogPrivate() :
        pw(NULL), darkBackground( false ) {}

    ~ProjectPreviewDialogPrivate() {
        clean();
    }

    void clean(){

        if ( !cvm.empty() ){
            ConnectionVariableMap::iterator icvm;

            for ( icvm = cvm.begin(); icvm != cvm.end(); ++icvm ){
               VariableMap& vm = icvm.data();

                if ( !vm.empty() ){

                    VariableMap::iterator ivm;
                    for ( ivm = vm.begin(); ivm != vm.end(); ++ivm ){
                        delete ivm.data();
                    }
                    vm.clear();
                }
            }

            cvm.clear();
        }

    }


    ProjectWindow * pw;
    bool darkBackground;

    ConnectionVariableMap cvm;
    IOPinList iopl;
    TemplateList tl;
};

class TableLineEdit : public QLineEdit
{
    Q_OBJECT

    public:
        TableLineEdit( QTable* t, const QString& v) :
            QLineEdit( t->viewport() ), tbl( t )
        {
            setText( v );
        }

        inline QTable* table() const { return tbl; }
    private:
        QTable* tbl;
};


class TableCellSpinBox : public QSpinBox
{
    Q_OBJECT

    public:
        TableCellSpinBox( QTable* t, int v ) :
            QSpinBox( t->viewport() ), tbl( t )
        {
            setValue( v );
        }

        inline QTable* table() const { return tbl; }
    protected:
        void keyPressEvent( QKeyEvent* e )
        {
            Q_ASSERT( e );
            if ( e->key() == Key_Return || e->key() == Key_Enter ){
                emit( valueChanged( value() ) );
            }
            QSpinBox::keyPressEvent( e );
        }
    private:
        QTable* tbl;
};


class TableCellButton : public QPushButton
{
    Q_OBJECT

    public:
        TableCellButton( QTable* t, bool v) :
            QPushButton( t->viewport() ), tbl( t )
        {
            setToggleButton( true );
            setOn( v );
        }

        inline QTable* table() const { return tbl; }

    public slots:
        virtual void setOn( bool on)
        {
            QPushButton::setOn( on );
            setText( on ? tr("1") : tr("0") ); 
        }

    private:
        QTable* tbl;
};

#endif

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPreviewDialogPrivate.hh 1200 2006-01-04 10:33:22Z modesto $
