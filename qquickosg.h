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

#include <QSGSimpleTextureNode>
#include <QSGTextureProvider>
#include <QQuickItem>

#include <osg/Texture2D>
#include <osgViewer/Viewer>

class QQuickOsgNode;

class QQuickOsgItem : public QQuickItem
{
    Q_OBJECT

public:

    class Renderer
    {
    protected:
        Renderer() {}
        Renderer( qreal fov, qreal zNear, qreal zFar, qreal ratio, const osg::Vec4 & color );
        virtual ~Renderer() {}

        virtual void synchronize( QQuickOsgItem *item );

        virtual void initOsg( qreal width, qreal height );

        virtual void createScene() = 0;
        virtual void updateScene() = 0;

        virtual void resizeFboTexture( qreal newWidth, qreal newHeight );
        virtual unsigned int fboTextureId();

        void update();

    protected:
        qreal m_width;
        qreal m_height;

        osg::ref_ptr<osgViewer::Viewer> m_osgViewer;
        osg::ref_ptr<osg::Texture2D> m_fboTexture;

        qreal m_fov = 45.0;
        qreal m_zNear = 0.1;
        qreal m_zFar = 100.0;
        qreal m_zNearFarRatio = 0.01;
        osg::Vec4 m_color = osg::Vec4( 0.5f, 0.5f, 0.5f, 1.0f );

    private:
        friend class QQuickOsgItem;
        friend class QQuickOsgNode;
        void *data;
    };

    QQuickOsgItem( QQuickItem *parent = nullptr );

    virtual Renderer *createRenderer() const = 0;

    void update();

protected:
    QSGNode * updatePaintNode( QSGNode *, UpdatePaintNodeData * ) override;

    virtual void mouseMoveEvent( QMouseEvent * event ) override;
    virtual void mousePressEvent( QMouseEvent * event ) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent( QWheelEvent * event ) override;
    virtual void touchEvent( QTouchEvent * event ) override;
    virtual void mouseDoubleClickEvent( QMouseEvent *event ) override;

private:
    QQuickOsgNode *m_pNode = nullptr;
};
