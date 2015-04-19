// $Id: ProjectPreviewTable.hh 1210 2006-01-06 10:17:35Z modesto $
//
//   FILE: ProjectPreviewTable.hh --
// AUTHOR: shivaji <shivaji@swac.cz>
//   DATE: 19 Aug 2004
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

#ifndef _LTEDITORPROJECTPREVIEWTABLE_HH
#define _LTEDITORPROJECTPREVIEWTABLE_HH

#include <qstring.h>
#include <qtable.h>

#include <lt/templates/IOPin.hh>
#include <lt/templates/Template.hh>

/**
 * Custom made table item for table in preview dialog(
 * ProjectPreviewDialog ).
 */
class ProjectPreviewTableItem : public QTableItem
{
public:
    /**
     * Constructor.
     */
    ProjectPreviewTableItem( bool shade, QTable * table, 
        QTableItem::EditType editType, const QString& content, 
        const QString& usrData );

    /**
     * Destructor.
     */
    virtual ~ProjectPreviewTableItem();

    /**
     * Paint the contents of the item.
     */
    void paint( QPainter *p, const QColorGroup &cg, const QRect &cr,
        bool selected );

    /**
     * Get user data.
     */
    inline QString getUserData();

    /**
     * Returns wheather background have shade or not.
     */
    bool isShade() const;

    /**
     * Swaps the shade property while sorting.
     */
    void swapShade( ProjectPreviewTableItem *i );

protected:

    /**
     * Sets the shade property.
     */
    void setShade( bool s );

private:
    bool m_shade;
    QString m_usrData;
};


/**
 * Custom made table for preview dialog( ProjectPreviewDialog ).
 */
class QKeyEvent;
class ProjectPreviewTable:public QTable
{
    Q_OBJECT

public:
    /**
     * Constructors
     */
    ProjectPreviewTable( QWidget * parent = 0,
            const char * name = 0 );
    ProjectPreviewTable( int numRows, int numCols,
            QWidget * parent = 0, const char * name = 0);

    /**
     * Destructors.
     */
    virtual ~ProjectPreviewTable();

    /**
     * Sets an iopin for row r.
     */
    void setIOPin( int r, const lt::IOPin* );

    /**
     * Gets the iopin of row r.
     */
    lt::IOPin* iopin( int r );

    /**
     * Sets the cell widget w.
     */
    virtual void setCellWidget( int r, int c, QWidget* w );

    /**
     * Adjusts to the last sorting order, called only when the contents is
     * refreshed.
     */
    void adjustToLastSortingOrder();

    /**
     * Sets the column width of the table columns.
     */
    void setColumnWidths();

    /**
     * Calculate minimum column width for a column,
     * minimum column width depends on the width of the column header label.
     */
    int minimumColumnWidth( int col ) const;

    /**
     * Calculate the allowed column width for a column.
     */
    int allowedColumnWidth( int col ) const;

    /**
     * Getter for the current column width for a column.
     */
    int knownColumnWidth( int col ) const;

    /**
     * Calculate the width (in pixels) of the text.
     */
    int pixelWidthOf( const QString& content ) const;

    /**
     * Used to set item item at row row and column col. Mainly used to set
     * contents width
     */
    virtual void setItem(int row, int col, QTableItem * item);

    /**
     * Initializer, set the column labels and default values.
     */
    void init( int mode );

    /**
     * Returns the preview mode the table is in.
     */
    int mode();

    /**
     * TODO
     **/
    QString getUserData( int row, int col );

protected:

    /**
     * Sorts column col for the whole row.
     */
    virtual void sortColumn( int col, bool, bool );

    /**
     *
     */
    void keyPressEvent( QKeyEvent* );

protected
    slots:

    /**
     * Swaps data in row1 and row2.
     */
    virtual void swapRows( int row1, int row2, bool swapHeader = false );

private:

    /**
     * Adjusts the cell widget's tab order after sorting.
     */
    void adjustCellWidgetsTabOrder();

    typedef struct ProjectPreviewTablePrivate;
    ProjectPreviewTablePrivate * d;
};
#endif /* _LTEDITORPROJECTPREVIEWTABLE_HH */
                                                                                                                             
// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPreviewTable.hh 1210 2006-01-06 10:17:35Z modesto $

