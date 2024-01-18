import QtQuick
import QtQuick.Window
import FluentUI
import FluentPlayer
//import QtMultimedia 5.15
import QtMultimedia

FluWindow {
    id: window
    width: 800
    height: 600
    minimumWidth: 500
    minimumHeight: 400
    title:"视频播放器"
    showDark: true
    fitsAppBarWindows: visibility === Window.FullScreen ? true : false

    QtObject{
        id:d
        property bool flag: true
        property bool isFullScreen : window.visibility === Window.FullScreen
        property int currVolume: 100
    }

    ListModel{
        id:model_speed
        ListElement{
            text:"2.0x"
            value:2.0
        }
        ListElement{
            text:"1.5x"
            value:1.5
        }
        ListElement{
            text:"1.25x"
            value:1.25
        }
        ListElement{
            text:"1.0x"
            value:1.0
        }
        ListElement{
            text:"0.75x"
            value:0.75
        }
        ListElement{
            text:"0.5x"
            value:0.5
        }
    }

    Rectangle{
        id:layout_background
        anchors.fill: parent
        color: FluColors.Black
    }

    FluentPlayer{
        id:player
        source: "file:///C:/Users/zhuzi/Desktop/video/mvxaFtMxTNDBvvj5.mp4"
        //        source: "file:///C:/Users/Administrator/Desktop/Video/z89qvLhwGanvCzUe.mp4"
        onPositionChanged: {
            if(d.flag){
                slider.value = position
            }
        }
//        speed: model_speed.get(menu_speed.currIndex).value
//        speed:1.0
        volume: d.currVolume
        videoOutput:video_output
    }

    VideoOutput{
        id:video_output
        anchors.fill: parent
        //        source: player
    }

    Timer{
        id:timer_hide_delay
        interval: 1000
        onTriggered: {
            layout_control.opacity = false
        }
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onCanceled: {
            layout_control.opacity = false
        }
        onExited: {
            layout_control.opacity = false
        }
        onPositionChanged:
            (mouse)=> {
                layout_control.opacity = true
                if(mouse.x > layout_control.x && mouse.y > layout_control.y && mouse.x <layout_control.x + layout_control.width && mouse.y<layout_control.y + layout_control.height){
                    timer_hide_delay.stop()
                }else{
                    timer_hide_delay.restart()
                }
            }
        FluArea{
            id:layout_control
            width: parent.width-30
            height: 100
            opacity: 1
            visible: opacity
            anchors{
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 15
            }
            MouseArea{
                anchors.fill: parent
            }
            Behavior on opacity {
                SequentialAnimation {
                    PauseAnimation { duration: 300 }
                    NumberAnimation { duration: 83 }
                }
            }
            FluSlider{
                id:slider
                anchors{
                    left: parent.left
                    right: parent.right
                    leftMargin: 15
                    rightMargin: 15
                    top: parent.top
                    topMargin: 15
                }
                text:formatDuration(value)
                from: 0
                to: player.duration
                value: player.position
                onPressedChanged: {
                    var position = value
                    if(pressed){
                        d.flag = false
                    }else{
                        player.seek(position)
                        d.flag = true;
                    }
                }
            }

            FluText{
                text: formatDuration(player.position)
                anchors{
                    left: slider.left
                    top: slider.bottom
                }
            }

            FluText{
                text: formatDuration(player.duration)
                anchors{
                    right: slider.right
                    top: slider.bottom
                }
            }

            FluIconButton{
                iconSource: player.playing ? FluentIcons.Pause : FluentIcons.Play
                anchors{
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 5
                }
                width: 30
                height: 30
                iconSize: 20
                verticalPadding: 0
                horizontalPadding: 0
                onClicked: {
                    if(player.playing){
                        player.pause()
                    }else{
                        player.play()
                    }
                }
            }

            Row{
                height: 30
                anchors{
                    right: parent.right
                    bottom: parent.bottom
                    rightMargin: 10
                    bottomMargin: 5
                }
                spacing: 8

                FluTextButton{
                    id:btn_speed
                    text: menu_speed.currIndex === 3 ? "倍速" : model_speed.get(menu_speed.currIndex).text
                    normalColor: FluTheme.fontPrimaryColor
                    onClicked: {
                        var pos = mapToItem(layout_background,0,0)
                        menu_speed.x = pos.x - (68-btn_speed.width)/2
                        menu_speed.y =  pos.y - (menu_speed.count * 36)
                        menu_speed.open()
                    }
                }
                FluIconButton{
                    iconSource: player.volume === 0 ? FluentIcons.Mute : FluentIcons.Volume
                    width: 30
                    height: 30
                    iconSize: 16
                    verticalPadding: 0
                    horizontalPadding: 0
                    onClicked: {
                        if(player.volume != 0){
                            player.volume = 0
                        }else{
                            player.volume = d.currVolume
                        }
                    }
                }
                FluIconButton{
                    iconSource: d.isFullScreen ? FluentIcons.BackToWindow : FluentIcons.FullScreen
                    width: 30
                    height: 30
                    iconSize: 14
                    verticalPadding: 0
                    horizontalPadding: 0
                    onClicked: {
                        if(d.isFullScreen){
                            showNormal()
                        }else{
                            showFullScreen()
                        }
                    }
                }
            }
        }
    }

    FluMenu{
        id:menu_speed
        width: 68
        property int currIndex: 3
        Repeater{
            model: model_speed
            FluMenuItem{
                text: model.text
                onClicked: {
                    menu_speed.currIndex = index
                }
            }
        }
    }


    function formatDuration(duration) {
        const seconds = Math.floor(duration / 1000);
        const hours = Math.floor(seconds / 3600);
        const minutes = Math.floor((seconds % 3600) / 60);
        const remainingSeconds = seconds % 60;
        return `${pad(hours)}:${pad(minutes)}:${pad(remainingSeconds)}`;
    }

    function pad(value) {
        return value.toString().padStart(2, '0');
    }

}
