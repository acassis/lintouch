// $Id: MRU.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: MRU.cpp -- 
// AUTHOR: abdul <henry@swac.cz>
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

#include "MRU.hh"

#include <qsettings.h>
#include <qstringlist.h>

struct MRU::MRUPrivate {

    MRUPrivate() {
    }
    ~MRUPrivate() {
    }
    QStringList MRUProjects;
    unsigned int nLimit;
    QString domainName;
    QString productName;
    QString topGroup;
    QString projectListing;
    QString subKey;
    bool isLoadingMRU;
};

MRU::MRU( unsigned int nEntries ) : 
    d( new MRUPrivate() ) {

    d->nLimit = nEntries;
    d->isLoadingMRU = TRUE;
}

MRU::~MRU() {
    delete d;
}

void MRU::setRecentFiles( unsigned int nFilesToShow )
{
    d->nLimit = nFilesToShow;
}

int MRU::recentFiles() const
{
    return d->nLimit;
}

void MRU::clearList()
{
    d->MRUProjects.clear();
}

void MRU::addRecentProject( const QString & filename )
{

    if( filename.isEmpty() ) {
        return;
    }

    QStringList::iterator it = d->MRUProjects.find( filename );

    if ( it != d->MRUProjects.end() ){
        d->MRUProjects.remove( it );
    }

    d->MRUProjects.push_front( filename );

    while( ( d->MRUProjects.size() > d->nLimit ) ){
            d->MRUProjects.pop_back();
    }

    if( !d->isLoadingMRU ) {
        saveRecentProjectList();
    }
}

void MRU::saveRecentProjectList()
{
    QSettings settings;
    settings.setPath( d->domainName, d->productName );
    settings.beginGroup( d->topGroup );

    QStringList keys = settings.entryList( d->projectListing );
    int iExistingEntry = keys.count();
    settings.beginGroup ( d->projectListing );

    int f = 1;
    QStringList lstMRU = recentProjects();

    for( QStringList::iterator i = lstMRU.begin();
            i != lstMRU.end(); ++i, f++ ){
        QString strKey = QString( d->subKey ) + QString::number( f );
        settings.writeEntry( strKey, *i );
    }

    int iExtraNumberOfKey = iExistingEntry - f + 1;

    if( iExtraNumberOfKey > 0 ){

        for(; f <= iExistingEntry; f++ ){
            settings.removeEntry( QString( d->subKey ) +
                    QString::number( f ) );
        }
    }

    settings.endGroup();

}

QStringList MRU::recentProjects() const
{
    return d->MRUProjects;
}

void MRU::setPath( QString domainName, QString productName )
{
    d->domainName = domainName;
    d->productName = productName;
}

void MRU::setTopGroup( QString topGroup )
{
    d->topGroup = topGroup;
}

void MRU::setEntryList( QString projectListing )
{
    d->projectListing = projectListing;
}

void MRU::setSubKey( QString subKey )
{
    d->subKey = subKey;
}

void MRU::setLoadingComplete()
{
    d->isLoadingMRU = FALSE;
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: MRU.cpp 1169 2005-12-12 15:22:15Z modesto $
