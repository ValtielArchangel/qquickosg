# qquickosg
OpenSceneGraph QtQuick qml integration

Tested with Qt 5.4 on Windows, Linux, Android, iOS

Very simple to use:

```c++
class TestRenderer : public QQuickOsgItem::Renderer
{
    void createScene() override
    {
        osg::Node *node = Utils::loadIveFromQrc( ":/cow.ive" );
        m_osgViewer->setSceneData( node );
        m_osgViewer->setCameraManipulator( new osgGA::MultiTouchTrackballManipulator );
    }

    void updateScene() override { update(); }
    void synchronize( QQuickOsgItem *item ) override { Q_UNUSED( item ) }
};

class OsgItemTest : public QQuickOsgItem
{
    Q_OBJECT
public:
    OsgItemTest() { setAcceptedMouseButtons( Qt::AllButtons ); }
    QQuickOsgItem::Renderer * createRenderer() const override { return new TestRenderer(); }
};
```
![alt text](https://github.com/sparadon/qquickosg/raw/master/screen.png "Result")

