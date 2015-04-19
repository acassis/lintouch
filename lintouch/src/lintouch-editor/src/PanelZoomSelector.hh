// $Id:PanelZoomSelector.hh
//
//   FILE: PanelZoomSelector.hh --
// AUTHOR: shivaji <shivaji@swac.cz>
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

#ifndef _LTEDITORPANELZOOMSELECTOR_HH
#define _LTEDITORPANELZOOMSELECTOR_HH

#include <qcombobox.h>
#include <qstringlist.h>

/**
 * Allows change in the zoom coefficient for projects
 * from a list of valid coefficients.User can also type valid
 * coefficient and confirm it with the Enter key. After confirmation
 * the user entered coefficient can be obtained by making use of 
 * the signal activated(QString&). Focus out event of the selector
 * also add valid entered coefficient to the list.
*/
class PanelZoomSelector : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY( int zoomCoefficient READ zoomCoefficient
            WRITE setZoomCoefficient )
public:
    /**
     * Constructs the class PanelZoomSelector
     */
    PanelZoomSelector( QWidget * parent = 0, 
            const char * name = 0 );
    /**
     * Destroys the class
     */
    ~PanelZoomSelector();

    /**
    * Sets a list of zoom coefficients in the zoom selector.
    * However zoom coefficients 25%, 50%,100%, 200%, 400%
    * are always included in the selector
    **/
    void setZoomCoefficientList( const QStringList & zoomCoefficients );

    /**
    * Returns the number of zoom coefficients in the selector
    * The parameter zoomCoefficientsList contains the entire list
    **/
    int zoomCoefficientList( QStringList & zoomCoefficientsList);

    /**
    * Returns the currently selected Zoom coefficient.
    * Only the numerical part of the coefficient is
    * returned.
    */
    int zoomCoefficient() const;

    /**
     * Sets the coefficient as the current Zoom Coefficient besides
     * adding it to the list if not present in the selector. 
     */
    void setZoomCoefficient( int zoomCoefficient );

protected:
    /**
    * TODO
    **/
    void setZoomCoefficient( QString zoomCoefficient );

    /**
    * Returns the numerical part of the Zoom Coefficient
    */
    int zoomCoefficient( QString strZoomCoeff ) const;

    /**
    * Sorts the zoom coefficient numerically
    */
    void sortCoefficientList();

protected slots:
    /**
    * Handles the lostFocus signal from the zoom selector 
    */
    void focusOutSlot();

    /**
    *Handles the returnPressed siganl from the zoom selector 
    */
    void pressedReturnSlot();
 
};

#endif /* _LTEDITORPANELZOOMSELECTOR_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelZoomSelector.hh 1169 2005-12-12 15:22:15Z modesto $

