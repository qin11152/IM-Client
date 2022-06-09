import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle
{

    //传递出去当前是哪个id对应的对话被点击了，找出对应的聊天记录
    signal signalFriendListClicked(string strId);

    //增加元素到模型中，也就是增加一个会话到好友列表之中
    function addElementToModel(strImagePath,strName,strId)
    {
        friendListModel.append({"imagePath":strImagePath,"name":strName,"idx":strId});
    }

    function insertElementToModel(strImagePath,strName,strId)
    {
        friendListModel.insert(0,{"imagePath":strImagePath,"name":strName,"idx":strId});
    }

    //初始化的时候谁都不是当前对象
    function initColor()
    {
        console.log("qqqqqq");
        friendListView.currentItem.color="transparent";
        friendListView.lastIndex=19999;
        //friendListView.currentItem.color="grey";
        friendListView.lastItem=null;
    }

    //清空好友列表
    function clearModel()
    {
        friendListModel.clear();
    }

    //根据id找到这个好友所在模型中的位置
    function findPosInModel(strId)
    {
        for(var i=0;i<friendListModel.count;++i)
        {
            if(friendListModel.get(i).id===strId)
            {
                moveActiveFriend2Top(i);
                break;
            }
        }
    }

    //把这个会话移动到列表最顶端，当有新对话来时使用
    function moveActiveFriend2Top(iPos)
    {
        friendListModel.move(iPos,0,1);
    }

    //构造完成会对颜色的处理并移动到最上边的一个对话
    Component.onCompleted:
    {
        if(friendListModel.count===0)
        {
            return;
        }
        friendListView.lastIndex=19999;
        //friendListView.currentItem.color="grey";
        friendListView.lastItem=null;
    }

    id: main;
    width: 280;
    height: 600;
    color: "transparent";

    //会话的model包含id，名称和头像（目前使用的是字母，不是真正头像）
    ListModel
    {
        id:friendListModel;
        /*ListElement
        {
            imagePath:""
            name:"strName"
            idx:1
        }*/
    }

    ListView
    {
        id: friendListView;
        model: friendListModel;
        delegate: friendListDelegate;
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

    Component
    {
        id:friendListDelegate;
        Rectangle
        {
            //每一个会话的具体内容
            id: friendLabel;
            //anchors.left: parent.left;
            width: parent.width;
            height: 80;
            color: ListView.isCurrentItem? "grey":"transparent";
            MouseArea
            {
                anchors.fill: parent;
                //点击这个会话时，上次点击的颜色变成透明，这次点击的变成灰色，并更新这次的index等为上次点击
                onClicked:
                {
                    if(friendListView.lastItem!==null)
                    {
                        friendListView.lastItem.color="transparent";
                    }
                    color="grey";
                    friendListView.currentIndex=index;
                    friendListView.lastIndex=index;
                    friendListView.lastItem=friendListView.currentItem;
                    main.signalFriendListClicked(idx);
                }
                hoverEnabled: true;
                //鼠标移动到上方时变为银色，如果移动到上次点击的item则不做处理
                onEntered:
                {
                    scrollBar.visible=true;
                    if(friendListView./*currentIndex*/lastIndex===index)
                    {
                        return;
                    }
                    color="silver";
                }
                //移走后颜色变为正常
                onExited:
                {
                    scrollBar.visible=false;
                    if(friendListView./*currentIndex*/lastIndex===index)
                    {
                        return;
                    }
                    color="transparent";
                }
            }
            //红色标识，标识有信息来了
            Rectangle
            {
                color: "red";
                width: 24;
                height: 24;
                radius: 12;
                anchors.top: parent.top;
                anchors.topMargin: 10;
                anchors.right: parent.right;
                anchors.rightMargin: 20;
                visible: false;
                Text
                {
                    anchors.centerIn: parent;
                    id:msgCount;
                    font.pixelSize: 20;
                    text:"2";
                }
            }

            //头像区域
            Rectangle
            {
                id:imageLabel;
                color: "transparent";
                width: 50;
                height: 50;
                anchors.top: parent.top;
                anchors.topMargin: 20;
                anchors.left: parent.left;
                anchors.leftMargin: 15;
                Text
                {
                    id: imagePathText;
                    elide: Text.ElideRight;
                    anchors.fill: parent;
                    font.family: "msyh";
                    font.pixelSize: 14;
                    color: "black";
                    text: imagePath;
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
                anchors.topMargin: 20;
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
