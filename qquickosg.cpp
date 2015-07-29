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

#include "qquickosg.h"

#include <QQuickWindow>
#include <QScreen>
#include <QOpenGLContext>

#include <osgViewer/Renderer>

// ===================================================================
class QQuickOsgNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:

    QQuickOsgNode( QQuickWindow *window ) :
        m_pWindow( window )
    {
        m_pQtContext = m_pWindow->openglContext();
        m_pOsgContext = new QOpenGLContext();
        m_pOsgContext->setFormat( m_pWindow->requestedFormat() );
        m_pOsgContext->setShareContext( m_pQtContext );
        m_pOsgContext->create();
    }

    ~QQuickOsgNode()
    {
        if ( m_pRenderer )
            delete m_pRenderer;

        if ( m_pOsgContext )
            delete m_pOsgContext;

        if ( m_pFboTexture )
            delete m_pFboTexture;
    }

    void scheduleRender()
    {
        m_renderPending = true;
        m_pWindow->update();
    }

    QSGTexture * texture() const override
    {
        return QSGSimpleTextureNode::texture();
    }

public slots:

    void render()
    {
        if( m_renderPending )
        {
            m_renderPending = false;

            m_pQtContext->doneCurrent();
            m_pOsgContext->makeCurrent( m_pWindow );

            m_pRenderer->updateScene();
            m_pRenderer->m_osgViewer->frame();

#           ifdef Q_OS_IOS
                glFlush();
#           endif

            m_pOsgContext->doneCurrent();
            m_pQtContext->makeCurrent( m_pWindow );

            markDirty( QSGNode::DirtyMaterial );
            emit textureChanged();

            scheduleRender();
        }
    }

private:   
    void setTexture( QSGTexture *texture )
    {
        m_pFboTexture = texture;
        setTextureCoordinatesTransform( QSGSimpleTextureNode::MirrorVertically );
        QSGSimpleTextureNode::setTexture( m_pFboTexture );
    }

private:
    bool m_renderPending = false;
    QQuickWindow *m_pWindow = nullptr;
    QOpenGLContext *m_pOsgContext = nullptr;
    QOpenGLContext *m_pQtContext = nullptr;
    QQuickOsgItem::Renderer *m_pRenderer = nullptr;
    QSGTexture *m_pFboTexture = nullptr;

    friend class QQuickOsgItem;
};
// ===================================================================

QQuickOsgItem::QQuickOsgItem( QQuickItem *parent ) :
        QQuickItem( parent )
{
    setFlag( ItemHasContents );
}

QSGNode * QQuickOsgItem::updatePaintNode( QSGNode *n, UpdatePaintNodeData * )
{
    QQuickOsgNode *node = static_cast<QQuickOsgNode *>( n );

    if ( !node && ( width() <= 0 || height() <= 0 ) )
        return nullptr;

    if ( !node )
    {
        if ( !m_pNode )
            m_pNode = new QQuickOsgNode( window() );
        node = m_pNode;
    }

    if ( !node->m_pRenderer )
    {
        node->m_pRenderer = createRenderer();
        node->m_pRenderer->initOsg( width(), height() );
        node->m_pRenderer->data = node;
        QSGTexture * texture = window()->createTextureFromId( node->m_pRenderer->fboTextureId(), QSize( width(), height() ) );
        node->setTexture( texture );
        node->setRect( 0, 0, width(), height() );
        node->m_pRenderer->createScene();
        connect( window(), &QQuickWindow::beforeRendering, m_pNode, &QQuickOsgNode::render );
    }

    node->m_pRenderer->synchronize( this );

    if ( width() != node->rect().width() || height() != node->rect().height() )
    {
        node->m_pRenderer->resizeFboTexture( width(), height() );
        QSGTexture * oldTexture = node->texture();
        delete oldTexture;
        QSGTexture * newTexture = window()->createTextureFromId( node->m_pRenderer->fboTextureId(), QSize( width(), height() ) );
        node->setTexture( newTexture );
    }

    node->setRect( 0, 0, width(), height() );
    node->scheduleRender();

    return node;
}

void QQuickOsgItem::mouseMoveEvent( QMouseEvent * event )
{
    if ( !m_pNode )
        return;

    m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->mouseMotion( (float)event->x(), (float)event->y() );
    event->accept();
}

void QQuickOsgItem::mousePressEvent( QMouseEvent * event )
{
    if ( !m_pNode )
        return;

    osgGA::GUIEventAdapter::MouseButtonMask button;

    if ( Qt::LeftButton == event->button() )
        button = osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
    else if ( Qt::RightButton == event->button() )
        button = osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON;
    else if ( Qt::MidButton == event->button() )
        button = osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON;
    else
        return;

    m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->mouseButtonPress(
                (float)event->x(), (float)event->y(), button );

    event->accept();
}

void QQuickOsgItem::mouseDoubleClickEvent( QMouseEvent *event )
{
    if ( !m_pNode )
        return;

    osgGA::GUIEventAdapter::MouseButtonMask button;

    if ( Qt::LeftButton == event->button() )
        button = osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
    else if ( Qt::RightButton == event->button() )
        button = osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON;
    else if ( Qt::MidButton == event->button() )
        button = osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON;
    else
        return;

    m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->mouseDoubleButtonPress(
                (float)event->x(), (float)event->y(), button );

    event->accept();
}

void QQuickOsgItem::mouseReleaseEvent( QMouseEvent * event )
{
    if ( !m_pNode )
        return;

    osgGA::GUIEventAdapter::MouseButtonMask button;

    if ( Qt::LeftButton == event->button() )
        button = osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
    else if ( Qt::RightButton == event->button() )
        button = osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON;
    else if ( Qt::MidButton == event->button() )
        button = osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON;
    else
        return;

    //std::cout << "x: " << (float)event->x() << "y: " << (float)event->y() << std::endl << "w: " << width() << "h: " << height() << std::endl;

    m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->mouseButtonRelease(
                (float)event->x(), (float)event->y(), button );

    event->accept();
}

void QQuickOsgItem::wheelEvent( QWheelEvent * event )
{
    if ( !m_pNode )
        return;

    m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->mouseScroll(
                event->delta() < 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN );
}

void QQuickOsgItem::touchEvent( QTouchEvent * event )
{
    if ( !m_pNode )
        return;

    osg::ref_ptr<osgGA::GUIEventAdapter> osg_event;

    for ( auto itr = event->touchPoints().begin(); itr != event->touchPoints().end(); ++itr )
    {
        float x = itr->pos().x();
        float y = itr->pos().y();
        int id = itr->id();

        if ( itr->state() == Qt::TouchPointPressed )
        {
            if ( !osg_event.valid() )
                osg_event = m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->touchBegan( id, osgGA::GUIEventAdapter::TOUCH_BEGAN, x, y );
            else
                osg_event->addTouchPoint( id, osgGA::GUIEventAdapter::TOUCH_BEGAN, x, y );
        }
        else if ( itr->state() == Qt::TouchPointMoved )
        {
            if ( !osg_event.valid() )
                osg_event = m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->touchMoved( id, osgGA::GUIEventAdapter::TOUCH_MOVED, x, y );
            else
                osg_event->addTouchPoint( id, osgGA::GUIEventAdapter::TOUCH_MOVED, x, y );
        }
        else if ( itr->state() == Qt::TouchPointReleased )
        {
            if ( !osg_event.valid() )
                osg_event = m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->touchEnded( id, osgGA::GUIEventAdapter::TOUCH_ENDED, x, y, event->touchPoints().count() );
            else
                osg_event->addTouchPoint( id, osgGA::GUIEventAdapter::TOUCH_ENDED, x, y );
        }
        else if ( itr->state() == Qt::TouchPointStationary )
        {
            if ( !osg_event.valid() )
                osg_event = m_pNode->m_pRenderer->m_osgViewer->getEventQueue()->touchMoved( id, osgGA::GUIEventAdapter::TOUCH_STATIONERY, x, y);
            else
                osg_event->addTouchPoint( id, osgGA::GUIEventAdapter::TOUCH_STATIONERY, x, y );
        }
    }

    event->accept();
}


// ===============================================================================================================
QQuickOsgItem::Renderer::Renderer( qreal fov, qreal zNear, qreal zFar, qreal ratio, const osg::Vec4 & color ) :
    m_fov( fov ),
    m_zNear( zNear ),
    m_zFar( zFar ),
    m_zNearFarRatio( ratio ),
    m_color( color )
{

}

void QQuickOsgItem::Renderer::synchronize( QQuickOsgItem *item )
{
    Q_UNUSED( item )
}

void QQuickOsgItem::Renderer::initOsg( qreal width, qreal height )
{
    m_width = width;
    m_height = height;

    m_osgViewer = new osgViewer::Viewer();
    m_osgViewer->setUpViewerAsEmbeddedInWindow( 0, 0, m_width, m_height );
    m_osgViewer->getEventQueue()->windowResize( 0, 0, m_width, m_height );

    osg::Camera *camera = m_osgViewer->getCamera();
    camera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    camera->setViewport( 0, 0, m_width, m_height );
    camera->setProjectionMatrixAsPerspective( m_fov, m_width / m_height, m_zNear, m_zFar );
    camera->setNearFarRatio( m_zNearFarRatio );
    camera->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    camera->setClearColor( m_color );

    m_fboTexture = new osg::Texture2D();
    m_fboTexture->setTextureSize( m_width, m_height );
    m_fboTexture->setInternalFormat( GL_RGBA );
    m_fboTexture->setFilter( osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR );
    m_fboTexture->setFilter( osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR );
    m_fboTexture->apply( *camera->getGraphicsContext()->getState() );

    camera->attach( osg::Camera::COLOR_BUFFER, m_fboTexture, 0, 0 );

    osgViewer::Renderer *renderer = dynamic_cast<osgViewer::Renderer*>( camera->getRenderer() );
    if ( renderer )
        renderer->setCameraRequiresSetUp( true );
}

void QQuickOsgItem::Renderer::resizeFboTexture( qreal newWidth, qreal newHeight )
{
    m_width = newWidth;
    m_height = newHeight;

    if ( !m_fboTexture.valid() )
        return;

    osg::Camera *camera = m_osgViewer->getCamera();

    m_fboTexture->setTextureSize( newWidth, newHeight );
    m_fboTexture->dirtyTextureObject();
    m_fboTexture->apply( *camera->getGraphicsContext()->getState() );

    osgViewer::Renderer *renderer = dynamic_cast<osgViewer::Renderer*>( camera->getRenderer() );
    renderer->setCameraRequiresSetUp( true );

    camera->setViewport( 0, 0, newWidth, newHeight );
    camera->setProjectionMatrixAsPerspective( m_fov, newWidth / newHeight, m_zNear, m_zFar );

    m_osgViewer->getEventQueue()->windowResize( 0, 0, m_width, m_height );
}

unsigned int QQuickOsgItem::Renderer::fboTextureId()
{
    if ( !m_osgViewer.valid() || !m_fboTexture.valid() )
        return 0;

    unsigned int ctxId = m_osgViewer->getCamera()->getGraphicsContext()->getState()->getContextID();
    return m_fboTexture->getTextureObject( ctxId )->id();
}

void QQuickOsgItem::Renderer::update()
{
    if ( data )
        ((QQuickOsgNode *) data)->scheduleRender();
}

// ===============================================================================================================

#include "qquickosg.moc"
