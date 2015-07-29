TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    qquickosg.cpp \
    osgitemtest.cpp \
    GLES2ShaderGenVisitor.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    qquickosg.h \
    osgitemtest.h \
    utils.h \
    GLES2ShaderGenVisitor.h


win32:INCLUDEPATH += $$PWD/sg3rdParty/OpenSceneGraph-3.2.1-sdk-win-x64-msvc2013/include
android:INCLUDEPATH += $$PWD/sg3rdParty/OpenSceneGraph-3.2.0-sdk-android/include
ios:INCLUDEPATH += $$PWD/sg3rdParty/OpenSceneGraph-3.2.1-sdk-ios/include

win32:CONFIG( debug, debug|release ):LIBS += -L$$PWD/sg3rdParty/OpenSceneGraph-3.2.1-sdk-win-x64-msvc2013/lib/ -lOpenThreadsd -losgTextd -losgd -losgViewerd -losgDBd -losgGAd -losgUtild
win32:CONFIG( release, debug|release ):LIBS += -L$$PWD/sg3rdParty/OpenSceneGraph-3.2.1-sdk-win-x64-msvc2013/lib/ -lOpenThreads -losgText -losg -losgViewer -losgDB -losgGA -losgUtil

android:LIBS += -L$$PWD/sg3rdParty/OpenSceneGraph-3.2.0-sdk-android/lib/armeabi-v7a \
-losgdb_osg \
-losgdb_ive \
-losgdb_png \
-losgdb_jpeg \
-losgdb_serializers_osg \
-losgViewer \
-losgVolume \
-losgTerrain \
-losgText \
-losgShadow \
-losgSim \
-losgManipulator \
-losgGA \
-losgFX \
-losgDB \
-losgAnimation \
-losgUtil \
-losg \
-lOpenThreads \
-lpng \
-ljpeg

ios:LIBS += -L$$PWD/sg3rdParty/OpenSceneGraph-3.2.1-sdk-ios/lib \
-losgdb_osg \
-losgdb_ive \
-losgdb_imageio \
-losgdb_serializers_osg \
-losgViewer \
-losgVolume \
-losgTerrain \
-losgText \
-losgShadow \
-losgSim \
-losgManipulator \
-losgGA \
-losgFX \
-losgDB \
-losgAnimation \
-losgUtil \
-losg \
-lOpenThreads \
-framework Accelerate \
-framework MobileCoreServices \
-framework ImageIO \
-liconv \
-lsqlite3
