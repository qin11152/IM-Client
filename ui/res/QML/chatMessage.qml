import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.12

Rectangle
{
    //开始的时候聊天记录数量肯定是0
    //这里的聊天记录数量是已经加载进页面的聊天记录数量
    property int iRecordCount:0;
    property string strIdx:"";

    signal signalUpdateChatModel(string id);

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

    function insertMessageModel(strName,strMessage,bIsSelf,strShou,strId)
    {
        messageModel.insert(0,{"name":strName,"message":strMessage,"isSelf":bIsSelf,"shouzimu":strShou,"id":strId});
        iRecordCount++;
    }

    function appendMessageModel(strName,strMessage,bIsSelf,strShou,strId)
    {
        messageModel.append({"name":strName,"message":strMessage,"isSelf":bIsSelf,"shouzimu":strShou,"id":strId});
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

    id: main;
    width: 300;
    height: 300;
    color: "transparent";

    ListModel
    {
        id: messageModel;
    }
    ListView
    {
        id:messageView;
        model: messageModel;
        delegate: messageDelegate;
        anchors.fill: parent;
        orientation: ListView.Vertical
        property bool needUpddateModel:false;
        ScrollBar.vertical: ScrollBar
        {
            id: scrollBar;
            visible: false;
            policy: ScrollBar.AsNeeded;
        }
        //移动结束了，看是否需要更新数据
        onMovementEnded:
        {
            if(needUpddateModel===true)
            {
                console.log(strIdx);
                main.signalUpdateChatModel(strIdx);
            }
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
            height: name.height+message.height+25;
            color: "transparent";
            Rectangle
            {
                id: pic;
                width: 40;
                height: 40;
                radius: 20;
                color: "#8033CCFF";
                anchors.left: parent.left;
                anchors.leftMargin: (model.isSelf===true)?main.width-60:10;
                anchors.top: parent.top;
                anchors.topMargin: 5;
                Text
                {
                    font.family: "msyh";
                    font.pixelSize: 30;
                    text: model.shouzimu
                    anchors.centerIn: parent;
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
