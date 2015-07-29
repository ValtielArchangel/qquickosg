/*
* Copyright (c) 2015 Sparadon.com
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is furnished
* to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once

#include <QFile>
#include <osgDB/Registry>

struct Utils
{
    struct membuf: std::streambuf
    {
        membuf(char *base, std::ptrdiff_t n) { this->setg( base, base, base + n ); }
    };

    static osg::Node * loadIveFromQrc( const QString & resource )
    {
        QFile file( resource );

        if ( !file.open( QFile::ReadOnly ) )
        {
            qDebug() << "Error: error opening " << resource;
            return nullptr;
        }

        QByteArray bb = file.readAll();
        file.close();

        membuf buf( bb.data(), bb.length() );
        std::istream in( &buf );

        osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension( "ive" );
        if ( !rw )
            return nullptr;

        osgDB::ReaderWriter::ReadResult rr = rw->readNode( in );

        return rr.takeNode();
    }
};

