/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import AsemanTools 1.0

Item {
    id: sidemenu
    width: 100
    height: 62
    clip: true

    property Component delegate
    property variant item

    property real handleWidth: 20*Devices.density

    property alias menuWidth: item_frame.width
    property alias pressed: marea.pressed
    readonly property real percent: {
        var res = (menuWidth+item_frame.x)/menuWidth
        if(res < 0)
            res = 0
        else
        if(res > 1)
            res = 1
        return res
    }

    property int layoutDirection: View.layoutDirection
    readonly property bool showed: item_frame.x > -menuWidth

    onDelegateChanged: {
        if( item )
            item.destroy()
        if( !delegate )
            return

        item = delegate.createObject(item_parent)
    }

    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: percent*0.7
    }

    Item {
        id: main_frame
        anchors.fill: parent
        transformOrigin: Item.Center
        rotation: layoutDirection==Qt.RightToLeft? 180 : 0

        MouseArea {
            id: marea
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: lastState? parent.width : handleWidth + menuWidth
            x: item_frame.x
            onMouseXChanged: {
                var newX = item_frame.x + mouseX-pinX
                if( newX > 0 )
                    newX = 0
                if( Math.abs(x-startX) > 5*Devices.density )
                    movedX = true

                item_frame.x = newX
            }
            onPressed: {
                pinX = mouseX
                item_frame.anim = false
                startX = x
                movedX = false
            }
            onReleased: {
                item_frame.anim = true

                var basePercent = lastState? 0.8 : 0.2

                if( !movedX ) {
                    item_frame.x = -menuWidth
                    lastState = false
                    return
                }

                if( sidemenu.percent > basePercent ) {
                    item_frame.x = 0
                    lastState = true
                } else {
                    item_frame.x = -menuWidth
                    lastState = false
                }
            }

            property real pinX: 0
            property bool lastState: false
            property real startX: 0
            property bool movedX: false

            onLastStateChanged: {
                if( lastState )
                    BackHandler.pushHandler(sidemenu,sidemenu.discard)
                else
                    BackHandler.removeHandler(sidemenu)
            }
        }

        Item {
            id: item_frame
            width: {
                var res = Devices.isMobile? sidemenu.width-50*Devices.density : sidemenu.width/2 + 20*Devices.density
                if(res > 250*Devices.density)
                    res = 250*Devices.density
                return res
            }
            height: parent.height
            x: -width

            onWidthChanged: {
                if(!marea.lastState)
                {
                    item_frame.anim = false
                    item_frame.x = -menuWidth
                }
            }

            property bool anim: false

            Behavior on x {
                NumberAnimation{ easing.type: Easing.OutCubic; duration: item_frame.anim? 400 : 0 }
            }

            Item {
                id: item_parent
                width: parent.width
                height: parent.height
                transformOrigin: Item.Center
                rotation: layoutDirection==Qt.RightToLeft? 180 : 0
            }
        }
    }

    function discard() {
        item_frame.anim = true
        item_frame.x = -menuWidth
        marea.lastState = false
    }

    function show() {
        item_frame.anim = true
        item_frame.x = 0
        marea.lastState = true
    }
}
