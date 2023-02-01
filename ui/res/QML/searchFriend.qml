import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {

    //传递出去当前是哪个id对应的对话被点击了，找出对应的聊天记录
    signal signalSearchFriendListClicked(string strId,string strName);

    //通讯录窗口
    id: searchFriend;

    width: 200
    height: 100
    color: "transparent";

    ListView
    {
        id:searchFriendView;
        model: searchFriendModel;
        delegate: searchFriendDelegate;
        anchors.fill: parent;
        //上次点击的index
        property int lastIndex: -1;
        //上次点击对应的item
        property var lastItem: null;
        //增加滑动条
        ScrollBar.vertical: ScrollBar
        {
            id: scrollBar;
            visible: false;
            policy: ScrollBar.AsNeeded/*AlwaysOn*/;
        }
    }


    //列表委托
    Component
    {
        id:searchFriendDelegate;
        Rectangle
        {
            id: singleItemInView;
            width: parent.width;
            height: 80;

            MouseArea
            {
                anchors.fill: parent;
                //点击这个会话时，上次点击的颜色变成透明，这次点击的变成灰色，并更新这次的index等为上次点击
                onClicked:
                {
                    if(searchFriendView.lastItem!==null)
                    {
                        searchFriendView.lastItem.color="transparent";
                    }
                    color="grey";
                    searchFriendView.currentIndex=index;
                    searchFriendView.lastIndex=index;
                    searchFriendView.lastItem=searchFriendView.currentItem;
                    searchFriend.signalFriendListClicked(strIdx,name);
                }
                hoverEnabled: true;
                //鼠标移动到上方时变为银色，如果移动到上次点击的item则不做处理
                onEntered:
                {
                    scrollBar.visible=true;
                    if(searchFriendView.currentIndex===index)
                    {
                        return;
                    }
                    color="silver";
                }
                //移走后颜色变为正常
                onExited:
                {
                    scrollBar.visible=false;
                    if(searchFriendView.currentIndex===index)
                    {
                        return;
                    }
                    color="transparent";
                }
            }

            //头像区域
            Rectangle
            {
                id:imageLabel;
                //color: "#8033CCFF";
                width: 50;
                height: 50;
                //radius: 25;
                anchors.top: parent.top;
                anchors.topMargin: 20;
                anchors.left: parent.left;
                anchors.leftMargin: 15;
                Image {
                    id: profileImage
                    anchors.fill: parent;
                    source: imagePath;
                }
            }
            //名字区域
            Rectangle
            {
                id:nameLabel;
                color: "transparent";
                width: 180;
                height: 30;
                anchors.top: parent.top;
                anchors.topMargin: 30;
                anchors.left: imageLabel.right;
                anchors.leftMargin: 15;
                Text
                {
                    id: nameText;
                    elide: Text.ElideRight;
                    anchors.fill: parent;
                    font.family: "msyh";
                    font.pixelSize: 14;
                    color: "black";
                    text: name;
                }
            }

        }
    }

}
