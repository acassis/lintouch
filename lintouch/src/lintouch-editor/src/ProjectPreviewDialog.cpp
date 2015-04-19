// $Id: ProjectPreviewDialog.cpp 1570 2006-04-11 12:15:08Z modesto $
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

#include <qtable.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qptrlist.h>
#include <qlistbox.h>

#include <lt/project/View.hh>
#include <lt/project/Panel.hh>

#include "AddDelConnectionCommand.hh"
#include "AddDelPanelCommand.hh"
#include "AddDelVariableCommand.hh"
#include "Binding.hh"
#include "Bindings.hh"
#include "EditablePanelViewContainer.hh"
#include "EditorProject.hh"
#include "ChangeBindingCommand.hh"
#include "MainWindow.hh"
#include "PanelOrderChangeCommand.hh"
#include "ProjectPreviewDialog.hh"
#include "ProjectPreviewDialogPrivate.hh"
#include "ProjectPreviewTable.hh"
#include "ProjectWindow.hh"
#include "RenameConnectionCommand.hh"
#include "RenamePanelCommand.hh"
#include "RenameVariableCommand.hh"
#include "RenameViewCommand.hh"

using namespace lt;

enum
{
    COL_FIRST, COL_SECOND, COL_BIT, COL_NUMBER, COL_STRING, MAX_COL
};

#define     PREVIEW_IOPINS      0x1
#define     PREVIEW_VARIABLES   0x2


ProjectPreviewDialog::ProjectPreviewDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
        ProjectPreviewDialogBase( parent, name, fl ),
        d( new ProjectPreviewDialogPrivate() )
{
    Q_CHECK_PTR( d );

    m_IOPinValues->init( PREVIEW_IOPINS );
    m_ConnectionVariableValues->init( PREVIEW_VARIABLES );

    Q_ASSERT( m_IOPinValues->numCols() == MAX_COL );
    Q_ASSERT( m_ConnectionVariableValues->numCols() == MAX_COL );
}

ProjectPreviewDialog::~ProjectPreviewDialog()
{
    delete d;
}

void ProjectPreviewDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT( d );

    //qWarning( "ProjectPreviewDialog::projectChangedSlot: "
    //        " pw = %d, t = %d", (unsigned)pw, t );

    if( pw != d->pw || t == LTE_CMD_NOOP
        || t == LTE_CMD_VIEW_SELECT || t == LTE_CMD_PANEL_SELECT
        || t == LTE_CMD_PANEL_RENAME || t == LTE_CMD_VIEW_RENAME
        || t == LTE_CMD_PANEL_ADD || t == LTE_CMD_PANEL_DELETE
        || t == LTE_CMD_PANEL_ORDER_CHANGE || t == LTE_CMD_SELECTION_ADD
        || t == LTE_CMD_SELECTION_DEL
        || t == LTE_CMD_TEMPLATE_RENAME
        || t == LTE_CMD_TEMPLATE_TABORDER_CHANGE
        || t == LTE_CMD_BINDING_CHANGE
        || t == LTE_CMD_CONNECTION_RENAME
        || t == LTE_CMD_VARIABLE_RENAME
        || t == LTE_CMD_CONNECTION_ADD
        || t == LTE_CMD_CONNECTION_DEL
        || t == LTE_CMD_VARIABLE_ADD
        || t == LTE_CMD_VARIABLE_DEL )
    {
        bool enable = ( pw != NULL );
        m_tabPreviews->setEnabled( enable );
        m_IOPinValues->setEnabled( enable );
        m_ConnectionVariableValues->setEnabled( enable );
        m_comboPanels->setEnabled( enable );
        m_labelPanel->setEnabled( enable );

        bool update_cvm = ( d->pw && ( d->pw == pw ) );

        d->pw = pw;

        if ( d->pw ) {

            fillConnectionVariableMap( update_cvm );

            //reset values
            d->darkBackground = false;

            if ( t == LTE_CMD_PANEL_SELECT ||
                    t == LTE_CMD_TEMPLATE_RENAME ||
                    t == LTE_CMD_TEMPLATE_TABORDER_CHANGE ){

                if( d->pw->currentPanel().isEmpty() ) {
                    makeDialogEmpty();
                } else {
                    m_comboPanels->setCurrentText( d->pw->currentPanel() );
                    onPanelChanged( m_comboPanels->currentText() );
                }

            } else if ( t == LTE_CMD_VIEW_RENAME &&
                    pw->lastCommand()->rtti() == LTE_CMD_VIEW_RENAME ){

                RenameViewCommand* cmd =
                        (RenameViewCommand*)pw->lastCommand();
                Q_ASSERT( cmd );

                if ( m_comboPanels->caption() == cmd->oldViewName() ) {
                    m_comboPanels->setCaption( cmd->newViewName() );
                }

            } else if ( t == LTE_CMD_PANEL_RENAME &&
                    pw->lastCommand()->rtti() == LTE_CMD_PANEL_RENAME ){

                RenamePanelCommand* cmd =
                        (RenamePanelCommand*)pw->lastCommand();
                Q_ASSERT( cmd );

                if ( m_comboPanels->caption() == cmd->viewName() ){
                    int index = indexInPanelCombobox( cmd->oldPanelName() );
                    m_comboPanels->changeItem( cmd->newPanelName(), index );
                }
            } else if ( t == LTE_CMD_PANEL_ADD
                        || t == LTE_CMD_PANEL_DELETE ){

                if ( pw->lastCommand()->rtti()
                        == LTE_CMD_PANEL_ADD_DEL ){
                    AddDelPanelCommand* cmd =
                            (AddDelPanelCommand*)pw->lastCommand();
                    Q_ASSERT(cmd);
                    if(pw->currentView() == cmd->viewName()) {

                        if ( t == LTE_CMD_PANEL_ADD ){
                            m_comboPanels->insertItem( cmd->panelName(),
                                    cmd->position() );
                        } else if ( t == LTE_CMD_PANEL_DELETE ){
                            int index = indexInPanelCombobox(
                                    cmd->panelName() );
                            m_comboPanels->removeItem( index );

                            if ( pw->project()->views()[
                                    cmd->viewName() ].panels().count() == 0){
                                makeDialogEmpty();
                            }
                        }
                    }

                }
            } else if ( t == LTE_CMD_PANEL_ORDER_CHANGE ){

                if ( pw->lastCommand()->rtti() ==
                             LTE_CMD_PANEL_ORDER_CHANGE){

                    PanelOrderChangeCommand* cmd =
                    (PanelOrderChangeCommand*)pw->lastCommand();

                    Q_ASSERT(cmd);

                    if ( m_comboPanels->caption() == cmd->viewName() ){
                        int index = indexInPanelCombobox(
                                cmd->newPanel() );
                        Q_ASSERT( index == cmd->oldPos() );

                        index = indexInPanelCombobox(
                                cmd->oldPanel() );
                        Q_ASSERT( index == cmd->newPos() );

                        m_comboPanels->changeItem( cmd->newPanel(),
                                cmd->newPos() );
                        m_comboPanels->changeItem( cmd->oldPanel(),
                                cmd->oldPos() );
                    }

                }
            } else if ( ( t == LTE_CMD_BINDING_CHANGE &&
                    pw->lastCommand()->rtti() == LTE_CMD_BINDING_CHANGE )||
                    ( t == LTE_CMD_CONNECTION_RENAME &&
                    pw->lastCommand()->rtti() ==
                                         LTE_CMD_CONNECTION_RENAME )||
                    ( t == LTE_CMD_VARIABLE_RENAME &&
                    pw->lastCommand()->rtti() == LTE_CMD_VARIABLE_RENAME )||
                    ( t == LTE_CMD_CONNECTION_ADD &&
                    pw->lastCommand()->rtti() == LTE_CMD_CONNECTION_ADD_DEL )||
                    ( t == LTE_CMD_CONNECTION_DEL &&
                    pw->lastCommand()->rtti() == LTE_CMD_CONNECTION_ADD_DEL )||
                    ( t == LTE_CMD_VARIABLE_ADD &&
                    pw->lastCommand()->rtti() == LTE_CMD_VARIABLE_ADD_DEL )||
                    ( t == LTE_CMD_VARIABLE_DEL &&
                    pw->lastCommand()->rtti() == LTE_CMD_VARIABLE_ADD_DEL )){

                updateConnectionVariableMap( t );

                if( d->pw->currentPanel().isEmpty() ) {
                    makeDialogEmpty();
                } else {
                    onPanelChanged( m_comboPanels->currentText() );
                }
            } else {
                populateFromView( d->pw->currentView() );
            }

            setCaption( QString( tr("%1/%2 Preview - Lintouch Editor") )
                    .arg( d->pw->projectFileInfo().baseName())
                    .arg( d->pw->currentView() ) );

        } else {

            setCaption( tr("Preview - Lintouch Editor") );

            makeDialogEmpty();
        }
    }
}

void ProjectPreviewDialog::resizeEvent( QResizeEvent* )
{
    m_IOPinValues->setColumnWidths();
    m_ConnectionVariableValues->setColumnWidths();
}

void ProjectPreviewDialog::populateFromView( const QString& vn )
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );

    const ViewMap& vm = d->pw->project()->views();
    m_comboPanels->clear();

    if( vn.isEmpty() ) return;


    Q_ASSERT( vm.contains( vn ) );

    const View& v = vm[ vn ];
    const PanelMap& pm = v.panels();

    m_comboPanels->setCaption( vn );
    for( unsigned ip = 0; ip < pm.count(); ip++ ) {
        m_comboPanels->insertItem( pm.keyAt( ip ) );
    }
    bool panelsExist = ( pm.count() > 0 );
    m_comboPanels->setEnabled( panelsExist );

    if ( panelsExist ){
        if ( pm.contains( d->pw->currentPanel() ) ){
            m_comboPanels->setCurrentText( d->pw->currentPanel() );
        } else {
            m_comboPanels->setCurrentItem( 0 );
        }
        panelChanged( m_comboPanels->currentText() );
    }
}

void ProjectPreviewDialog::setRowValue( QTable *tbl, int r,
            const QString& columnOne, const QString& columnTwo,
            const QString& dataColumnOne, const QString& dataColumnTwo )
{
    Q_ASSERT( tbl );

    if ( r >= tbl->numRows() ){
        tbl->insertRows( r );
    }
    setCellText( tbl, r, COL_FIRST, columnOne,
            dataColumnOne, false, true );
    setCellText( tbl, r, COL_SECOND, columnTwo, dataColumnTwo, false, true );

    IOPin::TypeList tl = getSupportedTypes( tbl, dataColumnOne,
            dataColumnTwo );

    for( IOPin::TypeList::const_iterator ityp = tl.begin();
        ityp != tl.end(); ++ityp ){
        IOPin::Type type = *ityp;

        if ( ( type == IOPin::NumberType )
            || ( type == IOPin::BitType )
            || ( type == IOPin::StringType ) ){
            QWidget* cellWidget = iopinWidget( tbl, type, dataColumnOne,
                    dataColumnTwo );

            int col = ( type == IOPin::NumberType )? COL_NUMBER :
                    ( type == IOPin::BitType) ? COL_BIT : COL_STRING;

            if (cellWidget) {
                setCellWidget( tbl, r, col, cellWidget );
            }
        }
    }

    for (int c = COL_BIT; c <= COL_STRING; c++ ){

        if ( tbl->cellWidget( r, c ) == NULL ){
            setCellText( tbl, r, c, "" );
        }
    }
}


Template* ProjectPreviewDialog::affectedTemplate( const QString& tn,
        const QString& pn /*= QString::null*/ )
{
    Q_ASSERT( m_comboPanels );
    Q_ASSERT( tn.isEmpty() == false );

    QString vn = m_comboPanels->caption();
    Q_ASSERT( vn.isEmpty() == false );

    QString panel = pn;
    if(panel.isEmpty()) {
        panel = m_comboPanels->currentText();
    }
    Q_ASSERT( panel.isEmpty() == false );

    const ViewMap& vm = d->pw->project()->views();
    Q_ASSERT( vm.contains( vn ) );

    const View& v = vm[ vn ];
    const PanelMap& pm = v.panels();
    Q_ASSERT( pm.contains( panel ) );

    const Panel& p = pm[ panel ];
    TemplateDict td = p.templates();
    Template* t = td[ tn ];
    Q_ASSERT( t );

    return t;
}

const IOPinList& ProjectPreviewDialog::affectedIOPins(
        int mode, const QString& data1, const QString& data2 )
{
    Q_ASSERT( d );
    Q_ASSERT( d->iopl.autoDelete() == false );

    d->iopl.clear();

    if ( mode == PREVIEW_IOPINS ){

        Template* t = affectedTemplate( data1 );
        Q_ASSERT( t );

        IOPin* iop = t->iopins()[ data2 ];
        Q_ASSERT( iop );

        d->iopl.append( iop );

    } else if ( mode == PREVIEW_VARIABLES ) {

        const EditorProject* ep = d->pw->project();
        const BindingsList& bl =
            ep->bindings().bindingsByVariable(data1,data2);

        for(BindingsList::const_iterator ibl = bl.begin();
                ibl != bl.end(); ++ibl) {
            d->iopl.append(ep->views()[(*ibl)->view()].
                    panels()[(*ibl)->panel()].
                    templates()[(*ibl)->templ()]->iopins()[(*ibl)->iopin()]);
        }
    }
    return d->iopl;
}

const TemplateList& ProjectPreviewDialog::affectedTemplates(
        int mode, const QString& data1, const QString& /*data2*/ )
{
    Q_ASSERT( d );
    Q_ASSERT( d->tl.autoDelete() == false );

    d->tl.clear();

    if ( mode == PREVIEW_IOPINS ){

        Template* t = affectedTemplate( data1 );
        Q_ASSERT( t );
        d->tl.append( t );

    } else if ( mode == PREVIEW_VARIABLES ){

        QString vn = m_comboPanels->caption();
        Q_ASSERT( vn.isEmpty() == false );

        QString pn = m_comboPanels->currentText();
        Q_ASSERT( pn.isEmpty() == false );

        const BindingsList& bl = d->pw->project()->
            bindings().bindingsByView( vn );
        BindingsList::const_iterator ibl;

        for( ibl = bl.begin(); ibl != bl.end(); ++ibl ) {
            const BindingPtr bp = *ibl;
            Q_ASSERT( !bp.isNull() );

            if(!bp->valid()) {
                continue;
            }

            Template* t = affectedTemplate( bp->templ(), bp->panel() );
            Q_ASSERT( t );
            d->tl.append( t );
        }
    }
    return d->tl;
}

IOPin::TypeList ProjectPreviewDialog::getSupportedTypes( QTable* t,
        const QString& data1, const QString& data2 )
{
    Q_ASSERT( t );
    int mode = ((ProjectPreviewTable*)t)->mode();
    IOPin::TypeList tl;

    if(mode == PREVIEW_VARIABLES) {
        const ConnectionDict& cd = d->pw->project()->connections();
        // don't assume the connection is in the connections() map, it can
        // easily be in the hidden map in which case we don't know what
        // types supports it
        // see #164
        if( cd.contains( data1 ) ) {
            const VariableDict& vd = cd[data1]->variables();
            Q_ASSERT( vd.contains( data2 ) );
            tl.append(vd[data2]->type());
        }

    } else {

        IOPinList iopl = affectedIOPins( mode, data1, data2 );

        IOPin* iop = iopl.first();
        tl = iop->types();

        for( ; iop; iop = iopl.next() ) {
            IOPin::TypeList btl = iop->types();

            if ( tl != btl ){

                for( IOPin::TypeList::const_iterator ityp = btl.begin();
                        ityp != btl.end(); ++ityp ){
                    IOPin::Type type = *ityp;

                    if ( tl.find( type ) == tl.end() ){
                        tl.append( type );
                    }
                }

            }
        }
    }
    return tl;
}

QWidget* ProjectPreviewDialog::iopinWidget( QTable* tbl,
        const IOPin::Type& type, const QString& data1, const QString&
        data2 )
{
    Q_ASSERT( tbl );

    int mode = ((ProjectPreviewTable*)tbl)->mode();
    IOPinList iopl = affectedIOPins( mode, data1, data2 );

    if ( type == IOPin::BitType ) {

        bool v = false;

        if ( mode == PREVIEW_IOPINS ){

            Q_ASSERT( iopl.count() == 1 );

            const IOPin* iop = iopl.first();
            v = iop->realBitValue();

        } else if ( mode == PREVIEW_VARIABLES ) {

            Q_ASSERT( d );

            Q_ASSERT( d->cvm.contains( data1 ) );
            const VariableMap& vm = d->cvm[ data1 ];

            Q_ASSERT( vm.contains( data2 ) );
            const VariableData* vd = vm[ data2 ];
            Q_ASSERT( vd );

            Q_ASSERT( vd->type == IOPin::BitType );
            v = vd->bv;

        } else {
            return NULL;
        }

        QPushButton* pb = new TableCellButton( tbl, v );
        return (QWidget*)pb;

    } else if ( type == IOPin::NumberType ) {

        int v = 0;

        if ( mode == PREVIEW_IOPINS ){

            Q_ASSERT( iopl.count() == 1 );

            const IOPin* iop = iopl.first();
            v = iop->realNumberValue();

        } else if ( mode == PREVIEW_VARIABLES ) {

            Q_ASSERT( d );

            Q_ASSERT( d->cvm.contains( data1 ) );
            const VariableMap& vm = d->cvm[ data1 ];

            Q_ASSERT( vm.contains( data2 ) );
            const VariableData* vd = vm[ data2 ];
            Q_ASSERT( vd );

            Q_ASSERT( vd->type == IOPin::NumberType );
            v = vd->iv;

        } else {
            return NULL;
        }

        QSpinBox* sb = new TableCellSpinBox( tbl, v );
        sb->setMinValue( INT_MIN );
        sb->setMaxValue( INT_MAX );
        return (QWidget*)sb;

    } else if ( type == IOPin::StringType ){

        QString v = "";

        if ( mode == PREVIEW_IOPINS ){

            Q_ASSERT( iopl.count() == 1 );

            const IOPin* iop = iopl.first();
            v = iop->realStringValue();

        } else if ( mode == PREVIEW_VARIABLES ) {

            Q_ASSERT( d );

            Q_ASSERT( d->cvm.contains( data1 ) );
            const VariableMap& vm = d->cvm[ data1 ];

            Q_ASSERT( vm.contains( data2 ) );
            const VariableData* vd = vm[ data2 ];
            Q_ASSERT( vd );

            Q_ASSERT( vd->type == IOPin::StringType );
            v = vd->sv;

        } else {
            return NULL;
        }

        QLineEdit* le = new TableLineEdit( tbl, v );
        return (QWidget*)le;

    }

    return NULL;
}

void ProjectPreviewDialog::setCellText( QTable* t, int r, int c,
        const QString& v, const QString& ud/* = ""*/,
        bool editable/* = false*/, bool enabled /*=false*/  ){
    Q_ASSERT( t );
    Q_ASSERT( d );
    Q_ASSERT( c >= COL_FIRST && c<= COL_STRING );

    t->setItem( r, c,
            new ProjectPreviewTableItem(
                    d->darkBackground,
                    t, (editable)?QTableItem::WhenCurrent:
                    QTableItem::Never, v, ud ) );
    t->item( r, c )->setEnabled( enabled );
}

void ProjectPreviewDialog::setCellWidget( QTable* t, int r, int c,
        QWidget* w )
{
    Q_ASSERT( t );
    Q_ASSERT( d );
    Q_ASSERT( w );
    Q_ASSERT( c >= COL_BIT && c <= COL_STRING );

    // clear the cell before inserting new widget
    t->clearCellWidget( r, c );

    t->setCellWidget( r, c, w );
    if ( c == COL_BIT ){
        connect( w,
                SIGNAL( toggled( bool ) ),
                this,
                SLOT( iopinBitValueChanged( bool ) ) );
    } else if ( c == COL_NUMBER ){
        connect( w,
                SIGNAL( valueChanged( int ) ),
                this,
                SLOT( iopinNumberValueChanged( int ) ) );
    } else if ( c == COL_STRING ){
        connect( w,
                SIGNAL( textChanged( const QString& ) ),
                this,
                SLOT( iopinStringValueChanged( const QString& ) ) );

        connect( w,
                SIGNAL( returnPressed() ),
                this,
                SLOT( iopinStringValueChanged( ) ) );

    }
}

void ProjectPreviewDialog::iopinBitValueChanged( bool value )
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );
    Q_ASSERT( sender() );

    TableCellButton* cb = (TableCellButton*)sender();

    ProjectPreviewTable* t =
            (ProjectPreviewTable*)cb->table();
    Q_ASSERT( t );

    bool ok;
    int cell = cb->caption().toInt( &ok, 10 );
    Q_ASSERT( ok );
    int r = cell/t->numCols();

    QString data1 = t->getUserData( r, COL_FIRST );
    QString data2 = t->getUserData( r, COL_SECOND );

    if ( t->mode() == PREVIEW_VARIABLES ){

        Q_ASSERT( d->cvm.contains( data1 ) );
        VariableMap& vm = d->cvm[ data1 ];

        Q_ASSERT( vm.contains( data2 ) );
        VariableData* vd = vm[ data2 ];
        Q_ASSERT( vd );

        Q_ASSERT( vd->type == IOPin::BitType );
        vd->bv = value;
        vm[ data2 ] = vd;

    }

    IOPinList iopl = affectedIOPins( t->mode(), data1, data2);

    TemplateList tl = affectedTemplates( t->mode(), data1, data2);

    for(IOPin* iop = iopl.first() ; iop; iop = iopl.next() ) {
        iop->setRealBitValue( value );
    }

    for(Template* tmpl = tl.first() ; tmpl; tmpl = tl.next() ) {
        tmpl->iopinsChanged();
    }

    cb->setOn( value );

    // nottify other windows about iopin change
    ((MainWindow*)
     parent())->projectChangedSlot( d->pw, LTE_CMD_IOPINS_CHANGE );

    // distribute only when t->mode() == PREVIEW_VARIABLES because variable
    // change changes iopins as well but not the other way around
    if(t->mode() == PREVIEW_VARIABLES) {
        distributeVariableChange(data1, data2);
    }

    cb->setFocus();
}

void ProjectPreviewDialog::iopinNumberValueChanged( int value )
{
    Q_ASSERT( d );
    Q_ASSERT( sender() );

    TableCellSpinBox* csb = (TableCellSpinBox*)sender();

    ProjectPreviewTable* t =
            (ProjectPreviewTable*)csb->table();
    Q_ASSERT( t );

    bool ok;
    int cell = csb->caption().toInt( &ok, 10 );
    Q_ASSERT( ok );
    int r = cell/t->numCols();

    QString data1 = t->getUserData( r, COL_FIRST );
    QString data2 = t->getUserData( r, COL_SECOND );

    if ( t->mode() == PREVIEW_VARIABLES ){

        Q_ASSERT( d->cvm.contains( data1 ) );
        VariableMap& vm = d->cvm[ data1 ];

        Q_ASSERT( vm.contains( data2 ) );
        VariableData* vd = vm[ data2 ];
        Q_ASSERT( vd );

        Q_ASSERT( vd->type == IOPin::NumberType );
        vd->iv = value;
        vm[ data2 ] = vd;

    }


    IOPinList iopl = affectedIOPins( t->mode(), data1, data2 );

    TemplateList tl = affectedTemplates( t->mode(), data1, data2 );

    for(IOPin* iop = iopl.first() ; iop; iop = iopl.next() ) {
        iop->setRealNumberValue( value );
    }

    for(Template* tmpl = tl.first() ; tmpl; tmpl = tl.next() ) {
        tmpl->iopinsChanged();
    }

    csb->setValue( value );

    // nottify other windows about iopin change
    ((MainWindow*)
     parent())->projectChangedSlot( d->pw, LTE_CMD_IOPINS_CHANGE );

    // distribute only when t->mode() == PREVIEW_VARIABLES because variable
    // change changes iopins as well but not the other way around
    if(t->mode() == PREVIEW_VARIABLES) {
        distributeVariableChange(data1, data2);
    }

    csb->setFocus();
}

void ProjectPreviewDialog::iopinStringValueChanged()
{
    Q_ASSERT( sender() );

    TableLineEdit* cle = (TableLineEdit*)sender();

    iopinStringValueChanged( cle->text() );
}

void ProjectPreviewDialog::iopinStringValueChanged(
        const QString& value )
{
    Q_ASSERT( d );
    Q_ASSERT( sender() );

    TableLineEdit* cle = (TableLineEdit*)sender();

    ProjectPreviewTable* t =
            (ProjectPreviewTable*)cle->table();
    Q_ASSERT( t );

    bool ok;
    int cell = cle->caption().toInt( &ok, 10 );
    Q_ASSERT( ok );
    int r = cell/t->numCols();

    QString data1 = t->getUserData( r, COL_FIRST );
    QString data2 = t->getUserData( r, COL_SECOND );

    if ( t->mode() == PREVIEW_VARIABLES ){

        Q_ASSERT( d->cvm.contains( data1 ) );
        VariableMap& vm = d->cvm[ data1 ];

        Q_ASSERT( vm.contains( data2 ) );
        VariableData* vd = vm[ data2 ];
        Q_ASSERT( vd );

        Q_ASSERT( vd->type == IOPin::StringType );
        vd->sv = value;
        vm[ data2 ] = vd;

    }

    IOPinList iopl = affectedIOPins( t->mode(), data1, data2);

    TemplateList tl = affectedTemplates( t->mode(), data1, data2);

    for(IOPin* iop = iopl.first() ; iop; iop = iopl.next() ) {
        iop->setRealStringValue( value );
    }

    for(Template* tmpl = tl.first() ; tmpl; tmpl = tl.next() ) {
        tmpl->iopinsChanged();
    }

    cle->setText( value );

    // nottify other windows about iopin change
    ((MainWindow*)
     parent())->projectChangedSlot( d->pw, LTE_CMD_IOPINS_CHANGE );

    // distribute only when t->mode() == PREVIEW_VARIABLES because variable
    // change changes iopins as well but not the other way around
    if(t->mode() == PREVIEW_VARIABLES) {
        distributeVariableChange(data1, data2);
    }

    cle->setFocus();
    cle->setCursorPosition( cle->text().length() );
}


void ProjectPreviewDialog::panelChanged( const QString& pn )
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );
    onPanelChanged( pn );
}

void ProjectPreviewDialog::populatePreviewTable( const QString& vn,
        const QString& pn, int pt )
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );

    Q_ASSERT( vn.isEmpty() == false );
    Q_ASSERT( pn.isEmpty() == false );

    const ViewMap& vm = d->pw->project()->views();
    Q_ASSERT( vm.contains( vn ) );

    //1. Populate the Template IOPin tab

    int prevNumRows = 0, currNumRows = 0;
    int r, c;

    if ( pt & PREVIEW_IOPINS ){

        prevNumRows = m_IOPinValues->numRows();

        //clear cell widgets$
        for ( r = 0; r < prevNumRows; r++ ){
            for ( c = COL_BIT; c <= COL_STRING; c++ ){
                if ( m_IOPinValues->cellWidget( r, c ) ){
                    m_IOPinValues->clearCellWidget( r, c );
                }
            }
        }

        const View& v = vm[ vn ];
        const PanelMap& pm = v.panels();
        r = 0;
        const Panel& p = pm[ pn ];
        const TemplateDict& td = p.templates();
        bool rowSet;

        for( unsigned it = 0; it < td.count(); it++ ) {
            const Template* t = td[ it ];
            const IOPinDict& iopind = t->iopins();
            rowSet = false;

            for( unsigned ipin = 0; ipin < iopind.count(); ipin++ ) {
                const IOPin* iopin = iopind[ ipin ];
                setRowValue( m_IOPinValues, r++, td.keyAt( it ),
                        iopin->label(), td.keyAt( it ),
                        iopind.keyAt( ipin ) );
                rowSet = true;
            }

            //shade changes template wise
            if ( rowSet ){
                d->darkBackground = !d->darkBackground;
                rowSet = false;
            }
        }
        currNumRows = r;

        if ( prevNumRows > currNumRows ){
            QMemArray<int> rowsToGo( prevNumRows-currNumRows );

            int i=0;
            for ( int r = currNumRows; r < prevNumRows; r++,++i ){
                rowsToGo[i] = r;
            }
            m_IOPinValues->removeRows( rowsToGo );
        }

        if ( currNumRows > 0 )
            m_IOPinValues->setColumnWidths();
        m_IOPinValues->adjustToLastSortingOrder();
    }

    if ( pt & PREVIEW_VARIABLES ){

        //2. Populate the Variable tab
        d->darkBackground = false;
        prevNumRows = m_ConnectionVariableValues->numRows();

        //clear cell widgets
        for ( r = 0; r < prevNumRows; r++ ){
            for ( c = COL_BIT; c <= COL_STRING; c++ ){
                if ( m_ConnectionVariableValues->cellWidget( r, c ) ){
                    m_ConnectionVariableValues->clearCellWidget( r, c );
                }
            }
        }
        const BindingsList& bl = d->pw->project()->
            bindings().bindingsByPanel( vn, pn );
#ifdef _CONSISTENCY_CHECKS_
        d->pw->project()->bindings().consistencyCheck();
#endif
#ifdef _COMPILE_IN_DUMP_
        Bindings::dump(bl);
#endif
        BindingsList::const_iterator ibl;
        QMap<QString,QStringList> data;

        for( ibl = bl.begin(); ibl != bl.end(); ++ibl ) {
            const BindingPtr bp = *ibl;
            Q_ASSERT( !bp.isNull() );
            if(!bp->valid()) {
                continue;
            }
            Q_ASSERT( bp->valid() );

            QString key = QString("%1:%2").arg( bp->connection() )
                .arg( bp->variable() );

            if ( data.contains( key ) ) {
                continue;
            } else {
                QStringList sl;
                sl += bp->connection();
                sl += bp->variable();
                data.insert( key, sl );
            }
        }
        r = 0;
        QMap<QString,QStringList>::const_iterator di;

        for ( di = data.begin(); di != data.end(); ++di ){
            const QStringList& sl = di.data();
            Q_ASSERT( sl.count() == 2 );
            setRowValue( m_ConnectionVariableValues, r++,
                    sl[ 0 ], sl[ 1 ], sl[ 0 ], sl[ 1 ] );
            d->darkBackground = !d->darkBackground;//variable
            //wise shade change
        }

        currNumRows = r;

        if ( prevNumRows > currNumRows ){
            QMemArray<int> rowsToGo( prevNumRows-currNumRows );

            int i=0;
            for ( int r = currNumRows; r < prevNumRows; r++,++i ){
                rowsToGo[i] = r;
            }
            m_ConnectionVariableValues->removeRows( rowsToGo );
        }

        if ( currNumRows > 0 )
            m_ConnectionVariableValues->setColumnWidths();
        m_ConnectionVariableValues->adjustToLastSortingOrder();

    }
}

void ProjectPreviewDialog::onPanelChanged( const QString& pn )
{
    Q_ASSERT( pn.isEmpty() == false );

    QString vn = m_comboPanels->caption();
    Q_ASSERT( vn.isEmpty() == false );

    populatePreviewTable( vn, pn, PREVIEW_IOPINS|PREVIEW_VARIABLES );

}


int ProjectPreviewDialog::indexInPanelCombobox( const QString& pn,
        ComparisonFlags c /*= Qt::ExactMatch*/ ){
    QListBox* lb = m_comboPanels->listBox();
    Q_ASSERT( lb );
    QListBoxItem* lbi = lb->findItem( pn,
            c );
    Q_ASSERT( lbi );
    return lb->index( lbi );
}

void ProjectPreviewDialog::makeDialogEmpty()
{
    m_comboPanels->clear();

    m_IOPinValues->setNumRows(0);
    m_ConnectionVariableValues->setNumRows(0);
}

void ProjectPreviewDialog::fillConnectionVariableMap( bool update )
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );
    Q_ASSERT( d->pw->project() );

    if ( !update ) {
        d->clean();
    }

    const ConnectionDict& cd = d->pw->project()->connections();
    for ( unsigned ic = 0; ic < cd.count(); ic++ ){

        const Connection* c = cd[ ic ];
        Q_ASSERT( c );
        const VariableDict& vd = c->variables();

        for ( unsigned iv = 0; iv < vd.count(); iv++ ){

            const Variable* v = vd[ iv ];
            Q_ASSERT( v );

            VariableData* varData = NULL;

            if ( update ){
                if ( ( d->cvm.contains( cd.keyAt( ic ) ) ) &&
                    ( d->cvm[ cd.keyAt( ic ) ].contains(
                                                vd.keyAt( iv ) ) ) ){
                    varData = d->cvm[ cd.keyAt( ic ) ][ vd.keyAt( iv ) ];
                }
            }
            if(!varData) {
                varData = new VariableData;
                Q_CHECK_PTR(varData);
            }
            varData->type = v->type();

            d->cvm[ cd.keyAt( ic ) ][ vd.keyAt( iv ) ] = varData;
        }
    }
}

void ProjectPreviewDialog::updateConnectionVariableMap(
    CommandType t)
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );

    if ( t == LTE_CMD_CONNECTION_RENAME &&
        d->pw->lastCommand()->rtti() == LTE_CMD_CONNECTION_RENAME ){

        RenameConnectionCommand* cmd =
            (RenameConnectionCommand*)d->pw->lastCommand();
        Q_ASSERT( cmd );

        if ( d->cvm.contains( cmd->oldName() ) ){

            VariableMap vm = d->cvm[ cmd->oldName() ];

            d->cvm.remove( cmd->oldName() );
            Q_ASSERT( !d->cvm.contains( cmd->oldName() ) );

            d->cvm[ cmd->newName() ] = vm;
        }

    } else if ( t == LTE_CMD_VARIABLE_RENAME &&
        d->pw->lastCommand()->rtti() == LTE_CMD_VARIABLE_RENAME ){

        RenameVariableCommand* cmd =
                (RenameVariableCommand*)d->pw->lastCommand();
        Q_ASSERT( cmd );

        Q_ASSERT( d->cvm.contains( cmd->connection() ) );
        VariableMap& vm = d->cvm[ cmd->connection() ];

        Q_ASSERT( vm.contains( cmd->oldName() ) );
        VariableData* vd = vm[ cmd->oldName() ];
        Q_ASSERT( vd );

        vm.remove( cmd->oldName() );
        Q_ASSERT( !vm.contains( cmd->oldName() ) );

        vm[ cmd->newName() ] = vd;

    } else if ( t == LTE_CMD_CONNECTION_DEL &&
        d->pw->lastCommand()->rtti() == LTE_CMD_CONNECTION_ADD_DEL ){

        AddDelConnectionCommand* cmd =
            (AddDelConnectionCommand*)d->pw->lastCommand();
        Q_ASSERT( cmd );

        if ( d->cvm.contains( cmd->name() ) ){

            VariableMap& vm = d->cvm[ cmd->name() ];

            if ( !vm.empty() ){
                VariableMap::iterator ivm;
                for ( ivm = vm.begin(); ivm != vm.end(); ++ivm ){
                    delete ivm.data();
                }
                vm.clear();
            }

            d->cvm.remove( cmd->name() );
            Q_ASSERT( !d->cvm.contains( cmd->name() ) );
        }

    } else if ( t == LTE_CMD_VARIABLE_ADD &&
        d->pw->lastCommand()->rtti() == LTE_CMD_VARIABLE_ADD_DEL ){

        AddDelVariableCommand* cmd =
            (AddDelVariableCommand*)d->pw->lastCommand();
        Q_ASSERT( cmd );

        VariableMap& vm = d->cvm[ cmd->connection() ];

        Q_ASSERT( cmd->variableType() != IOPin::InvalidType );
        VariableData* vd = new VariableData;
        vd->type = cmd->variableType();

        vm[ cmd->name() ] = vd;
        Q_ASSERT( vm.contains( cmd->name() ) );

    } else if ( t == LTE_CMD_VARIABLE_DEL &&
        d->pw->lastCommand()->rtti() == LTE_CMD_VARIABLE_ADD_DEL ){

        AddDelVariableCommand* cmd =
            (AddDelVariableCommand*)d->pw->lastCommand();
        Q_ASSERT( cmd );

        Q_ASSERT( d->cvm.contains( cmd->connection() ) );
        VariableMap& vm = d->cvm[ cmd->connection() ];

        Q_ASSERT( vm.contains( cmd->name() ) );
        VariableData* vd  = vm[ cmd->name() ];

        Q_ASSERT( vd );
        delete vd;

        vm.remove( cmd->name() );
        Q_ASSERT( !vm.contains( cmd->name() ) );

    }
}

void ProjectPreviewDialog::distributeVariableChange(
        const QString& c, const QString& v)
{
    VariableData* vd = d->cvm[c][v];
    Q_ASSERT(vd);

    BindingsList bl = d->pw->project()->
        bindings().bindingsByVariable( c, v );

    QString t, io;

    for(int i=0; i<m_IOPinValues->numRows(); ++i) {

        t = m_IOPinValues->getUserData(i, COL_FIRST);
        io = m_IOPinValues->getUserData(i, COL_SECOND);

        for(BindingsList::iterator j=bl.begin();
                j != bl.end(); ++j)
        {
            if((*j)->templ() == t && (*j)->iopin() == io) {
                switch(vd->type) {
                    case IOPin::BitType:
                        {
                        TableCellButton* w =
                            (TableCellButton*)m_IOPinValues->cellWidget(i,
                                    COL_BIT);
                        Q_ASSERT(w);
                        w->setOn(vd->bv);
                        break;
                        }
                    case IOPin::NumberType:
                        {
                        TableCellSpinBox* w =
                            (TableCellSpinBox*)m_IOPinValues->cellWidget(i,
                                    COL_NUMBER);
                        Q_ASSERT(w);
                        w->setValue(vd->iv);
                        break;
                        }
                    case IOPin::StringType:
                        {
                        TableLineEdit* w =
                            (TableLineEdit*)m_IOPinValues->cellWidget(i,
                                    COL_STRING);
                        Q_ASSERT(w);
                        w->setText(vd->sv);
                        break;
                        }
                    default:
                        Q_ASSERT(false);
                }
                bl.remove(j);
                break;
            }
        }
        if(bl.isEmpty()) {
            break;
        }
    }
}

void ProjectPreviewDialog::polish()
{
    m_IOPinValues->setColumnWidths();
    m_ConnectionVariableValues->setColumnWidths();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPreviewDialog.cpp 1570 2006-04-11 12:15:08Z modesto $
