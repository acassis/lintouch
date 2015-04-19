// $Id: MRU.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: MRU.hh  -- 
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 31 July 2004
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

#ifndef _LTEDITORMRU_HH
#define _LTEDITORMRU_HH

#include <qstringlist.h>
/**
 * This class stores the path to the most recently opened files, in 
 * order of there opening ie the latest opened file always before the file
 * opened previously. It also saves the path to the disc in the same order.
 * The number of most recently opened files to be remembered can be
 * controlled by calling the function setRecentFiles() of this class.
 */
class MRU {

    public:
    /**
    * Constructs the class. The default number of most recently opened
    * files to be remember is set to 10.
    */
    MRU( unsigned int nEntries = 10 );

    /**
    * Destroys the class.
    **/
    ~MRU( );

    /**
    * This function allows setting of the number of most recently opened
    * files to be remembered by the class.
    */
    void setRecentFiles( unsigned int nFilesToShow );

    /**
     * This returns the number of most recently opened file remembered by
     * the class.
     */
    int recentFiles() const;

    /**
    * This function sets the path at which the MRU files will be
    * saved.\domainName usually should be the internet domain name and
    * productName usually the official name of the product.
    * \ex: domainName = swac.de, productName = lintouch-editor-1.2
    */
    void setPath( QString domainName, QString productName );

    /**
    * This sets the name of the file in an unix based platform in which the
    * MRU path are saved.
    * \ex: if topGroup = "/lintouch-editor-1.2" the name of file will be
    * lintouch-editor-1.2rc
    */
    void setTopGroup( QString topGroup );

    /**
    * This sets the heading under which the MRU files will be saved in the
    * rc files. \ex: projectListing = "/recently_opened"
    */
    void setEntryList( QString projectListing );

    /**
    * This sets the sub key of the MRU files in the rc files
    * \ex: subKey = "/file". The MRU files will be saved as :
    * \ file1="path to the latest MRU"
    * \ file2 = "path to the previous MRU" and so on.
    */
    void setSubKey( QString subKey );

    /**
    * This clears the list of MRU remembered by the class.
    */
    void clearList();

    /**
     * Appends the path of opened project to MRU list.
     * \The path is always at the top of the list. ie latest file at the
     * begining of the list.
     */
    void addRecentProject( const QString& filename );

    /**
     * MRU list of opened project(s)
     */
    QStringList recentProjects() const;

    /**
    * This function sets the flag which signals that the class completed
    * loading of the MRU files from the disc. After this function is called
    * any subsequent call of addRecentProject( ) will not only add the file
    * path to the buffer maintained by this class but also save the path to
    * the disc.
    */
    void setLoadingComplete();

    protected:

    /**
    *  Saves the MRU list 
    */
    void saveRecentProjectList();

    private:
    typedef struct MRUPrivate;
    MRUPrivate * d;

};
#endif /* _LTEDITORMRU_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: MRU.hh 1169 2005-12-12 15:22:15Z modesto $
