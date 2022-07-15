import QtQuick 2.3
import QtQuick.Controls 2.15

Rectangle
{

    id:main;
    width: 800
    height: 600

    signal signalAgreeAdd(string strName);
    signal signalRequestAddFriend(string strFriendId,string strVerifyMsg);

    function insertNewAddFriendRequest(strShouzimu,strName,strVerifyMsg,bIsValid)
    {
        addFriendListModel.insert(0,{"shouZimu":strShouzimu,"name":strName,"verifyMsg":strVerifyMsg,"isValid":bIsValid});
    }

    function appendAddFriendRequest(strShouzimu,strName,strVerifyMsg,bIsValid)
    {
        addFriendListModel.append({"shouZimu":strShouzimu,"name":strName,"verifyMsg":strVerifyMsg,"isValid":bIsValid});
    }

    Text
    {
        id:tips;
        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 70;
        width: 150;
        height: 20
        font.family: "msyh"
        font.pointSize: 16
        font.bold: true;
        text:
        {
            "请输入好友id";
        }
    }

    Text
    {
        id:addTips;
        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.left: tips.right;
        anchors.leftMargin: 50;
        width: 150;
        height: 20
        font.family: "msyh"
        font.pointSize: 16
        font.bold: true;
        text:
        {
            "已发送请求";
        }
        visible: false;
    }

    Rectangle
    {
        id:textEditRect;
        anchors.top: tips.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 70;
        width: 300;
        height: 40;
        color: "lightgrey";
        border.color: "grey";
        TextEdit
        {
            id:textEdit;
            color: "red";
            font.family: "msyh"
            font.pointSize: 20
            focus: true;
            anchors.fill: parent;
            wrapMode: TextEdit.WrapAnywhere;

        }
    }

    Rectangle
    {
        id:verifyMsgTextEditRect;
        anchors.top: textEditRect.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 70;
        width: 300;
        height: 40;
        color: "lightgrey";
        border.color: "grey";
        TextEdit
        {
            id:verifyMsgTextEdit;
            color: "red";
            font.family: "msyh"
            font.pointSize: 20
            focus: true;
            anchors.fill: parent;
            wrapMode: TextEdit.WrapAnywhere;

        }
    }

    Timer
    {
        id:tipsTimer;
        interval: 2000;
        onTriggered:
        {
            stop();
            addTips.visible=false;
        }
    }

    Button
    {
        id: addFriendBtn;
        height: 30;
        width: 50;
        anchors.top: tips.bottom;
        anchors.topMargin: 20;
        anchors.right: parent.right;
        anchors.rightMargin: 30;
        Text
        {
            text: "添加"
            font.family: "msyh";
            font.pixelSize: 24;
            anchors.centerIn: parent;
        }
        onClicked:
        {
            main.signalRequestAddFriend(textEdit.text,verifyMsgTextEdit.text);
            addTips.visible=true;
            if(tipsTimer.running)
            {
                return;
            }
            else
            {
                tipsTimer.start();
            }
        }
    }

    ListView
    {
        id:addFriendListView;
        model: addFriendListModel;
        delegate: addFriendListDelegate;
        orientation: ListView.Vertical;
        //anchors.fill: parent;
        height: parent.height-110;
        width: parent.width;
        anchors.top: verifyMsgTextEditRect.bottom;
        clip: true;
        /*ScrollBar.vertical: ScrollBar
        {
            id: scrollBar;
            policy: ScrollBar.AsNeeded;
        }*/
    }

    ListModel
    {
        id:addFriendListModel;
        /*ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }
        ListElement
        {
            shouZimu:"A"
            name:"qqq"
            verifyMsg:"我是你爸爸"
            isValid:false
        }
        ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }
        ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }
        ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }
        ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }
        ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }
        ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }
        ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }
        ListElement
        {
            shouZimu:"B"
            name:"lll"
            verifyMsg:"我是你爸爸"
            isValid:true
        }*/
    }

    Component
    {
        id:addFriendListDelegate;
        Rectangle
        {

            width:main.width;
            height: 80;
            Rectangle
            {
                id: pic;
                width: 60;
                height: 60;
                radius: 30;
                color: "#8033CCFF";
                anchors.left: parent.left;
                anchors.leftMargin: 20;
                anchors.top: parent.top;
                anchors.topMargin: 10;
                Text
                {
                    font.family: "msyh";
                    font.pixelSize: 30;
                    text: model.shouZimu
                    anchors.centerIn: parent;
                }
            }

            Text
            {
                id: name;
                width: 150;
                height: 20;
                elide: Text.ElideRight;
                anchors.top: parent.top;
                anchors.topMargin: 10;
                anchors.left: pic.right;
                anchors.leftMargin: 15;
                text:model.name;
                font.family: "msyh";
                font.pixelSize: 20;
            }

            Text {
                id: message;
                width: main.width*0.7;
                height: 20;
                wrapMode: Text.WrapAnywhere;
                anchors.left: pic.right;
                anchors.leftMargin: 10;
                anchors.top: name.bottom;
                anchors.topMargin: 10;
                text:model.verifyMsg;
                elide: Text.ElideRight;
                font.family: "msyh";
                font.pixelSize: 24;
            }

            Button
            {
                id:verifyButton;
                height:40;
                width:50;
                anchors.right:parent.right;
                anchors.rightMargin: 30;
                anchors.top: parent.top;
                anchors.topMargin: 10;
                visible:!model.isValid;
                Text
                {
                    anchors.centerIn: parent;
                    text: "同意";
                    font.family: "msyh";
                    font.pixelSize: 24;
                }
                MouseArea
                {
                    anchors.fill: parent;
                    onClicked:
                    {
                        main.signalAgreeAdd(model.name);
                        model.isValid=true;
                    }
                }
            }

            Rectangle
            {
                height: 40;
                width: 60;
                anchors.right:parent.right;
                anchors.rightMargin: 30;
                anchors.top: parent.top;
                anchors.topMargin: 10;
                visible:model.isValid;
                Text {
                    id: validLabel;
                    text: qsTr("已通过");
                    font.pixelSize: 24;
                    font.family: "msyh";
                    anchors.centerIn: parent;
                }

            }
        }
    }

}
