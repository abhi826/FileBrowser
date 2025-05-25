import QtQuick
import QtQuick.Controls 6.5
import FileBrowser 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Lazy File Browser (Tree View)"

    TreeView {
        anchors.fill: parent
        model: FileModelInstance
        clip: true

        delegate: Item {
            required property int row
            implicitHeight: 30
            implicitWidth: 700

            Row {
                spacing: 16
                Text { text: model.name; width: 300 }
                Text { text: model.size; width: 100 }
                Text { text: model.type; width: 100 }
                Text { text: model.modified; width: 200 }
            }
        }

        ScrollBar.vertical: ScrollBar {}
    }
}
