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

#include "qquickosg.h"

#if defined( Q_OS_ANDROID ) || defined( Q_OS_IOS )
#include <osgDB/Registry>
    USE_OSGPLUGIN(ive)
    USE_OSGPLUGIN(osg)
    USE_OSGPLUGIN(osg2)
#   if defined( Q_OS_ANDROID )
        USE_OSGPLUGIN(png)
        USE_OSGPLUGIN(jpeg)
#   else
        USE_OSGPLUGIN(imageio)
#endif
    USE_SERIALIZER_WRAPPER_LIBRARY(osg)
#endif

class TestRenderer : public QQuickOsgItem::Renderer
{
    void createScene() override;
    void updateScene() override;
    void synchronize( QQuickOsgItem *item ) override { Q_UNUSED( item ) }
};

class OsgItemTest : public QQuickOsgItem
{
    Q_OBJECT

public:
    OsgItemTest() { setAcceptedMouseButtons( Qt::AllButtons ); }

    QQuickOsgItem::Renderer * createRenderer() const override { return new TestRenderer(); }

};
