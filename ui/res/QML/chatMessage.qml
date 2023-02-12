import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.12

import CPPService 1.0

Rectangle
{
    //开始的时候聊天记录数量肯定是0
    //这里的聊天记录数量是已经加载进页面的聊天记录数量
    property int iRecordCount:0;
    property string strIdx:"";
    property bool showMoreIsVisible:false;

    signal signalUpdateChatModel(string id);
    signal signalProfileImageClicked(string id);

    function updateFriendImage(strId,strImagePath)
    {
        for(var i=0;i<messageModel.count;++i)
        {
            if(messageModel.get(i).id===strId)
            {
                messageModel.get(i).imagePath=strImagePath;
            }
        }
    }

    //设置id
    function setId(strId)
    {
        strIdx=strId;
    }

    //令对话框移动到底部
    function scrollToEnd()
    {
        messageView.positionViewAtEnd();
    }

    function scrollToPosition(iPosition)
    {
        //console.log("count",iPosition);
        messageView.positionViewAtIndex(iPosition,ListView.Beginning)
//        if(messageView.count-iPosition<10)
//        {
//            messageView.positionViewAtIndex(messageView.count-iPosition,ListView.Beginning);
//        }
//        else
//        {
//            messageView.positionViewAtIndex(10,ListView.Beginning);
//        }
    }

    function insertMessageModel(strName,strMessage,bIsSelf,strShou,strId,strImagePath)
    {
        messageModel.insert(0,{"name":strName,"message":strMessage,"isSelf":bIsSelf,"shouzimu":strShou,"id":strId,"imagePath":strImagePath});
        iRecordCount++;
    }

    function appendMessageModel(strName,strMessage,bIsSelf,strShou,strId,strImagePath)
    {
        messageModel.append({"name":strName,"message":strMessage,"isSelf":bIsSelf,"shouzimu":strShou,"id":strId,"imagePath":strImagePath});
        messageView.positionViewAtEnd();
        iRecordCount++;
    }

    function clearMessageModel()
    {
        messageModel.clear();
    }

    function setRecordCount(iCount)
    {
        iRecordCount=iCount;
    }

    function setBusyIndicatorStateFlag(bFlag)
    {
        //console.log("state:",bFlag);
        if(bFlag)
        {
            busyIndi.running=bFlag;
        }
        else
        {
            time.start();
        }
    }
    property int coucccc:0;
    function getRecordCount()
    {
        coucccc= DataBase.getChatRecordCountFromDB(strIdx);
        return coucccc;
    }

    function needUpdate()
    {
        if(getRecordCount()>messageModel.count)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    Timer
    {
        id:time;
        interval: 300;
        onTriggered:
        {
            busyIndi.running=false;
        }
    }

    id: main;
    width: 300;
    height: 300;
    color: "transparent";
    MouseArea
    {
        anchors.fill: parent;
        onWheel:
            {
                if(showMoreIsVisible && wheel.angleDelta.y>0)
                {
                    main.signalUpdateChatModel(strIdx);
                    showMore.visible=false;
                    showMoreIsVisible=false;
                }
            }
    }    
    ListModel
    {
        id: messageModel;
    }

    Rectangle
    {
        id:viewWrapper;
        anchors.left: parent.left;
        anchors.top: parent.top//busyIndiRect.bottom;
        width: parent.width;
        height: parent.height//parent.height-busyIndiRect.height;
        ListView
        {
            id:messageView;
            model: messageModel;
            delegate: messageDelegate;
            anchors.fill: parent;
            orientation: ListView.Vertical
            property bool needUpddateModel:false;
            clip: true;
            ScrollBar.vertical: ScrollBar
            {
                id: scrollBar;
                visible: false;
                policy: ScrollBar.AsNeeded;
                onPositionChanged:
                {
                    if(!pressed)
                    {
                        return;
                    }

                    if(needUpdate() && !showMoreIsVisible && position===0)
                    {
                        showMoreIsVisible=true;
                        showMore.visible=true;
                    }
                    if(showMoreIsVisible && position!==0)
                    {
                        showMoreIsVisible=false;
                        showMore.visible=false;
                    }
                }
            }
            //移动结束了，看是否需要更新数据
            onMovementEnded:
            {
                //首先如果需要更新才进入逻辑
                if(needUpddateModel===true)
                {
                    if(!needUpdate())
                    {
                        return;
                    }

                    needUpddateModel=false;
                    //如果没有显示更多按钮，就显示出来
                    if(!showMoreIsVisible)
                    {
                        showMoreIsVisible=true;
                        showMore.visible=true;
                    }
                    //如果显示出来了就更新，并隐藏显示更多按钮
                    else
                    {
                        showMoreIsVisible=false;
                        showMore.visible=false;
                        main.signalUpdateChatModel(strIdx);
                    }
                }
                // if(needUpddateModel===true)
                // {
                //     main.signalUpdateChatModel(strIdx);
                //     needUpddateModel=false;
                // }
            }
            //每次y坐标变化了就看看是不是到了顶部了
            onContentYChanged:
            {
                // 下拉刷新判断逻辑：已经到头了，还下拉一定距离
                if (contentY < originY)
                {
                    var dy = contentY - originY;
                    if (dy < -20)
                    {
                        needUpddateModel = true;
                    }
                }
            }
        }
    }

    Rectangle
    {
        id:busyIndiRect;
        width: 40;
        height: 40;
        color: "transparent";
        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.leftMargin: (parent.width-width)/2;
        BusyIndicator
        {
            id:busyIndi;
            width: parent.width;
            height: parent.height;
            anchors.top: parent.top;
            anchors.left: parent.left;
            running: false;
        }
    }

    Rectangle
    {
        id:showMore;
        width: 60;
        height: 30;
        color: "transparent";
        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.leftMargin: (parent.width-width)/2;
        visible:false;
        Text
        {
            text:"show more";
            color:"blue";
            anchors.centerIn: parent;
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked:
                {
                    main.signalUpdateChatModel(strIdx);
                    showMore.visible=false;
                    showMoreIsVisible=false;
                }
            }
        }
    }

    Component
    {
        id: messageDelegate;

        Rectangle
        {
            MouseArea
            {
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered:
                {
                    scrollBar.visible=true;
                }
                onExited:
                {
                    scrollBar.visible=false;
                }
            }

            //anchors.left: parent.left;
            width: main.width;
            height: name.height+message.height+35;
            color: "transparent";
            Rectangle
            {
                id: pic;
                width: 40;
                height: 40;
                //radius: 20;
                //color: "#8033CCFF";
                anchors.left: parent.left;
                anchors.leftMargin: (model.isSelf===true)?main.width-60:10;
                anchors.top: parent.top;
                anchors.topMargin: 5;
                Image
                {
                    id: profileImage
                    anchors.fill: parent;
                    source: imagePath;
                }

                MouseArea
                {
                    anchors.fill: parent;
                    onClicked:
                    {
                        main.signalProfileImageClicked(model.id);
                    }
                }
            }

            Text
            {
                id: name;
                width: 150;
                height: contentHeight;
                elide: Text.ElideRight;
                anchors.top: parent.top;
                anchors.topMargin: 10;
                anchors.left: (model.isSelf===true)?pic.left:pic.right;
                anchors.leftMargin: (model.isSelf===true)?-(10+contentWidth):10;
                text:model.name;
                font.family: "msyh";
                font.pixelSize: 20;
            }
            Rectangle
            {
                id: greenRct;
                width: message.contentWidth+10;
                height: message.contentHeight+10;
                anchors.left: message.left;
                anchors.leftMargin: -5;
                anchors.top: message.top;
                anchors.topMargin: -5;
                radius: 5;
                color: "green";
                opacity: 0.6;
            }
            Text {
                id: message;
                width: main.width*0.7;
                height: contentHeight;
                wrapMode: Text.WrapAnywhere;
                anchors.left: (model.isSelf===true)?pic.left:pic.right;
                anchors.leftMargin: (model.isSelf===true)?-(10+contentWidth):10;
                anchors.top: name.bottom;
                anchors.topMargin: 10;
                text:model.message;
                font.family: "msyh";
                font.pixelSize: 24;
            }
        }
    }
}
