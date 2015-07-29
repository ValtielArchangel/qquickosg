import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

import com.sparadon 1.0

ApplicationWindow
{
    title: qsTr("Hello OSG")
    width: 320
    height: 240
    visible: true

    GridLayout
    {
        id: grid
        columns: 2
        anchors.fill: parent

        OsgItemTest
        {
            id: building
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        OsgItemTest
        {
            id: building1
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        OsgItemTest
        {
            id: building2
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        OsgItemTest
        {
            id: building3
            Layout.fillWidth: true
            Layout.fillHeight: true
         }
    }
}
