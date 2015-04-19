// $Id: check_libltzip.cpp,v 1.7 2004/09/21 14:45:55 hynek Exp $
//
//   FILE: check_libltzip.cpp --
// AUTHOR: Hynek Petrak <hynek@swac.cz>
//   DATE: 9 November 2003
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
#include <qfile.h>
#include <qtextstream.h>
#include <qcstring.h>
#include <qmap.h>
#include <stdlib.h>

#include <lt/zip/qunzip.hh>
#include <lt/zip/qzip.hh>
using namespace lt;

QString srcdir;
QString builddir;

class TestZip : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestZip );

    CPPUNIT_TEST( checkUnzip );
    CPPUNIT_TEST( checkZip );
    CPPUNIT_TEST( checkCreatedZip );
    CPPUNIT_TEST( checkAppendToZip );
    CPPUNIT_TEST( checkAppendedZip );
    CPPUNIT_TEST( finalTest );
    CPPUNIT_TEST_SUITE_END();

    public:

        void setUp()
        {
        }

        void tearDown()
        {
        }

        void checkUnzip()
        {
            QFile f(srcdir + "/tests/test.zip");
            f.open(IO_ReadOnly);
            QUnZip uz;
            CPPUNIT_ASSERT(uz.open(&f));
            CPPUNIT_ASSERT(uz.getFileCount() == 5);
            QString comment;
            CPPUNIT_ASSERT(uz.getGlobalComment(comment));
            CPPUNIT_ASSERT(comment == "Zip file comment.");
            CPPUNIT_ASSERT(uz.gotoFirstFile());
            QString name, word1, word2;
            do {
                CPPUNIT_ASSERT( uz.getCurrentFileInfo(name, NULL));
                if (name.endsWith("/") || name.endsWith("\\")) {
                    qWarning("a directory: "+ name);
                    continue;
                } else
                    qWarning("a file: "+ name);
                QBuffer b;
                bool ok;
                b.open(IO_WriteOnly);
                CPPUNIT_ASSERT(ok = uz.getCurrentFile(b));
                b.close();
                if (ok) {
                    b.open(IO_ReadOnly);
                    QTextStream ts(&b);
                    ts >> word1 >> word2;
                    CPPUNIT_ASSERT(word1 == "File");
                    CPPUNIT_ASSERT(name == word2);
                    b.close();
                }
            } while (uz.gotoNextFile());
            uz.close();
            f.close();
        }

        void checkZip()
        {
            QFile zf(builddir + "/a.zip");
            CPPUNIT_ASSERT(zf.open(IO_ReadWrite | IO_Truncate));
            QZip qz;
            CPPUNIT_ASSERT(qz.open(&zf));
            {
                QBuffer b;
                b.open(IO_WriteOnly);
                QTextStream s(&b);
                s << "File file1.txt";
                b.close();
                b.open(IO_ReadOnly);
                CPPUNIT_ASSERT(qz.storeFile("file1.txt", b));
                b.close();
            }
            {
                QBuffer b;
                b.open(IO_ReadOnly);
                CPPUNIT_ASSERT(qz.storeFile("folder1/", b));
                b.close();
            }
            {
                QBuffer b;
                b.open(IO_WriteOnly);
                QTextStream s(&b);
                s << "File folder1/file2.txt";
                b.close();
                b.open(IO_ReadOnly);
                CPPUNIT_ASSERT(qz.storeFile("folder1/file2.txt", b));
                b.close();
            }
            qz.setGlobalComment("Zip comment.");
            qz.close();
            zf.close();
        }

        void checkCreatedZip()
        {
            QFile f(builddir + "/a.zip");
            f.open(IO_ReadOnly);
            QUnZip uz;
            CPPUNIT_ASSERT(uz.open(&f));
            CPPUNIT_ASSERT(uz.getFileCount() == 3);
            QString comment;
            CPPUNIT_ASSERT(uz.getGlobalComment(comment));
            CPPUNIT_ASSERT(comment == "Zip comment.");
            CPPUNIT_ASSERT(uz.gotoFirstFile());
            QString name, word1, word2;
            do {
                CPPUNIT_ASSERT( uz.getCurrentFileInfo(name, NULL));
                if (name.endsWith("/") || name.endsWith("\\")) {
                    qWarning("a directory: "+ name);
                    continue;
                } else
                    qWarning("a file: "+ name);
                QBuffer b;
                bool ok;
                b.open(IO_WriteOnly);
                CPPUNIT_ASSERT(ok = uz.getCurrentFile(b));
                b.close();
                if (ok) {
                    b.open(IO_ReadOnly);
                    QTextStream ts(&b);
                    ts >> word1 >> word2;
                    CPPUNIT_ASSERT(word1 == "File");
                    CPPUNIT_ASSERT(name == word2);
                    b.close();
                }
            } while (uz.gotoNextFile());
            uz.close();
            f.close();
        }

        void checkAppendToZip()
        {
            QFile zf(builddir + "/a.zip");
            CPPUNIT_ASSERT(zf.open(IO_ReadWrite));
            QZip qz;
            CPPUNIT_ASSERT(qz.open(&zf, true));
            {
                QBuffer b;
                b.open(IO_WriteOnly);
                QTextStream s(&b);
                s << "File file3.txt";
                b.close();
                b.open(IO_ReadOnly);
                CPPUNIT_ASSERT(qz.storeFile("file3.txt", b));
                b.close();
            }
            {
                QBuffer b;
                b.open(IO_ReadOnly);
                CPPUNIT_ASSERT(qz.storeFile("folder2/", b));
                b.close();
            }
            qz.setGlobalComment("Zip comment.");
            qz.close();
            zf.close();
        }

        void checkAppendedZip()
        {
            QFile f(builddir + "/a.zip");
            f.open(IO_ReadOnly);
            QUnZip uz;
            CPPUNIT_ASSERT(uz.open(&f));
            CPPUNIT_ASSERT(uz.getFileCount() == 5);
            QString comment;
            CPPUNIT_ASSERT(uz.getGlobalComment(comment));
            CPPUNIT_ASSERT(comment == "Zip comment.");
            CPPUNIT_ASSERT(uz.gotoFirstFile());
            QString name, word1, word2;
            do {
                CPPUNIT_ASSERT( uz.getCurrentFileInfo(name, NULL));
                if (name.endsWith("/") || name.endsWith("\\")) {
                    qWarning("a directory: "+ name);
                    continue;
                } else
                    qWarning("a file: "+ name);
                QBuffer b;
                bool ok;
                b.open(IO_WriteOnly);
                CPPUNIT_ASSERT(ok = uz.getCurrentFile(b));
                b.close();
                if (ok) {
                    b.open(IO_ReadOnly);
                    QTextStream ts(&b);
                    ts >> word1 >> word2;
                    CPPUNIT_ASSERT(word1 == "File");
                    CPPUNIT_ASSERT(name == word2);
                    b.close();
                }
            } while (uz.gotoNextFile());
            uz.close();
            f.close();
            f.remove();
        }

        QByteArray getRandomizedFile(int length)
        {
            QByteArray ba;
            int i;
            ba.resize(length);
            for (i = 0; i < length; i++) {
                ba[i] = (unsigned char) rand();
            }
            return ba;
        }

        typedef QMap<int, QByteArray> FileMap;
        FileMap prepareFiles()
        {
            FileMap id;
            int i;
            int lengths[] = {0, 1, 2, 3, 127, 128, 511, 512, 513, 
                768, 1000, 1023, 1024, 1025, 2047, 2048, 2049, 2050,
                10238, 10239, 10240, 10241, 10242, 23777, 102400,
                23537, 23561, 23627, 23669, 23687, 20480};
            int cnt = sizeof(lengths)/sizeof(int);
            for (i = 0; i < cnt; i++) {
                id[lengths[i]] = getRandomizedFile(lengths[i]);    
            }
            return id;
        }

        void repack(QString zipFileName)
        {
            QFile zipFile(zipFileName);
            zipFile.open(IO_ReadOnly);
            CPPUNIT_ASSERT(zipFile.isOpen());

            QUnZip u;
            CPPUNIT_ASSERT(u.open(&zipFile));

            QBuffer newZip;
            newZip.open(IO_ReadWrite);

            QZip z;
            CPPUNIT_ASSERT(z.open(&newZip));

            int filescount = 0;
            if (u.gotoFirstFile())
            {
                do
                {
                    QString filename;
                    QDateTime dt;
                    CPPUNIT_ASSERT(u.getCurrentFileInfo(filename, &dt));

                    QBuffer raw;
                    raw.open(IO_WriteOnly);
                    int method;
                    unsigned long crc, uncompressed_size;
                    CPPUNIT_ASSERT(u.getCurrentFileRaw(raw, method, 
                                crc, uncompressed_size));
                    raw.close();
                    raw.open(IO_ReadOnly);
                    CPPUNIT_ASSERT(z.storeFileRaw(filename, 
                                raw, dt, method, crc, uncompressed_size));
                    filescount++;
                }
                while (u.gotoNextFile());
            }

            z.close();
            u.close();
            zipFile.close();
            CPPUNIT_ASSERT(zipFile.open(IO_WriteOnly));
            zipFile.writeBlock(newZip.buffer());
        }

        void checkFilesInZip(QString zipFileName, FileMap map, 
                int count)
        {
            QFile zipFile(zipFileName);
            zipFile.open(IO_ReadOnly);
            CPPUNIT_ASSERT(zipFile.isOpen());

            QUnZip u;
            CPPUNIT_ASSERT(u.open(&zipFile));

            int filescount = 0;
            if (u.gotoFirstFile())
            {
                do
                {
                    QString filename;
                    QDateTime dt;
                    CPPUNIT_ASSERT(u.getCurrentFileInfo(filename, &dt));
                    filename.remove("dir/");

                    int key = filename.toInt();
                    CPPUNIT_ASSERT(map.contains(key));

                    QBuffer raw;
                    raw.open(IO_WriteOnly);
                    //int method;
                    //unsigned long crc, uncompressed_size;
                    CPPUNIT_ASSERT(u.getCurrentFile(raw));
                    raw.close();
                    // compare
                    raw.open(IO_ReadOnly);
                    CPPUNIT_ASSERT(raw.buffer() == map[key]);
                    filescount++;
                }
                while (u.gotoNextFile());
            }

            CPPUNIT_ASSERT(filescount == count);

            u.close();
            zipFile.close();
        }

        void finalTest()
        {
            QString file_name = builddir + "/zip1.zip";

            FileMap map = prepareFiles();

            QFile zf(file_name);
            CPPUNIT_ASSERT(zf.open(IO_ReadWrite | IO_Truncate));
            QZip qz;
            CPPUNIT_ASSERT(qz.open(&zf));

            FileMap::Iterator it; 
            for ( it = map.begin(); it != map.end(); ++it ) {
                QBuffer b(it.data());
                b.open(IO_ReadOnly);
                CPPUNIT_ASSERT(qz.storeFile(
                            QString("%1").arg(it.key()), b));
                b.close();
            }
            qz.setGlobalComment("Zip comment.");
            qz.close();
            zf.close();

            checkFilesInZip(file_name, map, map.count());
            repack(file_name);
            checkFilesInZip(file_name, map, map.count());

            CPPUNIT_ASSERT(zf.open(IO_ReadWrite));
            CPPUNIT_ASSERT(qz.open(&zf, true));

            for ( it = map.begin(); it != map.end(); ++it ) {
                QBuffer b(it.data());
                b.open(IO_ReadOnly);
                CPPUNIT_ASSERT(qz.storeFile(
                            QString("dir/%1").arg(it.key()), b));
                b.close();
            }
            qz.close();
            zf.close();

            checkFilesInZip(file_name, map, 2*map.count());
            repack(file_name);
            checkFilesInZip(file_name, map, 2*map.count());

            zf.remove();
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestZip );
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
// $Id: check_libltzip.cpp,v 1.7 2004/09/21 14:45:55 hynek Exp $
