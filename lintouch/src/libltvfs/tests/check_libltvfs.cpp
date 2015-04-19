// $Id: check_libwtngvfs.cpp,v 1.17 2004/09/20 08:49:03 jbar Exp $
//
//   FILE: check_libltvfs.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 15 January 2003
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


// custom tests
#include <cppunit/extensions/HelperMacros.h>
#include <qstring.h>
#include <qdir.h>
#include <qurl.h>

#include <lt/logger/ConsoleLogger.hh>
#include <lt/vfs/VFS.hh>
#include <lt/vfs/VFSLocal.hh>
#include <lt/vfs/VFSZIP.hh>
using namespace lt;

QString srcdir;
QString builddir;

class TestVFS : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestVFS );

    CPPUNIT_TEST( sanityCheck );
    CPPUNIT_TEST( localZIPCheck );
    CPPUNIT_TEST( localZIPWriteCheck );
    CPPUNIT_TEST( localMakeDirectoryCheck );
    CPPUNIT_TEST( localRemoveDirectoryCheck );
    CPPUNIT_TEST( localListDirectoryCheck );
    CPPUNIT_TEST( localSecurityCheck );
    CPPUNIT_TEST( localSubVFSCheck );
    CPPUNIT_TEST( localFileOpenCheck );
    CPPUNIT_TEST( localFileSynthetizedCheck );
    CPPUNIT_TEST( zipReadCheck );
    CPPUNIT_TEST( zipListCheck );
    CPPUNIT_TEST( zipDirectoryMakeCheck );
    CPPUNIT_TEST( zipDirectoryRemoveCheck );
    CPPUNIT_TEST( zipFileWriteCheck );
    CPPUNIT_TEST( existsCheck );
    CPPUNIT_TEST_SUITE_END();

    QString filename1, filename2, path_ro, path_rw, dir1, dir2,
            zip_existing, zip_nonexisting;
    VFSPtr vfs_rw1, vfs_ro1;
    QDir p_ro, p_rw;
    LoggerPtr logger;

    public:

    void setUp()
    {
        logger = LoggerPtr(new ConsoleLogger);

        filename1 = "check_libltvfs.cpp";
        filename2 = "ashlander";
        path_ro = QDir( srcdir + "/tests" ).absPath();
        path_rw = QDir( "build" ).absPath();
        dir1 = "Ald-ruhn";
        dir2 = "Seyda Neen";
        zip_existing = "Gnisis.zip";
        zip_nonexisting = "Sadrith Mora.ZIP";
        vfs_rw1 = VFSPtr(new VFSLocal(path_rw, false, logger));
        vfs_ro1 = VFSPtr(new VFSLocal(path_ro, true, logger));

        p_ro = QDir(path_ro);
        p_rw = QDir(path_rw);
        p_rw.rmdir(dir1);
        p_rw.rmdir(dir2);
    }

    void tearDown()
    {
        p_rw.rmdir(dir1);
        p_rw.rmdir(dir2);
    }

    void sanityCheck()
    {
        VFSPtr nonexistent(new VFSLocal(QString(path_ro) + "/" + dir1));
        CPPUNIT_ASSERT(!nonexistent->isValid());

        CPPUNIT_ASSERT(!VFSPtr(new VFSZIP(QString("http://google.com"),
                        true, logger))->isValid());

        CPPUNIT_ASSERT(vfs_rw1->isValid());
        CPPUNIT_ASSERT(!vfs_rw1->isReadOnly());
        CPPUNIT_ASSERT(vfs_ro1->isValid());
        CPPUNIT_ASSERT(vfs_ro1->isReadOnly());

        QUrl rootLocation = vfs_rw1->rootLocation();
        CPPUNIT_ASSERT(rootLocation.protocol() == "file");

        VFSPtr vzip1(new VFSZIP(path_ro + "/" + zip_existing, true, logger));
        CPPUNIT_ASSERT(vzip1->isValid());

        VFSPtr vzip2(new VFSZIP(path_ro, true, logger));
        CPPUNIT_ASSERT(!vzip2->isValid());

        VFSPtr vzipnonexistent(new VFSZIP(path_ro + "/" + zip_existing +
                    "/" + filename2));
        CPPUNIT_ASSERT(!vzipnonexistent->isValid());
    }

    void localZIPCheck()
    {
        p_rw.mkdir(zip_existing);
        p_rw.cd(zip_existing);

        VFSPtr vfs_rw1(new VFSLocal(p_rw.absPath(), true, logger));
        CPPUNIT_ASSERT(vfs_rw1->isValid());

        QString filename3 = p_rw.absPath() + "/" + zip_nonexisting;
        QFile f1(filename3);
        f1.open(IO_WriteOnly);
        f1.putch(0);
        f1.close();

        vfs_rw1 = VFSPtr(new VFSZIP(filename3, true, logger));
        CPPUNIT_ASSERT(vfs_rw1->isValid());

        p_rw.remove(zip_nonexisting);
        vfs_rw1 = VFSPtr(new VFSZIP(filename3, true, logger));
        CPPUNIT_ASSERT(!vfs_rw1->isValid());

        p_rw.cdUp();
        p_rw.rmdir(zip_existing);
    }

    void localZIPWriteCheck()
    {
        VFSPtr vlocal2(new VFSZIP(QString("build/ok.zip"), false,
                    logger));
        CPPUNIT_ASSERT(vlocal2->isValid());

        VFSPtr vlocal3(new VFSLocal(QString(srcdir+"/tests"), true, logger));
        QStringList resources = vlocal3->ls(
                "resources", VFSFilter::Files);
        for (QStringList::Iterator it1 = resources.begin();
                it1 != resources.end(); ++it1 )
        {
            QIODevice *dev = vlocal3->openFile("resources/" + *it1);
            CPPUNIT_ASSERT(dev->isOpen());
            QByteArray data(dev->readAll());
            delete dev;

            QIODevice *dest = vlocal2->openFile(*it1, IO_WriteOnly);
            CPPUNIT_ASSERT(dest->isOpen());

            dest->writeBlock(data);
            delete dest;
        }

        //check integrity of the ZIP file
        vlocal3.reset();
        vlocal2.reset();
        VFSPtr vlocal4(new VFSZIP(QString("build/ok.zip"), true,
                    logger));
        CPPUNIT_ASSERT(vlocal4->isValid());
        QIODevice *dev = vlocal4->openFile("car.png");
        CPPUNIT_ASSERT(dev->isOpen());
        CPPUNIT_ASSERT(dev->readAll());
        delete dev;

        QFile("build/ok.zip").remove();
    }

    void localMakeDirectoryCheck()
    {
        CPPUNIT_ASSERT(vfs_rw1->mkdir(dir2));
        CPPUNIT_ASSERT(p_rw.exists(dir2));

        CPPUNIT_ASSERT(vfs_rw1->mkdir(dir1));
        CPPUNIT_ASSERT(p_rw.exists(dir1));
    }

    void localRemoveDirectoryCheck()
    {
        p_rw.mkdir(dir1);
        p_rw.mkdir(dir2);

        CPPUNIT_ASSERT(vfs_rw1->rmdir(dir1));
        CPPUNIT_ASSERT(!p_rw.exists(dir1));

        CPPUNIT_ASSERT(vfs_rw1->rmdir(dir2));
        CPPUNIT_ASSERT(!p_rw.exists(dir2));
    }

    void localSecurityCheck()
    {
        CPPUNIT_ASSERT(vfs_rw1->mkdir("/" + dir1));
        CPPUNIT_ASSERT(p_rw.exists(dir1));
        p_rw.rmdir(dir1);

        CPPUNIT_ASSERT(!vfs_rw1->mkdir("../" + dir1));
        CPPUNIT_ASSERT(!p_rw.exists(dir1));
        p_rw.rmdir(dir1);
    }

    void localListDirectoryCheck()
    {
        p_rw.mkdir(dir1);

        p_rw.cd(dir1);
        QString filename3 = p_rw.absPath() + "/" + filename2;
        QFile f1(filename3);
        f1.open(IO_WriteOnly);
        f1.putch(0);
        f1.close();

        p_rw.mkdir(dir2);

        QStringList listing;
        listing = vfs_rw1->ls(dir1, VFSFilter::Dirs);
        logger->debug("vfs_rw1: " + vfs_rw1->rootLocation());
        CPPUNIT_ASSERT(listing.count() == 1);
        CPPUNIT_ASSERT(listing[0] == dir2);

        listing = vfs_rw1->ls(dir1, VFSFilter::Files);
        CPPUNIT_ASSERT(listing.count() == 1);
        CPPUNIT_ASSERT(listing[0] == filename2);

        listing = vfs_rw1->ls(dir1, VFSFilter::All);
        CPPUNIT_ASSERT(listing.count() == 2);
        CPPUNIT_ASSERT(listing[0] == dir2 || listing[0] == filename2);
        CPPUNIT_ASSERT(listing[1] == dir2 || listing[1] == filename2);

        listing = vfs_rw1->ls();
        CPPUNIT_ASSERT(listing.findIndex(dir1) != -1);

        p_rw.remove(filename2);
        p_rw.rmdir(dir2);
        p_rw.cdUp();
        p_rw.rmdir(dir1);
    }

    void localSubVFSCheck()
    {
        QDir p3(p_rw);
        p_rw.mkdir(dir1);
        p3.cd(dir1);
        p3.rmdir(dir2);

        VFSPtr sub(vfs_rw1->subVFS(dir1));
        CPPUNIT_ASSERT(sub->isValid());

        CPPUNIT_ASSERT(!sub->subVFS(dir2)->isValid());

        CPPUNIT_ASSERT(sub->mkdir(dir2));
        CPPUNIT_ASSERT(p3.exists(dir2));

        CPPUNIT_ASSERT(sub->rmdir(dir2));
        CPPUNIT_ASSERT(!p3.exists(dir2));

        p_rw.rmdir(dir1);
    }

    void localFileOpenCheck()
    {
        QIODevice *flocal1 = vfs_ro1->openFile(filename1, 0);
        CPPUNIT_ASSERT(!flocal1->isOpen());
        delete flocal1;

        flocal1 = vfs_ro1->openFile(filename1, IO_ReadOnly);
        CPPUNIT_ASSERT(flocal1->isOpen());
        delete flocal1;

        flocal1 = vfs_ro1->openFile("/" + filename1, IO_ReadOnly);
        CPPUNIT_ASSERT(flocal1->isOpen());
        delete flocal1;

        flocal1 = vfs_ro1->openFile("../" + filename1, IO_ReadOnly);
        CPPUNIT_ASSERT(!flocal1->isOpen());
        delete flocal1;

        flocal1 = vfs_rw1->openFile(filename2, IO_WriteOnly);
        CPPUNIT_ASSERT(flocal1->isOpen());
        p_rw.remove(filename2);
        delete flocal1;
    }

    void localFileSynthetizedCheck()
    {
        QIODevice *flocal1 = vfs_rw1->openFile(filename2,
                IO_WriteOnly);
        CPPUNIT_ASSERT(flocal1->isOpen());
        QString content =
            "Behold Ashlander!\nThou shall fail the Grand"
            "Test should any of the following turn out affirmative:\n"
            "1. thou dost not know where the land of Morrowind lies, or\n"
            "2. thou hast never been there.";
        CPPUNIT_ASSERT(flocal1->writeBlock(content.latin1(),
                    content.length()) == (Q_LONG) content.length());

        CPPUNIT_ASSERT(flocal1->at() == content.length());
        CPPUNIT_ASSERT(flocal1->isWritable());
        delete flocal1;

        //read back what has been written
        flocal1 = vfs_rw1->openFile(filename2, IO_ReadOnly);
        CPPUNIT_ASSERT(flocal1->isOpen());
        CPPUNIT_ASSERT(flocal1->size() == content.length());

        int firstChar = flocal1->getch();
        CPPUNIT_ASSERT((char) firstChar == content[0]);
        flocal1->ungetch(firstChar);

        char buf[10];
        Q_LONG bufRead = flocal1->readBlock(buf, 9);
        CPPUNIT_ASSERT(bufRead == 9);
        buf[bufRead] = 0; //end the string

        //compare the content
        CPPUNIT_ASSERT(content.startsWith(buf));
        CPPUNIT_ASSERT((int) flocal1->at() == bufRead);

        CPPUNIT_ASSERT(!flocal1->atEnd()); //sanity

        QByteArray rest = flocal1->readAll();
        CPPUNIT_ASSERT(content == QString(buf) + QString(rest));

        CPPUNIT_ASSERT(flocal1->atEnd());
        CPPUNIT_ASSERT(flocal1->getch() == -1);
        delete flocal1;

        CPPUNIT_ASSERT(vfs_rw1->unlink(filename2));
        CPPUNIT_ASSERT(!p_rw.exists(filename2));
        p_rw.remove(filename2);
    }

    void zipReadCheck()
    {
        VFSPtr vzip1(new VFSZIP(path_ro + "/" + zip_existing, true, logger));
        CPPUNIT_ASSERT(vzip1->isValid());

        QIODevice *fzip1 = vzip1->openFile(dir1 + "/" + filename2,
                IO_ReadOnly);
        CPPUNIT_ASSERT(fzip1->isOpen());

        QByteArray all = fzip1->readAll();
        CPPUNIT_ASSERT(!all.isEmpty());
        CPPUNIT_ASSERT(QString(all) ==
                "Ashlanders are the origin inhabitans of Morrowind.\n");
    }

    void zipListCheck()
    {
        VFSPtr vzip1(new VFSZIP(path_ro + "/" + zip_existing, true, logger));
        CPPUNIT_ASSERT(vzip1->isValid());

        QStringList listing;
        listing = vzip1->ls(dir1, VFSFilter::Dirs);
        CPPUNIT_ASSERT(listing.count() == 1);
        CPPUNIT_ASSERT(listing[0] == dir2);

        listing = vzip1->ls(dir1, VFSFilter::Files);
        CPPUNIT_ASSERT(listing.count() == 1);
        CPPUNIT_ASSERT(listing[0] == filename2);

        listing = vzip1->ls(dir1);
        CPPUNIT_ASSERT(listing.count() == 2);
        CPPUNIT_ASSERT(listing[0] == dir2 || listing[0] == filename2);
        CPPUNIT_ASSERT(listing[1] == dir2 || listing[1] == filename2);

        listing = vzip1->ls(".");
        CPPUNIT_ASSERT(listing.count() == 1);
        listing = vzip1->ls("");
        CPPUNIT_ASSERT(listing.count() == 1);
        listing = vzip1->ls("/");
        CPPUNIT_ASSERT(listing.count() == 1);
    }

    void zipDirectoryMakeCheck()
    {
        p_rw.remove(zip_nonexisting);
        VFSPtr vzip2(new VFSZIP(path_rw + "/" + zip_nonexisting,
                    false, logger));
        CPPUNIT_ASSERT(vzip2->isValid());
        CPPUNIT_ASSERT(vzip2->mkdir("Vivec"));
        CPPUNIT_ASSERT(!vzip2->mkdir("Vivec"));
        CPPUNIT_ASSERT(vzip2->mkdir("Vivec/Foreign Quarter"));
        CPPUNIT_ASSERT(!vzip2->mkdir("Vivec/House/Redoran"));

        VFSPtr vzip3(new VFSZIP(path_rw + "/" + zip_nonexisting,
                    false, logger));
        CPPUNIT_ASSERT(vzip3->isValid());
        CPPUNIT_ASSERT(!vzip3->mkdir("Vivec"));
        CPPUNIT_ASSERT(vzip3->mkdir("Ebonheart"));
        CPPUNIT_ASSERT(!vzip3->mkdir("Ebonheart"));

        VFSPtr vzip4(new VFSZIP(path_rw + "/" + zip_nonexisting, true, logger));
        CPPUNIT_ASSERT(vzip4->isValid());
        CPPUNIT_ASSERT(!vzip4->mkdir("Pelagiad"));

        p_rw.remove(zip_nonexisting);
    }

    void zipDirectoryRemoveCheck()
    {
        p_rw.remove(zip_nonexisting);
        VFSPtr vzip2(new VFSZIP(path_rw + "/" + zip_nonexisting,
                    false, logger));
        CPPUNIT_ASSERT(vzip2->isValid());
        CPPUNIT_ASSERT(!vzip2->rmdir("Vivec"));
        CPPUNIT_ASSERT(vzip2->mkdir("Vivec"));
        CPPUNIT_ASSERT(vzip2->rmdir("Vivec"));
        CPPUNIT_ASSERT(!vzip2->rmdir("Vivec"));

        CPPUNIT_ASSERT(vzip2->mkdir("Vivec"));
        CPPUNIT_ASSERT(vzip2->mkdir("Vivec/Ebonheart"));
        CPPUNIT_ASSERT(!vzip2->rmdir("Vivec"));
        CPPUNIT_ASSERT(vzip2->rmdir("Vivec/Ebonheart"));
        CPPUNIT_ASSERT(vzip2->rmdir("Vivec"));
        CPPUNIT_ASSERT(!vzip2->rmdir("Vivec"));

        p_rw.remove(zip_nonexisting);
    }

    void zipFileWriteCheck()
    {
        p_rw.remove(zip_nonexisting);
        QIODevice *fzip1;
        VFSPtr vzip2(new VFSZIP(path_rw + "/" + zip_nonexisting,
                    false, logger));
        CPPUNIT_ASSERT(vzip2->isValid());

        fzip1 = vzip2->openFile("Vivec/GuildRules.txt", IO_WriteOnly);
        CPPUNIT_ASSERT(!fzip1->isOpen());

        CPPUNIT_ASSERT(vzip2->mkdir("Vivec"));
        fzip1 = vzip2->openFile("Vivec/GuildRules.txt", IO_WriteOnly);
        CPPUNIT_ASSERT(fzip1->isOpen());

        QString data = "You nwah!";
        fzip1->writeBlock(data.latin1(), data.length());
        fzip1->close();
        delete fzip1;

        //read back what has been written
        fzip1 = vzip2->openFile("Vivec/GuildRules.txt");
        CPPUNIT_ASSERT(fzip1->isOpen());
        QByteArray all = fzip1->readAll();
        CPPUNIT_ASSERT((QString) all == "You nwah!");
        delete fzip1;

        CPPUNIT_ASSERT(vzip2->unlink("Vivec/GuildRules.txt"));
        CPPUNIT_ASSERT(!vzip2->unlink("Vivec/GuildRules.txt"));

        //create a file in the zip root
        fzip1 = vzip2->openFile("Rooted.txt", IO_WriteOnly);
        CPPUNIT_ASSERT(fzip1->isOpen());
        fzip1->writeBlock(data.latin1(), data.length());
        fzip1->close();
        delete fzip1;

        //read back what has been written
        fzip1 = vzip2->openFile("Rooted.txt");
        CPPUNIT_ASSERT(fzip1->isOpen());
        all = fzip1->readAll();
        CPPUNIT_ASSERT((QString) all == "You nwah!");
        delete fzip1;

        p_rw.remove(zip_nonexisting);
    }

    void existsCheck()
    {
        CPPUNIT_ASSERT(vfs_ro1->exists(zip_existing));
        CPPUNIT_ASSERT(vfs_ro1->exists(zip_existing, VFSFilter::Files));
        CPPUNIT_ASSERT(!vfs_ro1->exists(zip_existing, VFSFilter::Dirs));
        CPPUNIT_ASSERT(!vfs_ro1->exists(zip_nonexisting));

        VFSPtr vzip1(new VFSZIP(path_ro + "/" + zip_existing, true, logger));
        CPPUNIT_ASSERT(vzip1->exists(dir1));
        CPPUNIT_ASSERT(vzip1->exists(dir1, VFSFilter::Dirs));
        CPPUNIT_ASSERT(vzip1->exists(dir1 + "/", VFSFilter::Dirs));
        CPPUNIT_ASSERT(!vzip1->exists(dir1, VFSFilter::Files));
        CPPUNIT_ASSERT(!vzip1->exists(dir2));
        CPPUNIT_ASSERT(vzip1->exists("/"));
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestVFS );
// end of custom tests

// standard test suite
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main( int /* argc */, char ** /* argv */ )
{
    // figure out where our sources are and where we can write
    srcdir = getenv( "srcdir" );
    if( srcdir.isEmpty() ) srcdir = ".";
    builddir = getenv( "top_builddir" );
    if( builddir.isEmpty() ) builddir = ".";

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );

    bool wasThereError = runner.run( "", false );

    // shell expects 0 if successfull
    return !wasThereError;
}
// end of standard test suite

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_libwtngvfs.cpp,v 1.17 2004/09/20 08:49:03 jbar Exp $
