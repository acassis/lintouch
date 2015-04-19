// $Id: ProjectPreviewDialog.hh 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: ProjectPreviewDialog.hh --
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

#ifndef _LTEDITORPROJECTPREVIEWDIALOG_HH
#define _LTEDITORPROJECTPREVIEWDIALOG_HH

#include <lt/templates/IOPin.hh>
#include <lt/templates/Template.hh>

#include "CommandType.hh"
#include "ProjectPreviewDialogBase.h"

class ProjectWindow;
class MainWindow;
class QTableItem;
class QTable;

/**
 * Dialog is used to change the real value of iopins of templates of the
 * selected panel with in a currently selected view.
 */
class ProjectPreviewDialog : public ProjectPreviewDialogBase
{

    friend class MainWindow;

    Q_OBJECT

public:
    /**
     * Constructor.
     */
    ProjectPreviewDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );

    /**
     * Destructor.
     */
    virtual ~ProjectPreviewDialog();

public slots:

    /**
     * Notify when a project window is selected.
     */
    void projectChangedSlot( ProjectWindow* pw, CommandType t );

    /**
     * To polish the dialog just before it is shown.
     */
    virtual void polish();
protected:

    /**
     * Dialog event handling when dialog is resized.
     */
    virtual void resizeEvent( QResizeEvent* );

protected 
    slots:
    /**
     * Notification from the control, that a iopin bit value have changed.
     */
    void iopinBitValueChanged( bool b );

    /**
     * Notification from the control, that a iopin number value have changed.
     */
    void iopinNumberValueChanged( int v );

    /**
     * Notification from the control, that a iopin string value have changed.
     */
    void iopinStringValueChanged( const QString& v );

    /**
     * Same as above, created for convinience.
     */
    void iopinStringValueChanged();

    /**
     * Notification from the control, that a selected panel have changed.
     */
    void panelChanged( const QString& pn );

protected:

    /**
     * Implements the sequence of operations when a panel in changed.
     */
    void onPanelChanged( const QString& pn );

    /**
     * TODO
     */
    IOPin::TypeList getSupportedTypes( QTable* t, const QString& data1,
        const QString& data2);

    /**
     * TODO
     */
    Template* affectedTemplate( const QString& tn,
            const QString& pn = QString::null );

    /**
     * TODO
     */
    const IOPinList& affectedIOPins(
        int mode, const QString& data1, const QString& data2 );

    /**
     * TODO
     */
    const TemplateList& affectedTemplates(
        int mode, const QString& data1, const QString& data2 );

    /**
     * TODO
     */
    void fillConnectionVariableMap( bool );

    /**
     * TODO
     */
    void updateConnectionVariableMap( CommandType t );

    /**
     * TODO
     */
    void populatePreviewTable( const QString& vn, const QString& pn, int pt );
private:
    /**
     * Used to emptify contents of the controls.
     */
    void makeDialogEmpty();

    /**
     * Returns the index of the matching text from panel combobox
     */
    int indexInPanelCombobox( const QString& pn,
            ComparisonFlags c = Qt::ExactMatch );

    /**
     * Notification when a selected view have changed.
     */
    void populateFromView( const QString& vn );

    /**
     * Sets value to a perticular row.
     */
    void setRowValue( QTable* t, int r, const QString& columnOne,
            const QString& columnTwo, const QString& dataColumnOne,
            const QString& dataColumnTwo );

    /**
     * Sets a cell text as well make the cell enabled and editable.
     */
    void setCellText( QTable* t, int r, int c, const QString& v,
            const QString& d = "", bool editable = false,
            bool enabled = false );

    /**
     * Sets a widget to a cell.
     */
    void setCellWidget( QTable* t, int r, int c, QWidget* w );

    /**
     * Returns a suitable widget depending on iopin type.
     */
    QWidget* iopinWidget( QTable* tbl, const lt::IOPin::Type& type,
    const QString& data1, const QString& data2 );

    /**
     * Updates iopins acording the new value of a variable.
     */
    void distributeVariableChange(const QString& c, const QString& v);

    typedef struct ProjectPreviewDialogPrivate;
    ProjectPreviewDialogPrivate * d;
};

#endif /* _LTEDITORPROJECTPREVIEWDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPreviewDialog.hh 1774 2006-05-31 13:17:16Z hynek $
